#pragma once
#include "wfont.h"

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

	void Init(int maxTexture);

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
