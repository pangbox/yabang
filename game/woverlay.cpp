#include "woverlay.h"

#include "bitmap.h"
#include "wresourcemanager.h"
#include "wvideo.h"
#include "wview.h"
#include <cmath>

WtVertex* WOverlay::m_vtxList[9]{};
WtVertex WOverlay::m_vtx[4]{};
WtVertex* WOverlay::m_vl[5]{
	&m_vtx[0],
	&m_vtx[1],
	&m_vtx[2],
	&m_vtx[3],
	nullptr,
};

WOverlay::WOverlay() = default;

WOverlay::~WOverlay() {
	if (this->m_texHandle) {
		this->m_resrcMng->Release(this->m_texHandle);
	}
}

unsigned int WOverlay::GetWidth() const {
	return this->m_texWidth;
}

unsigned int WOverlay::GetHeight() const {
	return this->m_texHeight;
}

void WOverlay::SetCoordMode(int coordMode) {
	this->m_coordMode = coordMode;
}

int WOverlay::GetTexhandle() const {
	return this->m_texHandle;
}

int WOverlay::GetCoordMode() const {
	return this->m_coordMode;
}

bool WOverlay::Load(const char* filename, unsigned int flag) {
	char buf[260];
	if (this->m_texHandle) {
		this->m_resrcMng->Release(this->m_texHandle);
	}
	this->m_texHandle = this->m_resrcMng->LoadTexture(filename, flag | 0x80000, 0, nullptr);
	if (this->m_texHandle) {
		this->m_texWidth = this->m_resrcMng->GetTextureWidth(this->m_texHandle);
		this->m_texHeight = this->m_resrcMng->GetTextureHeight(this->m_texHandle);
		this->m_devTexWidth = this->m_resrcMng->VideoReference()->GetTextureWidth(this->m_texHandle);
		this->m_devTexHeight = this->m_resrcMng->VideoReference()->GetTextureHeight(this->m_texHandle);
		sprintf_s(buf, sizeof(buf), "WOverlay:%s", filename);
		this->SetLeakHint(buf);
	}
	return this->m_texHandle == 0;
}

bool WOverlay::Load(const char* filename, Bitmap* bitmap, unsigned int flag) {
	char buf[260];
	if (this->m_texHandle) {
		this->m_resrcMng->Release(this->m_texHandle);
	}
	this->m_texHandle = this->m_resrcMng->UploadTexture(filename, bitmap, flag | 0x80000, nullptr);
	if (this->m_texHandle) {
		this->m_texWidth = this->m_resrcMng->GetTextureWidth(this->m_texHandle);
		this->m_texHeight = this->m_resrcMng->GetTextureHeight(this->m_texHandle);
		this->m_devTexWidth = this->m_resrcMng->VideoReference()->GetTextureWidth(this->m_texHandle);
		this->m_devTexHeight = this->m_resrcMng->VideoReference()->GetTextureHeight(this->m_texHandle);
		sprintf_s(buf, sizeof(buf), "WOverlay:%s", filename);
		this->SetLeakHint(buf);
	}
	return this->m_texHandle == 0;
}

void WOverlay::SetClippingArea(const WRect* rect) {
	if (rect) {
		this->m_clipFlag = true;
		this->m_clipArea = *rect;
	} else {
		this->m_clipFlag = false;
	}
}

void WOverlay::DrawTexture(WView* view, int texHandle, const WRect& src, const WRect& dest, float shear, int type,
                           unsigned int diffuse) {
	for (int i = 0; i < 4; i++) {
		if ((i & 2) == 0) shear = 0.0f;
		m_vtx[i].x = ((i & 1) != 0 ? dest.w + dest.x : dest.x) - 0.5f + shear;
		m_vtx[i].y = ((i & 2) != 0 ? dest.h + dest.y : dest.y) - 0.5f;
		m_vtx[i].z = 0.001f;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].diffuse = diffuse;
		m_vtx[i].tu = (i & 1) != 0 ? src.w + src.x : src.x;
		m_vtx[i].tv = (i & 2) != 0 ? src.h + src.y : src.y;
	}
	view->DrawPolygonFan(m_vl, type | (texHandle & 0x7FF) | 0x20300000, 0, true);
}

int WOverlay::GetSection(WtVertex* out, float snapang, float ang) const {
	if (ang >= snapang) {
		if (ang == snapang) {
			out->x = this->m_clipArea.w + this->m_clipArea.x;
			out->y = this->m_clipArea.y - this->m_clipArea.h;
			out->tu = 1.0;
			out->tv = 0.0;
			return 1;
		}
		if (ang <= snapang || ang >= g_pif - snapang) {
			if (ang == g_pif - snapang) {
				out->x = this->m_clipArea.x - this->m_clipArea.w;
				out->y = this->m_clipArea.y - this->m_clipArea.h;
				out->tu = 0.0;
				out->tv = 0.0;
				return 3;
			}
			if (ang <= g_pif - snapang || ang >= snapang + g_pif) {
				if (ang == snapang + g_pif) {
					out->x = this->m_clipArea.x - this->m_clipArea.w;
					out->y = this->m_clipArea.h + this->m_clipArea.y;
					out->tu = 0.0;
					out->tv = 1.0;
					return 5;
				}
				if (ang <= snapang + g_pif || ang >= (g_pif * 2.0f) - snapang) {
					if (ang == (g_pif * 2.0f) - snapang) {
						out->x = this->m_clipArea.w + this->m_clipArea.x;
						out->y = this->m_clipArea.h + this->m_clipArea.y;
						out->tu = 1.0;
						out->tv = 1.0;
						return 7;
					}
					if (ang > (g_pif * 2.0f) - snapang) {
						out->x = this->m_clipArea.x + this->m_clipArea.w;
						out->y = this->m_clipArea.y - tanf(ang) * this->m_clipArea.w;
						out->tu = 1.0f;
						out->tv = 0.5f - tanf(ang) * this->m_clipArea.w * 0.5f / this->m_clipArea.h;
						return 8;
					}
					return -1;
				}
				out->x = tanf(ang - (g_pif * 1.5f)) * this->m_clipArea.h + this->m_clipArea.x;
				out->y = this->m_clipArea.y + this->m_clipArea.h;
				out->tv = 1.0;
				out->tu = tanf(ang - (g_pif * 1.5f)) * this->m_clipArea.h * 0.5f / this->m_clipArea.w + 0.5f;
				return 6;
			}
			out->x = this->m_clipArea.x - this->m_clipArea.w;
			out->y = tanf(ang) * this->m_clipArea.w + this->m_clipArea.y;
			out->tu = 0.0;
			out->tv = tanf(ang) * this->m_clipArea.w * 0.5f / this->m_clipArea.h + 0.5f;
			return 4;
		}
		out->x = this->m_clipArea.x - tanf(ang - g_pi2f) * this->m_clipArea.h;
		out->y = this->m_clipArea.y - this->m_clipArea.h;
		out->tv = 0.0f;
		out->tu = 0.5f - tanf(ang - g_pi2f) * this->m_clipArea.h * 0.5f / this->m_clipArea.w;
		return 2;
	}
	out->x = this->m_clipArea.x + this->m_clipArea.w;
	out->y = this->m_clipArea.y - tanf(ang) * this->m_clipArea.w;
	out->tu = 1.0f;
	out->tv = 0.5f - tanf(ang) * this->m_clipArea.w * 0.5f / this->m_clipArea.h;
	return 0;
}

