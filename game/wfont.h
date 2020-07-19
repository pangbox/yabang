#pragma once
#include "frcursor.h"
#include "wlist.h"
#include "woverlay.h"

class WFont : public WOverlay {
public:
	typedef FrCursor::eCursor eFontStyle;

	WFont();
	~WFont();

	void SetScale(float scale);
	float GetScale() const;
	void SetMode(eFontStyle type);
	void SetSpace(int space);
	int GetSpace() const;
	void SetColorSet(int idx, unsigned color);
	void SetCoordMode(int coordMode);
	float GetFontScale(WView* view) const;
	WFont* MakeClone();
	void SetFixedWidth(bool flag);
	void SetFontWidth(int width);
	int GetFontHeight();
	void Reset();
	void ResetOverlay();
	void Flush(WView* view);

private:
	eFontStyle m_eType{};
	int m_space{};
	float m_scale{};
	int m_colorset[10]{};
	bool m_useOverlay{};
	WList<WOverlay*> m_overlayList{4, 4};
};
