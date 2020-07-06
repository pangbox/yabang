#pragma once
#include "wvideo.h"
#include "wshadowview.h"
#include "wtextureview.h"

class WShadowViewHw : public WShadowView {
public:
	WRenderToTextureParam m_orgTexParam;
	WTextureView* m_filteredTexView;
	bool m_r2tBegun;
};