void WOverlay::ConvertSourceRectByTextureSize(WRect& newRc, const WRect& rc) const {
	if (this->m_texWidth == this->m_devTexWidth || !this->m_devTexWidth) {
		newRc.x = rc.x;
		newRc.w = rc.w;
	} else {
		newRc.x = (static_cast<float>(this->m_texWidth) * rc.x) / static_cast<float>(this->m_devTexWidth);
		newRc.w = (static_cast<float>(this->m_texWidth) * rc.w) / static_cast<float>(this->m_devTexWidth);
	}
	if (this->m_texHeight == this->m_devTexHeight || !this->m_devTexHeight) {
		newRc.y = rc.y;
		newRc.h = rc.h;
	} else {
		newRc.y = (static_cast<float>(this->m_texHeight) * rc.y) / static_cast<float>(this->m_devTexHeight);
		newRc.h = (static_cast<float>(this->m_texHeight) * rc.h) / static_cast<float>(this->m_devTexHeight);
	}
}

void WOverlay::DrawLine(WView* view, const WPoint& p1, const WPoint& p2, int type, unsigned int diffuse) {
	WtVertex vtx[2];
	WtVertex* vl[3]{&vtx[0], &vtx[1], nullptr};
	vtx[0].x = p1.x;
	vtx[0].y = p1.y;
	vtx[0].z = 0.001f;
	vtx[0].rhw = 1.0f;
	vtx[0].diffuse = diffuse;
	vtx[1].x = p2.x;
	vtx[1].y = p2.y;
	vtx[1].z = 0.001f;
	vtx[1].rhw = 1.0f;
	vtx[1].diffuse = diffuse;
	view->DrawPolygonFan(vl, type | 0x4000000, 0, true);
}

void WOverlay::DrawLine(WView* view, const WPoint& p1, const WPoint& p2, unsigned int diffuse1, unsigned int diffuse2,
                        int type) {
	WtVertex vtx[2];
	WtVertex* vl[3]{&vtx[0], &vtx[1], nullptr};
	vtx[0].x = p1.x;
	vtx[0].y = p1.y;
	vtx[0].z = 0.001f;
	vtx[0].rhw = 1.0f;
	vtx[0].diffuse = diffuse1;
	vtx[1].x = p2.x;
	vtx[1].y = p2.y;
	vtx[1].z = 0.001f;
	vtx[1].rhw = 1.0f;
	vtx[1].diffuse = diffuse2;
	view->DrawPolygonFan(vl, type | 0x4000000, 0, true);
}

void WOverlay::DrawBox(WView* view, const WRect& rect, int type, unsigned int diffuse, float depth) {
	m_vtx[0].x = rect.x - 0.5f;
	m_vtx[0].y = rect.y - 0.5f;
	m_vtx[0].z = depth;
	m_vtx[0].rhw = 1.0f;
	m_vtx[0].diffuse = diffuse;
	m_vtx[1].x = rect.w + rect.x - 0.5f;
	m_vtx[1].y = rect.y - 0.5f;
	m_vtx[1].z = depth;
	m_vtx[1].rhw = 1.0f;
	m_vtx[1].diffuse = diffuse;
	m_vtx[2].x = rect.x - 0.5f;
	m_vtx[2].y = rect.h + rect.y - 0.5f;
	m_vtx[2].z = depth;
	m_vtx[2].rhw = 1.0f;
	m_vtx[2].diffuse = diffuse;
	m_vtx[3].x = rect.w + rect.x - 0.5f;
	m_vtx[3].y = rect.h + rect.y - 0.5f;
	m_vtx[3].z = depth;
	m_vtx[3].rhw = 1.0f;
	m_vtx[3].diffuse = diffuse;
	if ((type & 0x7FF) != 0) {
		for (int i = 0; i < 4; i++) {
			m_vtx[i].tu = (i & 1) != 0 ? 0 : 1.0f;
			m_vtx[i].tv = (i & 2) != 0 ? 0 : 1.0f;
		}
	}
	view->DrawPolygonFan(m_vl, type | 0x20300000, 0, true);
}

void WOverlay::DrawRainbowBox(WView* view, const WRect& rect, unsigned int* aDiffuse, int type, float depth) {
	m_vtx[0].x = rect.x - 0.5f;
	m_vtx[0].y = rect.y - 0.5f;
	m_vtx[0].z = depth;
	m_vtx[0].rhw = 1.0f;
	m_vtx[0].diffuse = aDiffuse[0];
	m_vtx[1].x = rect.w + rect.x - 0.5f;
	m_vtx[1].y = rect.y - 0.5f;
	m_vtx[1].z = depth;
	m_vtx[1].rhw = 1.0;
	m_vtx[1].diffuse = aDiffuse[1];
	m_vtx[2].x = rect.x - 0.5f;
	m_vtx[2].y = rect.h + rect.y - 0.5f;
	m_vtx[2].z = depth;
	m_vtx[2].rhw = 1.0f;
	m_vtx[2].diffuse = aDiffuse[2];
	m_vtx[3].x = rect.w + rect.x - 0.5f;
	m_vtx[3].y = rect.h + rect.y - 0.5f;
	m_vtx[3].z = depth;
	m_vtx[3].rhw = 1.0f;
	m_vtx[3].diffuse = aDiffuse[3];
	if ((type & 0x7FF) != 0) {
		for (int i = 0; i < 4; i++) {
			m_vtx[i].tu = (i & 1) != 0 ? 0.0f : 1.0f;
			m_vtx[i].tv = (i & 2) != 0 ? 0.0f : 1.0f;
		}
	}
	view->DrawPolygonFan(m_vl, type | 0x20300000, 0, true);
}

