#pragma once
#include "wvideo.h"
#include "wview.h"

class WTextureView : public WView {
public:

private:
	struct TextureParam {
		struct RtTexInfo {
			unsigned int m_texStyle;
			WRenderToTextureSizeInfo m_sizeInfo;
			bool m_needToClear;
			unsigned int m_clearClr;
		};

		struct DepthSurfInfo {
			WRenderToTextureParam::DepthSurfInfo::SurfaceUsage m_surfUsage;
			WRenderToTextureSizeInfo m_sizeInfo;
			bool m_needToClear;
			float m_clearZ;
		};

		unsigned int m_numRts;
		RtTexInfo m_rtTexInfo[2];
		DepthSurfInfo m_depthSurfInfo;
	};

	TextureParam m_texParam;
	WRenderToTextureParam m_r2tParam;
	bool m_r2tBegun;
};
