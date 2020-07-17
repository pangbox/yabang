#include "dimouse.h"
#include <windows.h>

bool DirectInputMouse::InitDevice(HWND hWnd, bool exclusive) {
	DIPROPDWORD diProp;

	if (this->m_diMouse) {
		this->m_diMouse->Unacquire();
		this->m_diMouse->Release();
		this->m_diMouse = nullptr;
	}

	if (FAILED(this->m_diBackup->CreateDevice(GUID_SysMouse, &this->m_diMouse, nullptr))) {
		return false;
	}

	if (FAILED(this->m_diMouse->SetDataFormat(&c_dfDIMouse))) {
		return false;
	}

	DWORD flags = DISCL_FOREGROUND | (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE);
	if (FAILED(this->m_diMouse->SetCooperativeLevel(hWnd, flags))) {
		return false;
	}

	if (this->m_hEvent) {
		CloseHandle(this->m_hEvent);
		this->m_hEvent = nullptr;
	}

	this->m_hEvent = CreateEventA(nullptr, 0, 0, nullptr);
	if (!this->m_hEvent) {
		return false;
	}

	if (FAILED(this->m_diMouse->SetEventNotification(this->m_hEvent))) {
		return false;
	}

	diProp.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diProp.dwData = 16;
	diProp.diph.dwSize = sizeof(DIPROPDWORD);
	diProp.diph.dwHow = 0;
	diProp.diph.dwObj = 0;

	if (FAILED(this->m_diMouse->SetProperty(DIPROP_BUFFERSIZE, reinterpret_cast<LPCDIPROPHEADER>(&diProp)))) {
		this->m_diMouse->Release();
		this->m_diMouse = nullptr;
		return false;
	}

	this->m_passCur = 0;
	this->m_bufCur = 0;
	return true;
}

unsigned long DirectInputMouse::GetEventTime(int n) {
	if (n < 0 || n >= 4) return 0;
	return this->m_timeBuf[n];
}

void DirectInputMouse::FlushBuffer(DWORD timeStamp) {
	this->bUpdated = true;
	this->dwLastInputTime = timeGetTime();
	for (; this->m_passCur < this->m_bufCur; this->m_passCur += 1) {
		if (timeStamp < this->m_mouseBuf[this->m_passCur].dwTimeStamp) {
			break;
		}

		switch (this->m_mouseBuf[this->m_passCur].dwOfs) {
			case offsetof(DIMOUSESTATE, lX):
				this->m_x += this->m_mouseBuf[this->m_passCur].dwData;
				break;
			case offsetof(DIMOUSESTATE, lY):
				this->m_y += this->m_mouseBuf[this->m_passCur].dwData;
				break;
			case offsetof(DIMOUSESTATE, lZ):
				this->m_z += this->m_mouseBuf[this->m_passCur].dwData;
				break;
			case offsetof(DIMOUSESTATE, rgbButtons[0]):
				this->m_b[0] = static_cast<int>(this->m_mouseBuf[this->m_passCur].dwData & 0x80);
				if (this->m_mouseBuf[this->m_passCur].dwData & 0x80) {
					this->m_timeBuf[0] = this->m_mouseBuf[this->m_passCur].dwTimeStamp;
				}
				break;
			case offsetof(DIMOUSESTATE, rgbButtons[1]):
				this->m_b[1] = static_cast<int>(this->m_mouseBuf[this->m_passCur].dwData & 0x80);
				if (this->m_mouseBuf[this->m_passCur].dwData & 0x80) {
					this->m_timeBuf[1] = this->m_mouseBuf[this->m_passCur].dwTimeStamp;
				}
				break;
			case offsetof(DIMOUSESTATE, rgbButtons[2]):
				this->m_b[2] = static_cast<int>(this->m_mouseBuf[this->m_passCur].dwData & 0x80);
				if (this->m_mouseBuf[this->m_passCur].dwData & 0x80) {
					this->m_timeBuf[2] = this->m_mouseBuf[this->m_passCur].dwTimeStamp;
				}
				break;
			case offsetof(DIMOUSESTATE, rgbButtons[3]):
				this->m_b[3] = static_cast<int>(this->m_mouseBuf[this->m_passCur].dwData & 0x80);
				if (this->m_mouseBuf[this->m_passCur].dwData & 0x80) {
					this->m_timeBuf[3] = this->m_mouseBuf[this->m_passCur].dwTimeStamp;
				}
				break;
			default:
				break;
		}
	}
	if (this->m_bufCur == this->m_passCur) {
		this->m_passCur = 0;
		this->m_bufCur = 0;
	}
}

void DirectInputMouse::ClearBuffer() {
	this->m_bufCur = 0;
	this->m_passCur = 0;
	this->m_z = 0;
	this->m_y = 0;
	this->m_x = 0;
	this->m_b[0] = 0;
	this->m_b[1] = 0;
	this->m_b[2] = 0;
	this->m_b[3] = 0;
}

