#include "directinput.h"
#include "dimouse.h"
#include "dikeyboard.h"
#include <cassert>

WDirectInput::WDirectInput() {
	this->m_di = nullptr;
}

WDirectInput::~WDirectInput() {
	if (this->m_di) {
		this->m_di->Release();
		this->m_di = nullptr;
	}
}

WInputDev* WDirectInput::MakeClone(char* modeName, HWND hWnd) {
	if (!this->m_di) {
		auto* hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hWnd, GWL_HINSTANCE));
		HRESULT result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8A,
		                                    reinterpret_cast<void**>(&this->m_di), nullptr);
		if (FAILED(result)) {
			return nullptr;
		}
	}

	if (!_strcmpi(modeName, "mouse")) {
		return new DirectInputMouse(this->m_di, hWnd);
	}
	if (!_strcmpi(modeName, "keyboard")) {
		return new DirectInputKeyboard(this->m_di, hWnd);
	}
	return nullptr;
}

const char* WDirectInput::GetDeviceName() {
	return "DirectInput";
}

const char* WDirectInput::EnumModeName() {
	return "Keyboard";
}
