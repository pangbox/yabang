#include "wlock.h"

WLock::WLock() {
	InitializeCriticalSection(&this->m_criticalSection);
}

WLock::~WLock() {
	DeleteCriticalSection(&this->m_criticalSection);
}

void WLock::Lock() {
	unsigned int currentThread = GetCurrentThreadId();
	if (this->m_threadID == currentThread) {
		*reinterpret_cast<int*>(0xDEAD20CC) = 0;
	}
	EnterCriticalSection(&this->m_criticalSection);
	this->m_threadID = currentThread;
	this->m_locked = true;
}

void WLock::Unlock() {
	if (!this->m_locked) {
		return;
	}
	this->m_threadID = -1;
	this->m_locked = false;
	LeaveCriticalSection(&this->m_criticalSection);
}

void WLock::UnlockInThread(unsigned int threadID) {
	if (threadID == this->m_threadID && this->m_locked) {
		this->m_threadID = -1;
		this->m_locked = false;
		LeaveCriticalSection(&this->m_criticalSection);
	}
}
