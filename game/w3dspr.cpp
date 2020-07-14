#include "w3dspr.h"

#include "wresourcemanager.h"

constexpr static int g_uv[4][2] = {
	{0, 0},
	{1, 0},
	{1, 1},
	{0, 1},
};

W3dSpr::W3dSpr() {
	this->m_handle = 0;
	this->m_texLoaded = false;

	this->vtx[0].tu = (g_uv[0][0] * 255.0f + 0.5f) / 256.0f;
	this->vtx[0].tv = (g_uv[0][1] * 255.0f + 0.5f) / 256.0f;
	this->vtx[0].diffuse = 0xFFFFFFFF;
	this->vtx[1].tu = (g_uv[1][0] * 255.0f + 0.5f) / 256.0f;
	this->vtx[1].tv = (g_uv[1][1] * 255.0f + 0.5f) / 256.0f;
	this->vtx[1].diffuse = 0xFFFFFFFF;
	this->vtx[2].tu = (g_uv[2][0] * 255.0f + 0.5f) / 256.0f;
	this->vtx[2].tv = (g_uv[2][1] * 255.0f + 0.5f) / 256.0f;
	this->vtx[2].diffuse = 0xFFFFFFFF;
	this->vtx[3].tu = (g_uv[3][0] * 255.0f + 0.5f) / 256.0f;
	this->vtx[3].tv = (g_uv[3][1] * 255.0f + 0.5f) / 256.0f;
	this->vtx[3].diffuse = 0xFFFFFFFF;

	this->vl[0] = &this->vtx[0];
	this->vl[1] = &this->vtx[1];
	this->vl[2] = &this->vtx[2];
	this->vl[3] = &this->vtx[3];
	this->vl[4] = nullptr;
}

W3dSpr::~W3dSpr() {
	if (!this->m_texLoaded || !this->m_resrcMng || !this->m_handle) {
		return;
	}

	this->m_resrcMng->Release(this->m_handle);
	this->m_handle = 0;
}

void W3dSpr::SetPos(const WVector& vec) {
	this->pos = vec;
}

void W3dSpr::Render(WView* view, int type, w_spr_align align) {
	if (!this->m_handle) {
		return;
	}

	WVector za;
	za.x = this->pos.x - view->GetCamera().pivot.x;
	za.y = this->pos.y - view->GetCamera().pivot.y;
	za.z = this->pos.z - view->GetCamera().pivot.z;
	za.Normalize();

	WVector xa;
	xa.x = za.z * VEC_UNIT_POS_Y.y - za.y * VEC_UNIT_POS_Y.z;
	xa.y = za.z * VEC_UNIT_POS_Y.x - xa.x * VEC_UNIT_POS_Y.y;
	xa.z = za.z * VEC_UNIT_POS_Y.z - za.z * VEC_UNIT_POS_Y.x;
	xa.Normalize();

	if (align) {
		if (align == W3dAniSpr::CAMERA_X_ALIGN) {
			za = VEC_UNIT_POS_Y;
		}
	} else {
		za = WCrossProduct(xa, za);
		za.Normalize();
	}

	this->vl[0]->x = xa.x * this->w1 + this->pos.x + za.x * this->h2;
	this->vl[0]->y = xa.y * this->w1 + this->pos.y + za.y * this->h2;
	this->vl[0]->z = xa.z * this->w1 + this->pos.z + za.z * this->h2;
	this->vl[1]->x = xa.x * this->w2 + this->pos.x + za.x * this->h2;
	this->vl[1]->y = xa.y * this->w2 + this->pos.y + za.y * this->h2;
	this->vl[1]->z = xa.z * this->w2 + this->pos.z + za.z * this->h2;
	this->vl[2]->x = xa.x * this->w2 + this->pos.x + za.x * this->h1;
	this->vl[2]->y = xa.y * this->w2 + this->pos.y + za.y * this->h1;
	this->vl[2]->z = xa.z * this->w2 + this->pos.z + za.z * this->h1;
	this->vl[3]->x = xa.x * this->w1 + this->pos.x + za.x * this->h1;
	this->vl[3]->y = xa.y * this->w1 + this->pos.y + za.y * this->h1;
	this->vl[3]->z = xa.y * this->w1 + this->pos.y + za.y * this->h1;
	view->DrawPolygonFan(this->vl, type | (this->m_handle & 0x7FF), 1024, false);
}

void W3dSpr::Rotate(float x, float y, float& tx, float& ty, float angle) const {
	tx = std::cos(angle) * x - std::sin(angle) * y;
	ty = std::sin(angle) * x + std::cos(angle) * y;
}

void W3dSpr::SetRect(float width, float height, float pivotX, float pivotY) {
	this->w1 = -pivotX;
	this->w2 = width - pivotX;
	this->h1 = -pivotY;
	this->h2 = height - pivotY;
}

void W3dSpr::SetColor(unsigned int color) {
	this->vl[0]->diffuse = color;
	this->vl[1]->diffuse = color;
	this->vl[2]->diffuse = color;
	this->vl[3]->diffuse = color;
}

int W3dSpr::LoadSprite(const char* filename, int type) {
	this->m_handle = this->m_resrcMng->LoadTexture(filename, type, 0, nullptr);
	if (!this->m_handle) {
		return 1;
	}
	this->m_texLoaded = true;

	char buf[260];
	sprintf_s(buf, sizeof(buf), "W3dSpr:%s", filename);
	this->SetLeakHint(buf);
	return 0;
}

int W3dSpr::AttachTexture(int texHandle) {
	this->m_handle = texHandle;
	return 1;
}
