#include "wtitlefont.h"

#include "bitmap.h"
#include "wresourcemanager.h"

WTitleFont::WTitleFont() = default;

WTitleFont::WTitleFont(WTITLEFONT* info) {
	this->Create(info);
}

WTitleFont::~WTitleFont() {
	this->Erase();
}

const WTITLEFONT& WTitleFont::GetFontInfo() const {
	return this->m_info;
}

void WTitleFont::Erase() {
	if (this->m_iTexIndex) {
		for (int i = 0; i < this->m_numTex; ++i) {
			if (this->m_iTexIndex[i]) {
				this->m_resrcMng->Release(this->m_iTexIndex[i]);
				this->m_iTexIndex[i] = 0;
			}
		}
		delete[] this->m_iTexIndex;
		this->m_iTexIndex = nullptr;
	}
	if (this->m_fWidthIndex) {
		delete[] this->m_fWidthIndex;
		this->m_fWidthIndex = nullptr;
	}
	this->m_pCharSet = nullptr;
}

float WTitleFont::PutChar(WView* view, float x, float y, int nChar, int type, unsigned int diffuse) const {
	int ch = nChar % this->m_numPerPage;
	WRect src{
		static_cast<float>(ch % this->m_numWidth * this->m_info.fontw) /
		static_cast<float>(this->m_info.texw),
		static_cast<float>(ch / this->m_numWidth * this->m_info.fonth) /
		static_cast<float>(ch % this->m_numWidth * this->m_info.fontw),
		static_cast<float>(this->m_info.fontw) / static_cast<float>(this->m_info.texw),
		static_cast<float>(this->m_info.fonth) / static_cast<float>(this->m_info.texh),
	};
	WRect dest{
		x,
		y,
		static_cast<float>(this->m_info.fonth) * this->GetFontScale(view),
		static_cast<float>(this->m_info.fontw) * this->GetFontScale(view),
	};
	DrawTexture(view, this->m_iTexIndex[nChar / this->m_numPerPage], src, dest, type, diffuse, 0.0, 0, true);
	return this->m_fWidthIndex[nChar] * this->GetFontScale(view);
}

float WTitleFont::GetCharWidthA(WView* view, int nChar) const {
	return this->GetFontScale(view) * this->m_fWidthIndex[nChar];

}

float WTitleFont::PrintInside(WView* view, float x, float y, const char* pText, int type, unsigned diffuse,
                              Bitmap* bmp) {
	float xPos = x;
	while (*pText) {
		char ch = *pText++;
		char* nChar = strchr(this->m_pCharSet, ch);
		if (ch) {
			xPos += this->PutChar(view, xPos, y, nChar - this->m_pCharSet, type, diffuse);
		} else if (ch == ' ') {
			xPos += static_cast<float>(this->m_info.fontw) * 0.5f;
		}
	}
	return xPos - x;
}

float WTitleFont::GetTextWidthInside(WView* view, const char* pText) {
	float width = 0.0f;
	while (*pText) {
		char ch = *pText++;
		char* nChar = strchr(this->m_pCharSet, ch);
		if (ch) {
			width += this->m_fWidthIndex[nChar - this->m_pCharSet];
		} else if (ch == ' ') {
			width += static_cast<float>(this->m_info.fontw) * 0.5f;
		}
	}
	return this->GetFontScale(view) * width;
}

void WTitleFont::Create(WTITLEFONT* info) {
	this->Erase();
	this->m_info = *info;
	int numHeight = this->m_info.texh / this->m_info.fonth;
	this->m_numWidth = this->m_info.texw / this->m_info.fontw;
	this->m_numTex = this->m_info.numPages;
	this->m_numCharSet = this->m_numWidth * numHeight * this->m_numTex;
	this->m_numPerPage = this->m_numWidth * numHeight;
	this->m_pCharSet = this->m_info.pCharSet;
	this->m_iTexIndex = new int[this->m_numTex];
	this->m_fWidthIndex = new float[this->m_numCharSet];

	for (int i = 0, n = 0; i < this->m_numTex; i++) {
		this->m_iTexIndex[i] = 0;
		Bitmap* bitmap = this->m_resrcMng->LoadBitmapA(info->filename[i], 0, false);
		if (!bitmap) {
			continue;
		}
		char* extension = strrchr(info->filename[i], '.');
		if (_strcmpi(extension, ".tga")) {
			this->m_iTexIndex[i] = this->m_resrcMng->LoadTexture(info->filename[i], 0x00080000, 0, nullptr);
		} else {
			this->m_iTexIndex[i] = this->m_resrcMng->LoadTexture(info->filename[i], 0x80080000, 0, nullptr);
		}
		for (int y = 0; y <= this->m_info.texh - this->m_info.fonth; y += this->m_info.fonth) {
			for (int x = 0; x <= this->m_info.texw - this->m_info.fontw; x += this->m_info.fontw) {
				int maxW = 0;
				uint8_t* scanline = &bitmap->m_vram[y * bitmap->m_pitch];
				for (int gy = 0; gy < this->m_info.fonth; gy++) {
					int curMax = maxW;
					for (int gx = x + maxW; gx < x + this->m_info.fontw; gx++) {
						int value;
						switch (bitmap->m_bi->bmiHeader.biBitCount) {
							case 8: {
								value = scanline[gx];
								break;
							}
							default:
							case 24: {
								uint8_t* px = scanline + 3 * gx;
								value = (px[0] + px[1] + px[2]) / 3;
								break;
							}
							case 32: {
								value = scanline[4 * gx + 3];
								break;
							}
						}
						if (value > 50 && curMax > maxW) {
							maxW = curMax;
						}
						++curMax;
					}
					scanline += bitmap->m_pitch;
				}
				this->m_fWidthIndex[n++] = static_cast<float>(maxW);
			}
		}
		this->m_info.texw = this->m_resrcMng->VideoReference()->GetTextureWidth(this->m_iTexIndex[i]);
		this->m_info.texh = this->m_resrcMng->VideoReference()->GetTextureHeight(this->m_iTexIndex[i]);
		delete bitmap;
	}
}