void WOverlay::DrawLineBox(WView* view, const WRect& rect, int type, unsigned int diffuse) {
	WPoint lb{rect.x, rect.y};
	WPoint lt{rect.w + rect.x, rect.y};
	WPoint rt{rect.w + rect.x, rect.h + rect.y};
	WPoint rb{rect.x, rect.h + rect.y};
	DrawLine(view, lt, rt, type | 0x300000, diffuse);
	DrawLine(view, rt, rb, type | 0x300000, diffuse);
	DrawLine(view, rb, lb, type | 0x300000, diffuse);
	DrawLine(view, lb, lt, type | 0x300000, diffuse);
}

double WOverlay::GetUnit(WView* view, float unit, wUnitMode mode) {
	switch (mode) {
		case W_UNIT_XPOS:
			if ((this->m_coordMode & 0x100) == 0) {
				return unit;
			}
			switch (this->m_coordMode & 0x06) {
				case 0x00:
					return view->GetWidth() / 640.0 * unit;
				case 0x02:
					return view->GetWidth() / 640.0 * (unit + 320.0);
				case 0x04:
					return view->GetWidth() / 640.0 * (unit + 640.0);
				default:
					break;
			}
			break;
		case W_UNIT_YPOS:
			if ((this->m_coordMode & 0x100) == 0) {
				return unit;
			}
			switch (this->m_coordMode & 0x60) {
				case 0x00:
					return view->GetHeight() / 480.0 * unit;
				case 0x20:
					return view->GetHeight() / 480.0 * (unit + 240.0);
				case 0x40:
					return view->GetHeight() / 480.0 * (unit + 480.0);
				default:
					break;
			}
			break;
		case W_UNIT_WIDTH:
			if ((this->m_coordMode & 0x1000) == 0) {
				return unit;
			}
			return view->GetWidth() / 640.0 * unit;
		case W_UNIT_HEIGHT:
			if ((this->m_coordMode & 0x1000) == 0) {
				return unit;
			}
			return view->GetHeight() / 480.0 * unit;
		default:
			return unit;
	}
}

WRect WOverlay::ConvertRect(WView* view, const WRect& rect, int coorFlag) {
	WRect result;
	float pivotX = (coorFlag & 0x06) != 0 ? ((coorFlag & 0x02) != 0 ? 0.5f : 1.0f) * view->GetWidth() : 0.0f;
	float pivotY = (coorFlag & 0x60) != 0 ? ((coorFlag & 0x20) != 0 ? 0.5f : 1.0f) * view->GetHeight() : 0.0f;
	if ((coorFlag & 0x100) != 0) {
		result.x = view->GetWidth() * rect.x / 640.0f + pivotX;
		result.y = view->GetHeight() * rect.y / 480.0f + pivotY;
	} else {
		result.x = pivotX + rect.x;
		result.y = pivotY + rect.y;
	}
	if ((coorFlag & 0x1000) != 0) {
		result.w = view->GetWidth() * rect.w / 640.0f;
		result.h = view->GetHeight() * rect.h / 480.0f;
	} else {
		result.w = rect.w;
		result.h = rect.h;
	}
	return result;
}

void WOverlay::DrawPicture(WView* view, const WRect& rect, int texhandle, int type, unsigned int diffuse) {
	m_vtx[0].x = rect.x - 0.5f;
	m_vtx[0].y = rect.y - 0.5f;
	m_vtx[0].z = 0.001f;
	m_vtx[0].tu = 0.0f;
	m_vtx[0].tv = 0.0f;
	m_vtx[0].rhw = 1.0f;
	m_vtx[0].diffuse = diffuse;
	m_vtx[1].x = rect.w + rect.x - 0.5f;
	m_vtx[1].y = rect.y - 0.5f;
	m_vtx[1].z = 0.001f;
	m_vtx[1].tu = 1.0f;
	m_vtx[1].tv = 0.0f;
	m_vtx[1].rhw = 1.0f;
	m_vtx[1].diffuse = diffuse;
	m_vtx[2].x = rect.x - 0.5f;
	m_vtx[2].y = rect.h + rect.y - 0.5f;
	m_vtx[2].z = 0.001f;
	m_vtx[2].tu = 0.0f;
	m_vtx[2].tv = 1.0f;
	m_vtx[2].rhw = 1.0f;
	m_vtx[2].diffuse = diffuse;
	m_vtx[3].x = rect.w + rect.x - 0.5f;
	m_vtx[3].y = rect.h + rect.y - 0.5f;
	m_vtx[3].z = 0.001f;
	m_vtx[3].tu = 1.0f;
	m_vtx[3].tv = 1.0f;
	m_vtx[3].rhw = 1.0f;
	m_vtx[3].diffuse = diffuse;
	view->DrawPolygonFan(m_vl, type | (texhandle & 0x7FF) | 0x20000000, 0, true);
}

