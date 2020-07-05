#include "wime.h"

#include <cassert>

#include "wutil.h"

char *WIme::GetTextComp() {
	return this->m_TextComp;
}

int WIme::IME_Enter() {
	if (!this->m_hWnd) {
		return 0;
	}
	this->m_hIMC = ImmGetContext(this->m_hWnd);
	return this->m_hIMC != nullptr;
}

void WIme::Leave() {
	ImmReleaseContext(this->m_hWnd, this->m_hIMC);
}

int WIme::Check() {
	return !(this->m_property & IME_PROP_SPECIAL_UI) && (this->m_property & IME_PROP_AT_CARET);
}

void WIme::ClearData() {
	for (int i = 0; i < 32; i++) {
		this->m_hwndCand[i] = nullptr;
		this->m_candList[i] = nullptr;
	}

	this->m_nState = 0;
	this->m_nCompLen = 0;
	this->old_current = 0;
	this->current = 0;
	this->pos = 0;
}

void WIme::OnStartComposition(DWORD dwCommand, long dwData) {
	if (!(this->m_property & IME_PROP_SPECIAL_UI) && (this->m_property & IME_PROP_AT_CARET)) {
		this->m_nState |= 1u;
		this->m_nCompLen = 0;
		this->current = 0;
		this->pos = 0;
	}
}

void WIme::OnSetContext(DWORD dwCommand, long dwData) {
	if (this->m_property & IME_PROP_AT_CARET) {
		dwData &= ~0x80000001;
	}

	DefWindowProcA(this->m_hWnd, WM_IME_SETCONTEXT, dwCommand, dwData);
}

void WIme::OnEndComposition(DWORD dwCommand, long dwData) {
	if (!(this->m_property & IME_PROP_SPECIAL_UI) && (this->m_property & IME_PROP_AT_CARET)) {
		this->m_nState &= ~1;
		this->m_nCompLen = 0;
		this->m_TextComp[0] = 0;
		this->m_TextComp[1] = 0;
		this->m_TextComp[2] = 0;
	}
}

void WIme::OnCompositionFull(DWORD dwCommand, long dwData) {
	return;
}

int WIme::OnNotify(DWORD dwCommand, long dwData) {
	switch (dwCommand) {
	case 3:
	case 4:
	case 5:
		return 1;

	case 8:
		this->IME_Enter();
		ImmGetConversionStatus(this->m_hIMC, &this->m_dwConvMode, &dwCommand);
		ImmReleaseContext(this->m_hWnd, this->m_hIMC);
		return 1;

	default:
		return 0;
	}
}

void WIme::OnControl(DWORD dwCommand, long dwData) {
	return;
}

void WIme::PutString(char *str) {
	int len = strlen(str);
	for (int i = 0; i < len; ++i) {
		this->ringBuf[this->current++ & 0x3FF] = str[i];
	}
}

int WIme::GetResultString() {
	static char str[0x80];
	DWORD dwBuf;

	if (this->m_hWnd) {
		this->m_hIMC = ImmGetContext(this->m_hWnd);
		this->m_hIMC = m_hIMC;
		if (this->m_hIMC) {
			dwBuf = ImmGetCompositionStringA(this->m_hIMC, GCS_RESULTSTR, nullptr, 0);
			if (dwBuf) {
				if (dwBuf > 0x7F)
					dwBuf = 0x7F;
				ImmGetCompositionStringA(this->m_hIMC, GCS_RESULTSTR, str, dwBuf);
				str[dwBuf] = 0;
				this->ProcessResultString(str);
			}
			ImmReleaseContext(this->m_hWnd, this->m_hIMC);
		}
	}
	return 1;
}

int WIme::GetCompString(long flag) {
	static char str[0x80];
	static char strAttr[0x80];
	
	DWORD dwBuf;
	DWORD dwAttr;

	if (!this->m_hWnd) {
		return 0;
	}

	this->m_hIMC = ImmGetContext(this->m_hWnd);
	if (!this->m_hIMC) {
		return 0;
	}

	dwBuf = ImmGetCompositionStringA(this->m_hIMC, GCS_COMPSTR, nullptr, 0);
	if (dwBuf) {
		ImmGetCompositionStringA(this->m_hIMC, GCS_COMPSTR, str, dwBuf);
		str[dwBuf] = 0;
		if (flag & GCS_COMPATTR) {
			dwAttr = ImmGetCompositionStringA(this->m_hIMC, GCS_COMPATTR, nullptr, 0);
			if (dwAttr) {
				ImmGetCompositionStringA(this->m_hIMC, GCS_COMPATTR, strAttr, dwAttr);
				strAttr[dwAttr] = 0;
			}
		}
		this->ProcessCompString(str, strAttr);
		this->m_nCompLen = dwBuf;
	}
	ImmReleaseContext(this->m_hWnd, this->m_hIMC);

	return 1;
}

