#pragma once
#include <windows.h>

class WLock {
public:
	unsigned int m_threadID;
	CRITICAL_SECTION m_criticalSection;
	bool m_locked;
};
