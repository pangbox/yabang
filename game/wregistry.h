#pragma once
#include <windows.h>

class WRegistry {
public:
	WRegistry();
	~WRegistry();

	void SetLocation(const char* loc);
	bool Read(const char* field, char* out, int len);
	int Scan(const char* field, const char* fmt, ...);
	void Write(const char* field, const char* str, ...);
	void Write(const char* field, unsigned v);

protected:
	HKEY m_root{};
	char m_location[128]{};
	int m_code{};
};