void WOverlay::DrawFrameOverlay1(WView* view, WOverlay* frame, float sizeL, float sizeT, float sizeR, float sizeB,
                                 const RECT& rect, unsigned int color) {
	WRect srcRect[9];
	WRect frameRect[9];
	WRect r;
	float sizeLr = sizeL + sizeR;
	float sizeTb = sizeT + sizeB;
	auto texW = static_cast<float>(frame->m_texWidth);
	auto texH = static_cast<float>(frame->m_texHeight);
	float xOffset = 0.5f / texW, yOffset = 0.5f / texH;
	float invW = 1.0f / texW, invH = 1.0f / texH;
	float x = static_cast<float>(rect.left), y = static_cast<float>(rect.top);
	float w = static_cast<float>(rect.right), h = static_cast<float>(rect.bottom);

	frameRect[0].x = 0.0;
	frameRect[0].y = 0.0;
	frameRect[0].w = sizeL;
	frameRect[0].h = sizeT;

	frameRect[1].x = sizeL;
	frameRect[1].y = 0.0;
	frameRect[1].w = texW - sizeLr;
	frameRect[1].h = sizeT;

	frameRect[2].x = texW - sizeR;
	frameRect[2].y = 0.0;
	frameRect[2].w = sizeR;
	frameRect[2].h = sizeT;

	frameRect[3].x = 0.0;
	frameRect[3].y = sizeB;
	frameRect[3].w = sizeL;
	frameRect[3].h = texH - sizeTb;

	frameRect[4].x = sizeL;
	frameRect[4].y = sizeB;
	frameRect[4].w = texW - sizeLr;
	frameRect[4].h = texH - sizeTb;

	frameRect[5].x = texW - sizeR;
	frameRect[5].y = sizeB;
	frameRect[5].w = sizeR;
	frameRect[5].h = texH - sizeTb;

	frameRect[6].x = 0.0;
	frameRect[6].y = texH - sizeB;
	frameRect[6].w = sizeL;
	frameRect[6].h = sizeB;

	frameRect[7].x = sizeL;
	frameRect[7].y = texH - sizeB;
	frameRect[7].w = texW - sizeLr;
	frameRect[7].h = sizeB;

	frameRect[8].x = texW - sizeR;
	frameRect[8].y = texH - sizeB;
	frameRect[8].w = sizeR;
	frameRect[8].h = sizeB;

	srcRect[0].x = 0.0f * invW + xOffset;
	srcRect[0].y = 0.0f * invH + yOffset;
	srcRect[0].w = sizeL * invW - xOffset;
	srcRect[0].h = sizeT * invH - yOffset;

	srcRect[1].x = sizeL * invW + xOffset;
	srcRect[1].y = 0.0f * invH + yOffset;
	srcRect[1].w = (texW - sizeLr) * invW - xOffset;
	srcRect[1].h = sizeT * invH - yOffset;

	srcRect[2].x = (texW - sizeR) * invW + xOffset;
	srcRect[2].y = 0.0f * invH + yOffset;
	srcRect[2].w = sizeR * invW - xOffset;
	srcRect[2].h = sizeT * invH - yOffset;

	srcRect[3].x = 0.0f * invW + xOffset;
	srcRect[3].y = sizeB * invH + yOffset;
	srcRect[3].w = sizeL * invW - xOffset;
	srcRect[3].h = sizeL * invH - yOffset;

	srcRect[4].x = srcRect[1].x;
	srcRect[4].y = srcRect[3].y;
	srcRect[4].w = srcRect[1].w;
	srcRect[4].h = (texH - sizeTb) * invH - yOffset;

	srcRect[5].x = srcRect[2].x;
	srcRect[5].y = srcRect[3].y;
	srcRect[5].w = srcRect[2].w;
	srcRect[5].h = srcRect[4].h;

	srcRect[6].x = 0.0f * invW + xOffset;
	srcRect[6].y = (texH - sizeB) * invH + yOffset;
	srcRect[6].w = sizeL * invW - xOffset;
	srcRect[6].h = sizeB * invH - yOffset;

	srcRect[7].x = srcRect[1].x;
	srcRect[7].y = srcRect[6].y;
	srcRect[7].w = srcRect[1].w;
	srcRect[7].h = srcRect[6].h;

	srcRect[8].x = srcRect[2].x;
	srcRect[8].y = srcRect[6].y;
	srcRect[8].w = srcRect[2].w;
	srcRect[8].h = srcRect[6].h;

	r.x = x;
	r.y = y;
	r.w = sizeL;
	r.h = sizeT;
	frame->Render(view, srcRect[0], r, 0x2000000, color, 0.0, 0);

	r.x = x + sizeL;
	r.y = y;
	r.w = w - sizeLr;
	r.h = sizeT;
	frame->Render(view, srcRect[1], r, 0x2000000, color, 0.0, 0);

	r.x = w + x - sizeR;
	r.y = y;
	r.w = sizeR;
	r.h = sizeT;
	frame->Render(view, srcRect[2], r, 0x2000000, color, 0.0, 0);

	r.x = x;
	r.y = y + sizeT;
	r.w = sizeL;
	r.h = h - sizeTb;
	frame->Render(view, srcRect[3], r, 0x2000000, color, 0.0, 0);

	r.x = x + sizeL;
	r.y = y + sizeT;
	r.w = w - sizeLr;
	r.h = h - sizeTb;
	frame->Render(view, srcRect[4], r, 0x2000000, color, 0.0, 0);

	r.x = w + x - sizeR;
	r.y = y + sizeT;
	r.w = sizeR;
	r.h = h - sizeTb;
	frame->Render(view, srcRect[5], r, 0x2000000, color, 0.0, 0);

	r.x = x;
	r.y = h + y - sizeB;
	r.w = sizeL;
	r.h = sizeB;
	frame->Render(view, srcRect[6], r, 0x2000000, color, 0.0, 0);

	r.x = x + sizeL;
	r.y = h + y - sizeB;
	r.w = w - sizeLr;
	r.h = sizeB;
	frame->Render(view, srcRect[7], r, 0x2000000, color, 0.0, 0);

	r.x = w + x - sizeR;
	r.y = h + y - sizeB;
	r.w = sizeR;
	r.h = sizeB;
	frame->Render(view, srcRect[8], r, 0x2000000, color, 0.0, 0);
}

