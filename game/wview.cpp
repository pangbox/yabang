#include "wview.h"

#include "waabb.h"
#include "wresourcemanager.h"

WView* g_view = nullptr;

WtVertex WView::m_vtx[4]{};
WtVertex* WView::m_vl[5]{
	&m_vtx[0],
	&m_vtx[1],
	&m_vtx[2],
	&m_vtx[3],
	nullptr,
};

float WView::GetClipNearValue() const {
	return this->clip_near;
}

float WView::GetClipFarValue() const {
	return this->clip_far;
}

float WView::GetFOV() const {
	return this->SCREEN_YS / this->SCREEN_XS * this->FOV * 1.3333333f;
}

float WView::GetWidth() const {
	return this->SCREEN_XS;
}

float WView::GetHeight() const {
	return this->SCREEN_YS;
}

float WView::GetRatio() const {
	return this->SCREEN_YS / this->SCREEN_XS;
}

bool WView::InFrustum(const WSphere& sphere) const {
	for (auto p : this->frustum) {
		if (p.x * sphere.pos.x + p.z * sphere.pos.z + p.normal.p[1] * sphere.pos.y + p.dis > sphere.radius) {
			return false;
		}
	}
	return true;
}

bool WView::InFrustumSafe(const WSphere& sphere) const {
	for (auto p : this->frustumSafe) {
		if (p.x * sphere.pos.x + p.z * sphere.pos.z + p.normal.p[1] * sphere.pos.y + p.dis > sphere.radius) {
			return false;
		}
	}
	return true;
}

void WView::ResetClippingArea() {
	WRect rect;
	rect.x = 0.0;
	rect.y = 0.0;
	rect.w = this->SCREEN_XS;
	rect.h = this->SCREEN_YS;
	this->SetClippingArea(rect);
}

const WMatrix& WView::GetCamera() const {
	return this->camera;
}

WVideoDev* WView::GetVideoDevice() const {
	return this->m_resrcMng->VideoReference();
}

void WView::SetReflective(bool value) {
	this->m_isReflective = value;
}

const WxViewState& WView::xGetViewState() const {
	return this->m_xViewState;
}

bool WView::InFrustum(const Waabb& aabb) const {
	for (auto p : this->frustum) {
		float x = p.x >= 0.0f ? aabb.min.x : aabb.max.x;
		float y = p.y >= 0.0f ? aabb.min.y : aabb.max.y;
		float z = p.z >= 0.0f ? aabb.min.z : aabb.max.z;
		if (z * p.z + x * p.x + y * p.y + p.dis > 0.0f) {
			return false;
		}
	}
	return true;
}

float WView::GetScale() const {
	return this->m_scale;
}

bool WView::GetReflective() const {
	return this->m_isReflective;
}

WView::PROJECTION_MODE WView::GetProjectionMode() const {
	return this->m_projMode;
}

void WView::DrawProjPolygonFan(WtVertex** wl, int drawOption, int drawOption2) {
	this->DrawPolygonFan(wl, drawOption, drawOption2, true);
}

bool WView::InFrustum(const WVector& vec2) const {
	for (auto p : this->frustum) {
		if (p.x * vec2.x + p.z * vec2.z + p.normal.p[1] * vec2.y + p.dis > 0.0f) {
			return false;
		}
	}
	return true;
}

bool WView::InFrustumSafe(const Waabb& aabb) const {
	for (auto p : this->frustumSafe) {
		float x = p.x >= 0.0f ? aabb.min.x : aabb.max.x;
		float y = p.y >= 0.0f ? aabb.min.y : aabb.max.y;
		float z = p.z >= 0.0f ? aabb.min.z : aabb.max.z;
		if (z * p.z + x * p.x + y * p.y + p.dis > 0.0f) {
			return false;
		}
	}
	return true;
}

