#pragma once

class WView;
class TexCacheManager;
class WFont;

class FrGraphicInterface {
public:
	FrGraphicInterface(WView* pView);
	~FrGraphicInterface();

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
