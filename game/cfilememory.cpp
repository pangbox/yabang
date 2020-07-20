#include "cfilememory.h"

#include <cstdio>
#include <cstring>

cFileMemory::cFileMemory(cFile* fp) {
	this->m_nLen = fp->Length();
	this->buffer = new char[this->m_nLen];
	fp->Read(this->buffer, this->m_nLen);
	this->offset = 0;
	delete fp;
}

cFileMemory::~cFileMemory() {
	delete[] this->buffer;
}

int cFileMemory::Read(void* data, int size) {
	if (size < 0) {
		return 0;
	}
	if (size > this->m_nLen - this->offset) {
		size = this->m_nLen - this->offset;
	}
	if (size) {
		memcpy(data, this->buffer + this->offset, size);
		this->offset += size;
	}
	return size;
}

int cFileMemory::GetByte() {
	if (this->offset >= this->m_nLen) {
		return -1;
	}
	return this->buffer[this->offset++];
}

void cFileMemory::Seek(int pos, int origin) {
	switch (origin) {
		case SEEK_SET:
			this->offset = pos;
			break;
		case SEEK_CUR:
			this->offset += pos;
			break;
		case SEEK_END:
			this->offset = pos + this->m_nLen;
			break;
	}
}

int cFileMemory::Tell() {
	return this->offset;
}
