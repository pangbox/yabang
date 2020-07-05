#pragma once
#include <limits>
#include <wmath.h>

struct LightSet {
	unsigned int type = 0;
	WVector nearOne = {};
	unsigned int diffuse = 0;
	unsigned int ambient = 0;
	unsigned int ambient2 = 0xFFFFFF;
};

struct WxBatchState {
	uint32_t xiFlag0 = 0;
	uint32_t xiFlag1 = 0;
	int xiBaseVtxIdx = 0;
	int xNumVertices = 0;
	int xiBaseIdxIdx = 0;
	int xNumIndices = 0;
	uint32_t xdwDiffuse = 0xFFFFFFFF;
	WMatrix xmW = {};
	float xfDepth = std::numeric_limits<float>::max();
	int xnmTransfs = 0;
	const WMatrix *const *xpapmW = nullptr;
	const WMatrix *const *xpapmO = nullptr;
};

struct WxViewState {
	LightSet xLight = {};
	WMatrix4 xmView = {};
	WMatrix4 xmProj = {};
};
