#pragma once
#include "cfile.h"

class cFileMap final : public cFile {
public:
	cFileMap();
	~cFileMap() override;
	bool Open(const char* pFilename) override;
	int Read(void* data, int len) override;
	int GetByte() override;
	int Tell() override;
	void Seek(int pos, int origin) override;

private:
	void* m_hFile = nullptr;
	void* m_hMap = nullptr;
	char* m_pBuff = nullptr;
	int m_offset = 0;
};
