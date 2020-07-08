#include "wresourcemanager.h"

void WResourceManager::StrcpyLower(char* out, const char* src) {
	for (; *src; src++) {
		*out++ = (*src >= 'A' && *src <= 'Z') ? (*src - 0x20) : *src;
	}
	*out = 0;
}

const char* WResourceManager::FindMatchFile(const char* filename) {
	char lowername[64] = {0};

	if (!this->m_matchDirectory) {
		return filename;
	}

	const char* basename = strrchr(filename, '/');
	if (!basename) {
		basename = strrchr(filename, '\\');
		if (!basename) {
			return filename;
		}
	}

	// Increment past the directory separator.
	basename++;

	StrcpyLower(lowername, basename);

	auto* match = this->m_matchList.Find(lowername);
	if (!match) {
		return filename;
	}

	return match->fullname;
}
