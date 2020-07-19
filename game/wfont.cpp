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

void WFont::Reset() {
}

void WFont::ResetOverlay() {
	for (auto *it = this->m_overlayList.Start(); it != nullptr; it = this->m_overlayList.Next()) {
		this->m_resrcMng->Release(it);
	}
	this->m_overlayList.Reset();
}

void WFont::Flush(WView* view) {
	if (!this->m_useOverlay && this->m_overlayList.Start() != nullptr) {
		this->ResetOverlay();
	}
	this->m_useOverlay = 0;
}