void WOverlay::DrawFrameOverlay9(WView* view, WOverlay** frames, const RECT& rect, unsigned int color) {
	auto sizeL = static_cast<float>(frames[3]->m_texWidth);
	auto sizeR = static_cast<float>(frames[5]->m_texWidth);
	auto sizeT = static_cast<float>(frames[1]->m_texHeight);
	auto sizeB = static_cast<float>(frames[7]->m_texHeight);
	auto sizeLr = sizeR + sizeL;
	auto sizeTb = sizeB + sizeT;
	auto x = static_cast<float>(rect.left);
	auto y = static_cast<float>(rect.top);
	auto w = static_cast<float>(rect.right);
	auto h = static_cast<float>(rect.bottom);

	WRect srcRect[9];
	srcRect[0].x = 0.0;
	srcRect[0].y = 0.0;
	srcRect[0].w = 1.0;
	srcRect[0].h = 1.0;
	srcRect[1].x = 0.0;
	srcRect[1].y = 0.0;
	srcRect[1].w = 1.0;
	srcRect[1].h = 1.0;
	srcRect[2].x = 0.0;
	srcRect[2].y = 0.0;
	srcRect[2].w = 1.0;
	srcRect[2].h = 1.0;
	srcRect[3].x = 0.0;
	srcRect[3].y = 0.0;
	srcRect[3].w = 1.0;
	srcRect[3].h = 1.0;
	srcRect[4].x = 0.0;
	srcRect[4].y = 0.0;
	srcRect[4].w = 1.0;
	srcRect[4].h = 1.0;
	srcRect[5].x = 0.0;
	srcRect[5].y = 0.0;
	srcRect[5].w = 1.0;
	srcRect[5].h = 1.0;
	srcRect[6].x = 0.0;
	srcRect[6].y = 0.0;
	srcRect[6].w = 1.0;
	srcRect[6].h = 1.0;
	srcRect[7].x = 0.0;
	srcRect[7].y = 0.0;
	srcRect[7].w = 1.0;
	srcRect[7].h = 1.0;
	srcRect[8].x = 0.0;
	srcRect[8].y = 0.0;
	srcRect[8].w = 1.0;
	srcRect[8].h = 1.0;

	WRect r;
	r.x = x;
	r.y = y;
	r.h = sizeT;
	r.w = sizeL;
	frames[0]->Render(view, srcRect[0], r, 0x2080000, color, 0.0, 0);

	r.x = x + sizeL;
	r.h = sizeT;
	r.w = w - sizeLr;
	r.y = y;
	frames[1]->Render(view, srcRect[1], r, 0x2080000, color, 0.0, 0);

	r.w = sizeR;
	r.x = w + x - sizeR;
	r.h = sizeT;
	r.y = y;
	frames[2]->Render(view, srcRect[2], r, 0x2080000, color, 0.0, 0);

	r.x = x;
	r.y = y + sizeT;
	r.w = sizeL;
	r.h = h - sizeTb;
	frames[3]->Render(view, srcRect[3], r, 0x2080000, color, 0.0, 0);

	r.x = x + sizeL;
	r.y = y + h - sizeTb;
	r.w = w - sizeLr;
	r.h = h - sizeTb;
	frames[4]->Render(view, srcRect[4], r, 0x2080000, color, 0.0, 0);

	r.x = w + x - sizeR;
	r.y = y + h - sizeTb;
	r.w = sizeR;
	r.h = h - sizeTb;
	frames[5]->Render(view, srcRect[5], r, 0x2080000, color, 0.0, 0);

	r.x = x;
	r.y = h + y - sizeB;
	r.w = sizeL;
	r.h = sizeB;
	frames[6]->Render(view, srcRect[6], r, 0x2080000, color, 0.0, 0);

	r.x = x + sizeL;
	r.y = h + y - sizeB;
	r.w = w - sizeLr;
	r.h = sizeB;
	frames[7]->Render(view, srcRect[7], r, 0x2080000, color, 0.0, 0);

	r.x = w + x - sizeR;
	r.y = h + y - sizeB;
	r.w = sizeR;
	r.h = sizeB;
	frames[8]->Render(view, srcRect[8], r, 0x2080000, color, 0.0, 0);
}

bool WOverlay::CrossRect(const WRect& r1, const WRect& r2, WRect* out) {
	out->x = (r1.x < r2.x ? r2 : r1).x;
	out->y = (r1.y < r2.y ? r2 : r1).y;
	out->w = (r1.w + r1.x > r2.w + r2.x ? r2.w + r2.x : r1.w + r1.x) - out->x;
	out->h = (r1.h + r1.y > r2.h + r2.y ? r2.h + r2.y : r1.h + r1.y) - out->y;
	return out->w < 0.0 || out->h < 0.0;
}

void WOverlay::SetClippingArea(WView* view, const WRect* rect) {
	if (!rect && !view) {
		this->m_clipFlag = false;
		return;
	}

	this->m_clipFlag = true;
	if (rect) {
		this->m_clipArea = *rect;
	}

	if (!view) {
		return;
	}

	if (rect) {
		WRect r2;
		r2.w = view->GetWidth();
		r2.h = view->GetHeight();
		r2.x = 0.0;
		r2.y = 0.0;
		WRect out;
		CrossRect(this->m_clipArea, r2, &out);
		this->m_clipArea.x = out.x;
		this->m_clipArea.y = out.y;
		this->m_clipArea.w = out.w;
		this->m_clipArea.h = out.h;
	} else {
		this->m_clipArea.x = 0.0;
		this->m_clipArea.y = 0.0;
		this->m_clipArea.w = view->GetWidth();
		this->m_clipArea.h = view->GetHeight();
	}
}

void WOverlay::RenderWithShear(WView* view, const WRect& src, const WRect& dest, float shear, int type,
                               unsigned int diffuse) {
	WRect convSrc;
	this->ConvertSourceRectByTextureSize(convSrc, src);

	WRect rect;
	rect.x = dest.x;
	rect.y = dest.y;
	rect.w = dest.w;
	rect.h = dest.h;
	this->DrawTexture(view, this->m_texHandle, convSrc, ConvertRect(view, rect, this->m_coordMode), shear, type,
	                  diffuse);
}

