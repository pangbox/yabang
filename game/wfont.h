#pragma once
#include "frcursor.h"
#include "wlist.h"
#include "woverlay.h"

class WFont : public WOverlay {
public:
	typedef FrCursor::eCursor eFontStyle;

	WFont();
	void SetMode(eFontStyle type);

private:
	eFontStyle m_eType{};
	int m_space{};
	float m_scale{};
	int m_colorset[10]{};
	bool m_useOverlay{};
	WList<WOverlay*> m_overlayList{4, 4};
};
