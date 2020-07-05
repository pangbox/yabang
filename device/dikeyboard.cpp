#include "dikeyboard.h"

int DirectInputKeyboard::m_ascii2Scan[256];
int DirectInputKeyboard::m_scan2AsciiWithShift[256];
int DirectInputKeyboard::m_scan2AsciiWithoutShift[256];

DirectInputKeyboard::WConvScanCode DirectInputKeyboard::m_convList[99] = {
	{0x01, 0x1B, 0x1B},
	{0x02, 0x31, 0x21},
	{0x03, 0x32, 0x40},
	{0x04, 0x33, 0x23},
	{0x05, 0x34, 0x24},
	{0x06, 0x35, 0x25},
	{0x07, 0x36, 0x5E},
	{0x08, 0x37, 0x26},
	{0x09, 0x38, 0x2A},
	{0x0A, 0x39, 0x28},
	{0x0B, 0x30, 0x29},
	{0x0C, 0x2D, 0x5F},
	{0x0D, 0x3D, 0x2B},
	{0x0E, 0x08, 0x08},
	{0x0F, 0x09, 0x09},
	{0x10, 0x51, 0x51},
	{0x11, 0x57, 0x57},
	{0x12, 0x45, 0x45},
	{0x13, 0x52, 0x52},
	{0x14, 0x54, 0x54},
	{0x15, 0x59, 0x59},
	{0x16, 0x55, 0x55},
	{0x17, 0x49, 0x49},
	{0x18, 0x4F, 0x4F},
	{0x19, 0x50, 0x50},
	{0x1A, 0x5B, 0x7B},
	{0x1B, 0x5D, 0x7D},
	{0x1C, 0x0D, 0x0D},
	{0x1D, 0x1E, 0x1E},
	{0x1E, 0x41, 0x41},
	{0x1F, 0x53, 0x53},
	{0x20, 0x44, 0x44},
	{0x21, 0x46, 0x46},
	{0x22, 0x47, 0x47},
	{0x23, 0x48, 0x48},
	{0x24, 0x4A, 0x4A},
	{0x25, 0x4B, 0x4B},
	{0x26, 0x4C, 0x4C},
	{0x27, 0x3B, 0x3A},
	{0x28, 0x27, 0x22},
	{0x29, 0x60, 0x7E},
	{0x2A, 0x81, 0x81},
	{0x2B, 0x5C, 0x7C},
	{0x2C, 0x5A, 0x5A},
	{0x2D, 0x58, 0x58},
	{0x2E, 0x43, 0x43},
	{0x2F, 0x56, 0x56},
	{0x30, 0x42, 0x42},
	{0x31, 0x4E, 0x4E},
	{0x32, 0x4D, 0x4D},
	{0x33, 0x2C, 0x3C},
	{0x34, 0x2E, 0x3E},
	{0x35, 0x2F, 0x3F},
	{0x36, 0x82, 0x82},
	{0x38, 0x1C, 0x1C},
	{0x39, 0x20, 0x20},
	{0x3A, 0x0B, 0x0B},
	{0x3B, 0x0E, 0x0E},
	{0x3C, 0x0F, 0x0F},
	{0x3D, 0x10, 0x10},
	{0x3E, 0x11, 0x11},
	{0x3F, 0x12, 0x12},
	{0x40, 0x13, 0x13},
	{0x41, 0x14, 0x14},
	{0x42, 0x15, 0x15},
	{0x43, 0x16, 0x16},
	{0x44, 0x17, 0x17},
	{0x45, 0x83, 0x83},
	{0x46, 0x0A, 0x0A},
	{0x47, 0x85, 0x85},
	{0x48, 0x86, 0x86},
	{0x49, 0x87, 0x87},
	{0x4A, 0x88, 0x88},
	{0x4B, 0x89, 0x89},
	{0x4C, 0x8A, 0x8A},
	{0x4D, 0x8B, 0x8B},
	{0x4E, 0x8C, 0x8C},
	{0x4F, 0x8D, 0x8D},
	{0x50, 0x8E, 0x8E},
	{0x51, 0x8F, 0x8F},
	{0x52, 0x90, 0x90},
	{0x57, 0x18, 0x18},
	{0x58, 0x19, 0x19},
	{0x66, 0x66, 0x66},
	{0x9C, 0x91, 0x91},
	{0x9D, 0x1F, 0x1F},
	{0xB7, 0x84, 0x84},
	{0xB8, 0x1D, 0x1D},
	{0xC7, 0x0C, 0x0C},
	{0xC8, 0x06, 0x06},
	{0xC9, 0x02, 0x02},
	{0xCB, 0x04, 0x04},
	{0xCD, 0x05, 0x05},
	{0xCF, 0x1A, 0x1A},
	{0xD0, 0x07, 0x07},
	{0xD1, 0x03, 0x03},
	{0xD2, 0x7F, 0x7F},
	{0xD3, 0x01, 0x01},
	{0x37, 0x92, 0x92},
};

