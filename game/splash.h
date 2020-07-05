#pragma once
#include <windows.h>

#include "baseobject.h"
#include "singleton.h"

typedef BOOL(__stdcall* SetLayeredWindowAttributesProc)(HWND, COLORREF, BYTE, DWORD);

class CSplash : public BaseObject, public WSingleton<CSplash> {
public:
	CSplash();
	CSplash(LPCTSTR name, unsigned int transparentColor);
	~CSplash();

	void OnPaint(HWND hWnd) const;
	void SetText(char* str);
	int CloseSplash();
	void HideSplash() const;
	void FreeResources();
	bool MakeTransparent() const;
	HRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) const;
	unsigned int SetBitmap(HBITMAP h);
	bool SetTransparentColor(unsigned int colorRef);
	HWND RegAndCreateWindow();
	void ShowSplash();
	unsigned int SetBitmap(LPCTSTR name);
	WPARAM DoLoop();
	
private:
	void Init();
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	unsigned int m_colorRef;
	int m_bitmapW;
	int m_bitmapH;
	HBITMAP m_bitmap;
	HWND m_hWnd{};
	const TCHAR* m_className;
	char m_textStr[128];
	int m_textX;
	int m_textY;
	SetLayeredWindowAttributesProc m_setLayeredWindowAttributesProc;
};
