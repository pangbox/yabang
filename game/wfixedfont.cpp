#include "wfixedfont.h"

void WFixedFont::Init(int maxTexture) {
	this->m_font = nullptr;
    this->m_clone = false;
    this->m_eType = FrCursor::UNSELECT;
    this->m_bFullEnglish = false;
    this->m_bFixedWidth = false;
    this->m_bAntialiased = 0;
    this->m_update_flag = false;
    this->m_nMaxTexture = maxTexture;
    this->m_texIndex = new int [maxTexture];
    this->m_bitmap = new Bitmap*[maxTexture];
    this->m_update = new bool[maxTexture];
    this->m_flush_num = 0;
    for (int i = 0; i < maxTexture; i++) {
        this->m_bitmap[i] = nullptr;
        this->m_texIndex[i] = 0;
        this->m_update[i] = false;
    }
    this->m_bytePerChar = 0;
    this->m_nMaxWidth = 0;
    this->m_iFontHeight = 0;
    this->m_iFontWidth = 0;
    this->m_nowY = 0;
    this->m_nowX = 0;
}

