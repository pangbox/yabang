#pragma once
#include "wdevice.h"
#include <windows.h>
#include <timeapi.h>

class WInputDev : public WDevice {
public:
	enum Sort {
		Axis,
		Button,
		KeyState,
		BufState,
		ImeCompState,
		ImeCandWinState,
		ImeCompDoneState,
		ImeOpenState,
		ImeAlphaNumericState,
		ImeCandListState,
		ImeConvState,
		ImePropAtCaret,
		ImeInitAfterWndShown,
		ImeTermKeyMapSharing,
	};

	WInputDev(): dwLastInputTime(0) {
		this->active = TRUE;
		this->bUpdated = FALSE;
	}

	~WInputDev() { }

	virtual bool InitDevice(HWND hwnd, bool exclusive) {
		return false;
	}

	virtual void SetActive(bool stat) {
		this->active = stat;
	}

	virtual void Reset() { }

	virtual void Update(unsigned long timeStamp) { }

	virtual int GetState(int sort, int n) {
		return 0;
	}

	virtual unsigned long GetEventTime(int n) {
		return 0;
	}

	virtual bool IsAlphaNumericMode() {
		return true;
	}

	virtual void SetAlphaNumericMode() { }

	virtual void SetAlphaNumericMode(BOOL alnum) { }

	virtual BOOL IsUpdated() {
		return this->bUpdated;
	}

	virtual unsigned long GetLastInputTime() {
		return this->dwLastInputTime;
	}

	virtual void ResetInputTime() {
		this->dwLastInputTime = timeGetTime();
	}

	virtual void SetState(int sort, int n) { }

	virtual void SetOpenStatus(BOOL bOpen) { }

	virtual WInputDev* MakeClone(char* modeName, HWND hwnd) {
		return nullptr;
	}

protected:
	bool active;
	bool bUpdated;
	DWORD dwLastInputTime;
};

void AddInputDevice(WInputDev* inputDev);
