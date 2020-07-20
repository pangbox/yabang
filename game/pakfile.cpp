#include "pakfile.h"

#include <io.h>
#include <fcntl.h>
#include <windows.h>

#include "cfiledefault.h"
#include "cipher.h"
#include "spakinfo.h"
#include "wlist.h"
#include "wresourcemanager.h"

sPak* pak = nullptr;

sPak* OpenPAKFILE2(cFile* fp) {
	// This is how many more bytes the in-memory file headers are, for calculating
	// the correct sPakinfo allocation size.
	constexpr auto perFileDiff = sizeof(sPakinfo) - 14;

	int ftOffset;
	int len;

	len = 0;
	if (!fp) {
		return nullptr;
	}

	sPak* pakFile = new sPak();
	pakFile->lock = CreateEventA(nullptr, FALSE, TRUE, nullptr);
	memset(pakFile->hash, 0xFF, sizeof(pakFile->hash));

	// Seek to the PAK trailer.
	fp->Seek(-9, SEEK_END);
	int trailerOffset = fp->Tell();

	// Read trailer information.
	fp->Read(&ftOffset, 4);
	fp->Read(&pakFile->num, 4);
	if (pakFile->num <= 0) {
		operator delete(pakFile);
		return nullptr;
	}

	// Allocate pointer list.
	pakFile->list = new sPakinfo*[pakFile->num];

	// Allocate pak info. The trailer is assumed to come right after the file
	// table, and we use pointer arithmetic to determine how much space we
	// need (since file names are variable size.)
	const size_t pakInfoLen = trailerOffset - ftOffset + pakFile->num * perFileDiff;
	char* const pakInfoBlock = new char[pakInfoLen];

	// Seek to file table.
	fp->Seek(ftOffset, SEEK_SET);

	int piOffset = 0;
	for (int i = 0; i < pakFile->num; i++) {
		auto& pakInfo = pakFile->list[i];
		pakInfo = reinterpret_cast<sPakinfo*>(pakInfoBlock + piOffset);

		// Read name length and entry type.
		fp->Read(&len, 1);
		fp->Read(&pakInfo->type, 1);

		if ((pakInfo->type & 0xF0) == 0) {
			pakInfo->type |= 0x10;
		}

		if ((pakInfo->type & 0xF0) == 0x10) {
			// Older XOR'd entry.
			len += 1;
			fp->Read(&pakInfo->offset, 4);
			fp->Read(&pakInfo->packedsize, 4);
			fp->Read(&pakInfo->size, 4);
			fp->Read(pakInfo->filename, len);

			pakInfo->size ^= 0x71;
			for (int j = 0; j < len - 1; ++j)
				pakInfo->filename[j] ^= 0x71;
		} else if ((pakInfo->type & 0xF0) == 0x20) {
			// XTEA-encrypted entry.

			// Read metadata + filename
			fp->Read(&pakInfo->offset, 4);
			fp->Read(&pakInfo->packedsize, 4);
			fp->Read(&pakInfo->size, 4);
			fp->Read(pakInfo->filename, len);

			// Decrypt metadata
			unsigned int v[2];
			unsigned int w[2];
			v[0] = pakInfo->offset;
			v[1] = pakInfo->size;
			Decipher(v, w, g_xteaKeyKr);
			pakInfo->offset = w[0];
			pakInfo->size = w[1];

			// Decrypt filename
			for (int j = 0; j < len; j += 8) {
				v[0] = reinterpret_cast<uint32_t*>(pakInfo->filename)[0];
				v[1] = reinterpret_cast<uint32_t*>(pakInfo->filename)[1];
				Decipher(v, reinterpret_cast<uint32_t*>(&pakInfo->filename[j]), g_xteaKeyKr);
			}
		} else if ((pakInfo->type & 0xF0) == 0x80) {
			// Unencrypted entry.
			len += 1;
			fp->Read(&pakInfo->offset, 4);
			fp->Read(&pakInfo->packedsize, 4);
			fp->Read(&pakInfo->size, 4);
			fp->Read(pakInfo->filename, len);
		} else {
			continue;
		}
		// TODO: Need to revisit the hashing bits.
		// This is not the original algorithm.
		int hashVal = (pakInfo->filename[0] & 0xF) | (len & 0xF) << 4;
		pakInfo->next = pakFile->hash[hashVal];
		pakFile->hash[hashVal] = i;
		piOffset += static_cast<int>(sizeof(sPakinfo)) + len;
	}
	pakFile->next = nullptr;
	pakFile->offset = fp->Tell();
	pakFile->fp = fp;
	return pakFile;
}

void HookPAKFile(char* filename) {
	cFile* f;

	int fd = _open(filename, _O_BINARY, 0);
	if (fd == -1) {
		f = GetCFile(filename, 0x8000, 0);
		if (!f) {
			return;
		}
	} else {
		f = new cFileDefault(fd);
	}
	sPak* pakFile = OpenPAKFILE2(f);
	pakFile->next = pak;
	pak = pakFile;
}

void InitPakedFile() {
	WList<char*> pakList{16, 16};
	WIN32_FIND_DATAA findFileData;

	// Enumerate all of the pak files in the current directory.
	HANDLE hFindFile = FindFirstFileA("projectg*.pak", &findFileData);
	if (hFindFile != INVALID_HANDLE_VALUE) {
		do {
			char* filenameCopy = new char[strlen(findFileData.cFileName + 1)];
			strcpy(filenameCopy, findFileData.cFileName);
			pakList.AddItem(filenameCopy, nullptr, false);
		} while (FindNextFileA(hFindFile, &findFileData));
	}
	FindClose(hFindFile);

	// Load each pak file in lexicographically increasing order.
	while (true) {
		auto* it = pakList.Start();
		if (!it) {
			break;
		}
		// Find the lexicographically next pak file.
		auto* nextPak = it;
		while (it) {
			if (strcmp(it, nextPak) < 0) {
				nextPak = it;
			}
			it = pakList.Next();
		}
		pakList.DelItem(nextPak);
		HookPAKFile(nextPak);
		delete[] nextPak;
	}

	// TODO: Call SetAutoMatchDirectory
	//g_resourceManager->SetAutoMatchDirectory("data");
}
