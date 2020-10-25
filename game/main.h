#pragma once
#include <Windows.h>

extern HWND g_hWnd;
extern bool g_showTime;
extern bool g_bExitDrawLoading;
extern HANDLE g_hDrawLoading;
extern HANDLE g_hInstance;
extern unsigned int g_iLoadingThreadID;
extern TCHAR g_windowName[128];
