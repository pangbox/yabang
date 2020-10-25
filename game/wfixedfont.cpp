#include "wfixedfont.h"

#include "cfile.h"
#include "wresourcemanager.h"

WFixedFont::WFixedFont(int maxTexture) {
    this->Init(maxTexture);
}

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

float WFixedFont::PrintInside(WView* view, float x, float y, const char* pText, int type, unsigned diffuse, Bitmap* bmp) {
	// TODO: implement
    abort();
}

float WFixedFont::GetTextWidthInside(WView* view, const char* pText) {
    // TODO: implement
    abort();
}

int WFixedFont::Load(const char* fntName) {
	w_fnthead header;
    cFile* fp = this->m_resrcMng->GetCFile(fntName, 0xFFFF);
    if (!fp) {
        return 1;
    }
    unsigned int fontLen = fp->Length() - 16;
    if (this->m_font) {
        delete [] this->m_font;
        this->m_font = nullptr;
    }
    this->m_font = new uint8_t[fontLen];
    fp->Read(&header, 16);
    fp->Read(this->m_font, fontLen);
    CloseCFile(fp);
    this->m_bAntialiased = header.bAntialiased;
    this->m_bytePerChar = header.fontsize * ((header.fontsize * (header.bAntialiased != 0 ? 4 : 1) + 7) / 8) + 2;
    this->m_iFontWidth = header.fontsize;
    this->m_iFontHeight = header.fontsize;
    return 0;
}