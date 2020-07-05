#include <windows.h>
#include <atlbase.h>
#include <atlwin.h>
#include <commctrl.h>

#include "splash.h"

constexpr TCHAR g_className[] = TEXT("PangYa");
HWND g_hWnd = nullptr;
bool g_showTime = false;

void ShowSplash(bool bShow) {
	if (bShow) {
		new CSplash();
		WSingleton<CSplash>::Instance()->SetBitmap(TEXT("PangYaSetup.bmp"));
		WSingleton<CSplash>::Instance()->SetTransparentColor(0x00FF00);
		WSingleton<CSplash>::Instance()->ShowSplash();
	} else if (WSingleton<CSplash>::Instance()) {
		WSingleton<CSplash>::Instance()->CloseSplash();
		delete WSingleton<CSplash>::Instance();
	}
}

HANDLE WINAPI GetPangYaMutex() {
	HANDLE mutex = CreateMutexA(nullptr, 1, "{071784A2-EE35-4e6a-92D0-6E7A4B985171}");
	if (mutex) {
		DWORD err = GetLastError();
		if (err != ERROR_ALREADY_EXISTS && err != ERROR_ACCESS_DENIED) {
			return mutex;
		}
		HWND hPangYaWnd = FindWindow(g_className, nullptr);
		if (hPangYaWnd) {
			SetForegroundWindow(hPangYaWnd);
			SetActiveWindow(hPangYaWnd);
		}
	}
	ShowSplash(false);
	return nullptr;
}

bool CheckWndVersion() {
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	// ReSharper disable once CppDeprecatedEntity
	GetVersionEx(&versionInfo);
	return versionInfo.dwPlatformId && (versionInfo.dwMajorVersion > 4 || versionInfo.dwMajorVersion == 4 && versionInfo.dwMinorVersion);
}

bool CheckRelatedDLL(HWND hWnd) {
	HMODULE directInput = LoadLibrary(TEXT("dinput8.dll"));
	if (!directInput) {
		MessageBox(hWnd, TEXT("DirectX 관련 파일을 찾을 수 없습니다."), TEXT("팡야"), 0);
		return false;
	}
	FreeLibrary(directInput);
	return true;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 1;
	case WM_SIZE:
		g_showTime = wParam != SIZE_MAXHIDE && wParam != SIZE_MINIMIZED;
		break;
	case WM_ERASEBKGND:
		return 0;
	case WM_SYSCOMMAND:
		if (wParam != SC_RESTORE && wParam != SC_CLOSE) {
			return 0;
		}
		break;
	case WM_USER:
		DestroyWindow(hWnd);
		break;
	default:
		break;
	}

	// TODO: forward to CProjectG WndProc.
	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

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

	HANDLE globalMutex = GetPangYaMutex();
	if (globalMutex) {
		ShowSplash(true);
		WNDCLASS wndClass;
		wndClass.style = 0;
		wndClass.lpfnWndProc = WinProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // LoadIcon(hInstance, (LPCSTR)0x65);
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(4));
		wndClass.lpszMenuName = 0;
		wndClass.lpszClassName = g_className;
		if (RegisterClass(&wndClass)) {
			if (CheckWndVersion()) {
				RECT rect;
				AdjustWindowRectEx(&rect, 0xC40000u, 0, 0);
				OffsetRect(
					&rect,
					(rect.left - rect.right + GetSystemMetrics(SM_CXSCREEN)) / 2,
					(rect.top - rect.bottom + GetSystemMetrics(SM_CYSCREEN)) / 2);
				HWND hWnd = CreateWindowEx(
					0,
					g_className,
					TEXT("YaBang"),
					WS_CAPTION | WS_SIZEBOX,
					(rect.left - rect.right + GetSystemMetrics(SM_CXSCREEN)) / 2,
					(rect.top - rect.bottom + GetSystemMetrics(SM_CYSCREEN)) / 2,
					rect.right - rect.left,
					rect.bottom - rect.top,
					nullptr,
					nullptr,
					hInstance,
					nullptr);
				ShowWindow(hWnd, SW_HIDE);
				if (CheckRelatedDLL(hWnd))
				{
					g_hWnd = hWnd;
					MessageBox(nullptr, TEXT("Unimplemented."), TEXT("YaBang"), MB_OK);
				}
			} else {
				MessageBox(nullptr, TEXT("Windows 98 or Windows 2000 or newer operating system required."), TEXT("YaBang"), MB_OK);
			}
		}
	}

	ShowSplash(false);
	CoUninitialize();
	if (globalMutex) {
		ReleaseMutex(globalMutex);
		CloseHandle(globalMutex);
	}

	return 0;
}