#pragma once
#include "cfile.h"

class cFileRaw final : public cFile {
public:
	cFileRaw(int fhl_, int len);
	~cFileRaw() override;
	int Tell() override;
	int GetByte() override;
	int Read(void* data, int len) override;
	void Seek(int pos, int origin) override;
	void ReadBlock();

private:
	int current = 0;
	int offset = 0;
	int trace = 0;
	int bufsize = 0;
	char* buffer = nullptr;
	int fhl = 0;
};
