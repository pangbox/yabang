#include "wview.h"

#include "waabb.h"
#include "wresourcemanager.h"

WView* g_view = nullptr;
int g_foffset = 0;

WtVertex WView::m_vtx[4]{};
WtVertex* WView::m_vl[5]{
	&m_vtx[0],
	&m_vtx[1],
	&m_vtx[2],
	&m_vtx[3],
	nullptr,
};

// This seems pointless.
void (WView::* WView::m_fnUpdateCamera[2])() = {
	&WView::UpdateCamera_Perspective,
	&WView::UpdateCamera_Parallel,
};

WView::WView() {
	this->m_xViewState.xLight.type = 0;
	this->m_xViewState.xLight.diffuse = 0;
	this->m_xViewState.xLight.ambient = 0;
	this->m_xViewState.xLight.ambient2 = 0xFFFFFF;
	this->m_xViewState.xLight.nearOne.z = 0.0;
	this->m_xViewState.xLight.nearOne.y = 0.0;
	this->m_xViewState.xLight.nearOne.x = 0.0;
	this->update = 3;
	this->FOV = 1.1780972f;
	this->clip_near = 6.3999996f;
	this->clip_far = 1600.0f;
	this->clip_scale_z = 1.004016f;
	this->clip_near_scale = 6.4257021f;
	this->clip_scaled_near = 0.99600005f;
	this->clip_scaled_far = 249.00003f;
	this->m_fastclip = true;
	this->m_cliptype = 0;
	this->camera.pivot.z = 0.0;
	this->camera.pivot.y = 0.0;
	this->camera.pivot.x = 0.0;
	this->camera.za.y = 0.0;
	this->camera.za.x = 0.0;
	this->camera.ya.z = 0.0;
	this->camera.ya.x = 0.0;
	this->camera.xa.z = 0.0;
	this->camera.xa.y = 0.0;
	this->camera.za.z = 1.0;
	this->camera.ya.y = 1.0;
	this->camera.xa.x = 1.0;
	this->lastcam.za.z = 1.0;
	this->lastcam.ya.y = 1.0;
	this->lastcam.xa.x = 1.0;
	this->lastcam.pivot.z = 0.0;
	this->lastcam.pivot.y = 0.0;
	this->lastcam.pivot.x = 0.0;
	this->lastcam.za.y = 0.0;
	this->lastcam.za.x = 0.0;
	this->lastcam.ya.z = 0.0;
	this->lastcam.ya.x = 0.0;
	this->lastcam.xa.z = 0.0;
	this->lastcam.xa.y = 0.0;
	this->m_center_x = 0.5;
	this->m_center_y = 0.5;
	this->m_clipArea.x = 0.0;
	this->m_clipArea.y = 0.0;
	this->m_clipArea.w = 1.0;
	this->m_clipArea.h = 1.0;
	this->update |= 2u;
	this->right = 0.5;
	this->bottom = 0.5;
	this->SCREEN_XS = 1.0;
	this->SCREEN_YS = 1.0;
	this->m_bProcessEffect = true;
	this->proj_scale = 1.0;
	this->left = -0.5;
	this->top = -0.5;
	this->m_bDisableFog = false;
	this->m_isReflective = false;
	this->m_xViewState.xLight.type = 2;
	this->m_xViewState.xLight.nearOne = VEC_UNIT_NEG_Z;
	this->m_xViewState.xmView = MAT4_IDENTITY;
	this->m_xViewState.xmProj = MAT4_ZERO;
	this->update |= 2u;
	this->m_xNeedToUpdateViewTransfToVideo = true;
	this->m_xNeedToUpdateProjTransfToVideo = true;
	this->m_projMode = PERSPECTIVE;
	this->m_scale = 1.0;
	(this->*m_fnUpdateCamera[0])();
}

WView::~WView() = default;

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

