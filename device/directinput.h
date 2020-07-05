#pragma once
#include <winput.h>
#include <dinput.h>

class WDirectInput : public WInputDev {
public:
	WDirectInput();
	~WDirectInput();

	WInputDev *MakeClone(char *modeName, HWND hWnd) override;
	const char *GetDeviceName() override;
	const char *EnumModeName() override;

private:
	IDirectInput8A *m_di;
};