void WOverlay::DrawTexture(WView* view, int texHandle, const WRect& src, const WRect& dest, int type,
                           unsigned int diffuse, float angle, uint8_t flipflag, bool clipping) const {
	WRect srcClipped, newSrc, newDest, temp;
	float ey, ex, sx, sy;

	newSrc.x = src.x;
	newSrc.y = src.y;
	newSrc.w = src.w;
	newSrc.h = src.h;

	if ((this->m_coordMode & 0x400) == 0) {
		newDest.x = dest.x;
		newDest.y = dest.y;
		newDest.h = dest.h;
		newDest.w = dest.w;
		if (this->m_clipFlag) {
			srcClipped.x = this->m_clipArea.x;
			srcClipped.y = this->m_clipArea.y;
			srcClipped.w = this->m_clipArea.w;
			srcClipped.h = this->m_clipArea.h;
		}
		sy = view->xGetProjScale();
		if (sy > 1.0) {
			view->xConvScreenRectByProjScale(&newDest);
			if (this->m_clipFlag) {
				view->xConvScreenRectByProjScale(&srcClipped);
			}
		}
	} else {
		sy = std::floor(dest.y);
		newDest.x = std::floor(dest.x);
		newDest.y = std::floor(dest.y);
		newDest.w = dest.w;
		newDest.h = dest.h;
		if (this->m_clipFlag) {
			srcClipped.x = std::floor(this->m_clipArea.x);
			sy = std::floor(this->m_clipArea.y);
			srcClipped.y = std::floor(this->m_clipArea.y);
			srcClipped.w = this->m_clipArea.w;
			srcClipped.h = this->m_clipArea.h;
		}
	}

	if (clipping) {
		WRect r2;
		r2.x = 0.0;
		r2.y = 0.0;
		r2.w = view->GetWidth();
		r2.h = view->GetHeight();
		if (CrossRect(newDest, r2, &temp) || this->m_clipFlag && CrossRect(newDest, srcClipped, &temp)) {
			return;
		}
		if (newDest.h != temp.h || newDest.w != temp.w) {
			if (newDest.w == temp.w) {
				newSrc.x = src.x;
				newSrc.w = src.w;
			} else {
				sx = src.w / newDest.w;
				sy = temp.x - newDest.x;
				srcClipped.x = (temp.x - newDest.x) * sx + src.x;
				srcClipped.w = (temp.x + temp.w - (newDest.w + newDest.x) - (temp.x - newDest.x)) * sx + src.w;
				newSrc.x = srcClipped.x;
				newSrc.w = srcClipped.w;
			}
			if (newDest.h == temp.h) {
				newSrc.y = src.y;
				newSrc.h = src.h;
			} else {
				sx = src.h / newDest.h;
				sy = temp.y - newDest.y;
				srcClipped.y = (temp.y - newDest.y) * sx + src.y;
				srcClipped.h = (temp.y + temp.h - (newDest.h + newDest.y) - (temp.y - newDest.y)) * sx + src.h;
				newSrc.y = srcClipped.y;
				newSrc.h = srcClipped.h;
			}
			newDest = temp;
		}
	}

	if ((this->m_coordMode & 0x400) != 0) {
		sx = newDest.x - 0.5f;
		ex = newDest.x - 0.5f + newDest.w;
		sy = newDest.y - 0.5f;
		ey = newDest.y - 0.5f + newDest.h;
	}

	for (int i = 0; i < 4; i++) {
		if ((this->m_coordMode & 0x400) != 0) {
			m_vtx[i].x = (i & 1) != 0 ? ex : sx;
			m_vtx[i].y = (i & 2) != 0 ? ey : sy;
		} else {
			m_vtx[i].x = ((i & 1) != 0 ? newDest.w + newDest.x : newDest.x) - 0.5f;
			m_vtx[i].y = ((i & 2) != 0 ? newDest.h + newDest.y : newDest.y) - 0.5f;
		}
		m_vtx[i].z = 0.001f;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].diffuse = diffuse;
		m_vtx[i].tu = (i & 1) == ((flipflag & 1) == 0) ? newSrc.w + newSrc.x : newSrc.x;
		m_vtx[i].tv = (i & 2) == ((flipflag & 2) == 0) ? newSrc.h + newSrc.y : newSrc.y;
	}

	float scale = angle >= 0.0f ? angle : -angle;
	if (scale >= 0.00001f) {
		float px = newDest.w * 0.5f + newDest.x;
		float py = newDest.h * 0.5f + newDest.y;
		m_vtx[0].x = (m_vtx[0].x - px) * std::cos(angle) - (m_vtx[0].y - py) * std::sin(angle) + px;
		m_vtx[0].y = (m_vtx[0].x - px) * std::sin(angle) + (m_vtx[0].y - py) * std::cos(angle) + py;
		m_vtx[1].x = (m_vtx[1].x - px) * std::cos(angle) - (m_vtx[1].y - py) * std::sin(angle) + px;
		m_vtx[1].y = (m_vtx[1].x - px) * std::sin(angle) + (m_vtx[1].y - py) * std::cos(angle) + py;
		m_vtx[2].x = (m_vtx[2].x - px) * std::cos(angle) - (m_vtx[2].y - py) * std::sin(angle) + px;
		m_vtx[2].y = (m_vtx[2].x - px) * std::sin(angle) + (m_vtx[2].y - py) * std::cos(angle) + py;
		m_vtx[3].x = (m_vtx[3].x - px) * std::cos(angle) - (m_vtx[3].y - py) * std::sin(angle) + px;
		m_vtx[3].y = (m_vtx[3].x - px) * std::sin(angle) + (m_vtx[3].y - py) * std::cos(angle) + py;
	}

	view->DrawPolygonFan(m_vl, type | (texHandle & 0x7FF) | 0x20300000, 0, true);
}

void WOverlay::DrawArcClipTexture(WView* view, int texHandle, const WRect& src, const WRect& dest, float startang,
                                  float endang, int type, unsigned int diffuse) {
	WtVertex eVtx, sVtx, cVtx;

	m_vtx[0].x = dest.w + dest.x - 0.5f;
	m_vtx[0].y = dest.y - 0.5f;
	m_vtx[0].z = 0.001f;
	m_vtx[0].tu = src.x + src.w;
	m_vtx[0].tv = src.y;
	m_vtx[0].rhw = 1.0f;
	m_vtx[0].diffuse = diffuse;

	m_vtx[1].x = dest.x - 0.5f;
	m_vtx[1].y = dest.y - 0.5f;
	m_vtx[1].z = 0.001f;
	m_vtx[1].tu = src.x;
	m_vtx[1].tv = src.y;
	m_vtx[1].rhw = 1.0f;
	m_vtx[1].diffuse = m_vtx[0].diffuse;

	m_vtx[2].x = dest.w + dest.x - 0.5f;
	m_vtx[2].y = dest.h + dest.y - 0.5f;
	m_vtx[2].z = 0.001f;
	m_vtx[2].tu = src.x + src.w;
	m_vtx[2].tv = src.y + src.h;
	m_vtx[2].rhw = 1.0f;
	m_vtx[2].diffuse = m_vtx[0].diffuse;

	m_vtx[3].x = dest.x - 0.5f;
	m_vtx[3].y = dest.h + dest.y - 0.5f;
	m_vtx[3].z = 0.001f;
	m_vtx[3].tu = src.x;
	m_vtx[3].tv = src.y + src.h;
	m_vtx[3].rhw = 1.0f;
	m_vtx[3].diffuse = m_vtx[0].diffuse;

	this->m_clipArea.x = dest.w * 0.5f + dest.x;
	this->m_clipArea.y = dest.h * 0.5f + dest.y;
	this->m_clipArea.w = dest.w * 0.5f;
	this->m_clipArea.h = dest.h * 0.5f;

	eVtx.z = 0.001f;
	eVtx.diffuse = diffuse;
	eVtx.rhw = 1.0f;

	sVtx.z = 0.001f;
	sVtx.rhw = 1.0f;
	sVtx.diffuse = diffuse;

	cVtx.x = dest.w * 0.5f + dest.x;
	cVtx.y = dest.h * 0.5f + dest.y;
	cVtx.z = 0.001f;
	cVtx.tu = 0.5f;
	cVtx.tv = 0.5f;
	cVtx.rhw = 1.0f;
	cVtx.diffuse = diffuse;

	int sSec = this->GetSection(&sVtx, (atan(dest.h / dest.w) * g_pif) / g_pif, startang);
	int eSec = this->GetSection(&eVtx, (atan(dest.h / dest.w) * g_pif) / g_pif, endang);

	int i = 0;
	m_vtxList[i++] = &cVtx;
	m_vtxList[i++] = &sVtx;
	if (startang >= endang) {
		int n = (eSec - sSec + 8) / 2;
		if (n > 0) {
			int j = sSec / 2;
			do {
				m_vtxList[i++] = m_vl[j % 4];
				++j;
			} while (i < n);
		}
	} else {
		int n = (eSec - sSec) / 2;
		if (n > 0) {
			memcpy(&m_vtxList[2], &m_vl[sSec / 2], sizeof(WtVertex*) * n);
			i += n;
		}
	}
	m_vtxList[i++] = &eVtx;
	m_vtxList[i] = nullptr;
	view->DrawPolygonFan(m_vtxList, type | (texHandle & 0x7FF) | 0x20300000, 2048, true);
}

