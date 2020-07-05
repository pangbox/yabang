#pragma once
#include <winput.h>
#include <wproc.h>
#include <dinput.h>

class DirectInputMouse : public WProc, public WInputDev {
public:
	DirectInputMouse(IDirectInput8A *di, HWND hWnd);
	~DirectInputMouse();

	bool InitDevice(HWND hWnd, bool exclusive) override;
	unsigned long GetEventTime(int n) override;
	int GetState(int sort, int n) override;
	[[nodiscard]] HRESULT Acquire() const;
	WProc *ExternProc() override;
	void GetDeviceData();
	void Update(unsigned long timeStamp) override;
	LRESULT WinProc(UINT msg, WPARAM wParam, LPARAM lParam) override;

private:
	void FlushBuffer(DWORD timeStamp);
	void ClearBuffer();

	IDirectInput8A *m_diBackup = nullptr;
	IDirectInputDevice8A *m_diMouse = nullptr;
	DIDEVICEOBJECTDATA m_mouseBuf[256]{};
	unsigned int m_timeBuf[4]{};
	int m_x{};
	int m_y{};
	int m_z{};
	int m_b[4]{};
	int m_bufCur{};
	int m_passCur{};
	void *m_hEvent = nullptr;
	HWND m_hWnd = nullptr;
	bool m_moveMode{};
	int m_clientX{};
	int m_clientY{};
	int m_clientWidth{};
	int m_clientHeight{};
};
