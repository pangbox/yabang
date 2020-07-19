#pragma once
#include "wmath.h"
#include "wresource.h"
#include "wvideo.h"

class Bitmap;
class WView;

enum wUnitMode {
	W_UNIT_XPOS,
	W_UNIT_YPOS,
	W_UNIT_WIDTH,
	W_UNIT_HEIGHT,
};


class WOverlay : public WResource {
public:
	WOverlay();
	~WOverlay() override;

	virtual bool Load(const char* filename, unsigned int flag);
	virtual void Render(WView* view, const WRect& src, const WRect& dest, int type, unsigned int diffuse, float angle,
	                    uint8_t flipflag) const;
	virtual void SetCoordMode(int coordMode);

	[[nodiscard]] unsigned int GetWidth() const;
	[[nodiscard]] unsigned int GetHeight() const;
	[[nodiscard]] int GetTexhandle() const;
	[[nodiscard]] int GetCoordMode() const;
	bool Load(const char* filename, Bitmap* bitmap, unsigned flag);
	void SetClippingArea(const WRect* rect);
	void DrawTexture(WView* view, int texHandle, const WRect& src, const WRect& dest, float shear, int type,
	                 unsigned int diffuse);
	int GetSection(WtVertex* out, float snapang, float ang) const;
	void ConvertSourceRectByTextureSize(WRect& newRc, const WRect& rc) const;
	static void DrawLine(WView* view, const WPoint& p1, const WPoint& p2, int type, unsigned int diffuse);
	static void DrawLine(WView* view, const WPoint& p1, const WPoint& p2, unsigned int diffuse1, unsigned int diffuse2,
	                     int type);
	static void DrawBox(WView* view, const WRect& rect, int type, unsigned int diffuse, float depth);
	static void DrawRainbowBox(WView* view, const WRect& rect, unsigned int* aDiffuse, int type, float depth);
	static void DrawLineBox(WView* view, const WRect& rect, int type, unsigned int diffuse);
	float GetUnit(WView* view, float unit, wUnitMode mode) const;
	static WRect ConvertRect(WView* view, const WRect& rect, int coorFlag);
	static void DrawPicture(WView* view, const WRect& rect, int texhandle, int type, unsigned int diffuse);
	static void DrawFrameOverlay1(WView* view, WOverlay* frame, float sizeL, float sizeT, float sizeR, float sizeB,
	                              const RECT& rect, unsigned int color);
	static void DrawFrameOverlay9(WView* view, WOverlay** frames, const RECT& rect, unsigned int color);
	static bool CrossRect(const WRect& r1, const WRect& r2, WRect* out);
	void SetClippingArea(WView* view, const WRect* rect);
	void RenderWithShear(WView* view, const WRect& src, const WRect& dest, float shear, int type, unsigned int diffuse);
	void DrawTexture(WView* view, int texHandle, const WRect& src, const WRect& dest, int type, unsigned int diffuse,
	                 float angle, uint8_t flipflag, bool clipping) const;
	void DrawArcClipTexture(WView* view, int texHandle, const WRect& src, const WRect& dest, float startang,
	                        float endang, int type, unsigned int diffuse);
	void clip_2D_left(WView* view, WtVertex* out, WtVertex* in, int* outlen, int inlen);
	void clip_2D_right(WView* view, WtVertex* out, WtVertex* in, int* outlen, int inlen);
	void clip_2D_top(WView* view, WtVertex* out, WtVertex* in, int* outlen, int inlen);
	void clip_2D_bottom(WView* view, WtVertex* out, WtVertex* in, int* outlen, int inlen);
	void ArcClipRender(WView* view, const WRect& src, const WRect& dest, float startang, float endang, int type,
	                   unsigned int diffuse);
	void DrawTextureWithAxis(WView* view, int texHandle, const WRect& src, const WRect& dest, float axisX,
	                         float axisY, float angle, int type, unsigned int diffuse, uint8_t flipFlag);
	void RenderWithAxis(WView* view, const WRect& src, const WRect& dest, float axisX, float axisY, float angle,
	                    int type, unsigned diffuse, uint8_t flipFlag);

private:
	int m_texHandle = 0;
	unsigned int m_texWidth = 0;
	unsigned int m_texHeight = 0;
	unsigned int m_devTexWidth = 0;
	unsigned int m_devTexHeight = 0;
	WRect m_clipArea{};
	bool m_clipFlag = false;
	int m_coordMode = 0x2200;

	static WtVertex* m_vtxList[9];
	static WtVertex m_vtx[4];
	static WtVertex* m_vl[5];
};
