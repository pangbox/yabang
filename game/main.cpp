#include "main.h"
#include <atlbase.h>
#include <atlwin.h>
#include <commctrl.h>
#include <tchar.h>


#include "cmousecursor.h"
#include "coption.h"
#include "cprojectg.h"
#include "splash.h"
#include "resource.h"
#include "tikimagicboxdoc.h"
#include "wlist.h"

constexpr TCHAR g_defaultWindowName[] = TEXT("YaBang!");
constexpr TCHAR g_className[] = TEXT("PangYa");
HWND g_hWnd = nullptr;
bool g_showTime = false;
bool g_bExitDrawLoading = false;
HANDLE g_hDrawLoading = nullptr;
HANDLE g_hInstance = nullptr;
unsigned int g_iLoadingThreadID;
TCHAR g_windowName[128] = { 0 };
WMemFillBlock g_mem{0x4800, 5};

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

bool CheckRelatedDll(HWND hWnd) {
	HMODULE directInput = LoadLibrary(TEXT("dinput8.dll"));
	if (!directInput) {
		MessageBox(hWnd, TEXT("A DirectX library is missing. Please re-install DirectX 9."), TEXT("YaBang"),
		           MB_OK | MB_ICONERROR);
		return false;
	}
	FreeLibrary(directInput);
	return true;
}

void SetWindowTitle(HWND hWnd) {
	_tcscpy_s(g_windowName, _countof(g_windowName), g_defaultWindowName);
	SetWindowText(hWnd, g_windowName);
}

unsigned int __stdcall DrawLoadingByThread(void* unused) {
	// TODO: implement.
	WaitForSingleObject(GetCurrentThread(), INFINITE);

	return 0;
}

int gameInit(HWND hWnd) {
	MSG Msg;
	RECT Rect;
	int result = -1;

	new CProjectG(hWnd);
	new CTikiMagicBoxDoc();

	if (WSingleton<CProjectG>::Instance()->Init()) {
		if (!g_hDrawLoading && !g_iLoadingThreadID) {
			g_hDrawLoading = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, DrawLoadingByThread, nullptr, 0, &g_iLoadingThreadID));
		}
		if (WSingleton<CSplash>::Instance()) {
			WSingleton<CSplash>::Instance()->HideSplash();
		}
		if (WSingleton<CProjectG>::Instance()->Ready()) {
			WSingleton<COption>::Instance()->vApplyLobbyScreenSize();
			// TODO: fix
			//WSingleton<CMouseCursor>::Instance()->m_mode = 0;
			// TODO: g_input requires CProjectG::Ready.
			//g_input->SetMousePointToLoginBox();
			// TODO: requires CBrowser
			//new CBrowser();
			GetClientRect(hWnd, &Rect);
			// TODO: requires CBrowser
			//WSingleton<CBrowser>::Instance()->Init(g_hInstance, hWnd, &Rect);
			WaitForSingleObject(g_hDrawLoading, 90 * 1000);
			if (WSingleton<CSplash>::Instance()) {
				WSingleton<CSplash>::Instance()->CloseSplash();
				delete WSingleton<CSplash>::Instance();
			}
			g_bExitDrawLoading = true;
			while (true) {
				while (!PeekMessage(&Msg, nullptr, 0, 0, 0)) {
					result = WSingleton<CProjectG>::Instance()->MainLoop(0);
					if (result > 0) {
						break;
					}
				}
				if (result > 0) {
					break;
				}
				if (!GetMessageA(&Msg, 0, 0, 0)) break;
				// TODO: requires CBrowser
				//if (!WSingleton<CBrowser>::Instance() || !WSingleton<CBrowser>::Instance()->PreTranslateMessage(&Msg)) {
				TranslateMessage(&Msg);
				DispatchMessageA(&Msg);
				//}
			}
			// TODO: requires CBrowser
			//if (WSingleton<CBrowser>::m_pInstance)
			//	delete WSingleton<CBrowser>::m_pInstance;
		}
	}
	g_bExitDrawLoading = true;
	// TODO: requires CLoginInfo.
	//result = CLoginInfo::Instance()->Destroy();
	if (WSingleton<CTikiMagicBoxDoc>::Instance())
		delete WSingleton<CTikiMagicBoxDoc>::Instance();
	if (WSingleton<CProjectG>::Instance())
		delete WSingleton<CProjectG>::Instance();
	return result;
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

	if (FAILED(CoInitialize(nullptr))) {
		MessageBox(nullptr, TEXT("Failed to initialize COM apartment."), TEXT("YaBang"), MB_OK | MB_ICONERROR);
	}

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
		wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMEICON));
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(4));
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = g_className;
		if (RegisterClass(&wndClass)) {
			RECT rect;
			rect.left = SM_CXSCREEN;
			rect.top = SM_CXSCREEN;
			rect.right = 800;
			rect.bottom = 600;
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
			if (CheckRelatedDll(hWnd)) {
				g_hWnd = hWnd;
				SetWindowTitle(hWnd);
				gameInit(hWnd);
				MessageBox(nullptr, TEXT("Unimplemented."), TEXT("YaBang"), MB_OK | MB_ICONERROR);
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
