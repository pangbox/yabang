#pragma once
#include <windows.h>

class WLock {
public:
	WLock();
	~WLock();
	void Lock();
	void Unlock();
	void UnlockInThread(unsigned int threadID);

private:
	unsigned int m_threadID = -1;
	CRITICAL_SECTION m_criticalSection{};
	bool m_locked = false;
};
