#pragma once
#include "cfile.h"

struct sPak;
struct sPakinfo;

class cFilePacked : public cFile {
public:
	cFilePacked(sPak* pak_, sPakinfo* pakinfo_, int len);
	~cFilePacked();
	void ReadBlock();
	int Tell() override;
	int GetByte() override;
	int Read(void* data, int len) override;
	void Seek(int pos, int origin) override;

protected:
	char* buffer;
	int current;
	int pivot;
	int trace;
	int size;
	int offset;
	int bufsize;
	sPak* pak;
	sPakinfo* pakinfo;
};
