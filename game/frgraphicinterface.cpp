#include "frgraphicinterface.h"

#include "frcursor.h"
#include "texcachemanager.h"
#include "wresourcemanager.h"
#include "wfixedfont.h"

FrGraphicInterface::FrGraphicInterface(WView* pView) {
	this->m_pView = pView;
	this->m_pCacheManager = nullptr;
	this->m_pFont12 = nullptr;
	this->m_maxTexCache = 32;
	this->m_cacheTexSize = 256;
	this->m_pCacheManager = new TexCacheManager();
	this->m_pCacheManager->Init(this->m_maxTexCache, this->m_cacheTexSize, this->m_cacheTexSize, 4, 4);
	this->m_pFont12 = g_resourceManager->GetFixedFont("fnt_돋움체_12.wft");
	if (this->m_pFont12) {
		this->m_pFont12->SetMode(FrCursor::ROLL_OVER);
	}
	this->m_pFont11 = g_resourceManager->GetFixedFont("fnt_돋움체_11.wft");
	if (this->m_pFont11) {
		this->m_pFont11->SetMode(FrCursor::ROLL_OVER);
	}
	this->m_textStyle = 0;
	this->m_textColor = 0xFF000000;
	this->m_textOutlineColor = -1;
	this->m_alpha = 1.0;
}

FrGraphicInterface::~FrGraphicInterface() {
	if (this->m_pCacheManager) {
		delete this->m_pCacheManager;
		this->m_pCacheManager = nullptr;
	}
	if (g_resourceManager) {
		if (this->m_pFont12) {
			g_resourceManager->Release(this->m_pFont12);
			this->m_pFont12 = nullptr;
		}
		if (this->m_pFont11) {
			g_resourceManager->Release(this->m_pFont11);
			this->m_pFont11 = nullptr;
		}
	}
}

void FrGraphicInterface::Line(const WPoint& p1, const WPoint& p2, unsigned int diffuse1, unsigned int diffuse2,
                              unsigned int type) {
	g_view->DrawLine2D(p1, p2, diffuse1, diffuse2, type);
}

void FrGraphicInterface::Box(const WRect& rect, unsigned int diffuse, unsigned int type, float depth) {
	WOverlay::DrawBox(g_view, rect, type, diffuse, depth);
}

unsigned int FrGraphicInterface::GetTextColor() const {
	return this->m_textColor;
}

void FrGraphicInterface::LineBox(const WRect& rect, unsigned int diffuse, unsigned int type, float unused) {
	WOverlay::DrawLineBox(g_view, rect, type, diffuse);
}

float FrGraphicInterface::GetViewWidth() const {
	return this->m_pView ? this->m_pView->GetWidth() : 0.0;
}

float FrGraphicInterface::GetViewHeight() const {
	return this->m_pView ? this->m_pView->GetHeight() : 0.0;
}

void FrGraphicInterface::UpdateTextureCacheInfo(const Bitmap* pBitmap) {
	const sTexCacheInfo* cacheInfo = this->m_pCacheManager->Get(*pBitmap);
	if (!cacheInfo) {
		cacheInfo = this->m_pCacheManager->Add(*pBitmap);
	}
	if (!cacheInfo) {
		return;
	}
	this->m_pCacheManager->UpdateTextureCacheInfo(*cacheInfo);
}

void FrGraphicInterface::RefreshTextureCacheInfo(const Bitmap* pBitmap) {
	this->m_pCacheManager->RefreshTexCache(*pBitmap);
}

float FrGraphicInterface::GetAlpha() const {
	return this->m_alpha;
}
