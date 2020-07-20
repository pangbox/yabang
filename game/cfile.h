#pragma once

class cFile;

cFile* GetCFileSub(const char* filename, int len, int pakonly);
cFile* GetCFile(const char* filename, int len, int pakonly);
void CloseCFile(class cFile* hdl);

class cFile {
public:
	cFile() = default;
	virtual ~cFile() = default;
	virtual bool Open(const char* pFilename);
	virtual int Read(void* data, int len) = 0;
	virtual int GetByte() = 0;
	virtual int Tell() = 0;
	virtual void Seek(int pos, int origin) = 0;
	[[nodiscard]] int Length() const;
	bool Scan(const char* fmt, ...);

protected:
	int m_nLen = 0;
};
