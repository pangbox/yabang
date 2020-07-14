#include "wresourcemanager.h"


#include "bitmap.h"
#include "w3dspr.h"
#include "woverlay.h"
#include "wresource.h"
#include "wtitlefont.h"
#include "wvideo.h"

WResourceManager* g_resourceManager = nullptr;

WVideoDev* WResourceManager::VideoReference() const {
	return this->video;
}

void WResourceManager::SetVideoReference(WVideoDev* video) {
	this->video = video;
}

const WList<w_texlist*>& WResourceManager::GetTextureList() const {
	return this->texList;
}

void WResourceManager::Lock(rmlType_t type) {
	this->m_lock[type].Lock();
}

void WResourceManager::Unlock(rmlType_t type) {
	this->m_lock[type].Unlock();
}

void WResourceManager::UnlockAllByThread(unsigned int threadID) {
	for (auto& lock : this->m_lock) {
		lock.UnlockInThread(threadID);
	}
}

void WResourceManager::SetUseLessVideoRam(int level) {
	this->m_savemem[0] = false;
	this->m_savemem[1] = level > 0;
	this->m_savemem[2] = level > 1;
}

void WResourceManager::Release(WResource* resrc) {
	delete resrc;
}

void WResourceManager::Release(int texhandle) {
	w_texlist* tex = this->FindTexture(texhandle);
	if (!tex) {
		return;
	}
	tex->count--;
	if (tex->count <= 0) {
		this->m_nByteUsedTexture -= tex->size;
		this->video->DestroyTexture(texhandle);
		this->texList.DelItem(tex);
		delete[] tex;
	}
}

w_texlist* WResourceManager::FindTexture(int texHandle) {
	for (auto* it = this->texList.Start(); it != nullptr; it = this->texList.Next()) {
		if (it->texhandle == texHandle) {
			return it;
		}
	}
	return nullptr;
}

WOverlay* WResourceManager::GetOverlay(const char* filename, unsigned int flag) {
	auto* overlay = new WOverlay();
	overlay->m_resrcMng = this;
	if (!filename || !overlay->Load(filename, flag)) {
		return overlay;
	}
	delete overlay;
	return nullptr;
}

WOverlay* WResourceManager::GetOverlay(const char* name, Bitmap* bitmap, unsigned int flag) {
	auto* overlay = new WOverlay();
	overlay->m_resrcMng = this;
	if (!name || !overlay->Load(name, bitmap, flag)) {
		return overlay;
	}
	delete overlay;
	return nullptr;
}

WTitleFont* WResourceManager::GetTitleFont() {
	auto* font = new WTitleFont();
	font->m_resrcMng = this;
	return font;
}

void WResourceManager::Release(WTitleFont* font) {
	delete font;
}

W3dSpr* WResourceManager::Get3DSpr(const char* filename, int type) {
	W3dSpr* spr = new W3dSpr();
	spr->m_resrcMng = this;
	if (!spr->LoadSprite(filename, type)) {
		return spr;
	}
	delete spr;
	return nullptr;
}

W3dAniSpr* WResourceManager::Get3DAniSpr(const char* filename, int type, float fSprSizeX, float fSprSizeY) {
	W3dAniSpr* spr = new W3dAniSpr();
	spr->m_resrcMng = this;
	if (!spr->LoadSpritesInOneTexture(filename, type, fSprSizeX, fSprSizeY)) {
		return spr;
	}
	delete spr;
	return nullptr;
}

void WResourceManager::StrcpyLower(char* out, const char* src) {
	for (; *src; src++) {
		*out++ = (*src >= 'A' && *src <= 'Z') ? (*src - 0x20) : *src;
	}
	*out = 0;
}

const char* WResourceManager::FindMatchFile(const char* filename) {
	char lowername[64] = {0};

	if (!this->m_matchDirectory) {
		return filename;
	}

	const char* basename = strrchr(filename, '/');
	if (!basename) {
		basename = strrchr(filename, '\\');
		if (!basename) {
			return filename;
		}
	}

	// Increment past the directory separator.
	basename++;

	StrcpyLower(lowername, basename);

	auto* match = this->m_matchList.Find(lowername);
	if (!match) {
		return filename;
	}

	return match->fullname;
}

WFixedFont* WResourceManager::GetFixedFont(const char* filename) {
	// TODO: Implement.
	abort();
}

int WResourceManager::UploadTexture(const char* texName, const Bitmap* bitmap, unsigned int type, RECT* rect) {
	// TODO: Implement.
	abort();
}

void WResourceManager::FixTexture(int texHandle, const Bitmap* bitmap, unsigned int type, RECT* rect) const {
	if (rect) {
		this->video->FixTexturePart(texHandle, *rect, bitmap->m_bi, bitmap->m_vram, type);
	} else {
		this->video->UpdateTexture(texHandle, bitmap->m_bi, bitmap->m_vram, type);
	}
}

int WResourceManager::LoadTexture(const char* filename, unsigned int type, int level, const char* texname) {
	// TODO: Implement.
	abort();
}

int WResourceManager::GetTextureWidth(int texHandle) {
	for (auto* it = this->texList.Start(); it != nullptr; it = this->texList.Next()) {
		if (it->texhandle == texHandle) {
			return it->width;
		}
	}
	return 0;
}

int WResourceManager::GetTextureHeight(int texHandle) {
	for (auto* it = this->texList.Start(); it != nullptr; it = this->texList.Next()) {
		if (it->texhandle == texHandle) {
			return it->height;
		}
	}
	return 0;
}
