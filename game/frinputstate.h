#pragma once
#include "wmath.h"

class FrWnd;
class FrScrollBar;
class CChatMsg;

struct FrInputState {
	unsigned int mouse{};
	bool hoverChecked{};
	WPoint mousePos;
	WPoint oldMousePos;
	float wheelDelta{};
	FrWnd* keyFocused{};
	FrScrollBar* wheelFocused{};
	CChatMsg* im{};
};
