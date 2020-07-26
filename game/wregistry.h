#pragma once
#include <windows.h>

class WRegistry {
public:
	WRegistry();
	~WRegistry();

	void SetLocation(const char* loc);
	bool Read(char* field, char* out, int len);
	int Scan(char* field, char* fmt, ...);
	void Write(char* field, const char* str, ...);
	void Write(char* field, unsigned v);

protected:
	HKEY m_root{};
	char m_location[128]{};
	int m_code{};
};
