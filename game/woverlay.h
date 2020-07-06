#pragma once
#include "wmath.h"
#include "wresource.h"

class WOverlay : public WResource {
public:

private:
	int m_texHandle;
	unsigned int m_texWidth;
	unsigned int m_texHeight;
	unsigned int m_devTexWidth;
	unsigned int m_devTexHeight;
	WRect m_clipArea;
	bool m_clipFlag;
	int m_coordMode;
};