void DirectInputKeyboard::ClearBuffer() {
	this->m_bufCur = 0;
	this->m_passCur = 0;
	this->m_asciiCur = 0;
	this->m_asciiPos = 0;
	memset(this->m_asciiBuf, 0, sizeof this->m_asciiBuf);
	memset(this->m_keyState, 0, sizeof this->m_keyState);
}

void DirectInputKeyboard::FlushBuffer(DWORD timeStamp) {
	this->Reset();
	for (; this->m_passCur < this->m_bufCur; ++this->m_passCur)
	{
		if (timeStamp < this->m_keyBuf[this->m_passCur & 0xFF].dwTimeStamp) {
			break;
		}
		DIDEVICEOBJECTDATA &buf = this->m_keyBuf[this->m_passCur & 0xFF];
		int shiftedScanCode =
			this->m_keyState[DIK_LSHIFT] || this->m_keyState[DIK_RSHIFT] ?
			DirectInputKeyboard::m_scan2AsciiWithShift[buf.dwOfs] :
			DirectInputKeyboard::m_scan2AsciiWithoutShift[buf.dwOfs];
		if (shiftedScanCode && this->m_keyState[DirectInputKeyboard::m_ascii2Scan[shiftedScanCode]]) {
			this->m_asciiBuf[this->m_asciiPos++ & 0xFF] = shiftedScanCode;
			this->m_timeBuf[shiftedScanCode] = buf.dwTimeStamp;
		}
	}
	if (this->m_bufCur == this->m_passCur) {
		this->m_passCur = 0;
		this->m_bufCur = 0;
	}
}

unsigned long DirectInputKeyboard::GetEventTime(int n) {
	return this->m_timeBuf[n];
}

int DirectInputKeyboard::GetState(int sort, int n) {
	int asciiCur;
	int asciiPos;

	switch(sort) {
	case 2:
		if (n == -1 || this->m_keyState[DirectInputKeyboard::m_ascii2Scan[n]]) {
			return 1;
		}
		return 0;
	case 3:
		if (n == -1) {
			return 1;
		}
		asciiCur = this->m_asciiCur;
		asciiPos = this->m_asciiPos;
		if (asciiCur >= asciiPos) {
			return 0;
		}
		while (this->m_asciiBuf[asciiCur] != n) {
			if (++asciiCur >= asciiPos) {
				return 0;
			}
		}
		return 1;
	default:
		return 0;
	}
}

void DirectInputKeyboard::Reset() {
	this->m_asciiCur = this->m_asciiPos;
}

HRESULT DirectInputKeyboard::Acquire() const {
	return this->m_diKey->Acquire();
}

DirectInputKeyboard::~DirectInputKeyboard() {
	if (this->m_diKey) {
		this->m_diKey->Unacquire();
		this->m_diKey->Release();
		this->m_diKey = nullptr;
	}
}

WProc *DirectInputKeyboard::ExternProc() {
	return this;
}

