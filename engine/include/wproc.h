#pragma once
#include <windows.h>

class WProc;

class WProcManager {
public:
	struct WHandle {
		HWND hWnd;
		WProc* proc;
	};

	void DelProc(WProc* proc) {
		int n = this->m_procNum;
		if (n <= 0) {
			return;
		}

		int i;
		for (i = 0; i < this->m_procNum; i++) {
			if (this->m_procList[i].proc == proc) {
				break;
			}
		}

		if (i < n) {
			this->m_procList[i].hWnd = this->m_procList[n - 1].hWnd;
			this->m_procList[i].proc = this->m_procList[n - 1].proc;
			--this->m_procNum;
		}
	}

	void AddProc(WProc* proc, HWND hWnd) {
		this->m_procList[this->m_procNum].hWnd = hWnd;
		this->m_procList[this->m_procNum].proc = proc;
		++this->m_procNum;
	}

private:
	WHandle m_procList[16] = {};
	int m_procNum = 0;
};

class WProc {
public:
	WProc() {
		this->m_include = nullptr;
	}

	virtual ~WProc() {
		if (this->m_include) {
			this->m_include->DelProc(this);
		}
	}

	virtual LRESULT WinProc(UINT msg, WPARAM wParam, LPARAM lParam) = 0;

	void SetProc(WProcManager* procMan, HWND hWnd)
	{
		this->m_include = procMan;
		procMan->AddProc(this, hWnd);
	}

private:
	WProcManager* m_include;
};
