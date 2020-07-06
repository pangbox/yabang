#pragma once
#include "wlist.h"
#include "wresrccache.h"
#include "wlock.h"

class w_texlist;
class WVideoDev;
class WPuppet;
class WSoundFx;
class WFont;
class WResource;

class WResourceManager {
public:

private:
	struct w_match_filename {
		char fullname[1];
	};
	
	WVideoDev* video;
	struct WAudioDev* audio;
	unsigned int m_nByteUsedTexture;
	int m_blankTexture;
	bool m_savemem[3];
	bool m_matchDirectory;
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