int DirectInputMouse::GetState(int sort, int n) {
	switch (sort) {
		case 1:
			return this->m_b[n];
		case 0:
			switch (n) {
				case 1:
					return this->m_x;
				case 2:
					return this->m_y;
				case 3:
					return this->m_z;
				default:
					return 0;
			}
		default:
			return 0;
	}
}

HRESULT DirectInputMouse::Acquire() const {
	if (this->m_moveMode) {
		return 1;
	}
	return this->m_diMouse->Acquire();
}

DirectInputMouse::DirectInputMouse(IDirectInput8A* di, HWND hWnd) {
	this->m_diBackup = di;
	this->m_hEvent = nullptr;
	this->active = true;
	this->m_diMouse = nullptr;
	this->m_moveMode = false;
	this->m_hWnd = hWnd;
	this->m_bufCur = 0;
	this->m_passCur = 0;
	this->m_z = 0;
	this->m_y = 0;
	this->m_x = 0;
	this->m_b[0] = 0;
	this->m_b[1] = 0;
	this->m_b[2] = 0;
	this->m_b[3] = 0;
	ZeroMemory(this->m_timeBuf, sizeof(this->m_timeBuf));
	this->bUpdated = false;
	this->dwLastInputTime = timeGetTime();
	this->DirectInputMouse::InitDevice(hWnd, true);
}

WProc* DirectInputMouse::ExternProc() {
	return this;
}

DirectInputMouse::~DirectInputMouse() {
	if (this->m_hEvent) {
		CloseHandle(this->m_hEvent);
		this->m_hEvent = nullptr;
	}
	if (this->m_diMouse) {
		this->m_diMouse->Unacquire();
		this->m_diMouse->Release();
		this->m_diMouse = nullptr;
	}
}

void DirectInputMouse::GetDeviceData() {
	HRESULT hResult;
	DIDEVICEOBJECTDATA dims[16];
	DWORD dwItems;

	if (!this->m_moveMode) {
		this->m_diMouse->Acquire();
	}

	do {
		dwItems = 16;
		hResult = this->m_diMouse->GetDeviceData(0x14, dims, &dwItems, 0);
		if (hResult != S_OK && hResult != DI_BUFFEROVERFLOW) {
			break;
		}
		DWORD items = dwItems;
		for (DWORD i = 0; i < dwItems; i += items) {
			DWORD bufCur = this->m_bufCur & 0xFF;
			if (items >= 0x100 - bufCur) {
				items = 0x100 - bufCur;
			}
			memcpy(&this->m_mouseBuf[bufCur], &dims[i], sizeof(DIDEVICEOBJECTDATA) * items);
			this->m_bufCur += items;
		}
	} while (hResult == DI_BUFFEROVERFLOW);
}

void DirectInputMouse::Update(unsigned long timeStamp) {
	POINT curPt;
	DIMOUSESTATE dims;

	this->bUpdated = false;
	this->m_z = 0;
	this->m_y = 0;
	this->m_x = 0;

	if (timeStamp && this->active) {
		if (this->m_diMouse) {
			int i = 0;
			do {
				++i;
				if (this->m_diMouse->GetDeviceState(sizeof(dims), &dims) == S_OK) {
					break;
				}
				ZeroMemory(&dims, sizeof(dims));
				if (this->m_moveMode == 0) {
					this->m_diMouse->Acquire();
				}
				++i;
			} while (i < 5);
			this->GetDeviceData();
			if (this->m_bufCur - this->m_passCur > 0) {
				this->FlushBuffer(timeStamp);
			}
		}
		if (this->m_moveMode) {
			GetCursorPos(&curPt);
			MoveWindow(
				this->m_hWnd,
				curPt.x - this->m_clientX,
				curPt.y - this->m_clientY,
				this->m_clientWidth,
				this->m_clientHeight,
				1
			);
		}
	} else {
		this->m_b[0] = 0;
		this->m_b[1] = 0;
		this->m_b[2] = 0;
		this->m_b[3] = 0;
	}
}

LRESULT DirectInputMouse::WinProc(UINT msg, WPARAM wParam, LPARAM lParam) {
	RECT clientRect;
	POINT pt;

	switch (msg) {
		case WM_LBUTTONUP:
			if (!this->m_moveMode) {
				break;
			}
		case WM_NCLBUTTONUP:
			this->m_moveMode = false;
			break;

		case WM_NCLBUTTONDOWN:
			if (wParam == 2) {
				this->m_moveMode = true;
				GetCursorPos(&pt);
				GetWindowRect(this->m_hWnd, &clientRect);
				this->m_clientX = pt.x - clientRect.left;
				this->m_clientY = pt.y - clientRect.top;
				this->m_clientHeight = clientRect.bottom - clientRect.top;
				this->m_clientWidth = clientRect.right - clientRect.left;
			}
			break;

		case WM_ACTIVATE:
			if (wParam) {
				if (!this->m_moveMode) {
					this->m_diMouse->Acquire();
				}
				this->GetDeviceData();
				this->ClearBuffer();
			} else {
				this->m_diMouse->Unacquire();
				this->m_moveMode = false;
				this->ClearBuffer();
			}
			return 1;

		default:
			break;
	}

	return 0;
}