void WOverlay::clip_2D_left(WView* view, WtVertex* out, WtVertex* in, int* outlen, int inlen) {
	for (int i = 0; i < inlen; i++) {
		if (in[i].x > 0.0f) {
			out[*outlen].x = in[i].x;
			out[*outlen].y = in[i].y;
			out[*outlen].tu = in[i].tu;
			out[*outlen].tv = in[i].tv;
			(*outlen)++;
		}
		WtVertex* pNext = &in[(i + 1) % inlen];
		if (in[i].x * pNext->x < 0.0f) {
			float x1 = in[i].x <= 0.0f ? -in[i].x : in[i].x;
			float x2 = pNext->x <= 0.0f ? -pNext->x : pNext->x;
			float t = x1 / (x1 + x2);
			out[*outlen].x = 0.0f;
			out[*outlen].y = (pNext->y - in[i].y) * t + in[i].y;
			out[*outlen].tu = (pNext->tu - in[i].tu) * t + in[i].tu;
			out[*outlen].tv = (pNext->tv - in[i].tv) * t + in[i].tv;
			(*outlen)++;
		}
	}
}

void WOverlay::clip_2D_right(WView* view, WtVertex* out, WtVertex* in, int* outlen, int inlen) {
	*outlen = 0;
	for (int i = 0; i < inlen; i++) {
		if (in[i].x < view->GetWidth()) {
			out[*outlen].x = in[i].x;
			out[*outlen].y = in[i].y;
			out[*outlen].tu = in[i].tu;
			out[*outlen].tv = in[i].tv;
			(*outlen)++;
		}
		WtVertex* pNext = &in[(i + 1) % inlen];
		if ((in[i].x - view->GetWidth()) * (pNext->x - view->GetWidth()) < 0.0f) {
			float x1 = in[i].x - view->GetWidth() <= 0.0f ? -(in[i].x - view->GetWidth()) : in[i].x - view->GetWidth();
			float x2 = pNext->x - view->GetWidth() <= 0.0f
				? -(pNext->x - view->GetWidth())
				: pNext->x - view->GetWidth();
			float t = x1 / (x1 + x2);
			out[*outlen].x = view->GetWidth();
			out[*outlen].y = (pNext->y - in[i].y) * t + in[i].y;
			out[*outlen].tu = (pNext->tu - in[i].tu) * t + in[i].tu;
			out[*outlen].tv = (pNext->tv - in[i].tv) * t + in[i].tv;
			(*outlen)++;
		}
	}
}

void WOverlay::clip_2D_top(WView* view, WtVertex* out, WtVertex* in, int* outlen, int inlen) {
	*outlen = 0;
	for (int i = 0; i < inlen; i++) {
		if (in[i].y > 0.0f) {
			out[*outlen].x = in[i].x;
			out[*outlen].y = in[i].y;
			out[*outlen].tu = in[i].tu;
			out[*outlen].tv = in[i].tv;
			(*outlen)++;
		}
		WtVertex* pNext = &in[(i + 1) % inlen];
		if (pNext->y * in[i].y < 0.0f) {
			float y1 = in[i].y <= 0.0f ? -in[i].y : in[i].y;
			float y2 = pNext->y <= 0.0f ? -pNext->y : pNext->y;
			float t = y1 / (y1 + y2);
			out[*outlen].x = (pNext->x - in[i].x) * t + in[i].x;
			out[*outlen].y = 0.0f;
			out[*outlen].tu = (pNext->tu - in[i].tu) * t + in[i].tu;
			out[*outlen].tv = (pNext->tv - in[i].tv) * t + in[i].tv;
			(*outlen)++;
		}
	}
}

void WOverlay::clip_2D_bottom(WView* view, WtVertex* out, WtVertex* in, int* outlen, int inlen) {
	*outlen = 0;
	for (int i = 0; i < inlen; i++) {
		if (in[i].y < view->GetHeight()) {
			out[*outlen].x = in[i].x;
			out[*outlen].y = in[i].y;
			out[*outlen].tu = in[i].tu;
			out[*outlen].tv = in[i].tv;
			(*outlen)++;
		}
		WtVertex* pNext = &in[(i + 1) % inlen];
		if ((pNext->x - view->GetHeight()) * (in[i].y - view->GetHeight()) < 0.0f) {
			float y1 = in[i].y - view->GetHeight() <= 0.0f
				? -(in[i].y - view->GetHeight())
				: in[i].y - view->GetHeight();
			float y2 = pNext->y - view->GetHeight() <= 0.0f
				? -(pNext->y - view->GetHeight())
				: pNext->y - view->GetHeight();
			float t = y1 / (y1 + y2);
			out[*outlen].x = (pNext->x - in[i].x) * t + in[i].x;
			out[*outlen].y = view->GetHeight();
			out[*outlen].tu = (pNext->tu - in[i].tu) * t + in[i].tu;
			out[*outlen].tv = (pNext->tv - in[i].tv) * t + in[i].tv;
			(*outlen)++;
		}
	}
}

void WOverlay::Render(WView* view, const WRect& src, const WRect& dest, int type, unsigned int diffuse, float angle,
                      uint8_t flipflag) const {
	WRect convSrc;
	this->ConvertSourceRectByTextureSize(convSrc, src);
	WRect rect;
	rect.x = dest.x;
	rect.y = dest.y;
	rect.w = dest.w;
	rect.h = dest.h;
	this->DrawTexture(view, this->m_texHandle, convSrc, ConvertRect(view, rect, this->m_coordMode), type, diffuse,
	                  angle, flipflag, true);
}

