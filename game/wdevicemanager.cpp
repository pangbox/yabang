#include "wdevice.h"
#include "WDeviceManager.h"
#include "winput.h"
#include "wvideo.h"

WDeviceManager::WDeviceManager(const char* directory, int loadtype) {
    this->LoadModule(directory, loadtype);
}

WDeviceManager::~WDeviceManager() {
    for (auto* device = this->m_devList.Start(); device != nullptr; device = this->m_devList.Next()) {
        delete device;
    }
    for (auto* dll = this->m_dllList.Start(); dll != nullptr; dll = this->m_dllList.Next()) {
        FreeLibrary(dll);
    }
}

void WDeviceManager::SetHWND(HWND hWnd) {
	this->m_hWnd = hWnd;
}

bool WDeviceManager::ResetVideoDevice(int bWindowed, int iWidth, int iHeight, int iColor, int lWndStyle, int fillMode) {
    WVideoDev* videoDev = this->m_vidList.Start();
    if (!videoDev) {
        return false;
    }

    WDevice* dev = this->m_devList.Start();
    if (!dev) {
        return false;
    }

    while (true) {
        if (strstr(dev->GetDeviceName(), videoDev->GetDeviceName()))
            break;
        if (strstr(videoDev->GetDeviceName(), dev->GetDeviceName()))
            break;
        dev = this->m_devList.Next();
        if (dev) {
	        continue;
        }
        videoDev = this->m_vidList.Next();
        if (!videoDev) {
	        return false;
        }
        dev = this->m_devList.Start();
    }
    videoDev->Reset(bWindowed, iWidth, iHeight, iColor, lWndStyle, fillMode);
}

const char* WDeviceManager::EnumVideoDevice() {
    static char text[512];
    int i = 0;
    for (WVideoDev* videoDev = this->m_vidList.Start(); videoDev != nullptr; videoDev = this->m_vidList.Next())
    {
        auto* devName = videoDev->GetDeviceName();
        int nameLen = strlen(devName);
        memcpy(&text[i], devName, nameLen + 1);
        i += nameLen + 1;
    }
    text[i] = 0;
    return text;
}

const char* WDeviceManager::EnumVideoMode(char* deviceName) {
    for (WVideoDev* videoDev = this->m_vidList.Start(); videoDev != nullptr; videoDev = this->m_vidList.Next()) {
        if (!_strcmpi(videoDev->GetDeviceName(), deviceName)) {
            return videoDev->EnumModeName();
        }
    }
    return nullptr;
}

void WDeviceManager::LoadModule(const char* directory, int loadtype) {
	char drive[4];
	_WIN32_FIND_DATAA fd;                         
    char path[260];                               
    char szModulePath[260];                       
    char name[128];                               
    char filename[128];                           
    char filter[128];                             
    char ext[8];                                  

    GetModuleFileNameA(nullptr, szModulePath, MAX_PATH);
    _splitpath_s(szModulePath, drive, sizeof(drive), path, sizeof(path), name, sizeof(name), ext, sizeof(ext));
    strcpy_s(filter, sizeof(filter), drive);
    strcat_s(filter, sizeof(filter), path);
    strcat_s(filter, sizeof(filter), "wangreal");
    char* cfgPart = strrchr(name, '_');
    if (cfgPart) {
        strcat_s(filter, sizeof(filter), cfgPart);
    }
	strcat(filter, ".dll");

    HANDLE hFindFile = FindFirstFileA(filter, &fd);
    if (hFindFile == INVALID_HANDLE_VALUE) {
	    FindClose(hFindFile);
		return;
    }

	do {
		if (!strstr(fd.cFileName, "wang")) {
			continue;
		}

		strcpy_s(filename, sizeof(filename), fd.cFileName);
		HMODULE module = LoadLibraryA(filename);
		if (!module) {
			continue;
		}

        WVideoDev* (*WEnumVideoDevices)(int) = nullptr;
        WInputDev* (*WEnumInputDevices)(int) = nullptr;

        if ((loadtype & 1) != 0) {
            WEnumVideoDevices = reinterpret_cast<WVideoDev * (*)(int)>(GetProcAddress(module, "WEnumVideoDevices"));
        }

        if ((loadtype & 8) != 0) {
            WEnumInputDevices = reinterpret_cast<WInputDev * (*)(int)>(GetProcAddress(module, "WEnumInputDevices"));
        }

		GetProcAddress(module, "WVersion");

		if (!WEnumVideoDevices && !WEnumInputDevices) {
			FreeLibrary(module);
			continue;
		}

		if (WEnumVideoDevices)
		{
			int i = 0;
			while (true)
			{
				WVideoDev* videoDev = WEnumVideoDevices(i);
				if (!videoDev)
					break;
				this->m_vidList.AddItem(videoDev, nullptr, false);
				++i;
			}
		}

		if (WEnumInputDevices)
		{
			int i = 0;
			while (true)
			{
				WInputDev* inputDev = WEnumInputDevices(i);
				if (!inputDev)
					break;
				this->m_inpList.AddItem(inputDev, nullptr, false);
				++i;
			}
		}
		this->m_dllList.AddItem(module, nullptr, false);
	} while (FindNextFileA(hFindFile, &fd));
	FindClose(hFindFile);
}

WVideoDev* WDeviceManager::CreateVideoDevice(const char* deviceName, const char* modeName, int iTnL) {
    WVideoDev* videoDev = this->m_vidList.Start();
    while (videoDev) {
        if (!deviceName || !_strcmpi(videoDev->GetDeviceName(), deviceName)) {
            this->m_devList.AddItem(videoDev, nullptr, false);
            return videoDev;
        }
        videoDev = this->m_vidList.Next();
    }
    return nullptr;
}

WInputDev* WDeviceManager::CreateInputDevice(const char* deviceName, const char* modeName) {
    WInputDev* inputDev = this->m_inpList.Start();
    while (inputDev) {
        if (!deviceName || !_strcmpi(inputDev->GetDeviceName(), deviceName)) {
            this->m_devList.AddItem(inputDev, nullptr, false);
            return inputDev;
        }
        inputDev = this->m_inpList.Next();
    }
    return nullptr;
}

void WDeviceManager::Release(WDevice* device) {
    this->m_devList.DelItem(device);
    delete device;
}
