#include "cfileraw.h"


#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <io.h>

cFileRaw::cFileRaw(int fhl_, int len) {
	this->fhl = fhl_;
	this->m_nLen = _lseek(fhl_, 0, SEEK_END);
	this->offset = 0;
	this->bufsize = this->m_nLen;
	if (this->bufsize > len) {
		this->bufsize = len;
	}
	this->buffer = new char[this->bufsize];
	_lseek(this->fhl, 0, SEEK_SET);
	if (this->offset >= this->m_nLen) {
		this->current = this->bufsize;
	} else {
		int readLen = this->bufsize;
		if (readLen > this->m_nLen - this->offset) {
			readLen = this->m_nLen - this->offset;
		}
		this->trace = this->offset;
		this->current = this->bufsize - readLen;
		this->offset += _read(this->fhl, &this->buffer[this->current], readLen);
	}
}

cFileRaw::~cFileRaw() {
	delete this->buffer;
	_close(this->fhl);
}

int cFileRaw::Tell() {
	return this->current + this->offset - this->bufsize;
}

void cFileRaw::ReadBlock() {
	if (this->offset >= this->m_nLen) {
		this->current = this->bufsize;
	} else {
		int readLen = this->bufsize;
		if (readLen > this->m_nLen - this->offset) {
			readLen = this->m_nLen - this->offset;
		}
		this->current = this->bufsize - readLen;
		this->trace = this->offset;
		this->offset += _read(this->fhl, &this->buffer[this->bufsize - readLen], readLen);
	}
}

int cFileRaw::GetByte() {
	if (this->current >= this->bufsize) {
		if (this->offset >= this->m_nLen) {
			return -1;
		}
		ReadBlock();
	}
	return static_cast<uint8_t>(this->buffer[this->current++]);
}

int cFileRaw::Read(void* data, int len) {
	if (this->bufsize + this->m_nLen - this->current - this->offset <= len)
		len = this->bufsize + this->m_nLen - this->current - this->offset;

	int dataRead = 0;
	while (dataRead < len) {
		if (this->current >= this->bufsize) {
			if (this->offset >= this->m_nLen) {
				this->current = this->bufsize;
			} else {
				int readLen = this->m_nLen - this->offset;
				if (readLen > this->bufsize) {
					readLen = this->bufsize;
				}
				this->current = this->bufsize - readLen;
				this->trace = this->offset;
				this->offset += _read(this->fhl, &this->buffer[this->bufsize - readLen], readLen);
			}
		}
		int readLen = len - dataRead;
		if (readLen > this->bufsize - this->current) {
			readLen = this->bufsize - this->current;
		}
		memcpy(static_cast<char*>(data) + dataRead, &this->buffer[this->current], readLen);
		this->current += readLen;
		dataRead += readLen;
	}
	return dataRead;
}

void cFileRaw::Seek(int pos, int origin) {
	int offset;
	switch (origin) {
		case SEEK_SET:
			offset = pos;
			break;
		case SEEK_CUR:
			offset = pos + this->current + this->offset - this->bufsize;
			break;
		case SEEK_END:
			offset = pos + this->m_nLen;
			break;
		default:
			// TODO: Figure out how to handle invalid origin values.
			abort();
	}
	if (offset > this->m_nLen) {
		offset = this->m_nLen;
	}
	if (offset < 0) {
		offset = 0;
	}
	if (offset < this->trace || offset >= this->offset) {
		_lseek(this->fhl, offset, SEEK_SET);
		int foffset = _tell(this->fhl);
		this->offset = foffset;
		this->trace = foffset;
		if (foffset >= this->m_nLen) {
			this->current = this->bufsize;
		} else {
			int readLen = this->bufsize;
			if (readLen > this->m_nLen - foffset) {
				readLen = this->m_nLen - foffset;
			}
			this->trace = foffset;
			this->current = this->bufsize - readLen;
			this->offset += _read(this->fhl, &this->buffer[this->bufsize - readLen], readLen);
		}
	} else {
		this->current = offset + this->bufsize - this->offset;
	}
}