void WOverlay::ArcClipRender(WView* view, const WRect& src, const WRect& dest, float startang, float endang, int type,
                             unsigned int diffuse) {
	WRect convSrc;
	this->ConvertSourceRectByTextureSize(convSrc, src);
	WRect rect;
	rect.x = dest.x;
	rect.y = dest.y;
	rect.w = dest.w;
	rect.h = dest.h;
	this->DrawArcClipTexture(view, this->m_texHandle, convSrc, ConvertRect(view, rect, this->m_coordMode), startang,
	                         endang, type, diffuse);
}

void WOverlay::DrawTextureWithAxis(WView* view, int texHandle, const WRect& src, const WRect& dest, float axisX,
                                   float axisY, float angle, int type, unsigned int diffuse, uint8_t flipFlag) {
	WtVertex outVtx[8];
	WtVertex inVtx[8];
	if (std::abs(angle) < 0.00001f) {
		this->DrawTexture(view, texHandle, src, dest, type, diffuse, 0.0, flipFlag, true);
		return;
	}
	for (int i = 0; i < 4; i++) {
		if ((this->m_coordMode & 0x400) != 0) {
			m_vtx[i].x = std::floor((i & 1) != 0 ? dest.w + dest.x : dest.x) - 0.5f;
			m_vtx[i].y = std::floor((i & 2) != 0 ? dest.h + dest.y : dest.y) - 0.5f;
		} else {
			m_vtx[i].x = ((i & 1) != 0 ? dest.w + dest.x : dest.x) - 0.5f;
			m_vtx[i].y = ((i & 2) != 0 ? dest.h + dest.y : dest.y) - 0.5f;
		}
		m_vtx[i].z = 0.001f;
		m_vtx[i].tu = (i & 1) == !flipFlag ? src.w + src.x : src.x;
		m_vtx[i].tv = (i & 2) == (flipFlag & 2) == 0 ? src.h + src.y : src.y;
		m_vtx[i].rhw = 1.0f;
		m_vtx[i].diffuse = diffuse;
	}
	float w = view->GetWidth(), h = view->GetHeight();
	int clipFlag = 0;
	m_vtx[0].x = (m_vtx[0].x - axisX) * std::cos(angle) - (m_vtx[0].y - axisY) * std::sin(angle) + axisX;
	m_vtx[0].y = (m_vtx[0].x - axisX) * std::sin(angle) + (m_vtx[0].y - axisY) * std::cos(angle) + axisY;
	if (m_vtx[0].x < 0) clipFlag |= 1;
	if (m_vtx[0].x > w) clipFlag |= 2;
	if (m_vtx[0].y < 0) clipFlag |= 4;
	if (m_vtx[0].y > h) clipFlag |= 8;
	m_vtx[1].x = (m_vtx[1].x - axisX) * std::cos(angle) - (m_vtx[1].y - axisY) * std::sin(angle) + axisX;
	m_vtx[1].y = (m_vtx[1].x - axisX) * std::sin(angle) + (m_vtx[1].y - axisY) * std::cos(angle) + axisY;
	if (m_vtx[1].x < 0) clipFlag |= 1;
	if (m_vtx[1].x > w) clipFlag |= 2;
	if (m_vtx[1].y < 0) clipFlag |= 4;
	if (m_vtx[1].y > h) clipFlag |= 8;
	m_vtx[2].x = (m_vtx[2].x - axisX) * std::cos(angle) - (m_vtx[2].y - axisY) * std::sin(angle) + axisX;
	m_vtx[2].y = (m_vtx[2].x - axisX) * std::sin(angle) + (m_vtx[2].y - axisY) * std::cos(angle) + axisY;
	if (m_vtx[2].x < 0) clipFlag |= 1;
	if (m_vtx[2].x > w) clipFlag |= 2;
	if (m_vtx[2].y < 0) clipFlag |= 4;
	if (m_vtx[2].y > h) clipFlag |= 8;
	m_vtx[3].x = (m_vtx[3].x - axisX) * std::cos(angle) - (m_vtx[3].y - axisY) * std::sin(angle) + axisX;
	m_vtx[3].y = (m_vtx[3].x - axisX) * std::sin(angle) + (m_vtx[3].y - axisY) * std::cos(angle) + axisY;
	if (m_vtx[3].x < 0) clipFlag |= 1;
	if (m_vtx[3].x > w) clipFlag |= 2;
	if (m_vtx[3].y < 0) clipFlag |= 4;
	if (m_vtx[3].y > h) clipFlag |= 8;
	memcpy(inVtx, m_vtx, sizeof(WtVertex) * 4);
	int outLen = 4;
	if ((clipFlag & 1) != 0) {
		this->clip_2D_left(view, outVtx, inVtx, &outLen, outLen);
		memcpy(inVtx, outVtx, sizeof(WtVertex) * outLen);
	}
	if ((clipFlag & 2) != 0) {
		this->clip_2D_right(view, outVtx, inVtx, &outLen, outLen);
		memcpy(inVtx, outVtx, sizeof(WtVertex) * outLen);
	}
	if ((clipFlag & 4) != 0) {
		this->clip_2D_top(view, outVtx, inVtx, &outLen, outLen);
		memcpy(inVtx, outVtx, sizeof(WtVertex) * outLen);
	}
	if ((clipFlag & 8) != 0) {
		this->clip_2D_bottom(view, outVtx, inVtx, &outLen, outLen);
		memcpy(inVtx, outVtx, sizeof(WtVertex) * outLen);
	}
	if (!clipFlag) {
		view->DrawPolygonFan(m_vl, type | (texHandle & 0x7FF) | 0x20300000, 1024, true);
		return;
	}
	if (!outLen) {
		return;
	}
	for (int i = 0; i < outLen; i++) {
		m_vtxList[i] = &inVtx[i];
		m_vtxList[i]->z = 0.001f;
		m_vtxList[i]->rhw = 1.0f;
		m_vtxList[i]->diffuse = diffuse;
	}
	m_vtxList[outLen] = nullptr;
	view->DrawPolygonFan(m_vtxList, type | (texHandle & 0x7FF) | 0x20300000, 0, true);
}

void WOverlay::RenderWithAxis(WView* view, const WRect& src, const WRect& dest, float axisX, float axisY, float angle,
                              int type, unsigned int diffuse, uint8_t flipFlag) {
	WRect convSrc;
	this->ConvertSourceRectByTextureSize(convSrc, src);

	WRect rect;
	rect.x = dest.x;
	rect.y = dest.y;
	rect.w = dest.w;
	rect.h = dest.h;

	this->DrawTextureWithAxis(view, this->m_texHandle, convSrc, ConvertRect(view, rect, this->m_coordMode), axisX,
	                          axisY, angle, type, diffuse, flipFlag);
}
