#pragma once
#include <windows.h>

class CWangrealApplication {
public:
	CWangrealApplication();
	virtual ~CWangrealApplication();

	virtual int MainLoop(int unknown) = 0;
	virtual bool Ready();
	virtual LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
};
