#pragma once
#include "spak.h"
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

#pragma pack(push,1)
struct bmp_info {
	int32_t size;
	int32_t width;
	int32_t height;
	int16_t planes;
	int16_t bitcount;
	int32_t compression;
	int32_t sizeimage;
	int32_t xpelspermeter;
	int32_t ypelspermeter;
	int32_t clrused;
	int32_t clrimportant;
};

struct bmp_head {
	int8_t signature[2];
	int32_t size;
	int16_t reserved1;
	int16_t reserved2;
	int32_t offbits;
};

struct sTgaHeader {
	uint8_t id_size;
	uint8_t color_map_type;
	uint8_t iType;
	int16_t color_map_origin;
	uint16_t color_map_length;
	uint8_t color_map_size;
	uint16_t origin_x;
	uint16_t origin_y;
	uint16_t width;
	uint16_t height;
	uint8_t bitcount;
	uint8_t descript;
};
#pragma pack(pop)

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
	void UnlockAllByThread(unsigned int threadId);
	void SetUseLessVideoRam(int level);
	void Release(WResource* resource);
	void Release(int hTex);
	w_texlist* FindTexture(int texHandle);
	w_texlist* FindTexture(const char* texName);
	w_texlist* AddTexture(const char* texName, int texHandle, int width, int height);
	WOverlay* GetOverlay(const char* filename, unsigned int flag);
	WOverlay* GetOverlay(const char* name, Bitmap* bitmap, unsigned int flag);
	WTitleFont* GetTitleFont();
	void Release(WTitleFont* font);
	W3dSpr* Get3DSpr(const char* filename, int type);
	W3dAniSpr* Get3DAniSpr(const char* filename, int type, float fSprSizeX, float fSprSizeY);
	void StrcpyLower(char* out, const char* src);
	const char* FindMatchFile(const char* filename);
	void AddOriginList(WResource* resource);
	WFixedFont* GetFixedFont(const char* filename);
	int UploadTexture(const char* texName, const Bitmap* bitmap, unsigned int type, RECT* rect);
	void FixTexture(int texHandle, const Bitmap* bitmap, unsigned int type, RECT* rect) const;
	Bitmap* Combine4Alpha(Bitmap* bitmap, Bitmap* alpha);
	bool CheckMaskTexture(Bitmap** ppBitmap, const char* filename);
	int LoadTexture(const char* filename, unsigned int type, int level, const char* texName);
	int GetTextureWidth(int texHandle);
	int GetTextureHeight(int texHandle);
	Bitmap* MakeQuarterBitmap(Bitmap* bitmap);
	void AddMissingTexture(const char* filename);
	Bitmap* LoadBRES(const char* zipName, const char* filename);
	Bitmap* LoadBMP(const char* filename);
	Bitmap* LoadJPG(const char* filename);
	Bitmap* LoadPNG(const char* filename, bool bFromMem, bool bNet);
	Bitmap* LoadTGA(const char* filename);
	Bitmap* LoadBitmapA(const char* filename, int level, bool bNet);
	cFile* GetCFile(const char* filename, int len);

private:
	WVideoDev* m_video = nullptr;
	WAudioDev* m_audio = nullptr;
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
