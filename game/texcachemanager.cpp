#include "texcachemanager.h"

#include "bitmap.h"
#include "wresourcemanager.h"

TexCacheManager::TexCacheManager() = default;

TexCacheManager::~TexCacheManager() {
	this->m_Cache.Clear();
	this->m_CurCacheIdx = 0;
	this->m_infoMap.clear();
	this->ReleaseAllTextures();
}

const sTexCacheInfo* TexCacheManager::Draw(const Bitmap& bitmap) {
	const sTexCacheInfo* result = this->Get(bitmap);
	if (!result) {
		result = this->Add(bitmap);
	}
	return result;
}

int TexCacheManager::GetCacheTexture(int index) const {
	return this->m_aTexture[index];
}

void TexCacheManager::ReleaseAllTextures() {
	if (!this->m_aTexture) {
		return;
	}
	for (int i = 0; i < this->m_nMaxCache; ++i) {
		if (g_resourceManager && this->m_aTexture[i]) {
			g_resourceManager->Release(this->m_aTexture[i]);
			this->m_aTexture[i] = 0;
		}
	}
	if (this->m_aTexture) {
		delete this->m_aTexture;
		this->m_aTexture = nullptr;
	}
}

bool TexCacheManager::Init(int nMaxCache, int w, int h, int bw, int bh) {
	this->m_texHeight = h;
	this->m_nMaxCache = nMaxCache;
	this->m_CurCacheIdx = 0;
	this->m_texWidth = w;
	this->m_Cache.Init(w, h, bw, bh);
	this->ReleaseAllTextures();
	this->m_aTexture = new int[nMaxCache];
	memset(this->m_aTexture, 0, nMaxCache * sizeof(int));
	return true;
}

bool TexCacheManager::CreateTexture(unsigned int texIdx) const {
	Bitmap temp{int32_t(this->m_texWidth), int32_t(this->m_texHeight), 32};
	memset(temp.m_vram, 0, temp.m_pitch * temp.m_bi->bmiHeader.biHeight);
	this->m_aTexture[texIdx] = g_resourceManager->UploadTexture(nullptr, &temp, 0x80000000, 0);
	return this->m_aTexture[texIdx] > 0;
}

void TexCacheManager::FillTexture(const sTexCacheInfo& info) {
	RECT r;
	r.left = info.rcPixel.tl.x;
	r.top = info.rcPixel.tl.y;
	r.right = info.rcPixel.br.x;
	r.bottom = info.rcPixel.br.y;
	if (info.pBitmap->m_bi->bmiHeader.biBitCount == 32) {
		g_resourceManager->FixTexture(info.texHandle, info.pBitmap, 0, &r);
	}
}

void TexCacheManager::UpdateTextureCacheInfo(const sTexCacheInfo& info) {
	RECT r;
	r.left = info.rcPixel.tl.x;
	r.top = info.rcPixel.tl.y;
	r.right = info.rcPixel.br.x;
	r.bottom = info.rcPixel.br.y;
	if (info.pBitmap->m_bi->bmiHeader.biBitCount == 32) {
		g_resourceManager->FixTexture(info.texHandle, info.pBitmap, 0, &r);
	}
}

void TexCacheManager::ClearAll() {
	this->m_Cache.Clear();
	this->m_infoMap.clear();
}

void TexCacheManager::Clear(int idx) {
	this->m_Cache.Clear();

	for (auto it = this->m_infoMap.cbegin(); it != this->m_infoMap.cend();) {
		if (it->second.idx == idx) {
			this->m_infoMap.erase(it++);
		} else {
			++it;
		}
	}
}

const sTexCacheInfo* TexCacheManager::Get(const Bitmap& bitmap) {
	auto it = this->m_infoMap.find(&bitmap);
	if (it != this->m_infoMap.end()) {
		return &it->second;
	}
	return nullptr;
}

void TexCacheManager::RefreshTexCache(const Bitmap& bitmap) {
	auto it = this->m_infoMap.find(&bitmap);
	if (it != this->m_infoMap.end()) {
		return this->UpdateTextureCacheInfo(it->second);
	}
}

void TexCacheManager::InvalidateCache(const Bitmap& bitmap) {
	auto it = this->m_infoMap.find(&bitmap);
	if (it == this->m_infoMap.end()) {
		return;
	}
	if (it->second.idx == this->m_CurCacheIdx) {
		this->Clear(it->second.idx);
	} else {
		this->m_infoMap.erase(it);
	}
}

const sTexCacheInfo* TexCacheManager::Add(const Bitmap& bitmap) {
	sTexCacheInfo& cacheInfo = this->m_infoMap[&bitmap];
	if (this->m_Cache.Add(cacheInfo, bitmap.m_bi->bmiHeader.biWidth, bitmap.m_bi->bmiHeader.biHeight)) {
		if (!this->m_aTexture[this->m_CurCacheIdx]) {
			this->CreateTexture(this->m_CurCacheIdx);
		}
		cacheInfo.texHandle = this->m_aTexture[this->m_CurCacheIdx];
		cacheInfo.idx = this->m_CurCacheIdx;
		cacheInfo.pBitmap = &bitmap;
		this->FillTexture(cacheInfo);
		return &cacheInfo;
	}
	this->m_CurCacheIdx = (this->m_CurCacheIdx + 1) % this->m_nMaxCache;
	this->Clear(this->m_CurCacheIdx);
	if (this->m_Cache.Add(cacheInfo, bitmap.m_bi->bmiHeader.biWidth, bitmap.m_bi->bmiHeader.biHeight)) {
		if (!this->m_aTexture[this->m_CurCacheIdx]) {
			this->CreateTexture(this->m_CurCacheIdx);
		}
		cacheInfo.texHandle = this->m_aTexture[this->m_CurCacheIdx];
		cacheInfo.idx = this->m_CurCacheIdx;
		cacheInfo.pBitmap = &bitmap;
		this->FillTexture(cacheInfo);
		return &cacheInfo;
	}
	auto it = this->m_infoMap.find(&bitmap);
	if (it != this->m_infoMap.end()) {
		this->m_infoMap.erase(it);
	}
	return nullptr;
}
