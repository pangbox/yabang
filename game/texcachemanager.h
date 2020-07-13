#pragma once
#include <map>

#include "rectcache.h"

class Bitmap;

struct sTexCacheInfo : RectCache::RectInfo {
	int idx;
	const Bitmap* pBitmap;
	int texHandle;
};

class TexCacheManager {
public:
	TexCacheManager();
	~TexCacheManager();
	const sTexCacheInfo* Draw(const Bitmap& bitmap);
	int GetCacheTexture(int index) const;
	void ReleaseAllTextures();
	bool Init(int nMaxCache, int w, int h, int bw, int bh);
	bool CreateTexture(unsigned texIdx) const;
	void FillTexture(const sTexCacheInfo& info);
	void UpdateTextureCacheInfo(const sTexCacheInfo& info);
	void ClearAll();
	void Clear(int idx);
	const sTexCacheInfo* Get(const Bitmap& bitmap);
	void RefreshTexCache(const Bitmap& bitmap);
	void InvalidateCache(const Bitmap& bitmap);
	const sTexCacheInfo* Add(const Bitmap& bitmap);

private:
	int* m_aTexture = nullptr;
	RectCache m_Cache;
	int m_nMaxCache = 0;
	int m_CurCacheIdx = 0;
	unsigned int m_texWidth = 0;
	unsigned int m_texHeight = 0;
	std::map<const Bitmap*, sTexCacheInfo> m_infoMap;
};
