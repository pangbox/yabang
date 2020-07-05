#pragma once
#include <winput.h>
#include <wproc.h>
#include <imm.h>

class WIme : public WInputDev, public WProc {
public:
	WIme();
	WIme(HWND hwnd);
	~WIme();

	char *GetTextComp();
	int GetState(int code, int n) override;
	void SetAlphaNumericMode(bool alnum);
	bool IsAlphaNumericMode() override;
	void Reset() override;
	void Update(unsigned long timeStamp) override;
	const char *GetDeviceName() override;
	WProc *ExternProc() override;
	void SetActive(bool stat) override;
	LRESULT WinProc(UINT msg, WPARAM wparam, LPARAM lparam) override;
	WInputDev *MakeClone(char *modeName, HWND hwnd) override;

protected:
	void OnStartComposition(DWORD dwCommand, long dwData);
	void OnSetContext(DWORD dwCommand, long dwData);
	void OnEndComposition(DWORD dwCommand, long dwData);
	void OnCompositionFull(DWORD dwCommand, long dwData);
	int OnNotify(DWORD dwCommand, long dwData);
	void OnControl(DWORD dwCommand, long dwData);
	int GetResultString();
	int GetCompString(long flag);
	virtual void ProcessResultString(char *str);
	virtual void ProcessCompString(char *str, char *strattr);
	void OnInputLangChange(DWORD dwCommand, long dwData);
	void OnComposition(DWORD dwCommand, long dwData);
	void OnChar(unsigned int nChar);

private:
	int IME_Enter();
	void Leave();
	int Check();
	void ClearData();
	void PutString(char *str);
	void InitIme(HWND hwnd_);

	DWORD m_property;
	HKL m_hKeyLayout;
	int m_nState;
	int m_nCompLen;
	HWND m_hwndCand[32];
	CANDIDATELIST *m_candList[32];
	int m_charWidth;
	int m_charHeight;
	bool m_bActive;
	HIMC m_hIMC;
	HWND m_hWnd;
	char m_TextComp[3];
	DWORD m_dwConvMode;
	char *buff;
	int current;
	int old_current;
	char ringBuf[1024];
	int pos;
};
