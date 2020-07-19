#pragma once
#include "wmath.h"

class WView;
class TexCacheManager;
class WFont;

class FrGraphicInterface {
public:
	FrGraphicInterface(WView* pView);
	~FrGraphicInterface();

	void Line(const WPoint& p1, const WPoint& p2, unsigned diffuse1, unsigned diffuse2, unsigned type);
	void Box(const WRect& rect, unsigned diffuse, unsigned type, float depth);
	unsigned GetTextColor() const;
	void LineBox(const WRect& rect, unsigned diffuse, unsigned type, float unused);
	float GetViewWidth() const;
	float GetViewHeight() const;
	float GetAlpha() const;

private:
	WView* m_pView;
	TexCacheManager* m_pCacheManager;
	float m_alpha;
	unsigned int m_textColor;
	unsigned int m_textOutlineColor;
	unsigned int m_textStyle;
	WFont* m_pFont12;
	WFont* m_pFont11;
	int m_maxTexCache;
	int m_cacheTexSize;
};
