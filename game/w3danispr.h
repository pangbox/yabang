#pragma once
#include "wlist.h"
#include "wmath.h"
#include "wresource.h"
#include "wvideo.h"
#include "wview.h"

typedef struct _WSPRITE {
	int hTexture;
	float fStartU;
	float fStartV;
	float fEndU;
	float fEndV;
} WSPRITE;

class W3dAniSpr : public WResource {
public:
	enum w_spr_align {
		CAMERA_XY_ALIGN,
		CAMERA_X_ALIGN,
		X_ALIGN,
		Y_ALIGN,
		Z_ALIGN,
		NO_ALIGN,
	};

	W3dAniSpr();
	~W3dAniSpr();
	int GetSpriteNum() const;
	void SetPos(const WVector& vec);
	void Rotate(float x, float y, float& tx, float& ty, float angle) const;
	void SetRect(float width, float height, float pivotX, float pivotY);
	void SetColor(unsigned color);
	WSPRITE* FindSprite(int nSprNum);
	void Render(WView* view, int type, w_spr_align align, int nSprNum);
	void Render(WView* view, float angle, int type, int nSprNum);
	void Render(WView* view, const WVector& angle, int type, int nSprNum);
	int LoadSpritesInOneTexture(const char* filename, int type, float fSprSizeX, float fSprSizeY);

private:
	WList<int> m_TextureList{8, 0};
	WList<_WSPRITE*> m_SpriteList{8, 0};
	int m_nTotalSprite = 0;
	float w1 = 0;
	float w2 = 0;
	float h1 = 0;
	float h2 = 0;
	WVector pos{};
	WtVertex vtx[4]{};
	WtVertex* vl[5]{};
};
