#include "splash.h"

CSplash::CSplash() {
	Init();
}

CSplash::CSplash(LPCTSTR name, unsigned transparentColor) {
	Init();
	SetBitmap(name);
	SetTransparentColor(transparentColor);
}

CSplash::~CSplash() {
	FreeResources();
}

void CSplash::Init() {
	this->m_hWnd = nullptr;
	this->m_className = TEXT("SPLASH");
	this->m_colorRef = 0;
	this->m_bitmapH = 0;
	this->m_bitmapW = 0;
	strcpy_s(this->m_textStr, sizeof(this->m_textStr), "Initializing....");
	this->m_textX = 10;
	this->m_textY = 100;
	HMODULE user32 = GetModuleHandle(TEXT("USER32.DLL"));
	if (user32) {
		this->m_setLayeredWindowAttributesProc = reinterpret_cast<SetLayeredWindowAttributesProc>(GetProcAddress(
			user32, "SetLayeredWindowAttributes"));
	} else {
		this->m_setLayeredWindowAttributesProc = nullptr;
	}
}

void CSplash::OnPaint(HWND hWnd) const {
	RECT rect;
	PAINTSTRUCT paint;

	HDC hdc = BeginPaint(hWnd, &paint);
	if (this->m_bitmap) {
		GetClientRect(m_hWnd, &rect);
		HDC hdcSrc = CreateCompatibleDC(hdc);
		HGDIOBJ prevBitmap = SelectObject(hdcSrc, this->m_bitmap);
		BitBlt(hdc, 0, 0, this->m_bitmapW, this->m_bitmapH, hdcSrc, 0, 0, SRCCOPY);
		SelectObject(hdcSrc, prevBitmap);
		DeleteDC(hdcSrc);
	} else {
		SetBkMode(hdc, 1);
		TextOutA(hdc, this->m_textX, this->m_textY, this->m_textStr, strlen(this->m_textStr));
	}
	EndPaint(hWnd, &paint);
}

void CSplash::SetText(char* str) {
	strcpy_s(this->m_textStr, sizeof(this->m_textStr), str);
	OnPaint(this->m_hWnd);
	InvalidateRect(this->m_hWnd, nullptr, FALSE);
}

int CSplash::CloseSplash() {
	ShowWindow(this->m_hWnd, SW_HIDE);
	if (!this->m_hWnd) {
		return 0;
	}
	DestroyWindow(this->m_hWnd);
	this->m_hWnd = nullptr;
	UnregisterClass(this->m_className, GetModuleHandleA(nullptr));
	return 1;
}

void CSplash::HideSplash() const {
	ShowWindow(this->m_hWnd, SW_HIDE);
}

void CSplash::FreeResources() {
	if (this->m_bitmap) {
		DeleteObject(this->m_bitmap);
	}
	this->m_bitmap = nullptr;
}

bool CSplash::MakeTransparent() const {
	if (this->m_hWnd && this->m_setLayeredWindowAttributesProc && this->m_colorRef) {
		LONG exStyle = GetWindowLongA(this->m_hWnd, GWL_EXSTYLE);
		SetWindowLongA(this->m_hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
		this->m_setLayeredWindowAttributesProc(this->m_hWnd, this->m_colorRef, 0, 1);
	}
	return true;
}

LRESULT CSplash::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const {
	if (msg == WM_PAINT) {
		OnPaint(hWnd);
		return 0;
	}
	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

unsigned int CSplash::SetBitmap(HBITMAP h) {
	BITMAP bmi;
	if (this->m_bitmap) {
		DeleteObject(this->m_bitmap);
	}
	this->m_bitmap = nullptr;
	if (!h) {
		return 1;
	}
	this->m_bitmap = h;
	if (GetObjectA(h, sizeof(bmi), &bmi)) {
		this->m_bitmapW = bmi.bmWidth;
		this->m_bitmapH = bmi.bmHeight;
		return 1;
	}
	if (this->m_bitmap) {
		DeleteObject(this->m_bitmap);
	}
	this->m_bitmap = nullptr;
	return 0;
}

bool CSplash::SetTransparentColor(unsigned int colorRef) {
	this->m_colorRef = colorRef;
	if (this->m_hWnd && this->m_setLayeredWindowAttributesProc && this->m_colorRef) {
		LONG exStyle = GetWindowLongA(this->m_hWnd, GWL_EXSTYLE);
		SetWindowLongA(this->m_hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
		this->m_setLayeredWindowAttributesProc(this->m_hWnd, this->m_colorRef, 0, 1);
	}
	return true;
}

LRESULT CALLBACK CSplash::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static CSplash* splash;

	if (msg == WM_CREATE) {
		splash = *reinterpret_cast<CSplash**>(lParam);
	}
	if (!splash || msg != WM_PAINT) {
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	splash->OnPaint(hWnd);
	return 0;
}

HWND CSplash::RegAndCreateWindow() {
	WNDCLASSEX wndClass{};
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = 0x3000;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 30;
	wndClass.hInstance = GetModuleHandle(nullptr);
	wndClass.hIcon = nullptr;
	wndClass.hCursor = LoadCursor(nullptr, IDC_WAIT);
	wndClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(1));
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = this->m_className;
	wndClass.hIconSm = nullptr;

	if (!RegisterClassEx(&wndClass)) {
		return nullptr;
	}

	int screenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYFULLSCREEN);
	if (!this->m_bitmapH && !this->m_bitmapW) {
		this->m_bitmapW = 480;
		this->m_bitmapH = 360;
	}

	HWND hWnd = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		this->m_className,
		nullptr,
		WS_POPUP,
		static_cast<int>(screenWidth - this->m_bitmapW) / 2,
		static_cast<int>(screenHeight - this->m_bitmapH) / 2,
		this->m_bitmapW,
		this->m_bitmapH,
		nullptr,
		nullptr,
		nullptr,
		this);
	this->m_hWnd = hWnd;
	if (hWnd) {
		MakeTransparent();
		ShowWindow(this->m_hWnd, SW_SHOW);
		UpdateWindow(this->m_hWnd);
	}
	return this->m_hWnd;
}

void CSplash::ShowSplash() {
	ShowWindow(this->m_hWnd, SW_HIDE);
	if (this->m_hWnd) {
		DestroyWindow(this->m_hWnd);
		this->m_hWnd = nullptr;
		UnregisterClass(this->m_className, GetModuleHandle(nullptr));
	}
	RegAndCreateWindow();
	strcpy_s(this->m_textStr, sizeof(this->m_textStr), "");
	OnPaint(this->m_hWnd);
	InvalidateRect(this->m_hWnd, nullptr, 0);
	ShowWindow(this->m_hWnd, SW_SHOW);
}

unsigned int CSplash::SetBitmap(LPCTSTR name) {
	return SetBitmap(static_cast<HBITMAP>(LoadImage(nullptr, name, 0, 0, 0, 0x10)));
}

WPARAM CSplash::DoLoop() {
	MSG msg;
	if (!this->m_hWnd) {
		ShowSplash();
	}
	while (GetMessageA(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}
	return msg.wParam;
}
