#pragma once
#include "frcursor.h"
#include "frguiitem.h"
#include "frwnd.h"

class FrGuiItem;
class Bitmap;

class FrButton : public FrWnd {
public:
	typedef FrCursor::eCursor eButMode;
	typedef BGTYPE eButStyle;
	typedef BUTTONPARTS eButPushStyle;

	enum eButDrawStyle {
		BD_NONE = 0x0,
		BD_OWNERDRAW = 0x1,
	};

private:
	FrGuiItem* m_pItem;
	const Bitmap* m_pBitmap[4];
	const Bitmap* m_pBtnBg[4];
	eButMode m_status;
	eButStyle m_style;
	eButPushStyle m_pushStyle;
	eButDrawStyle m_drawStyle;
	float m_pushDelay;
	float m_pushedTime;
	float m_fBlinktime;
	bool m_bPressed;
	bool m_bPrevPressed;
	bool m_useRightButton;
	bool m_blink;
	bool m_blinkEnable;
};
