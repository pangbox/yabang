#include "cfiledefault.h"

#include <cstdint>
#include <io.h>

cFileDefault::cFileDefault(int fd) : m_fd(fd) {}

cFileDefault::~cFileDefault() {
	_close(m_fd);
}

int cFileDefault::Read(void* data, int len) {
	return _read(m_fd, data, len);
}

int cFileDefault::GetByte() {
	uint8_t code = 0;
	return this->Read(&code, 1) ? static_cast<uint8_t>(code) : -1;
}

int cFileDefault::Tell() {
	return _tell(m_fd);
}

void cFileDefault::Seek(int pos, int origin) {
	_lseek(m_fd, pos, origin);
}
