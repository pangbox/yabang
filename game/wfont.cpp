#include "wfont.h"


#include "wresourcemanager.h"
#include "wview.h"

WFont::WFont() {
	SetCoordMode(0x2600);
	this->m_scale = 1.0;
	this->m_useOverlay = false;
	this->m_colorset[1] = 0xFF0000;
	this->m_colorset[2] = 0x00FF00;
	this->m_colorset[3] = 0x0000FF;
	this->m_space = 1;
}

WFont::~WFont() {
	this->ResetOverlay();
}

void WFont::SetScale(float scale) {
	this->m_scale = scale;
}

float WFont::GetScale() const {
	return this->m_scale;
}

void WFont::SetMode(eFontStyle type) {
	this->m_eType = type;
}

void WFont::SetSpace(int space) {
	this->m_space = space;
}

int WFont::GetSpace() const {
	return this->m_space;
}

void WFont::SetColorSet(int idx, unsigned int color) {
	this->m_colorset[idx] = color;
}

void WFont::SetCoordMode(int coordMode) {
	WOverlay::SetCoordMode(coordMode | 0x400);
}

float WFont::GetTextWidth(WView* view, const char* text) {
	return this->PrintOut(view, 0.0, 0.0, text, 0, 0, false, nullptr);
}

float WFont::GetFontScale(WView* view) const {
	if (!view || (this->GetCoordMode() & 0x100) == 0) {
		return this->m_scale;
	}
	return static_cast<float>(view->GetWidth() * this->m_scale / 640.0f);
}

WFont* WFont::MakeClone() {
	return nullptr;
}

void WFont::SetFixedWidth(bool flag) {}

void WFont::SetFontWidth(int width) {}

int WFont::GetFontHeight() {
	return 0;
}

void WFont::Reset() {}

void WFont::ResetOverlay() {
	for (auto* it = this->m_overlayList.Start(); it != nullptr; it = this->m_overlayList.Next()) {
		this->m_resrcMng->Release(it);
	}
	this->m_overlayList.Reset();
}

void WFont::Flush(WView* view) {
	if (!this->m_useOverlay && this->m_overlayList.Start() != nullptr) {
		this->ResetOverlay();
	}
	this->m_useOverlay = false;
}

template <size_t N, typename... T>
constexpr bool ScanArg(const char* str, char const (&prefix)[N], const char* fmt, T ... args) {
	auto arg = strstr(str, prefix);
	if (arg) {
		sscanf(arg + N - 1, fmt, args...);
		return true;
	}
	return false;
}

float WFont::PrintOut(WView* view, float x, float y, const char* text, int type, unsigned int diffuse, bool draw,
                      Bitmap* bmp) {
	// TODO: This function could use some cleanup.
	char temp[1024];
	char filename[32];
	unsigned int currentColor;

	float len = 0.0;
	if (draw) {
		if (!bmp) {
			x = this->GetUnit(view, x, W_UNIT_XPOS);
			y = this->GetUnit(view, y, W_UNIT_YPOS);
		}
		currentColor = diffuse;
	} else {
		currentColor = 0;
	}
	for (const char* ptr = text; *ptr;) {
		if (*ptr == '\\') {
			char esc = ptr[1];
			if (esc == '<') {
				WRect rect;
				float scale = 1.0;
				float xs = 256.0, ys = 256.0;
				float ox = 0.0, oy = 0.0;
				ptr += 2;
				unsigned int tagLen = strchr(ptr, '>') - ptr;
				memcpy(temp, ptr, tagLen);
				ptr += tagLen;
				temp[tagLen] = 0;
				ScanArg(temp, "src=", "%s", filename);
				ScanArg(temp, "size=", "%f %f", &xs, &ys);
				if (!ScanArg(temp, "rect=", "%f %f %f %f", &rect, &rect.y, &rect.w, &rect.h)) {
					rect.x = 0.0;
					rect.y = 0.0;
					rect.w = xs;
					rect.h = ys;
				}
				ScanArg(temp, "scale=", "%f", &scale);
				if (ScanArg(temp, "offset=", "%f %f", &ox, &oy)) {
					if ((this->GetCoordMode() & 0x100) != 0) {
						if (view) {
							ox *= view->GetWidth() / 640.0f;
							oy *= view->GetHeight() / 480.0f;
						}
					}
				}
				this->m_useOverlay = true;
				WOverlay* overlay = this->m_overlayList.Find(filename);
				if (!overlay) {
					overlay = this->m_resrcMng->GetOverlay(filename, 0);
					this->m_overlayList.AddItem(overlay, filename, true);
				}
				if (draw && overlay) {
					overlay->SetCoordMode(0x1200);
					WRect src{
						rect.x / xs,
						rect.y / ys,
						rect.w / xs,
						rect.h / ys,
					};
					WRect dest{
						ox + x + len,
						oy + y,
						rect.w * scale,
						rect.h * scale,
					};
					overlay->Render(view, src, dest, type, currentColor, 0.0, 0);
				}
				ptr++;
			} else if (esc == '0') {
				currentColor = diffuse;
				ptr += 2;
			} else if (esc >= '1' && esc <= '9') {
				// Multiply colorset color with diffuse.
				unsigned int c = this->m_colorset[esc - '0'];
				currentColor = diffuse;
				currentColor &= 0xFF00FFFF;
				currentColor |= (c >> 16 & 0xFF) * (diffuse >> 16 & 0xFF) << 8;
				currentColor &= 0xFFFF00FF;
				currentColor |= (c >> 8 & 0xFF) * (diffuse >> 8 & 0xFF);
				currentColor &= 0xFFFFFF00;
				currentColor |= (c >> 0 & 0xFF) * (diffuse >> 0 & 0xFF) >> 8;
				ptr += 2;
			} else {
				ptr++;
			}
		} else {
			// Print text until next escape.
			const char* pText = ptr;
			int n = strlen(ptr);
			for (int j = 0; j < n; j++) {
				if (ptr[j] == '\\') {
					// Hit escape; copy to temp buffer, set increment to j.
					n = j;
					if (ptr + j != nullptr) {
						memcpy(temp, ptr, j);
						temp[j] = 0;
						pText = temp;
						break;
					}
				} else if (ptr[j] < 0) {
					// Hit shift sequence - skip next char.
					j++;
				}
			}
			ptr += n;
			if (draw) {
				len += this->PrintInside(view, len + x, y, pText, type, currentColor, bmp);
			} else {
				len += this->GetTextWidthInside(view, pText);
			}
		}
	}
	float scale = 1.0;
	if ((this->GetCoordMode() & 0x1000) != 0 && view) {
		scale = 640.0f / view->GetWidth();
	}
	return scale * len;
}
