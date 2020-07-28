#pragma once
#include <deque>
#include <string>

#include "cmainframe.h"
#include "cwangrealapplication.h"
#include "frcmdtarget.h"
#include "singleton.h"
#include "waviencoder.h"
#include "wlist.h"

class WDeviceManager;
class WProcManager;
class WReceivedPacket;

class CProjectG : public CMainFrame, public CWangrealApplication, public WSingleton<CProjectG>, FrCmdTarget {
public:

protected:
	DWORD m_unkDword = 0;
	bool m_bUnkFlag1 = false;
	bool m_bUnkFlag2 = false;
	std::string m_szUnkStr;
	std::string m_szUnkLogin1;
	std::string m_szUnkLogin2;
	WList<int> m_inputDevices{16, 16};
	DWORD* m_pdwUnk1 = nullptr;
	DWORD m_mainThreadId = 0;
	DWORD m_dwUnkFlag1 = 0;
	DWORD m_dwUnkFlag2 = 0;
	DWORD m_baseTime = 0;
	float m_FPS = 60.0f;
	DWORD m_dwUnk2 = 4;
	WDeviceManager* m_deviceManager = nullptr;
	WProcManager* m_procManager = nullptr;
	WAVIEncoder m_wAviEncoder;
	std::deque<WReceivedPacket*> m_packetDeque;
	DWORD m_fpControl;
	bool m_bHidePrivacy;
	bool m_bHideGUI;
	bool m_bHidePII;
	DWORD m_captureNum;
	WMatrix m_unkMatrix;
};

