#pragma once
#include <Windows.h>
#include "wlist.h"

class WVideoDev;
class WAudioDev;
class WInputDev;
class WDevice;

class WDeviceManager {
public:
	WDeviceManager(const char* directory, int loadtype);
	~WDeviceManager();
	void SetHWND(HWND hWnd);
	bool ResetVideoDevice(int bWindowed, int iWidth, int iHeight, int iColor, int lWndStyle, int fillMode);
	const char* EnumVideoDevice();
	const char* EnumVideoMode(char* deviceName);
	void LoadModule(const char* directory, int loadtype);
	WVideoDev* CreateVideoDevice(const char* deviceName, const char* modeName, int iTnL);
	WInputDev* CreateInputDevice(const char* deviceName, const char* modeName);
	void Release(WDevice* device);

protected:
	WList<WVideoDev*> m_vidList{8, 0};
	WList<WAudioDev*> m_audList{8, 0};
	WList<WInputDev*> m_inpList{8, 0};
	WList<HINSTANCE> m_dllList{8, 0};
	WList<WDevice*> m_devList{8, 0};
	HWND m_hWnd = nullptr;
};
