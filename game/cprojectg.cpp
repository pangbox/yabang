#include "cprojectg.h"

#include <algorithm>

#include "coption.h"
#include "csoundmanager.h"
#include "main.h"
#include "wdevicemanager.h"
#include "wmilessoundsystem.h"
#include "wproc.h"
#include "wresourcemanager.h"

WInputDev* g_keyboard = nullptr;
WInputDev* g_mouse = nullptr;
WInputDev* g_ime = nullptr;

WMilesSoundSystem* g_miles = nullptr;
CSoundManager* g_audio = nullptr;

CProjectG::CProjectG(HWND hWnd) {
	// TODO: Implement...
	this->m_procManager = new WProcManager();
	this->m_deviceManager = new WDeviceManager(nullptr, 9);
	g_resourceManager = new WResourceManager();
	new COption(hWnd, this->m_deviceManager, g_resourceManager);
}

bool CProjectG::Init() {
	this->m_deviceManager->SetHWND(g_hWnd);
	if (!this->SetVideoDevice() || !this->SetInputDevice()) {
		return false;
	}
	this->SetAudioDevice();
	UpdateWindow(g_hWnd);
	ShowWindow(g_hWnd, SW_SHOW);
	InvalidateRect(g_hWnd, nullptr, 0);
	UpdateWindow(g_hWnd);
	this->m_captureNum = 0;
	return true;
}

bool CProjectG::SetVideoDevice() {
    char caption[64] = { 0 };

	RECT wndRect;
    wndRect.left = 0;
    wndRect.top = 0;
    wndRect.right = 800;
    wndRect.bottom = 600;

	AdjustWindowRectEx(&wndRect, WS_CAPTION | WS_SIZEBOX, FALSE, 0);
    int centerX = (wndRect.left - wndRect.right + GetSystemMetrics(SM_CXSCREEN)) / 2;
    int centerY = (wndRect.top - wndRect.bottom + GetSystemMetrics(SM_CYSCREEN)) / 2;
    OffsetRect(&wndRect, centerX, centerY);
    MoveWindow(g_hWnd, centerX, centerY, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top, 0);
    this->m_dwUnkFlag1 |= 4u;
    WVideoDev* videoDev = this->m_deviceManager->CreateVideoDevice(nullptr, "Window", WSingleton<COption>::Instance()->vGetTnLMode());
    if (!videoDev)
    {
	    MessageBoxA(
		    g_hWnd,
		    "An error occurred due to one of the following problems:\n"
		    "\n"
		    "* DirectX 9.0c or higher is not installed, or\n"
		    "* Your display driver is out of date, or\n"
		    "* There is a problem with the performance of the display adapter.\n"
		    "\n"
		    "YaBang will now exit.",
		    caption,
		    MB_ICONERROR);
	    return false;
    }

	videoDev->SetMainThreadId(this->m_mainThreadId);
	WProc* externProc = videoDev->ExternProc();
	if (externProc) {
		externProc->SetProc(this->m_procManager, g_hWnd);
	}
	g_resourceManager->SetVideoReference(videoDev);
	double fps = videoDev->GetMonitorSupportFps();
	this->m_FPS = floor(fps);
	this->SetFPS(floor(fps));
	return true;
}

void CProjectG::AddInputDev(WInputDev* inputDev) {
	this->m_inputDevices.AddItem(inputDev, nullptr, false);
}

bool CProjectG::SetInputDevice() {
	g_keyboard = this->m_deviceManager->CreateInputDevice("DirectInput", "keyboard");
    if (g_keyboard->ExternProc()) {
        g_keyboard->ExternProc()->SetProc(this->m_procManager, g_hWnd);
    }
    this->AddInputDev(g_keyboard);

    g_mouse = this->m_deviceManager->CreateInputDevice("DirectInput", "mouse");
	if (g_mouse->ExternProc()) {
		g_mouse->ExternProc()->SetProc(this->m_procManager, g_hWnd);
	}
    this->AddInputDev(g_mouse);

    g_ime = this->m_deviceManager->CreateInputDevice("Ime", "");
	if (g_ime->ExternProc()) {
		g_ime->ExternProc()->SetProc(this->m_procManager, g_hWnd);
	}
    this->AddInputDev(g_ime);

    return true;
}

bool CProjectG::SetAudioDevice() {
    int frequency = WSingleton<COption>::Instance()->aGetMssFrequency();
    int bits = WSingleton<COption>::Instance()->aGetMssBits();
    int channels = WSingleton<COption>::Instance()->aGetMssChannels();
    bool enabled = WSingleton<COption>::Instance()->aIsMssEnabled();
    float sfxVol = WSingleton<COption>::Instance()->aGetSfxVolume();
    float bgmVol = WSingleton<COption>::Instance()->aGetBgmVolume();
    g_miles = new WMilesSoundSystem(g_hWnd, "mss", enabled, frequency, bits, channels);
    g_audio = new CSoundManager();
    if (!g_miles || !g_miles->Init(g_hWnd, frequency, bits))
    {
	    g_audio->SetMSS(nullptr);
	    return false;
    }
	g_miles->SpeakerType(static_cast<MSS_MC_SPEC>(WSingleton<COption>::Instance()->aGetMssSpeaker()));
	g_miles->SetBalance(WSingleton<COption>::Instance()->aGetMssBalance());
	g_miles->SetDistLimit(128.0, 960.0);
	g_audio->SetMSS(g_miles);
	if (enabled)
	{
		g_audio->SetVolume(std::clamp<float>(sfxVol, 0.0, 1.0));
		g_audio->SetBGMVolume(std::clamp<float>(bgmVol, 0.0, 1.0));
	}
	else
	{
		g_audio->SetVolume(0.0);
		g_audio->SetBGMVolume(0.0);
	}
	return true;
}

void CProjectG::FpuCheck() {
    this->m_fpControl = _controlfp(0, 0);
    if (this->m_fpControl != (PC_24 | MCW_EM)) {
        this->m_fpControl = _controlfp(PC_24, MCW_PC);
    }
}

void CProjectG::SetMoveLoginServer(const char* unk1, const char* unk2) {
    this->m_bUnkFlag2 = true;
    this->m_szUnkLogin1 = unk1;
    this->m_szUnkLogin2 = unk2;
}

void CProjectG::Process(float time) {
	// TODO: Implement...
	abort();
}

void CProjectG::Draw() {
	// TODO: Implement...
	abort();
}

uint32_t CProjectG::GetSystemTime() const {
	// TODO: Implement...
	abort();
}

int CProjectG::MainLoop(int unknown) {
	// TODO: Implement...
	abort();
}

LRESULT CProjectG::WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	// TODO: Implement...
	abort();
}

void CProjectG::SetWindowed(bool isWindowed) {
	if (isWindowed) {
		this->m_dwUnkFlag1 |= 0x4;
	} else {
		this->m_dwUnkFlag1 &= ~0x4;
	}
}
