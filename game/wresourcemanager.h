#pragma once
#include "w3dspr.h"
#include "wlist.h"
#include "wresrccache.h"
#include "wlock.h"
#include "woverlay.h"

struct w_texlist;
class WVideoDev;
class WPuppet;
class WSoundFx;
class WFont;
class WResource;
class WAudioDev;
class WFixedFont;
class WResourceManager;
class WTitleFont;

extern WResourceManager* g_resourceManager;

enum rmlType_t {
	RML_TEXTURE_LOADING = 0x0,
	RML_BITMAP_LOADING = 0x1,
	RML_PUPPET_LOADING = 0x2,
	RML_COUNT = 0x3,
};

struct w_texlist {
	int width;
	int height;
	int count;
	int texhandle;
	unsigned int size;
	char texname[1];
};

class WResourceManager {
public:
	struct w_match_filename {
		char fullname[1];
	};

	[[nodiscard]] WVideoDev* VideoReference() const;
	void SetVideoReference(WVideoDev* video);
	[[nodiscard]] const WList<w_texlist*>& GetTextureList() const;
	void Lock(rmlType_t type);
	void Unlock(rmlType_t type);
	void UnlockAllByThread(unsigned threadID);
	void SetUseLessVideoRam(int level);
	void Release(WResource* resrc);
	void Release(int texhandle);
	w_texlist* FindTexture(int texHandle);
	WOverlay* GetOverlay(const char* filename, unsigned flag);
	WOverlay* GetOverlay(const char* name, Bitmap* bitmap, unsigned flag);
	WTitleFont* GetTitleFont();
	void Release(WTitleFont* font);
	W3dSpr* Get3DSpr(const char* filename, int type);
	W3dAniSpr* Get3DAniSpr(const char* filename, int type, float fSprSizeX, float fSprSizeY);
	void StrcpyLower(char* out, const char* src);
	const char* FindMatchFile(const char* filename);
	WFixedFont* GetFixedFont(const char* filename);
	int UploadTexture(const char* texName, const Bitmap* bitmap, unsigned type, RECT* rect);
	void FixTexture(int texHandle, const Bitmap* bitmap, unsigned type, RECT* rect) const;
	int LoadTexture(const char* filename, unsigned type, int level, const char* texname);
	int GetTextureWidth(int texHandle);
	int GetTextureHeight(int texHandle);
	Bitmap* MakeQuarterBitmap(Bitmap* bitmap);
	void AddMissingTexture(const char* filename);
	Bitmap* LoadBRES(const char* zipname, const char* filename);
	Bitmap* LoadBMP(const char* filename);
	Bitmap* LoadJPG(const char* filename);
	Bitmap* LoadPNG(const char* filename, bool bFromMem, bool bNet);
	Bitmap* LoadTGA(const char* filename);
	Bitmap* LoadBitmapA(const char* filename, int level, bool bNet);

private:
	WVideoDev* video = nullptr;
	WAudioDev* audio = nullptr;
	unsigned int m_nByteUsedTexture = 0;
	int m_blankTexture = 0;
	bool m_savemem[3]{};
	bool m_matchDirectory = false;
	WList<char*> m_missingTextureList;
	WList<w_match_filename*> m_matchList;
	WList<w_texlist*> texList;
	WResrcCache<WPuppet*> puppetList;
	WResrcCache<WSoundFx*> soundList;
	WResrcCache<WFont*> fontList;
	WList<WSoundFx*> voiceList;
	WList<WResource*> originList;
	WLock m_lock[3];
};
