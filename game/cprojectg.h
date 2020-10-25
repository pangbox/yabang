#pragma once
#include <deque>
#include <string>

#include "cmainframe.h"
#include "cwangrealapplication.h"
#include "frcmdtarget.h"
#include "singleton.h"
#include "waviencoder.h"
#include "winput.h"
#include "wlist.h"

class CSoundManager;
class WMilesSoundSystem;
class WDeviceManager;
class WProcManager;
class WReceivedPacket;
class WInputDev;

extern WInputDev* g_keyboard;
extern WInputDev* g_mouse;
extern WInputDev* g_ime;

extern WMilesSoundSystem* g_miles;
extern CSoundManager* g_audio;

class CProjectG : public CMainFrame, public CWangrealApplication, public WSingleton<CProjectG>, FrCmdTarget {
public:
	explicit CProjectG(HWND hWnd);

	bool Init();
	bool SetVideoDevice();
	void AddInputDev(WInputDev* inputDev);
	bool SetInputDevice();
	bool SetAudioDevice();

	void FpuCheck();
	void SetMoveLoginServer(const char* unk1, const char* unk2);

	void Process(float time) override;
	void Draw() override;
	[[nodiscard]] uint32_t GetSystemTime() const override;
	int MainLoop(int unknown) override;
	LRESULT __stdcall WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) override;
	void SetWindowed(bool isWindowed);

protected:
	uint32_t m_unkDword = 0;
	bool m_bUnkFlag1 = false;
	bool m_bUnkFlag2 = false;
	std::string m_szUnkStr;
	std::string m_szUnkLogin1;
	std::string m_szUnkLogin2;
	WList<WInputDev*> m_inputDevices{16, 16};
	uint32_t* m_pdwUnk1 = nullptr;
	uint32_t m_mainThreadId = 0;
	uint32_t m_dwUnkFlag1 = 0;
	uint32_t m_dwUnkFlag2 = 0;
	uint32_t m_baseTime = 0;
	float m_FPS = 60.0f;
	uint32_t m_dwUnk2 = 4;
	WDeviceManager* m_deviceManager = nullptr;
	WProcManager* m_procManager = nullptr;
	WAVIEncoder m_wAviEncoder;
	std::deque<WReceivedPacket*> m_packetDeque;
	uint32_t m_fpControl = 0;
	bool m_bHidePrivacy = false;
	bool m_bHideGUI = false;
	bool m_bHidePII = false;
	uint32_t m_captureNum = 0;
	WMatrix m_unkMatrix;
};

