#pragma once
#include "wdevice.h"
#include <windows.h>
#include <timeapi.h>

class WInputDev : WDevice {
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

	~WInputDev() {
		return;
	}

	virtual bool InitDevice(HWND hwnd, bool exclusive) {
		return false;
	}

	virtual void SetActive(bool stat) {
		this->active = stat;
	}

	virtual void Reset() {
		return;
	}

	virtual void Update(unsigned long timeStamp) {
		return;
	}

	virtual int GetState(int sort, int n) {
		return 0;
	}

	virtual unsigned long GetEventTime(int n) {
		return 0;
	}

	virtual bool IsAlphaNumericMode() {
		return true;
	}

	virtual void SetAlphaNumericMode() {
		return;
	}

	virtual void SetAlphaNumericMode(BOOL alnum) {
		return;
	}

	virtual BOOL IsUpdated() {
		return this->bUpdated;
	}

	virtual unsigned long GetLastInputTime() {
		return this->dwLastInputTime;
	}

	virtual void ResetInputTime() {
		this->dwLastInputTime = timeGetTime();
	}

	virtual void SetState(int sort, int n) {
		return;
	}

	virtual void SetOpenStatus(BOOL bOpen) {
		return;
	}

	virtual WInputDev *MakeClone(char *modeName, HWND hwnd) {
		return NULL;
	}

protected:
	bool active;
	bool bUpdated;
	DWORD dwLastInputTime;
};

void AddInputDevice(WInputDev *inputDev);
