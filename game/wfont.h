#pragma once
#include "frcursor.h"
#include "wlist.h"
#include "woverlay.h"

class WFont : public WOverlay {
public:
	typedef FrCursor::eCursor eFontStyle;

	WFont();
	~WFont() override;

	void SetCoordMode(int coordMode) override;
	virtual WFont* MakeClone();
	virtual float GetTextWidth(WView* view, const char* text);
	virtual void SetFixedWidth(bool flag);
	virtual void SetFontWidth(int width);
	virtual int GetFontHeight();
	virtual void Flush(WView* view);
	virtual void Reset();
	virtual float PrintInside(WView* view, float x, float y, const char* pText, int type, unsigned int diffuse, Bitmap* bmp) = 0;
	virtual float GetTextWidthInside(WView* view, const char* pText) = 0;

	void SetScale(float scale);
	float GetScale() const;
	void SetMode(eFontStyle type);
	void SetSpace(int space);
	int GetSpace() const;
	void SetColorSet(int idx, unsigned color);
	float GetFontScale(WView* view) const;
	void ResetOverlay();
	float PrintOut(WView* view, float x, float y, const char* text, int type, unsigned diffuse, bool draw, Bitmap* bmp);

private:
	eFontStyle m_eType{};
	int m_space{};
	float m_scale{};
	int m_colorset[10]{};
	bool m_useOverlay{};
	WList<WOverlay*> m_overlayList{4, 4};
};
