#pragma once
#include "wmath.h"
#include "wresource.h"
#include "wscene.h"

class WView;
struct WtVertex;

extern WView* g_view;

class WView : public WResource {
public:
	enum UPDATE_MODE {
		BEGINESCENE = 0x1,
		CLEARZBUFF = 0x2,
		CLEARFRAME = 0x4,
	};

	enum UPDATE_TRANSFORM {
		UPDATE_NONE = 0x0,
		UPDATE_VIEW = 0x1,
		UPDATE_PROJ = 0x2,
	};

	enum PROJECTION_MODE {
		PERSPECTIVE = 0x0,
		PARALLEL = 0x1,
		NUM_PROJECTION_MODE = 0x2,
	};

	float GetClipNearValue() const;
	float GetClipFarValue() const;
	float GetFOV() const;
	float GetWidth() const;
	float GetHeight() const;
	float GetRatio() const;
	bool InFrustum(const WSphere& sphere) const;
	bool InFrustumSafe(const WSphere& sphere) const;
	void ResetClippingArea();
	void SetClippingArea(const WRect& rect);
	void ResetScreenCenter();
	void SetScreenCenter(const WVector2D& center);
	void SetScreenCenter(float x, float y);
	const WMatrix& GetCamera() const;
	float xGetProjScale() const;
	void xConvScreenRectByProjScale(WRect* rc) const;
	void DrawPolygonFan(WtVertex** vl, int drawOption, int drawOption2, bool projected);
	void CheckViewAndProjTransformUpdateToVideo();

private:
	WMatrix camera;
	WMatrix invcamera;
	WMatrix matrix;
	WPlane frustum[6];
	WPlane frustumSafe[6];
	float scalex;
	float scaley;
	float m_center_x;
	float m_center_y;
	float clip_near;
	float clip_far;
	float clip_scaled_near;
	float clip_scaled_far;
	float SCREEN_XS;
	float SCREEN_YS;
	unsigned int m_cliptype;
	bool m_fastclip;
	WRect m_clipArea;
	float clip_scale_z;
	float clip_near_scale;
	float FOV;
	WVector m_temp[512];
	WMatrix lastcam;
	int update;
	float proj_scale;
	float left;
	float right;
	float top;
	float bottom;
	bool m_bProcessEffect;
	bool m_bDisableFog;
	bool m_isReflective;
	WxViewState m_xViewState;
	bool m_xNeedToUpdateViewTransfToVideo;
	bool m_xNeedToUpdateProjTransfToVideo;
	PROJECTION_MODE m_projMode;
	float m_scale;
	WPlane frustumByCam[6];
};