void WIme::ProcessCompString(char *str, char *strattr) {
	if (lstrlenA(str) <= 2) {
		strcpy_s(this->m_TextComp, 3, str);
	}
}

void WIme::ProcessResultString(char *str) {
	this->PutString(str);
}

int WIme::GetState(int sort, int n) {
	int result;

	switch (sort) {
	default:
		return 0;
	case 3:
		switch (n) {
		case -1:
			return 1;
		case -2:
			result = 0;
			if (!(this->m_property & IME_PROP_SPECIAL_UI) && (this->m_property & IME_PROP_AT_CARET)) {
				strcpy_s(reinterpret_cast<char*>(&result), 4, this->m_TextComp);
			}
			return result;
		default:
			if (this->pos < this->current) {
				result = BYTE(this->ringBuf[this->pos++ & 0x3FF]);
				if (result < 0x80) {
					return result;
				}
				result |= BYTE(this->ringBuf[this->pos++ & 0x3FF]);
				return result;
			}
			return 0;
		}
	}
}

void WIme::SetAlphaNumericMode(bool alnum) {
	DWORD dwSenMode;

	if (this->m_hIMC) {
		ImmGetConversionStatus(this->m_hIMC, &this->m_dwConvMode, &dwSenMode);
		ImmSetConversionStatus(this->m_hIMC, alnum == 0, dwSenMode);
	} else {
		if (this->m_hWnd) {
			this->m_hIMC = ImmGetContext(this->m_hWnd);
		}

		if (this->m_hIMC == nullptr) {
			TRACE("ImmGetContext returned null.");
			return;
		}

		ImmGetConversionStatus(this->m_hIMC, &this->m_dwConvMode, &dwSenMode);
		ImmSetConversionStatus(this->m_hIMC, alnum == 0, dwSenMode);
		ImmReleaseContext(this->m_hWnd, this->m_hIMC);
	}
}

bool WIme::IsAlphaNumericMode() {
	DWORD dwSenMode;

	if (!this->m_hIMC) {
		if (this->m_hWnd) {
			this->m_hIMC = ImmGetContext(this->m_hWnd);
		}

		if (this->m_hIMC == nullptr) {
			TRACE("ImmGetContext returned null.");
			return false;
		}

		ImmGetConversionStatus(this->m_hIMC, &this->m_dwConvMode, &dwSenMode);
		ImmReleaseContext(this->m_hWnd, this->m_hIMC);
	}

	return this->m_dwConvMode == 0;
}

void WIme::Reset() {
	this->SetActive(true);
	ImmNotifyIME(this->m_hIMC, NI_COMPOSITIONSTR, 4, 0);
	this->ClearData();
	this->m_TextComp[0] = 0;
	this->m_TextComp[1] = 0;
	this->m_TextComp[2] = 0;
}

void WIme::Update(unsigned long timeStamp) {
	if (this->current == this->old_current) {
		this->bUpdated = false;
	} else {
		this->old_current = this->current;
		this->bUpdated = true;
		this->dwLastInputTime = timeGetTime();
	}
}

WIme::WIme() {
	this->buff = nullptr;
	this->current = 0;
	this->m_TextComp[0] = 0;
	this->m_TextComp[1] = 0;
	this->m_TextComp[2] = 0;
	this->m_bActive = false;
	ZeroMemory(&this->m_candList, sizeof(this->m_candList));
	this->m_charWidth = 0;
	this->m_charHeight = 0;
	return;
}

const char *WIme::GetDeviceName() {
	return "Ime";
}

WProc *WIme::ExternProc() {
	return this;
}

void WIme::SetActive(bool stat) {
	this->m_bActive = stat;
	this->pos = 0;
	this->current = 0;
}

WIme::~WIme() {
	return;
}

void WIme::InitIme(HWND hwnd_) {
	if (hwnd_) {
		this->m_hWnd = hwnd_;
	}

	this->m_hKeyLayout = GetKeyboardLayout(0);
	this->m_property = ImmGetProperty(this->m_hKeyLayout, 4u);
	this->ClearData();
}

void WIme::OnInputLangChange(DWORD dwCommand, long dwData) {
	int i;
	CANDIDATEFORM Candidate;

	if (ImmIsIME(this->m_hKeyLayout) && this->m_property & IME_PROP_AT_CARET) {
		this->ClearData();
	}

	this->m_hKeyLayout = GetKeyboardLayout(0);
	this->m_property = ImmGetProperty(m_hKeyLayout, IGP_PROPERTY);
	this->ClearData();
	if (this->m_hWnd) {
		this->m_hIMC = ImmGetContext(this->m_hWnd);
		if (m_hIMC != nullptr) {
			i = 0;
			do {
				if (!(this->m_property & IME_PROP_AT_CARET) && ImmGetCandidateWindow(this->m_hIMC, i, &Candidate)) {
					if (Candidate.dwStyle) {
						Candidate.dwStyle = 0;
						ImmSetCandidateWindow(this->m_hIMC, &Candidate);
					}
				}
				++i;
			} while ( i < 32 );
			ImmReleaseContext(this->m_hWnd, this->m_hIMC);
		}
	}
}

