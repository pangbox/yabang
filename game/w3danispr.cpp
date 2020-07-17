#include "w3danispr.h"

#include "wresourcemanager.h"

constexpr static int g_uv[4][2] = {
	{0, 0},
	{1, 0},
	{1, 1},
	{0, 1},
};

constexpr static int g_match[4][2] = {
	{1, 2},
	{2, 2},
	{2, 1},
	{1, 1},
};

W3dAniSpr::W3dAniSpr() {
	this->m_nTotalSprite = 0;

	this->vtx[0].tv = (g_uv[0][0] * 255.0f + 0.5f) / 256.0f;
	this->vtx[0].tu = (g_uv[0][1] * 255.0f + 0.5f) / 256.0f;
	this->vtx[0].diffuse = 0xFFFFFFFF;
	this->vtx[1].tv = (g_uv[1][0] * 255.0f + 0.5f) / 256.0f;
	this->vtx[1].tu = (g_uv[1][1] * 255.0f + 0.5f) / 256.0f;
	this->vtx[1].diffuse = 0xFFFFFFFF;
	this->vtx[2].tv = (g_uv[2][0] * 255.0f + 0.5f) / 256.0f;
	this->vtx[2].tu = (g_uv[2][1] * 255.0f + 0.5f) / 256.0f;
	this->vtx[2].diffuse = 0xFFFFFFFF;
	this->vtx[3].tv = (g_uv[3][0] * 255.0f + 0.5f) / 256.0f;
	this->vtx[3].tu = (g_uv[3][1] * 255.0f + 0.5f) / 256.0f;
	this->vtx[3].diffuse = 0xFFFFFFFF;

	this->vl[0] = &this->vtx[0];
	this->vl[1] = &this->vtx[1];
	this->vl[2] = &this->vtx[2];
	this->vl[3] = &this->vtx[3];
	this->vl[4] = nullptr;
}

W3dAniSpr::~W3dAniSpr() {
	this->AllDelSprite();
	for (auto hTexture = this->m_TextureList.Start(); hTexture != 0; hTexture = this->m_TextureList.Next()) {
		this->m_resrcMng->Release(hTexture);
		this->m_TextureList.DelItem(hTexture);
	}
}

int W3dAniSpr::GetSpriteNum() const {
	return this->m_nTotalSprite;
}

void W3dAniSpr::SetPos(const WVector& vec) {
	this->pos = vec;
}

void W3dAniSpr::Rotate(float x, float y, float& tx, float& ty, float angle) const {
	tx = std::cos(angle) * x - std::sin(angle) * y;
	ty = std::sin(angle) * x + std::cos(angle) * y;
}

void W3dAniSpr::SetRect(float width, float height, float pivotX, float pivotY) {
	this->w1 = -pivotX;
	this->w2 = width - pivotX;
	this->h1 = -pivotY;
	this->h2 = height - pivotY;
}

void W3dAniSpr::SetColor(unsigned int color) {
	this->vl[0]->diffuse = color;
	this->vl[1]->diffuse = color;
	this->vl[2]->diffuse = color;
	this->vl[3]->diffuse = color;
}

WSPRITE* W3dAniSpr::FindSprite(int nSprNum) {
	WSPRITE* sprite = this->m_SpriteList.Start();
	for (int i = 0; i < nSprNum; ++i) {
		if (!sprite) {
			break;
		}
		sprite = this->m_SpriteList.Next();
	}
	return sprite;
}

void W3dAniSpr::Render(WView* view, int type, w_spr_align align, int nSprNum) {
	WSPRITE* pSpr = this->FindSprite(nSprNum);
	if (!pSpr) {
		return;
	}

	this->vtx[0].tu = pSpr->fStartV;
	this->vtx[0].tv = pSpr->fStartU;
	this->vtx[1].tu = pSpr->fEndV;
	this->vtx[1].tv = pSpr->fStartU;
	this->vtx[2].tu = pSpr->fEndV;
	this->vtx[2].tv = pSpr->fEndU;
	this->vtx[3].tu = pSpr->fStartV;
	this->vtx[3].tv = pSpr->fEndU;

	const auto& camera = view->GetCamera();

	switch (align) {
		case CAMERA_XY_ALIGN:
			this->vl[0]->pos = this->pos + camera.xa * this->w1 + camera.ya * this->h2;
			this->vl[1]->pos = this->pos + camera.xa * this->w2 + camera.ya * this->h2;
			this->vl[2]->pos = this->pos + camera.xa * this->w2 + camera.ya * this->h1;
			this->vl[3]->pos = this->pos + camera.xa * this->w1 + camera.ya * this->h1;
			break;
		case CAMERA_X_ALIGN:
			this->vl[0]->pos = this->pos + camera.xa * this->w1 + VEC_UNIT_POS_Y * this->h2;
			this->vl[1]->pos = this->pos + camera.xa * this->w2 + VEC_UNIT_POS_Y * this->h2;
			this->vl[2]->pos = this->pos + camera.xa * this->w2 + VEC_UNIT_POS_Y * this->h1;
			this->vl[3]->pos = this->pos + camera.xa * this->w1 + VEC_UNIT_POS_Y * this->h1;
			break;
		case X_ALIGN:
			this->vl[0]->pos = this->pos + WVector{this->w1, this->h2, 0};
			this->vl[1]->pos = this->pos + WVector{this->w2, this->h2, 0};
			this->vl[2]->pos = this->pos + WVector{this->w2, this->h1, 0};
			this->vl[3]->pos = this->pos + WVector{this->w1, this->h1, 0};
			break;
		case Y_ALIGN:
			this->vl[0]->pos = this->pos + WVector{0, this->h2, this->w1};
			this->vl[1]->pos = this->pos + WVector{0, this->h2, this->w2};
			this->vl[2]->pos = this->pos + WVector{0, this->h1, this->w2};
			this->vl[3]->pos = this->pos + WVector{0, this->h1, this->w1};
			break;
		case Z_ALIGN:
			this->vl[0]->pos = this->pos + WVector{this->w1, 0, this->h2};
			this->vl[1]->pos = this->pos + WVector{this->w2, 0, this->h2};
			this->vl[2]->pos = this->pos + WVector{this->w2, 0, this->h1};
			this->vl[3]->pos = this->pos + WVector{this->w1, 0, this->h1};
			break;
		default:
			break;
	}

	if ((type & 0x4000000) != 0) {
		this->vl[0]->diffuse = 0xFFFFFFFF;
		this->vl[1]->diffuse = 0xFFFFFFFF;
		this->vl[2]->diffuse = 0xFFFFFFFF;
		this->vl[3]->diffuse = 0xFFFFFFFF;
		view->DrawPolygonFan(this->vl, type, 1024, false);
	} else {
		view->DrawPolygonFan(this->vl, type | (pSpr->hTexture & 0x7FF), 1024, false);
	}
}

