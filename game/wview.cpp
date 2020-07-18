﻿#include "wview.h"

#include "wresourcemanager.h"

WView* g_view = nullptr;

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

const WMatrix& WView::GetCamera() const {
	return this->camera;
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