bool DirectInputKeyboard::InitDevice(HWND hWnd, bool exclusive) {
	DIPROPDWORD prop;

	if (!this->m_diBackup) {
		return false;
	}

	if (this->m_diKey) {
		this->m_diKey->Unacquire();
		this->m_diKey->Release();
		this->m_diKey = nullptr;
	}

	if (this->m_diBackup->CreateDevice(GUID_SysKeyboard, &this->m_diKey, nullptr) != S_OK) {
		return false;
	}

	if (this->m_diKey->SetDataFormat(&c_dfDIKeyboard) != S_OK) {
		return false;
	}

	DWORD flags = DISCL_FOREGROUND | (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE);
	if (this->m_diKey->SetCooperativeLevel(hWnd, flags) != S_OK) {
		return false;
	}

	prop.diph.dwHow = 0;
	prop.diph.dwObj = 0;
	prop.diph.dwSize = sizeof(DIPROPDWORD);
	prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	prop.dwData = 64;

	if (this->m_diKey->SetProperty(DIPROP_BUFFERSIZE, reinterpret_cast<LPCDIPROPHEADER>(&prop)) != S_OK) {
		return false;
	}

	if (this->m_diKey->Acquire() != S_OK) {
		return false;
	}

	return true;
}

void DirectInputKeyboard::GetDeviceData() {
	HRESULT result;
	DIDEVICEOBJECTDATA dims[64];
	DWORD dwItems;

	this->m_diKey->Acquire();
	do {
		dwItems = 64;
		result = this->m_diKey->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), dims, &dwItems, 0);
		if (FAILED(result) && result != DI_BUFFEROVERFLOW) {
			break;
		}
		DWORD items = dwItems;
		for (DWORD i = 0; i < dwItems; i += items) {
			DWORD bufCur = this->m_bufCur & 0xFF;
			if (items >= 0x100 - bufCur) {
				items = 0x100 - bufCur;
			}
			memcpy(&this->m_keyBuf[bufCur], &dims[i], sizeof(DIDEVICEOBJECTDATA) * items);
			this->m_bufCur += items;
		}
	} while (result == DI_BUFFEROVERFLOW);
}

void DirectInputKeyboard::Update(unsigned long timeStamp) {
	if (timeStamp && this->active) {
		if (this->m_diKey) {
			for (int i = 0; i < 5; i++) {
				if (this->m_diKey->GetDeviceState(256, this->m_keyState) != ERROR_INVALID_ACCESS) {
					break;
				}
				memset(this->m_keyState, 0, 0x100);
				this->m_diKey->Acquire();
			}
			this->GetDeviceData();
			if (this->m_bufCur - this->m_passCur > 0) {
				this->FlushBuffer(timeStamp);
			}
		}
	} else {
		memset(this->m_keyState, 0, sizeof this->m_keyState);
	}
}

DirectInputKeyboard::DirectInputKeyboard(IDirectInput8A *di, HWND hWnd) {
	memset(this->m_keyState, 0, sizeof this->m_keyState);
	this->m_diKey = nullptr;
	this->m_diBackup = di;
	this->DirectInputKeyboard::InitDevice(hWnd, false);
	memset(m_scan2AsciiWithShift, 0, sizeof m_scan2AsciiWithShift);
	memset(m_scan2AsciiWithoutShift, 0, sizeof m_scan2AsciiWithoutShift);
	memset(m_ascii2Scan, 0, sizeof m_ascii2Scan);
	memset(this->m_timeBuf, 0, sizeof this->m_timeBuf);
	this->m_bufCur = 0;
	this->m_passCur = 0;
	this->m_asciiCur = 0;
	this->m_asciiPos = 0;
	memset(this->m_asciiBuf, 0, sizeof this->m_asciiBuf);
	memset(this->m_keyState, 0, sizeof this->m_keyState);
	for (auto& i : m_convList) {
		m_scan2AsciiWithShift[i.diScan] = i.asciiWithShift;
		m_scan2AsciiWithoutShift[i.diScan] = i.ascii;
		m_ascii2Scan[i.ascii] = i.diScan;
	}
}

LRESULT DirectInputKeyboard::WinProc(UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	if (message != WM_ACTIVATE) {
		return 0;
	}
	if (wParam) {
		this->m_diKey->Acquire();
		this->GetDeviceData();
	} else {
		this->m_diKey->Unacquire();
	}
	this->m_bufCur = 0;
	this->m_passCur = 0;
	this->m_asciiCur = 0;
	this->m_asciiPos = 0;
	memset(this->m_asciiBuf, 0, sizeof this->m_asciiBuf);
	memset(this->m_keyState, 0, sizeof this->m_keyState);
	return 1;
}