void W3dAniSpr::Render(WView* view, float angle, int type, int nSprNum) {
	WSPRITE* sprite = this->FindSprite(nSprNum);
	if (!sprite) {
		return;
	}

	auto camera = view->GetCamera();
	this->vtx[0].tu = sprite->fStartV;
	this->vtx[0].tv = sprite->fStartU;
	this->vtx[1].tu = sprite->fEndV;
	this->vtx[1].tv = sprite->fStartU;
	this->vtx[2].tu = sprite->fEndV;
	this->vtx[2].tv = sprite->fEndU;
	this->vtx[3].tu = sprite->fStartV;
	this->vtx[3].tv = sprite->fEndU;

	for (int i = 0; i < 4; i++) {
		float w = g_match[i][0] == 1 ? this->w1 : this->w2;
		float h = g_match[i][1] == 1 ? this->h1 : this->h2;
		float s = std::sin(angle);
		float c = std::cos(angle);
		this->vl[i]->x = (w * c - h * s) * camera.xa.z + this->pos.z + (w * s + h * c) * camera.ya.z;
		this->vl[i]->y = (w * c - h * s) * camera.xa.y + this->pos.y + (w * s + h * c) * camera.ya.y;
		this->vl[i]->z = (w * c - h * s) * camera.xa.x + this->pos.x + (w * s + h * c) * camera.ya.x;
	}
	if ((type & 0x4000000) != 0) {
		this->vl[0]->diffuse = 0xFFFFFFFF;
		this->vl[1]->diffuse = 0xFFFFFFFF;
		this->vl[2]->diffuse = 0xFFFFFFFF;
		this->vl[3]->diffuse = 0xFFFFFFFF;
		view->DrawPolygonFan(this->vl, type, 1024, false);
	} else {
		view->DrawPolygonFan(this->vl, type | sprite->hTexture & 0x7FF, 1024, false);
	}
}

void W3dAniSpr::Render(WView* view, const WVector& angle, int type, int nSprNum) {
	WSPRITE* sprite = this->FindSprite(nSprNum);
	if (!sprite) {
		return;
	}
	this->vtx[0].tu = sprite->fStartV;
	this->vtx[0].tv = sprite->fStartU;
	this->vtx[1].tu = sprite->fEndV;
	this->vtx[1].tv = sprite->fStartU;
	this->vtx[2].tu = sprite->fEndV;
	this->vtx[2].tv = sprite->fEndU;
	this->vtx[3].tu = sprite->fStartV;
	this->vtx[3].tv = sprite->fEndU;
	for (int i = 0; i < 4; i++) {
		float w = g_match[i][0] == 1 ? this->w1 : this->w2;
		float h = g_match[i][1] == 1 ? this->h1 : this->h2;
		WMatrix r = RotMat(angle);
		this->vl[i]->x = h * r.ya.x + w * r.xa.x + r.za.x * 0.0f + r.pivot.x + this->pos.x;
		this->vl[i]->y = w * r.xa.y + h * r.ya.y + r.za.y * 0.0f + r.pivot.y + this->pos.y;
		this->vl[i]->z = w * r.xa.z + h * r.ya.z + r.za.z * 0.0f + r.pivot.z + this->pos.z;
	}
	if ((type & 0x4000000) != 0) {
		this->vl[0]->diffuse = -1;
		this->vl[1]->diffuse = -1;
		this->vl[2]->diffuse = -1;
		this->vl[3]->diffuse = -1;
		view->DrawPolygonFan(this->vl, type, 1024, false);
	} else {
		view->DrawPolygonFan(this->vl, type | (sprite->hTexture & 0x7FF), 1024, false);
	}
}

int W3dAniSpr::LoadSpritesInOneTexture(const char* filename, int type, float fSprSizeX, float fSprSizeY) {
	// TODO: Implement
	abort();
}

void W3dAniSpr::AllDelSprite() {
	WSPRITE* item = this->m_SpriteList.Start();
	while (item) {
		this->m_SpriteList.DelItem(item);
		delete item;
		--this->m_nTotalSprite;
	}
	this->m_SpriteList.Reset();
}