void WView::SetClippingArea(const WRect& rect) {
	this->m_clipArea.x = rect.x <= 0.0f ? 0.0f : rect.x;
	this->m_clipArea.y = rect.y <= 0.0f ? 0.0f : rect.y;
	float dx = this->SCREEN_XS - this->m_clipArea.x;
	this->m_clipArea.w = rect.w > dx ? dx : rect.w;
	float dy = this->SCREEN_YS - this->m_clipArea.y;
	this->m_clipArea.h = rect.h > dy ? dy : rect.h;
	if (this->m_resrcMng->VideoReference()) {
		this->m_resrcMng->VideoReference()->SetViewPort(static_cast<int>(this->m_clipArea.x),
		                                                static_cast<int>(this->m_clipArea.y),
		                                                static_cast<int>(this->m_clipArea.w),
		                                                static_cast<int>(this->m_clipArea.h));
	}
	this->update |= 2u;
}

void WView::ResetScreenCenter() {
	float y = this->SCREEN_YS * 0.5f;
	float x = this->SCREEN_XS * 0.5f;
	this->SetScreenCenter(x, y);
}

WVector2D WView::GetScreenCenter() const {
	return {this->m_center_x, this->m_center_y};
}

void WView::SetScreenCenter(const WVector2D& center) {
	this->SetScreenCenter(center.x, center.y);
}

void WView::SetScreenCenter(float x, float y) {
	if (x == this->m_center_x && y == this->m_center_y) {
		return;
	}
	this->update |= 0x2;
	this->m_center_x = x;
	this->m_center_y = y;
}

void WView::CheckReflectiveAndConvertCullFlag(int& drawFlag2) const {
	if (!this->m_isReflective) {
		return;
	}
	switch (drawFlag2 & 0xC00) {
		case 0x800:
			drawFlag2 &= ~0x800;
			break;
		case 0x000:
			drawFlag2 |= 0x800;
		default:
			break;
	}
}

void WView::SetClip(float nearplane, float farplane, bool setToRenderer) {
	this->clip_far = farplane;
	this->clip_near = nearplane;
	this->clip_scale_z = farplane / (farplane - nearplane);
	this->clip_near_scale = this->clip_scale_z * nearplane;
	this->clip_scaled_near = nearplane / this->clip_near_scale;
	this->clip_scaled_far = farplane / this->clip_near_scale;
	if (setToRenderer) {
		WVideoDev* videoDev = this->m_resrcMng->VideoReference();
		if (videoDev) {
			videoDev->m_clipScaleZ = this->clip_scale_z;
			videoDev->m_clipNearScale = this->clip_near_scale;
		}
	}
	this->update |= 0x2;
}

void WView::Render() const {
	WVideoDev* videoDev = this->m_resrcMng->VideoReference();
	videoDev->EndScene();
	videoDev->Paint();
}

void WView::SetPrevCamera(const WMatrix& mat) const {
	WMatrix4 mView;
	SetWMatrix4FromWMatrix(mView, ~mat);
	this->m_resrcMng->VideoReference()->XSetPrevViewTransform(mView);
}

void WView::SetCamera(const WMatrix& mat) {
	this->update |= 0x1;
	this->lastcam = mat;
}

bool WView::SetFogEnable(bool enable) const {
	if (this->m_resrcMng->VideoReference()) {
		return this->m_resrcMng->VideoReference()->SetFogEnable(enable);
	}
	return false;
}

void WView::SetFogState(float fogStart, float fogEnd, unsigned int color) const {
	if (this->m_resrcMng->VideoReference()) {
		this->m_resrcMng->VideoReference()->SetFogState(fogStart, fogEnd, color);
	}
}

void WView::SetViewport(float xs, float ys) {
	this->m_center_x = xs * 0.5f;
	this->m_clipArea.x = 0.0f;
	this->update |= 0x2;
	this->m_center_y = ys * 0.5f;
	this->m_clipArea.y = 0.0f;
	this->m_clipArea.w = xs;
	this->m_clipArea.h = ys;
	this->SCREEN_XS = xs;
	this->SCREEN_YS = ys;
}

void WView::SetFOV(float fov) {
	this->update |= 0x2;
	this->FOV = this->SCREEN_XS / this->SCREEN_YS * fov * 0.75f;
}

