#include <wdevice.h>
#include <winput.h>
#include <cassert>
#include <wlist.h>

#include "directinput.h"
#include "interop.h"
#include "pangyaver.h"
#include "wd3d8.h"
#include "wime.h"

WList<WVideoDev*> g_vidList(10, 10);
WList<WInputDev*> g_inpList(10, 10);

WMemFillBlock g_mem{0x4800, 5};

const char* const g_msgD3DInitFailed = "Direct3D 9";
const char* g_error;
bool g_formatChanged = false;

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason != DLL_PROCESS_ATTACH) {
		return TRUE;
	}

	return TRUE;
}

void AddInputDevice(WInputDev* inputDev) {
	g_inpList += inputDev;
}

void AddVideoDevice(WVideoDev* videoDev) {
	g_vidList += videoDev;
}

void EnumDirect3D8() {
	D3DCAPS9 d3dCaps9;
	D3DADAPTER_IDENTIFIER9 identifier;

	auto pyVer = ScanPangYaVersion();
	TRACE("PangYa Version: %s", pyVer);

	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d9) {
		g_error = g_msgD3DInitFailed;
		return;
	}

	for (UINT i = 0; i < d3d9->GetAdapterCount(); i++) {
		memset(&identifier, 0, sizeof identifier);
		if (d3d9->GetAdapterIdentifier(i, 0, &identifier) != S_OK || d3d9->GetDeviceCaps(i, D3DDEVTYPE_HAL, &d3dCaps9)
			!= S_OK) {
			continue;
		}
		auto* d3dDevice = new WDirect3D8(identifier.Description, i);
		if (pyVer == "645.00") {
			d3dDevice = reinterpret_cast<WDirect3D8*>(new WDirect3D8KR645(d3dDevice));
		} else if (pyVer == "852.00") {
			d3dDevice = reinterpret_cast<WDirect3D8*>(new WDirect3D8US852(d3dDevice));
		}
		AddVideoDevice(d3dDevice);
	}

	d3d9->Release();
}

WVideoDev* WEnumVideoDevices(int iIndex) {
	WVideoDev* it = g_vidList.Start();
	if (!it) {
		EnumDirect3D8();
		it = g_vidList.Start();
	}

	if (!it) {
		return nullptr;
	}

	for (int i = 0; i < iIndex; i++) {
		it = g_vidList.Next();
		if (!it) {
			return nullptr;
		}
	}

	return it;
}

WInputDev* WEnumInputDevices(int iIndex) {
	WInputDev* it = g_inpList.Start();
	if (!it) {
		AddInputDevice(new WDirectInput());
		AddInputDevice(new WIme());
		it = g_inpList.Start();
	}

	if (!it) {
		return nullptr;
	}

	for (int i = 0; i < iIndex; i++) {
		it = g_inpList.Next();
		if (!it) {
			return nullptr;
		}
	}

	return it;
}

const char* WVersion() {
	return "Sep 30 2014";
}

const char* WDeviceName() {
	return "DIrectX8";
}
