#include "cfilemap.h"


#include <cstdint>
#include <cstdio>
#include <windows.h>

cFileMap::cFileMap() = default;

cFileMap::~cFileMap() {
	if (this->m_pBuff) {
		UnmapViewOfFile(this->m_pBuff);
		this->m_pBuff = nullptr;
	}
	if (this->m_hMap) {
		CloseHandle(this->m_hMap);
		this->m_hMap = nullptr;
	}
	if (this->m_hFile) {
		CloseHandle(this->m_hFile);
		this->m_hFile = nullptr;
	}
}

bool cFileMap::Open(const char* pFilename) {
	this->m_hFile = CreateFileA(pFilename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
	                            nullptr);
	if (this->m_hFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	this->m_nLen = GetFileSize(this->m_hFile, nullptr);
	this->m_hMap = CreateFileMappingA(this->m_hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (this->m_hMap && GetLastError() == ERROR_INVALID_ORDINAL) {
		CloseHandle(this->m_hMap);
		this->m_hMap = nullptr;
	}
	if (this->m_hMap) {
		this->m_pBuff = static_cast<char*>(MapViewOfFile(this->m_hMap, FILE_MAP_READ, 0, 0, 0));
		this->m_offset = 0;
	}
	return true;
}

int cFileMap::Read(void* data, int len) {
	if (!this->m_pBuff) {
		return 0;
	}
	int copy = len;
	if (this->m_nLen - this->m_offset <= len) {
		copy = this->m_nLen - this->m_offset;
	}
	if (copy <= 0) {
		return 0;
	}
	memcpy(data, &this->m_pBuff[this->m_offset], copy);
	this->m_offset += copy;
	return copy;
}

int cFileMap::GetByte() {
	if (!this->m_pBuff || this->m_offset >= this->m_nLen || this->m_offset < 0) {
		return -1;
	}
	return static_cast<uint8_t>(this->m_pBuff[this->m_offset++]);
}

int cFileMap::Tell() {
	return this->m_offset;
}

void cFileMap::Seek(int pos, int origin) {
	switch (origin) {
		case SEEK_SET:
			this->m_offset = pos;
			break;
		case SEEK_CUR:
			this->m_offset += pos;
			break;
		case SEEK_END:
			this->m_offset = pos + this->m_nLen;
			break;
		default:
			break;
	}
}
