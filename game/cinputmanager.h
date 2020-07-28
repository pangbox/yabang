#pragma once
#include <cstdint>

#include "wlist.h"
#include "wmath.h"

class CInputManager {
public:
	struct sButton {};

protected:
	// TODO: Some state is missing still.
	WVector m_mouseDelta;
	WVector m_mousePoint;
	bool m_bFixPointer;
	sButton m_buttons[3];
	float m_fSensitivity;
	bool m_bActive;
	bool m_bSwapMouseButton;
};
