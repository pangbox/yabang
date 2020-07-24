#include "cfile.h"

#include <io.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <direct.h>
#include <windows.h>

#include "cfilememory.h"

char clonedirectory[128] = {0};

void makedirectory(const char* directory) {
	TCHAR curdir[MAX_PATH];
	char dir[MAX_PATH];

	// HACK: This function manipulates the CWD unnecessarily.
	// Should make improvements or replace.
	
	GetCurrentDirectory(_countof(curdir), curdir);
	const char* pathptr = directory;
	while (*pathptr) {
		pathptr = strchr(pathptr + 1, '\\');
		if (!pathptr) {
			pathptr = &directory[strlen(directory)];
		}
		if (pathptr[-1] != ':') {
			memcpy(dir, directory, pathptr - directory);
			dir[pathptr - directory] = 0;
			_mkdir(dir);
			_chdir(dir);
		}
	}
	SetCurrentDirectory(curdir);
}

cFile* GetCFileSub(const char* filename, int len, int pakonly) {
	if (len < 0) {
		cFile* f = GetCFile(filename, 4096, pakonly);
		if (!f) {
			return nullptr;
		}
		return new cFileMemory(f);
	}
	return nullptr;
}

cFile* GetCFile(const char* filename, int len, int pakonly) {
	char dir[260];
	char fname[260];

	cFile* f = GetCFileSub(filename, len, pakonly);

	if (f && clonedirectory[0]) {
		sprintf(fname, "%s%s", clonedirectory, filename);
		for (; strchr(fname, '/'); *strchr(fname, '/') = '\\') {}
		if (_access(fname, 0) == -1) {
			for (int v5 = 0; fname[v5]; v5++) {
				dir[v5] = fname[v5];
			}
			*strrchr(dir, 92) = 0;
			makedirectory(dir);
			FILE* out = fopen(fname, "wb");
			for (int i = 0; i < f->Length(); ++i) {
				fputc(f->GetByte(), out);
			}
			fclose(out);
		}
		f->Seek(0, SEEK_SET);
	}
	return f;
}

void CloseCFile(cFile* hdl) {
	delete hdl;
}

bool cFile::Open(const char* pFilename) {
	return false;
}

int cFile::Length() const {
	return this->m_nLen;
}

bool cFile::Scan(const char* fmt, ...) {
	char temp[128];

	int b = -1;
	int len = strlen(fmt);
	if (len <= 0) {
		return true;
	}

	va_list argp;
	va_start(argp, fmt);

	for (int i = 0; i < len; i++) {
		if (fmt[i] != '%') {
			continue;
		}
		if (b == -1) {
			b = this->GetByte();
			if (b == -1) {
				return false;
			}
		}
		while (b == '\n' || b == '\t' || b == '\r' || b == ' ') {
			b = this->GetByte();
			if (b == -1) {
				return false;
			}
		}
		switch (fmt[i + 1]) {
			case 'D':
			case 'U':
			case 'd':
			case 'u': {
				int j = 0;
				while (b >= '0' && b <= '9' || !j && b == '-') {
					temp[j++] = static_cast<char>(b);
					b = this->GetByte();
					if (b != -1) {
						break;
					}
				}
				temp[j] = 0;
				va_arg(argp, int) = atoi(temp);
				break;
			}
			case 'F':
			case 'G':
			case 'f':
			case 'g': {
				int j = 0;
				bool decimal = false;
				while (true) {
					if (!decimal && b == '.') {
						decimal = true;
					} else if ((b < '0' || b > '9') && (j != 0 || b != '-')) {
						break;
					}
					temp[j++] = static_cast<char>(b);
					b = this->GetByte();
					if (b == -1) {
						break;
					}
				}
				temp[j] = 0;
				va_arg(argp, float) = static_cast<float>(atof(temp));
				break;
			}
			case 'N':
			case 'n': {
				char* arg = va_arg(argp, char*);
				int j = 0;
				while (b != '\t' && b != '\r' && b != -1) {
					arg[j++] = static_cast<char>(b);
					b = this->GetByte();
					if (b == '\n') {
						break;
					}
				}
				arg[j] = 0;
				break;
			}
			case 'S':
			case 's': {
				char* arg = va_arg(argp, char*);
				int j = 0;
				while (b != '\t' && b != '\r' && b != ' ' && b != -1) {
					arg[j] = static_cast<char>(b);
					b = this->GetByte();
					++j;
					if (b == '\n') {
						break;
					}
				}
				arg[j] = 0;
				break;
			}
			case 'X':
			case 'x': {
				int value = 0;
				int negate = 0;
				while (true) {
					if (!negate && b == '-') {
						negate = 1;
					} else {
						if ((b < '0' || b > '9') && (b < 'a' || b > 'f') && (b < 'A' || b > 'F')) {
							break;
						}
						if (b >= 'a') {
							b -= 32;
						}
						value *= 0x10;
						if (b > '9') {
							value += b - 55;
						} else {
							value += b - 48;
						}
					}
					b = this->GetByte();
					if (b == -1) {
						return false;
					}
				}
				if (negate) {
					value = -value;
				}
				va_arg(argp, int) = value;
				break;
			}
			default:
				break;
		}
	}
	if (b != -1 && b != '\r') {
		this->Seek(-1, SEEK_CUR);
	}

	va_end(argp);
	return true;
}
