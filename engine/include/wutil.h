#pragma once
#include <stdio.h>

static void WriteToFile(LPCTSTR filename, LPBYTE data, DWORD dwBytesToWrite = 0) {
	HANDLE hFile = nullptr;
	DWORD dwBytesWritten;
	BOOL bErrorFlag = FALSE;

	if (dwBytesToWrite == 0) {
		dwBytesToWrite = strlen(LPCSTR(data));
	}

	hFile = CreateFile(filename, FILE_APPEND_DATA, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
	                   nullptr);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(nullptr, TEXT("Error opening file."), TEXT("yabang/device"), 0);
		ExitProcess(1);
		return;
	}

	while (dwBytesToWrite > 0) {
		bErrorFlag = WriteFile(hFile, data, dwBytesToWrite, &dwBytesWritten, nullptr);

		if (!bErrorFlag) {
			MessageBox(nullptr, TEXT("Error writing to file."), TEXT("yabang/wangreal"), 0);
			ExitProcess(1);
			break;
		}

		data += dwBytesWritten;
		dwBytesToWrite -= dwBytesWritten;
	}

	CloseHandle(hFile);
}

#ifdef ENABLE_TRACE
static void AppendToFile(char *data, LPCTSTR filename) {
	static HANDLE hFile = NULL;
	DWORD dwBytesToWrite = strlen(data);
	DWORD dwBytesWritten;
	BOOL bErrorFlag = FALSE;

	if (hFile == NULL) {
		hFile = CreateFile(filename, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			MessageBox(0, TEXT("Error opening log."), TEXT("yabang/wangreal"), 0);
			ExitProcess(1);
			return;
		}
	}

	while (dwBytesToWrite > 0) {
		bErrorFlag = WriteFile(hFile, data, dwBytesToWrite, &dwBytesWritten, NULL);

		if (!bErrorFlag) {
			MessageBox(0, TEXT("Error writing to log."), TEXT("yabang/wangreal"), 0);
			ExitProcess(1);
			break;
		}

		data += dwBytesWritten;
		dwBytesToWrite -= dwBytesWritten;
	}
}

class Trace {
public:
	Trace(const char *file, const char *fn, int line)
		: file(file), fn(fn), line(line) {}

	void operator()(const char *msgfmt, ...) {
		static char buffer[4096];
		static char msgbuf[4096];

		va_list args;
		va_start(args, msgfmt);
		vsprintf_s(msgbuf, 4096, msgfmt, args);
		perror(msgbuf);
		va_end(args);

		sprintf_s(buffer, 4096, "[%s:%s:%d]: %s\n", file, fn, line, msgbuf);
		AppendToFile(buffer, TEXT("!!yabangreal.log"));
	}
private:
	const char *file, *fn;
	int line;
};

#define TRACE Trace(__FILE__, __FUNCTION__, __LINE__)
#else
static inline void Trace(...) {}
#define TRACE Trace
#endif