void WView::DrawLine2D(const WPoint& p1, const WPoint& p2, unsigned int diffuse, int type) {
	WtVertex vtx[2];
	WtVertex* vl[3];

	vtx[0].x = p1.x;
	vtx[0].y = p1.y;
	vtx[0].z = 0.001f;
	vtx[0].rhw = 1.0f;
	vtx[0].diffuse = diffuse;

	vtx[1].x = p2.x;
	vtx[1].y = p2.y;
	vtx[1].z = 0.001f;
	vtx[1].rhw = 1.0f;
	vtx[1].diffuse = diffuse;

	vl[0] = &vtx[0];
	vl[1] = &vtx[1];
	vl[2] = nullptr;

	this->DrawPolygonFan(vl, type | 0x4000000, 0, true);
}

void WView::DrawLine2D(const WPoint& p1, const WPoint& p2, unsigned int diffuse1, unsigned int diffuse2, int type) {
	WtVertex vtx[2];
	WtVertex* vl[3];

	vtx[0].x = p1.x;
	vtx[0].y = p1.y;
	vtx[0].z = 0.001f;
	vtx[0].rhw = 1.0f;
	vtx[0].diffuse = diffuse1;

	vtx[1].x = p2.x;
	vtx[1].y = p2.y;
	vtx[1].z = 0.001f;
	vtx[1].rhw = 1.0f;
	vtx[1].diffuse = diffuse2;

	vl[0] = &vtx[0];
	vl[1] = &vtx[1];
	vl[2] = nullptr;

	this->DrawPolygonFan(vl, type | 0x4000000, 0, true);
}

void WView::Draw2DTexture(const WRect& src, const WRect& dest, int texHandle, unsigned int diffuse, unsigned int type) {
	m_vtx[0].x = dest.x - 0.5f;
	m_vtx[0].y = dest.y - 0.5f;
	m_vtx[0].z = 0.001f;
	m_vtx[0].rhw = 1.0f;
	m_vtx[0].diffuse = diffuse;
	m_vtx[0].tu = src.x;
	m_vtx[0].tv = src.y;
	m_vtx[1].x = dest.w + dest.x - 0.5f;
	m_vtx[1].y = dest.y - 0.5f;
	m_vtx[1].z = 0.001f;
	m_vtx[1].rhw = 1.0f;
	m_vtx[1].diffuse = diffuse;
	m_vtx[1].tu = src.w + src.x;
	m_vtx[1].tv = src.y;
	m_vtx[2].x = dest.x - 0.5f;
	m_vtx[2].y = dest.h + dest.y - 0.5f;
	m_vtx[2].z = 0.001f;
	m_vtx[2].rhw = 1.0f;
	m_vtx[2].diffuse = diffuse;
	m_vtx[2].tu = src.x;
	m_vtx[2].tv = src.h + src.y;
	m_vtx[3].x = dest.w + dest.x - 0.5f;
	m_vtx[3].y = dest.h + dest.y - 0.5f;
	m_vtx[3].z = 0.001f;
	m_vtx[3].rhw = 1.0f;
	m_vtx[3].diffuse = diffuse;
	m_vtx[3].tu = src.w + src.x;
	m_vtx[3].tv = src.h + src.y;

	this->DrawPolygonFan(m_vl, type | (texHandle & 0x7FF) | 0x20380000, 0, true);
}

void WView::DrawLine(const WVector& v1, unsigned int c1, const WVector& v2, unsigned int c2, int type) {
	WtVertex vtx[2];
	WtVertex* p[3];

	vtx[0].pos = v1;
	vtx[0].diffuse = c1;
	vtx[1].pos = v2;
	vtx[1].diffuse = c2;

	p[0] = &vtx[0];
	p[1] = &vtx[1];
	p[2] = nullptr;

	this->DrawPolygonFan(p, type | 0x4000000, 0, false);
}

void WView::DrawAABB(const Waabb& aabb, unsigned int diffuse, bool solid, int type) {
	// Does nothing.
}

void WView::Clear(unsigned int clearColor, int mode) const {
	if (!this->m_resrcMng || !this->m_resrcMng->VideoReference()) {
		return;
	}

	switch (mode & 6) {
		case 2:
			this->m_resrcMng->VideoReference()->Clear(0, 2, 1.0f);
			break;
		case 4:
			this->m_resrcMng->VideoReference()->Clear(clearColor, 1, 1.0f);
			break;
		case 6:
			this->m_resrcMng->VideoReference()->Clear(clearColor, 3, 1.0f);
			break;
		default:
			break;
	}
}

