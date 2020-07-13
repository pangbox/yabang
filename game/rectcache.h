#pragma once
#include <cstdint>

#include "rectangle.h"

class RectCache {
public:
	enum eCRRes {
		eCRRes_Empty = 0x0,
		eCRRes_RectOnX = 0x1,
		eCRRes_RectOnY = 0x2,
		eCRRes_Error = 0x3,
	};

	struct RectInfo {
		Rectangle_<uint16_t> rcPixel;
		Rectangle_<uint16_t> rcBlock;
	};

	RectCache();
	~RectCache();

	int PixelToBlock_W(int w);
	int PixelToBlock_H(int h);
	int BlockToPixel_X(int x);
	int BlockToPixel_Y(int y);
	eCRRes CheckRect(int xInBlock, int yInBlock, int wInBlock, int hInBlock, const RectInfo*& pResInfo,
	                 const RectInfo*& pMinHeightInfo) const;
	int SkipX(int xInBlock, int yInBlock, int xDest, const RectInfo*& pMinHeightInfo) const;
	void Clear();
	void FillRect(const RectInfo& info) const;
	void Init(int w, int h, int bw, int bh);
	bool Add(RectInfo& info, int width, int height);

private:
	int m_Width = 0;
	int m_Height = 0;
	int m_BlockWidth = 0;
	int m_BlockHeight = 0;
	int m_BlockCountW = 0;
	int m_BlockCountH = 0;
	int m_EmptyArea = 0;
	const RectInfo** m_ppTable = nullptr;
};
