#pragma once
#include "wfont.h"

struct w_fnthead
{
	int32_t id;
	int32_t fontsize;
	int32_t bAntialiased;
	int32_t dummy;
};

class WFixedFont : public WFont {
public:
	struct w_fixedtext {
		int px;
		int py;
		int width;
		char msg[1];
	};

	struct w_flush_area {
		w_fixedtext* tex;
		float x;
		float y;
		unsigned int diffuse;
		int type;
	};

	struct w_temp_pair {
		w_fixedtext* tex;
		w_fixedtext* newone;
		char msg[1];
	};

	WFixedFont(int maxTexture);
	void Init(int maxTexture);

	float PrintInside(WView* view, float x, float y, const char* pText, int type, unsigned int diffuse, Bitmap* bmp) override;
	float GetTextWidthInside(WView* view, const char* pText) override;
	int Load(const char* fntName);

private:
	WList<w_fixedtext*> m_textList{8, 8};
	w_flush_area m_flush_list[128] = {};
	int m_flush_num{};
	bool m_update_flag{};
	bool m_bFullEnglish{};
	bool m_bFixedWidth{};
	int m_bAntialiased{};
	int m_nMaxTexture{};
	int* m_texIndex{};
	Bitmap** m_bitmap{};
	bool* m_update{};
	uint8_t* m_font{};
	int m_iFontWidth{};
	int m_iFontHeight{};
	int m_nMaxWidth{};
	int m_nowX{};
	int m_nowY{};
	int m_bytePerChar{};
	bool m_clone{};
};
