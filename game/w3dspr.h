#pragma once
#include "w3danispr.h"
#include "wmath.h"
#include "wresource.h"
#include "wvideo.h"
#include "wview.h"

class W3dSpr : public WResource {
public:
	typedef W3dAniSpr::w_spr_align w_spr_align;

	W3dSpr();
	~W3dSpr();
	void SetPos(const WVector& vec);
	void Render(WView* view, int type, w_spr_align align);
	void Rotate(float x, float y, float& tx, float& ty, float angle) const;
	void SetRect(float width, float height, float pivotX, float pivotY);
	void SetColor(unsigned color);
	int LoadSprite(const char* filename, int type);
	int AttachTexture(int texHandle);

private:
	int m_handle = 0;
	float w1 = 0;
	float w2 = 0;
	float h1 = 0;
	float h2 = 0;
	WVector pos{};
	WtVertex vtx[4]{};
	WtVertex* vl[5]{};
	bool m_texLoaded = false;
};