bool WView::InFrustum(const WVector& vec3) const {
	for (auto p : this->frustum) {
		if (p.x * vec3.x + p.z * vec3.z + p.normal.p[1] * vec3.y + p.dis > 0.0f) {
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

void WView::BeginScene() {
	if (this->m_resrcMng) {
		WVideoDev* videoDev = this->m_resrcMng->VideoReference();
		videoDev->BeginScene();
		if (this->SCREEN_XS != videoDev->GetWidth() || this->SCREEN_YS != videoDev->GetHeight()) {
			this->SetViewport(static_cast<float>(videoDev->GetWidth()), static_cast<float>(videoDev->GetHeight()));
		}
	}
	this->update |= 0x3;
	(this->*m_fnUpdateCamera[this->m_projMode])();
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

void WView::DrawAABB(WMatrix& m, const Waabb& aabb, unsigned diffuse, bool solid, int type) {
	abort();
}

void WView::DrawOBB(const WMatrix& mat, unsigned diffuse) {
	abort();
}

void WView::DrawOBB(const Wobb& obb, unsigned diffuse) {
	abort();
}

void WView::DrawSphere(const WVector& pivot, float length, unsigned int diffuse, int seg, int offset) {
	WVector vtx[24];

	if (seg > 24) {
		seg = 24;
	}

	for (int i = 0; i < seg; i++) {
		float s = sin(static_cast<float>(i) * 6.2831855f / static_cast<float>(seg));
		float c = cos(static_cast<float>(i) * 6.2831855f / static_cast<float>(seg));
		vtx[i].x = length * this->camera.xa.x * c + pivot.x + length * this->camera.ya.x * s;
		vtx[i].y = length * this->camera.xa.y * c + pivot.y + length * this->camera.ya.y * s;
		vtx[i].z = length * this->camera.xa.y * c + pivot.z + length * this->camera.ya.z * s;
	}

	WtVertex v1, v2;
	WtVertex* vl[3];
	vl[0] = &v1;
	vl[1] = &v2;
	vl[2] = nullptr;

	for (int i = 0; i < seg; i++) {
		WVector& vtx2 = vtx[(i + 1) % seg];
		if (!offset || (i + 1) % (offset + 1)) {
			v1.pos = vtx[i];
			v1.diffuse = diffuse;
			v2.pos = vtx2;
			v2.diffuse = diffuse;
		} else {
			v1.pos = vtx[i];
			v1.diffuse = 0xFF00FF00;
			v2.pos = vtx2;
			v2.diffuse = 0xFF00FF00;
		}
		this->DrawPolygonFan(vl, 0x4000000, 0, false);
	}
}

void WView::DrawSphere(const WSphere& sphere, unsigned int diffuse, int seg, int offset) {
	this->DrawSphere(sphere.pos, sphere.radius, diffuse, seg, offset);
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

void WView::Flush(unsigned int flag) {
	this->m_resrcMng->VideoReference()->Command(WDeviceMessageFlush, flag, 0);
	g_foffset = 0;
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

void WView::Projection2_Parallel(WtVertex* p, const WVector& vec) {
	WVector v = vec * this->matrix;
	p->x = v.x + this->m_center_x;
	p->y = v.y + this->m_center_y;
	p->z = (vec.z - this->clip_near) / (this->clip_far - this->clip_near);
	p->rhw = 1.0;
}

void WView::UpdateProjectionTransform_Perspective() {
	if ((this->update & 2) == 0) {
		return;
	}

	this->update &= ~2;
	float fw = this->SCREEN_XS;
	float fh = this->SCREEN_YS;
	float halfw = fw * 0.5f;
	float halfh = fh * 0.5f;
	float clipl = (this->m_clipArea.x - halfw) / fw;
	float clipr = (this->m_clipArea.x + this->m_clipArea.w - halfw) / fw;
	float clipt = (this->m_clipArea.y - halfh) / fh;
	float clipb = (this->m_clipArea.y + this->m_clipArea.h - halfh) / fh;
	this->m_xViewState.xmProj.xa.x = this->proj_scale / std::tan(this->FOV * 0.5f) * (fw / this->m_clipArea.w);
	this->m_xViewState.xmProj.ya.y = this->m_xViewState.xmProj.xa.x / (fh / fw) * (this->SCREEN_YS / this->m_clipArea.h)
		/ (fw / this->m_clipArea.w);
	this->m_xViewState.xmProj.za.x = ((this->m_center_x - halfw) / halfw - (clipr + clipl)) / (clipr - clipl) + (this->
		left + this->right) / (this->left - this->right);
	this->m_xViewState.xmProj.za.y = (this->top + this->bottom) / (this->bottom - this->top) - ((this->m_center_y -
		halfh) / halfh - (clipb + clipt)) / (clipb - clipt);
	this->m_xViewState.xmProj.za.z = this->clip_scale_z;
	this->m_xViewState.xmProj.wz = 1.0f;
	this->m_xViewState.xmProj.pivot.z = -this->clip_near_scale;
	this->m_xViewState.xmProj.wm = 0.0f;
	this->m_xNeedToUpdateProjTransfToVideo = true;
}

void WView::UpdateProjectionTransform_Parallel() {
	if ((this->update & 0x2) == 0) {
		return;
	}

	this->update &= ~0x2;
	float fw = this->SCREEN_XS;
	float fh = this->SCREEN_YS;
	float halfw = fw * 0.5f;
	float halfh = fh * 0.5f;
	float clipl = (this->m_clipArea.x - halfw) / fw;
	float clipr = (this->m_clipArea.x + this->m_clipArea.w - halfw) / fw;
	float clipt = (this->m_clipArea.y - halfh) / fh;
	float clipb = (this->m_clipArea.y + this->m_clipArea.h - halfh) / fh;
	this->m_xViewState.xmProj.xa.x = (this->proj_scale + this->proj_scale) / this->m_scale * (fw / this->m_clipArea.w);
	this->m_xViewState.xmProj.ya.y = this->m_xViewState.xmProj.xa.x / (fh / fw) * (fh / this->m_clipArea.h)
		/ (fw / this->m_clipArea.w);
	this->m_xViewState.xmProj.za.x = ((this->m_center_x - halfw) / halfw - (clipr + clipl)) / (clipr - clipl) + (this->
		right + this->left) / (this->left - this->right);
	this->m_xViewState.xmProj.za.y = (this->top + this->bottom) / (this->bottom - this->top) - ((this->m_center_y -
		halfh) / halfh - (clipb + clipt)) / (clipb - clipt);
	this->m_xViewState.xmProj.za.z = 1.0f / (this->clip_far - this->clip_near);
	this->m_xViewState.xmProj.wz = 0.0f;
	this->m_xViewState.xmProj.pivot.z = -1.0f / (this->clip_far - this->clip_near) * this->clip_near;
	this->m_xViewState.xmProj.wm = 1.0f;
	this->m_xNeedToUpdateProjTransfToVideo = true;
}

void WView::ClipPlane(WtVertex* out, const WtVertex* a, const WtVertex* b, int type, const WPlane& p) {
	WVector va = a->pos;
	WVector vb = b->pos;
	float t1 = a->x * p.x + a->y * p.y + va.z * p.z + p.dis;
	float t2 = b->x * p.x + b->y * p.y + vb.z * p.z + p.dis;
	float t1a = t1 / (t1 - t2);
	float t2a = 1.0f - t1a;
	va.x = va.x * t2a + vb.x * t1a;
	va.y = va.y * t2a + vb.y * t1a;
	va.z = va.z * t2a + vb.z * t1a;
	this->Projection2_Parallel(out, va);
	out->tu = a->tu + (b->tu - a->tu) * t1a;
	out->tv = a->tv + (b->tv - a->tv) * t1a;
	out->lu = a->lu * (b->lu - a->lu) * t1a;
	out->lv = a->lv * (b->lv - a->lv) * t1a;
	out->diffuse = ULblend(a->diffuse, b->diffuse, uint8_t(t1a * 255.0f));
}

void WView::UpdateCamera_Perspective() {
	if (!this->update) {
		return;
	}

	this->camera = this->lastcam;
	this->scalex = this->SCREEN_XS * 0.5f;
	this->scaley = this->SCREEN_YS * 0.5f;
	float tan[2]{
		std::tan(this->FOV * 0.5f),
		this->SCREEN_YS / this->SCREEN_XS * std::tan(this->FOV * 0.5f),
	};
	float safetan[2]{
		(this->scalex - 4.0f) / (this->scalex / tan[0]),
		(this->scaley - 4.0f) / (this->scaley / tan[1]),
	};
	this->invcamera = ~this->camera;
	this->matrix = this->invcamera * WVector{
		1.0f / this->clip_near_scale * (this->scalex / tan[0]),
		1.0f / this->clip_near_scale * -(this->scaley / tan[1]),
		1.0f / this->clip_near_scale,
	};
	this->frustum[0].normal = RotVec(VEC_UNIT_NEG_Z, this->camera);
	this->frustum[0].dis = -(
		this->frustum[0].x * (this->clip_near * this->camera.za.x + this->camera.pivot.x) +
		this->frustum[0].y * (this->clip_near * this->camera.za.y + this->camera.pivot.y) +
		this->frustum[0].z * (this->clip_near * this->camera.za.z + this->camera.pivot.z)
	);
	this->frustumSafe[0] = this->frustum[0];
	this->frustum[1].normal = RotVec(VEC_UNIT_POS_Z, this->camera);
	this->frustum[1].dis = -(
		this->frustum[1].x * (this->clip_far * this->camera.za.x + this->camera.pivot.x) +
		this->frustum[1].y * this->clip_far * this->camera.za.z +
		this->frustum[1].z * (this->clip_far * this->camera.za.z + this->camera.pivot.z)
	);
	this->frustumSafe[1] = this->frustum[1];
	this->frustum[2].normal = RotVec({-1.0f, 0.0f, -tan[0]}, this->camera);
	this->frustum[2].normal.Normalize();
	this->frustum[2].dis = -(
		this->frustum[2].x * this->camera.pivot.x +
		this->frustum[2].y * this->camera.pivot.y +
		this->frustum[2].z * this->camera.pivot.z
	);
	this->frustumSafe[2].normal = RotVec({-1.0f, 0.0f, -safetan[0]}, this->camera);
	this->frustumSafe[2].normal.Normalize();
	this->frustumSafe[2].dis = -(
		this->frustumSafe[2].x * this->camera.pivot.x +
		this->frustumSafe[2].y * this->camera.pivot.y +
		this->frustumSafe[2].z * this->camera.pivot.z
	);
	this->frustum[3].normal = RotVec({1.0f, 0.0f, -tan[0]}, this->camera);
	this->frustum[3].normal.Normalize();
	this->frustum[3].dis = -(
		this->frustum[3].x * this->camera.pivot.x +
		this->frustum[3].y * this->camera.pivot.y +
		this->frustum[3].z * this->camera.pivot.z
	);
	this->frustumSafe[3].normal = RotVec({1.0f, 0.0f, -safetan[0]}, this->camera);
	this->frustumSafe[3].normal.Normalize();
	this->frustumSafe[3].dis = -(
		this->frustumSafe[3].x * this->camera.pivot.x +
		this->frustumSafe[3].y * this->camera.pivot.y +
		this->frustumSafe[3].z * this->camera.pivot.z
	);
	this->frustum[4].normal = RotVec({0.0f, 1.0f, -tan[1]}, this->camera);
	this->frustum[4].normal.Normalize();
	this->frustum[4].dis = -(
		this->frustum[4].x * this->camera.pivot.x +
		this->frustum[4].y * this->camera.pivot.y +
		this->frustum[4].z * this->camera.pivot.z
	);
	this->frustumSafe[4].normal = RotVec({0.0f, 1.0f, -safetan[1]}, this->camera);
	this->frustumSafe[4].normal.Normalize();
	this->frustumSafe[4].dis = -(
		this->frustumSafe[4].x * this->camera.pivot.x +
		this->frustumSafe[4].y * this->camera.pivot.y +
		this->frustumSafe[4].z * this->camera.pivot.z
	);
	this->frustum[5].normal = RotVec({0.0f, -1.0f, -tan[1]}, this->camera);
	this->frustum[5].normal.Normalize();
	this->frustum[5].dis = -(
		this->frustum[5].x * this->camera.pivot.x +
		this->frustum[5].y * this->camera.pivot.y +
		this->frustum[5].z * this->camera.pivot.z
	);
	this->frustumSafe[5].normal = RotVec({0.0f, -1.0f, -safetan[1]}, this->camera);
	this->frustumSafe[5].normal.Normalize();
	this->frustumSafe[5].dis = -(
		this->frustumSafe[5].x * this->camera.pivot.x +
		this->frustumSafe[5].y * this->camera.pivot.y +
		this->frustumSafe[5].z * this->camera.pivot.z
	);
	if ((this->update & 1) != 0) {
		this->update &= ~0x1;
		SetWMatrix4FromWMatrix(this->m_xViewState.xmView, this->invcamera);
		this->m_xNeedToUpdateViewTransfToVideo = true;
	}
	this->UpdateProjectionTransform_Perspective();
}

void WView::UpdateCamera_Parallel() {
	if (!this->update) {
		return;
	}

	this->camera = this->lastcam;
	this->invcamera = ~this->camera;
	this->matrix = this->invcamera;
	this->scalex = this->SCREEN_XS * 0.5f;
	this->scaley = this->SCREEN_YS * 0.5f;
	float scale = 2 * this->scalex / this->m_scale;
	this->matrix.xa.x = scale * this->matrix.xa.x;
	this->matrix.ya.x = scale * this->matrix.ya.x;
	this->matrix.za.x = scale * this->matrix.za.x;
	this->matrix.pivot.x = scale * this->matrix.pivot.x;
	this->matrix.xa.y = -(scale * this->matrix.xa.y);
	this->matrix.ya.y = -(scale * this->matrix.ya.y);
	this->matrix.za.y = -(scale * this->matrix.za.y);
	this->matrix.pivot.y = -(scale * this->matrix.pivot.y);
	this->frustum[0].normal = RotVec(VEC_UNIT_NEG_Z, this->camera);
	this->frustum[0].dis = -(
		this->frustum[0].x * (this->camera.pivot.x + this->clip_near * this->camera.za.x) +
		this->frustum[0].y * (this->camera.pivot.y + this->clip_near * this->camera.za.y) +
		this->frustum[0].z * (this->camera.pivot.z + this->clip_near * this->camera.za.z)
	);
	this->frustumSafe[0] = this->frustum[0];
	this->frustum[1].normal = RotVec(VEC_UNIT_POS_Z, this->camera);
	this->frustum[1].dis = -(
		this->frustum[1].x * (this->camera.pivot.x + this->clip_far * this->camera.za.x) +
		this->frustum[1].y * (this->camera.pivot.y + this->clip_far * this->camera.za.y) +
		this->frustum[1].z * (this->camera.pivot.z + this->clip_far * this->camera.za.z)
	);
	this->frustumSafe[1] = this->frustum[1];
	float xscale = this->m_scale * 0.5f;
	this->frustum[2].normal = RotVec(VEC_UNIT_NEG_X, this->camera);
	this->frustum[2].dis = -(
		this->frustum[2].x * (this->camera.pivot.x - xscale * this->camera.xa.x) +
		this->frustum[2].y * (this->camera.pivot.y - xscale * this->camera.xa.y) +
		this->frustum[2].z * (this->camera.pivot.z - xscale * this->camera.xa.z)
	);
	this->frustumSafe[2] = this->frustum[2];
	this->frustum[3].normal = RotVec(VEC_UNIT_POS_X, this->camera);
	this->frustum[3].dis = -(
		this->frustum[3].x * (this->camera.pivot.x + xscale * this->camera.xa.x) +
		this->frustum[3].y * (this->camera.pivot.y + xscale * this->camera.xa.y) +
		this->frustum[3].z * (this->camera.pivot.z + xscale * this->camera.xa.z)
	);
	this->frustumSafe[3] = this->frustum[3];
	float yscale = this->SCREEN_YS / this->SCREEN_XS * this->m_scale * 0.5f;
	this->frustum[4].normal = RotVec(VEC_UNIT_POS_Y, this->camera);
	this->frustum[4].dis = -(
		this->frustum[4].x * (this->camera.pivot.x + yscale * this->camera.ya.x) +
		this->frustum[4].y * (this->camera.pivot.y + yscale * this->camera.ya.y) +
		this->frustum[4].z * (this->camera.pivot.z + yscale * this->camera.ya.z)
	);
	this->frustumSafe[4] = this->frustum[4];
	this->frustum[5].normal = RotVec(VEC_UNIT_NEG_Y, this->camera);
	this->frustum[5].dis = -(
		this->frustum[5].x * (this->camera.pivot.x - yscale * this->camera.ya.x) +
		this->frustum[5].y * (this->camera.pivot.y - yscale * this->camera.ya.y) +
		this->frustum[5].z * (this->camera.pivot.z - yscale * this->camera.ya.z)
	);
	this->frustumSafe[5] = this->frustum[5];

	this->frustumByCam[0].normal = VEC_UNIT_NEG_Z;
	this->frustumByCam[0].dis = 0.0;
	this->frustumByCam[1].normal = VEC_UNIT_POS_Z;
	this->frustumByCam[1].dis =
		-((VEC_UNIT_POS_Z.y + VEC_UNIT_POS_Z.x) * 0.0f + VEC_UNIT_POS_Z.z * (this->clip_far - this->clip_near));
	this->frustumByCam[2].normal = VEC_UNIT_NEG_X;
	this->frustumByCam[2].dis = -((VEC_UNIT_NEG_X.z + VEC_UNIT_NEG_X.y) * 0.0f + VEC_UNIT_NEG_X.x * -this->scalex);
	this->frustumByCam[3].normal = VEC_UNIT_POS_X;
	this->frustumByCam[3].dis = -((VEC_UNIT_POS_X.z + VEC_UNIT_POS_X.y) * 0.0f + VEC_UNIT_POS_X.x * this->scalex);
	this->frustumByCam[4].normal = VEC_UNIT_POS_Y;
	this->frustumByCam[4].dis = -((VEC_UNIT_POS_Y.z + VEC_UNIT_POS_Y.x) * 0.0f + VEC_UNIT_POS_Y.y * this->scaley);
	this->frustumByCam[5].normal = VEC_UNIT_NEG_Y;
	this->frustumByCam[5].dis = -((VEC_UNIT_NEG_Y.z + VEC_UNIT_NEG_Y.x) * 0.0f + VEC_UNIT_NEG_Y.y * -this->scaley);

	if ((this->update & 0x1) != 0) {
		this->update &= ~0x1;
		SetWMatrix4FromWMatrix(this->m_xViewState.xmView, this->invcamera);
		this->m_xNeedToUpdateViewTransfToVideo = true;
	}
	this->UpdateProjectionTransform_Parallel();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void WView::UpdateCamera() {
	(this->*m_fnUpdateCamera[this->m_projMode])();
}

void WView::DrawIndexedTriangles(WtVertex* p, int pNum, uint16_t* f, int fNum, int drawOption, int drawOption2) {
	if (this->proj_scale > 1.0) {
		return;
	}
	if (this->m_bDisableFog) {
		drawOption &= ~0x8000000;
	}
	this->DrawIndexedTrianglesDirect(p, pNum, f, fNum, drawOption, drawOption2);
	if ((drawOption & 0x400000) != 0) {
		g_foffset += fNum;
	}
}

void WView::DrawIndexedTrianglesDirect(WtVertex* plist, int pn, uint16_t* flist, int fn, int type, int type2) {
	WVideoDev* video = this->m_resrcMng->VideoReference();
	if (!video) {
		return;
	}
	if ((type & 0x400000) != 0) {
		for (int i = 0; i < pn; i++) {
			plist[i].lv = plist[i].x * this->invcamera.xa.z
				+ plist[i].z * this->invcamera.za.z
				+ plist[i].y * this->invcamera.ya.z
				+ this->invcamera.pivot.z;
		}
	}
	if (this->m_xNeedToUpdateViewTransfToVideo) {
		this->m_xNeedToUpdateViewTransfToVideo = false;
		video->XSetTransform(WTransformStateView, this->m_xViewState.xmView);
	}
	if (this->m_xNeedToUpdateProjTransfToVideo) {
		this->m_xNeedToUpdateProjTransfToVideo = false;
		video->XSetTransform(WTransformStateProjection, this->m_xViewState.xmProj);
	}
	if (this->m_isReflective) {
		switch (type2 & 0xC00) {
			case 0x800:
				type2 &= ~0x800;
				break;
			case 0x000:
				type2 |= 0x800;
			default:
				break;
		}
	}
	video->DrawIndexedTriangles(plist, pn, flist, fn, type, type2);
}

bool WView::IsShadowView() {
	return false;
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

const WMatrix& WView::GetLastCamera() const {
	return this->lastcam;
}

bool WView::ProcessEffect() const {
	return this->m_bProcessEffect;
}

float WView::GetFOV_Unmodified() const {
	return this->FOV;
}

const WMatrix& WView::GetInvCamera() const {
	return this->invcamera;
}
