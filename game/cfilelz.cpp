#include "cfilelz.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <windows.h>

constexpr uint16_t g_securityTable[8] = {0xFF21, 0x834F, 0x675F, 0x0034, 0xF237, 0x815F, 0x4765, 0x0233};

cFileLZ::cFileLZ(sPak* pak_, sPakinfo* pakinfo_, int len)
	: cFilePacked(pak_, pakinfo_, len) {}

cFileLZ::~cFileLZ() = default;

void cFileLZ::ReadBlockLz() {
	uint8_t d1;
	this->trace = this->offset;
	if (this->cFilePacked::current < this->bufsize || this->cFilePacked::offset < this->pivot + this->size) {
		if (this->cFilePacked::current >= this->bufsize) {
			ReadBlock();
		}
		d1 = static_cast<uint8_t>(this->cFilePacked::buffer[this->cFilePacked::current++]);
	} else {
		d1 = -1;
	}
	for (int i = 8; i != 0; --i, d1 >>= 1) {
		if ((d1 & 1) == 0) {
			if (this->cFilePacked::current >= this->bufsize) {
				if (this->cFilePacked::offset >= this->pivot + this->size) {
					this->buffer[this->offset++ & 0x1FFF] = -1;
					continue;
				}
				ReadBlock();
			}
			this->buffer[this->offset++ & 0x1FFF] = this->cFilePacked::buffer[this->cFilePacked::current++];
			continue;
		}
		uint16_t d2;
		cFilePacked::Read(&d2, 2);
		int srcOffset = LOWORD(this->offset) - (d2 & 0xFFF);
		uint16_t toCopy = (static_cast<uint16_t>(d2) >> 12) + 2;
		while (toCopy != 0) {
			int copyRem = 0x2000 - (this->offset & 0x1FFF);
			if (copyRem > toCopy) {
				copyRem = toCopy;
			}
			int copyLen = 0x2000 - (srcOffset & 0x1FFF);
			if (copyLen > copyRem) {
				copyLen = copyRem;
			}
			memcpy(&this->buffer[this->offset & 0x1FFF], &this->buffer[srcOffset & 0x1FFF],
			       static_cast<uint16_t>(copyLen));
			this->offset += static_cast<uint16_t>(copyLen);
			srcOffset += copyLen;
			toCopy -= copyLen;
		}
	}
	if (this->offset > this->m_nLen) {
		this->offset = this->m_nLen;
	}
}

int cFileLZ::GetByte() {
	if (this->current >= this->offset) {
		if (this->current >= this->m_nLen) {
			return -1;
		}
		this->ReadBlockLz();
	}
	return static_cast<uint8_t>(this->buffer[this->current++ & 0x1FFF]);
}

int cFileLZ::Tell() {
	return this->current;
}

int cFileLZ::Read(void* data, int len) {
	int totalRead = 0;
	if (len > this->m_nLen - this->current) {
		len = this->m_nLen - this->current;
	}
	if (len <= 0) {
		return 0;
	}
	while (true) {
		if (this->current >= this->offset) {
			this->ReadBlockLz();
		}
		int readLeft = len - totalRead;
		if (readLeft > this->offset - this->current) {
			readLeft = this->offset - this->current;
		}
		int readLen = 0x2000 - (this->current & 0x1FFF);
		if (readLen > readLeft) {
			readLen = readLeft;
		}
		memcpy(static_cast<char*>(data) + totalRead, &this->buffer[this->current & 0x1FFF], readLen);
		this->current += readLen;
		totalRead += readLen;
		if (totalRead >= len) {
			break;
		}
	}
	return totalRead;
}

void cFileLZ::Seek(int pos, int origin) {
	int offset;
	switch (origin) {
		case SEEK_SET:
			offset = pos;
			break;
		case SEEK_CUR:
			offset = pos + this->current;
			break;
		case SEEK_END:
			offset = pos + this->m_nLen;
			break;
		default:
			// TODO: handle invalid origin/whence on seek
			abort();
	}
	if (offset > this->m_nLen) {
		offset = this->m_nLen;
	}
	if (offset < 0) {
		offset = 0;
	}
	if (offset < this->trace) {
		this->current = 0;
		this->offset = 0;
		if (this->pivot < this->cFilePacked::trace || this->pivot >= this->cFilePacked::offset) {
			this->cFilePacked::offset = this->pivot;
			this->cFilePacked::trace = this->pivot;
			ReadBlock();
		} else {
			this->cFilePacked::current = this->pivot + this->bufsize - this->cFilePacked::offset;
		}
	}
	while (this->offset < offset) {
		this->ReadBlockLz();
	}
	this->current = offset;
}

cFileLZ2::cFileLZ2(sPak* pak_, sPakinfo* pakinfo_, int len)
	: cFileLZ(pak_, pakinfo_, len) {}

cFileLZ2::~cFileLZ2() = default;

void cFileLZ2::ReadBlockLz() {
	unsigned int control;

	bool v2 = this->cFilePacked::current < this->bufsize;
	this->trace = this->offset;
	if (v2 || this->cFilePacked::offset < this->pivot + this->size) {
		if (!v2) {
			ReadBlock();
		}
		control = static_cast<uint8_t>(this->cFilePacked::buffer[this->cFilePacked::current++]);
	} else {
		control = -1;
	}

	uint16_t d1 = control ^ 0xC8;
	for (int i = 8; i != 0; --i, d1 >>= 1) {
		if ((d1 & 1) == 0) {
			if (this->cFilePacked::current >= this->bufsize) {
				if (this->cFilePacked::offset >= this->pivot + this->size) {
					this->buffer[this->offset++ & 0x1FFF] = -1;
					continue;
				}
				ReadBlock();
			}
			this->buffer[this->offset++ & 0x1FFF] = this->cFilePacked::buffer[this->cFilePacked::current++];
			continue;
		}
		uint16_t d2;
		cFilePacked::Read(&d2, 2); // NOLINT(bugprone-parent-virtual-call)
		d2 ^= g_securityTable[(control >> 3) & 7];
		uint16_t srcOffset = (this->offset & 0xFFFF) - (d2 & 0xFFF);
		int toCopy = (d2 >> 12) + 2;
		while (toCopy) {
			int copyLen = 0x2000 - (this->offset & 0x1FFF);
			if (copyLen > toCopy) {
				copyLen = toCopy;
			}
			if (copyLen > 0x2000 - (srcOffset & 0x1FFF)) {
				copyLen = 0x2000 - (srcOffset & 0x1FFF);
			}
			memcpy(&this->buffer[this->offset & 0x1FFF], &this->buffer[srcOffset & 0x1FFF],
			       static_cast<uint16_t>(copyLen));
			this->offset += static_cast<uint16_t>(copyLen);
			srcOffset += copyLen;
			toCopy -= copyLen;
		}
	}
	if (this->offset > this->m_nLen) {
		this->offset = this->m_nLen;
	}
}
