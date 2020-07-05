#include <windows.h>
#include <atlbase.h>
#include <atlwin.h>
#include <commctrl.h>

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	CoInitialize(nullptr);

	INITCOMMONCONTROLSEX initCommonControlsEx{};
	initCommonControlsEx.dwSize = sizeof(initCommonControlsEx);
	initCommonControlsEx.dwICC = ICC_BAR_CLASSES | ICC_COOL_CLASSES;
	InitCommonControlsEx(&initCommonControlsEx);

	AtlAxWinInit();
	
	return 0;
}