void WIme::OnComposition(DWORD dwCommand, long dwData) {
	if (!(this->m_property & IME_PROP_SPECIAL_UI) && (this->m_property & IME_PROP_AT_CARET)) {
		if (dwData & GCS_RESULTSTR) {
			this->GetResultString();
		} else if (dwData & GCS_COMPSTR) {
			this->GetCompString(dwData);
		}
	}
}

void WIme::OnChar(unsigned int nChar) {
	// TODO: Imperfect compilation

	switch (nChar) {
	case '\b':
		this->ringBuf[this->current++ & 0x3FF] = '\b';
		break;
	case '\t':
		this->ringBuf[this->current++ & 0x3FF] = '\t';
		break;
	case '\r':
		this->ringBuf[this->current++ & 0x3FF] = '\r';
		break;
	case 0x1B:
		this->ringBuf[this->current++ & 0x3FF] = 0x1B;
		break;
	case 0xA1:
		this->ringBuf[this->current++ & 0x3FF] = 2;
		break;
	case 0xA2:
		this->ringBuf[this->current++ & 0x3FF] = 3;
		break;
	case 0xA3:
		this->ringBuf[this->current++ & 0x3FF] = 0x1A;
		break;
	case 0xA4:
		this->ringBuf[this->current++ & 0x3FF] = 0x0C;
		break;
	case 0xA5:
		this->ringBuf[this->current++ & 0x3FF] = 0x04;
		break;
	case 0xA6:
		this->ringBuf[this->current++ & 0x3FF] = 0x06;
		return; // ?
	case 0xA7:
		this->ringBuf[this->current++ & 0x3FF] = 0x05;
		break;
	case 0xA8u:
		this->ringBuf[this->current++ & 0x3FF] = 0x07;
		break;
	case 0xAEu:
		this->ringBuf[this->current++ & 0x3FF] = 0x01;
		break;
	default:
		break;
	}

	if (nChar >= 0x20 && nChar <= 0x7E) {
		char *ptr = (char *)&nChar;
		ptr[2] = nChar;
		ptr[3] = 0;
		this->PutString(&ptr[2]);
	}
}

WIme::WIme(HWND hwnd_) {
	this->m_hWnd = nullptr;
	this->m_bActive = false;
	this->m_hKeyLayout = nullptr;
	this->m_nState = 0;
	this->m_nCompLen = 0;
	this->m_property = 0;
	this->m_charWidth = 0;
	this->m_charHeight = 0;
	this->m_hIMC = nullptr;
	this->m_dwConvMode = 0;
	this->m_TextComp[0] = 0;
	this->m_TextComp[1] = 0;
	this->m_TextComp[2] = 0;
	this->old_current = 0;
	this->current = 0;
	this->pos = 0;
	this->bUpdated = false;
	this->dwLastInputTime = timeGetTime();
	if (hwnd_) {
		this->m_hWnd = hwnd_;
	}
	this->m_hKeyLayout = GetKeyboardLayout(0);
	this->m_property = ImmGetProperty(m_hKeyLayout, IGP_PROPERTY);
	this->ClearData();
}

LRESULT WIme::WinProc(UINT msg, WPARAM wparam, LPARAM lparam) {
	if (!this->m_bActive) {
		return 0;
	}

	switch ( msg ) {
	case WM_IME_SETCONTEXT:
		this->OnSetContext(wparam, lparam);
		return 1;
	case WM_IME_NOTIFY:
		return WIme::OnNotify(wparam, lparam);
	case WM_IME_CONTROL:
	case WM_IME_COMPOSITIONFULL:
		return 1;
		break;
	case WM_INPUTLANGCHANGE:
		this->OnInputLangChange(wparam, lparam);
		return 0;
	case WM_KEYDOWN:
		unsigned int nChar;
		nChar = (char)wparam;
		if (wparam >= 0x21 && wparam <= 0x2E) {
			nChar = wparam | 0x80;
			this->OnChar(nChar);
		}
		return 1;
	case WM_CHAR:
		this->OnChar(wparam);
		return 0;
	case WM_IME_STARTCOMPOSITION:
		this->OnStartComposition(wparam, lparam);
		return 1;
	case WM_IME_ENDCOMPOSITION:
		this->OnEndComposition(wparam, lparam);
		return 1;
	case WM_IME_COMPOSITION:
		this->OnComposition(wparam, lparam);
		return 1;
	default:
		return 0;
	}
}

WInputDev *WIme::MakeClone(char *modeName, HWND hwnd) {
	return new WIme(hwnd);
}
