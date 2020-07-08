#pragma once
#include "cfilepacked.h"

class cFileLZ : public cFilePacked {
public:
	cFileLZ(sPak* pak_, sPakinfo* pakinfo_, int len);
	~cFileLZ() override;
	virtual void ReadBlockLz();
	int GetByte() override;
	int Tell() override;
	int Read(void* data, int len) override;
	void Seek(int pos, int origin) override;

protected:
	int current = 0;
	int offset = 0;
	int trace = 0;
	char buffer[8192] = {0};
};

class cFileLZ2 : public cFileLZ {
public:
	cFileLZ2(sPak* pak_, sPakinfo* pakinfo_, int len);
	~cFileLZ2();
	void ReadBlockLz() override;
};
