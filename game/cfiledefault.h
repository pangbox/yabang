#pragma once
#include "cfile.h"

class cFileDefault final : public cFile {
public:
	explicit cFileDefault(int fd);
	~cFileDefault() override;
	int Read(void* data, int len) override;
	int GetByte() override;
	int Tell() override;
	void Seek(int pos, int origin) override;

private:
	int m_fd;
};
