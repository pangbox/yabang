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
