#include "cfilepacked.h"

#include <cstdio>
#include <windows.h>
#include "spak.h"
#include "spakinfo.h"

cFilePacked::cFilePacked(sPak* pak_, sPakinfo* pakinfo_, int len) {
	this->pak = pak_;
	this->pakinfo = pakinfo_;
	this->current = 0;
	this->m_nLen = pakinfo_->size;
	this->trace = pakinfo_->offset;
	this->pivot = pakinfo_->offset;
	this->offset = pakinfo_->offset;
	this->size = pakinfo_->packedsize;
	if (len < static_cast<int>(pakinfo_->packedsize))
		this->bufsize = len;
	else
		this->bufsize = pakinfo_->packedsize;
	this->buffer = new char[this->bufsize];
	ReadBlock();
}

cFilePacked::~cFilePacked() {
	delete[] this->buffer;
}

void cFilePacked::ReadBlock() {
	if (this->offset >= this->size + this->pivot) {
		this->current = this->bufsize;
		return;
	}
	WaitForSingleObject(this->pak->lock, INFINITE);
	if (this->pak->offset != this->offset) {
		this->pak->fp->Seek(this->offset, 0);
	}
	int readLen = this->size + this->pivot - this->offset;
	if (this->bufsize <= readLen) {
		readLen = this->bufsize;
	}
	this->current = this->bufsize - readLen;
	this->trace = this->offset;
	this->offset += this->pak->fp->Read(&this->buffer[this->current], readLen);
	this->pak->offset = this->offset;
	SetEvent(this->pak->lock);
}

int cFilePacked::Tell() {
	return this->current + this->offset - this->bufsize - this->pivot;
}

int cFilePacked::GetByte() {
	if (this->current >= this->bufsize) {
		if (this->offset >= this->pivot + this->size) {
			return -1;
		}
		ReadBlock();
	}
	return static_cast<uint8_t>(this->buffer[this->current++]);
}

int cFilePacked::Read(void* data, int len) {
	if (len > this->bufsize + this->pivot + this->size - this->offset - this->current) {
		len = this->bufsize + this->pivot + this->size - this->offset - this->current;
	}
	if (len <= 0) {
		return 0;
	}
	int i = 0;
	while (true) {
		if (this->current >= this->bufsize) {
			ReadBlock();
		}
		int copyLen = this->bufsize - this->current;
		if (copyLen > len - i) {
			copyLen = len - i;
		}
		memcpy(static_cast<char*>(data) + i, &this->buffer[this->current], copyLen);
		this->current += copyLen;
		i += copyLen;
		if (i >= len) {
			break;
		}
	}
	return i;
}

void cFilePacked::Seek(int pos, int origin) {
	int offset;
	switch (origin) {
		case SEEK_SET:
			offset = pos + this->pivot;
			break;
		case SEEK_CUR:
			offset = pos + this->current + this->offset - this->bufsize;
			break;
		case SEEK_END:
			offset = pos + this->pivot + this->size;
			break;
		default:
			offset = pos;
			break;
	}
	if (offset > this->pivot + this->size) {
		offset = this->pivot + this->size;
	}
	if (offset < this->pivot) {
		offset = this->pivot;
	}
	if (offset < this->trace || offset >= this->offset) {
		this->offset = offset;
		this->trace = offset;
		ReadBlock();
	} else {
		this->current = offset + this->bufsize - this->offset;
	}
}
