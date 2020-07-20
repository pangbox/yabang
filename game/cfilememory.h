#pragma once
#include "cfile.h"

class cFileMemory : public cFile {
public:
	cFileMemory(cFile* fp);
	~cFileMemory() override;

	int Read(void* data, int size) override;
	int GetByte() override;
	void Seek(int pos, int origin) override;
	int Tell() override;

private:
	char* buffer;
	int offset;
};