void WView::EndScene() {
	// Does nothing.
}

float WView::xGetProjScale() const {
	return this->proj_scale;
}

void WView::xConvScreenRectByProjScale(WRect* rc) const {
	if (this->proj_scale <= 1.0f) {
		return;
	}
	rc->x = (rc->x - (this->left + 0.5f) * this->SCREEN_XS) * this->proj_scale;
	rc->y = (rc->y - (this->top + 0.5f) * this->SCREEN_YS) * this->proj_scale;
	rc->w = rc->w * this->proj_scale;
	rc->h = rc->h * this->proj_scale;
}

void WView::DrawPolygonFan(WtVertex** vl, int drawOption, int drawOption2, bool projected) {
	if (this->proj_scale <= 1.0) {
		WVideoDev* video = this->m_resrcMng->VideoReference();
		if ((drawOption & 0x4000000) != 0) {
			if (vl[2]) {
				float dx = vl[0]->x - vl[1]->x;
				float dy = vl[0]->y - vl[1]->y;
				if (dx <= 0.001f && dx >= -0.001f && dy <= 0.001f && dy >= -0.001f) {
					vl[1] = vl[2];
				}
			}
			if ((drawOption & 0x400000) != 0) {
				int i = 0;
				if (projected) {
					for (WtVertex* vtx = *vl; vtx; vtx = vl[i++]) {
						vtx->vz = std::numeric_limits<float>::lowest();
					}
				} else if (*vl) {
					for (WtVertex* vtx = *vl; vtx; vtx = vl[i++]) {
						vtx->vz = vtx->x * this->invcamera.xa.z
							+ vtx->y * this->invcamera.ya.z
							+ vtx->z * this->invcamera.za.z
							+ this->invcamera.pivot.z;
					}
				}
			}
			this->CheckViewAndProjTransformUpdateToVideo();
			if (this->m_isReflective) {
				switch (drawOption2 & 0xC00) {
					case 0x0:
						drawOption2 |= 0x800;
						break;
					case 0x800:
						drawOption2 &= ~0x800;
						break;
					default:
						break;
				}
			}
			if (this->m_bDisableFog) {
				drawOption &= ~0x8000000;
			}
			video->DrawPolygonFan(vl, drawOption, 2, drawOption2, 2 * projected + 2);
		} else {
			int iNum = 3;
			while (vl[iNum]) {
				iNum++;
			}
			if ((drawOption & 0x400000) != 0) {
				int i = 0;
				if (projected) {
					for (WtVertex* vtx = *vl; vtx; vtx = vl[i++]) {
						vtx->vz = std::numeric_limits<float>::lowest();
					}
				} else if (*vl) {
					for (WtVertex* vtx = *vl; vtx; vtx = vl[i++]) {
						vtx->vz = vtx->x * this->invcamera.xa.z
							+ vtx->y * this->invcamera.ya.z
							+ vtx->z * this->invcamera.za.z
							+ this->invcamera.pivot.z;
					}
				}
			}
			this->CheckViewAndProjTransformUpdateToVideo();
			if (this->m_isReflective) {
				switch (drawOption2 & 0xC00) {
					case 0x0:
						drawOption2 |= 0x800;
						break;
					case 0x800:
						drawOption2 &= ~0x800;
						break;
					default:
						break;
				}
			}
			if (this->m_bDisableFog) {
				drawOption &= ~0x8000000;
			}
			video->DrawPolygonFan(vl, drawOption, iNum, drawOption2, 2 * projected + 2);
		}
	}
}

void WView::CheckViewAndProjTransformUpdateToVideo() {
	if (this->m_xNeedToUpdateViewTransfToVideo) {
		this->m_xNeedToUpdateViewTransfToVideo = false;
		this->m_resrcMng->VideoReference()->XSetTransform(WTransformStateView, this->m_xViewState.xmView);
	}
	if (this->m_xNeedToUpdateProjTransfToVideo) {
		this->m_xNeedToUpdateProjTransfToVideo = false;
		this->m_resrcMng->VideoReference()->XSetTransform(WTransformStateProjection, this->m_xViewState.xmProj);
	}
}
