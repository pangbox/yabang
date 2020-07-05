#pragma once
#include <winput.h>
#include <dinput.h>

class DirectInputKeyboard : public WInputDev, public WProc {
public:
	DirectInputKeyboard(IDirectInput8A *di, HWND hWnd);
	~DirectInputKeyboard();

	unsigned long GetEventTime(int n) override;
	int GetState(int sort, int n) override;
	void Reset() override;
	WProc *ExternProc() override;
	bool InitDevice(HWND hWnd, bool exclusive) override;
	void Update(unsigned long timeStamp) override;
	LRESULT WinProc(UINT message, WPARAM wParam, LPARAM lParam) override;

private:
	void ClearBuffer();
	void FlushBuffer(DWORD timestamp);
	[[nodiscard]] long Acquire() const;
	void GetDeviceData();

	IDirectInput8A *m_diBackup;
	IDirectInputDevice8A *m_diKey;
	DIDEVICEOBJECTDATA m_keyBuf[256]{};
	unsigned int m_timeBuf[256]{};
	int m_asciiBuf[256]{};
	int m_asciiPos;
	int m_asciiCur;
	int m_bufCur;
	int m_passCur;
	char m_keyState[256]{};

	struct WConvScanCode {
		int diScan;
		int ascii;
		int asciiWithShift;
	};

	static int m_ascii2Scan[256];
	static int m_scan2AsciiWithShift[256];
	static int m_scan2AsciiWithoutShift[256];
	static WConvScanCode m_convList[99];
};
