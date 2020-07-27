#include "wregistry.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>

WRegistry::WRegistry() {
	this->SetLocation(R"(HKEY_CURRENT_USER\Software\Ntreev\PangYa)");
}

WRegistry::~WRegistry() = default;

void WRegistry::SetLocation(const char* loc) {
	struct w_reg_pair {
		HKEY key;
		const char* name;
	};

	w_reg_pair list[] = {
		{HKEY_CURRENT_USER, "HKEY_CURRENT_USER"},
		{HKEY_CLASSES_ROOT, "HKEY_CLASSES_ROOT"},
		{HKEY_LOCAL_MACHINE, "HKEY_LOCAL_MACHINE"},
		{HKEY_USERS, "HKEY_USERS"},
		{HKEY_CURRENT_CONFIG, "HKEY_CURRENT_CONFIG"},
	};

	for (const auto& root : list) {
		if (strstr(loc, root.name) == loc) {
			this->m_root = root.key;
			loc = strchr(loc, '\\') + 1;
			break;
		}
	}

	strcpy(this->m_location, loc);
	if (this->m_location[strlen(this->m_location) - 1] == '\\') {
		this->m_location[strlen(this->m_location) - 1] = 0;
	}
}

bool WRegistry::Read(const char* field, char* out, int len) {
	DWORD type;
	DWORD dataLen;
	HKEY hKey;
	char subKey[128];

	if (strchr(field, '\\')) {
		sprintf(subKey, "%s\\%s", this->m_location, field);
		*strrchr(subKey, '\\') = 0;
		field = strrchr(field, '\\') + 1;
	} else {
		strcpy(subKey, this->m_location);
	}

	if (RegOpenKeyExA(this->m_root, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
		return false;
	}

	dataLen = len;
	bool result = true;
	if (RegQueryValueExA(hKey, field, nullptr, &type, reinterpret_cast<LPBYTE>(out), &dataLen) != ERROR_SUCCESS) {
		result = false;
	}

	RegCloseKey(hKey);
	return result;
}

int WRegistry::Scan(const char* field, const char* fmt, ...) {
	char text[512] = {0};
	char temp[128] = {0};
	int n = 0;

	int c = 0;
	if (!this->Read(field, text, _countof(temp))) {
		return 0;
	}

	if (!*fmt) {
		return 0;
	}

	va_list vl;
	va_start(vl, fmt);

	for (int i = 0; fmt[i]; i++) {
		if (fmt[i] != '%') {
			continue;
		}

		// Eat whitespace.
		for (char j = text[n]; j; j = text[++n]) {
			if (static_cast<uint8_t>(j) >= ' ') {
				break;
			}
		}

		// Take next token.
		size_t j;
		for (j = 0; j < _countof(temp); ++j) {
			if (static_cast<uint8_t>(text[n + j]) < ' ') {
				break;
			}
		}

		// No more tokens. Return.
		if (!j) {
			break;
		}

		// Advance n by j.
		memcpy(temp, &text[n], j);
		temp[j] = 0;
		n += j;
		++c;

		switch (fmt[i + 1]) {
			case 'D':
			case 'U':
			case 'd':
			case 'u':
				va_arg(vl, int) = atoi(temp);
				break;
			case 'F':
			case 'G':
			case 'f':
			case 'g':
				va_arg(vl, float) = static_cast<float>(atof(temp));
				break;
			case 'S':
			case 's':
				strcpy(va_arg(vl, char*), temp);
			case 'X':
			case 'x': {
				bool negate = temp[0] == '-';
				int result, digit;
				result = 0;
				if (temp[negate]) {
					char* pTemp = &temp[negate];
					do {
						int ch = static_cast<uint8_t>(*pTemp);
						if (ch < 'A') {
							digit = ch - (ch > '9' ? 'a' - 10 : '0');
						} else {
							digit = ch - ('A' - 10);
						}
						if (negate) {
							digit = -digit;
						}
						++pTemp;
						result = digit + 0x10 * result;
					} while (*pTemp);
				}
				va_arg(vl, int) = result;
				break;
			}
			default:
				break;
		}
	}

	va_end(vl);
	return c;
}

void WRegistry::Write(const char* field, const char* str, ...) {
	HKEY hKey;
	char buffer[1024];
	char subKey[128];
	va_list vl;

	va_start(vl, str);
	_vsnprintf(buffer, _countof(buffer), str, vl);

	if (strchr(field, '\\')) {
		sprintf(subKey, "%s\\%s", this->m_location, field);
		*strrchr(subKey, '\\') = 0;
		field = strrchr(field, '\\') + 1;
	} else {
		strcpy(subKey, this->m_location);
	}
	if (RegOpenKeyExA(this->m_root, subKey, 0, KEY_WRITE, &hKey)) {
		RegCreateKeyA(this->m_root, subKey, &hKey);
	}
	RegSetValueExA(hKey, field, 0, REG_SZ, reinterpret_cast<const BYTE*>(buffer), strlen(buffer));
	RegCloseKey(hKey);
}

void WRegistry::Write(const char* field, unsigned int v) {
	HKEY hKey;
	char subKey[128];

	if (strchr(field, '\\')) {
		sprintf(subKey, "%s\\%s", this->m_location, field);
		*strrchr(subKey, '\\') = 0;
		field = strrchr(field, '\\') + 1;
	} else {
		strcpy(subKey, this->m_location);
	}
	if (RegOpenKeyExA(this->m_root, subKey, 0, KEY_WRITE, &hKey)) {
		RegCreateKeyA(this->m_root, subKey, &hKey);
	}
	RegSetValueExA(hKey, field, 0, REG_DWORD, reinterpret_cast<const BYTE*>(v), 4);
	RegCloseKey(hKey);
}
