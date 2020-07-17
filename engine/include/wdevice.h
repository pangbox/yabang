#pragma once
#include "wproc.h"

extern const char* const g_msgD3DInitFailed;
extern const char* g_error;
extern bool g_formatChanged;

class WVideoDev;
class WInputDev;

WVideoDev* WEnumVideoDevices(int iIndex);
WInputDev* WEnumInputDevices(int iIndex);
const char* WVersion();
const char* WDeviceName();

class WDevice {
public:
	virtual ~WDevice() {}

	virtual const char* GetDeviceName() {
		return nullptr;
	}

	virtual const char* EnumModeName() {
		return "";
	}

	virtual WProc* ExternProc() {
		return nullptr;
	}
};
