#include "rectcache.h"

RectCache::RectCache() {
}

RectCache::~RectCache() {
	if (this->m_ppTable) {
		delete[] this->m_ppTable;
		this->m_ppTable = nullptr;
	}
}

int RectCache::PixelToBlock_W(int w) {
	return (this->m_BlockWidth + w - 1) / this->m_BlockWidth;
}

int RectCache::PixelToBlock_H(int h) {
	return (this->m_BlockHeight + h - 1) / this->m_BlockHeight;
}

int RectCache::BlockToPixel_X(int x) {
	return x * this->m_BlockWidth;
}

int RectCache::BlockToPixel_Y(int y) {
	return y * this->m_BlockHeight;
}

RectCache::eCRRes RectCache::CheckRect(int xInBlock, int yInBlock, int wInBlock, int hInBlock, const RectInfo*& pResInfo, const RectInfo*& pMinHeightInfo) const {
    if (xInBlock < xInBlock + wInBlock) {
	    const RectInfo** pp = &this->m_ppTable[yInBlock * this->m_BlockCountW + xInBlock];
	    int x = xInBlock;
	    while (true) {
		    const RectInfo* p = *pp;
		    if (p != nullptr) {
				if ((p->rcBlock.tl.x < 0 || p->rcBlock.tl.x > this->m_BlockCountW) ||
					(p->rcBlock.br.x < 0 || p->rcBlock.br.x > this->m_BlockCountW) ||
					(p->rcBlock.tl.y < 0 || p->rcBlock.tl.y > this->m_BlockCountH) ||
					(p->rcBlock.br.y < 0 || p->rcBlock.br.y > this->m_BlockCountH)) {
					pResInfo = p;
					return eCRRes_Error;
				}
				if (!pMinHeightInfo || p->rcBlock.br.y < pMinHeightInfo->rcBlock.br.y) {
					pMinHeightInfo = p;
				}
				pResInfo = p;
				return eCRRes_RectOnX;
		    }
		    ++pp;
		    if (++x >= xInBlock + wInBlock) {
				break;
		    }
	    }
    }
    if (hInBlock > 1) {
		int y = 1;
	    const RectInfo** pp = &this->m_ppTable[xInBlock + (yInBlock + 1) * this->m_BlockCountW];
	    while(true) {
		    if (*pp) {
			    pResInfo = *pp;
			    return eCRRes_RectOnY;
		    }
		    pp += this->m_BlockCountW;
			if (++y >= hInBlock) {
				break;
			}
	    }
    }
    if (hInBlock <= 1) {
	    pResInfo = nullptr;
	    return eCRRes_Empty;
    }
	int y = 1;
    const RectInfo** pp = &this->m_ppTable[(xInBlock - 1) + wInBlock + (yInBlock + 1) * this->m_BlockCountW];
    while (true) {
	    const RectInfo* p = *pp;
	    if (*pp) {
			if (!pMinHeightInfo || p->rcBlock.br.y < pMinHeightInfo->rcBlock.br.y) {
				pMinHeightInfo = p;
			}
			pResInfo = p;
			return eCRRes_RectOnX;
	    }
	    ++y;
	    pp += this->m_BlockCountW;
	    if (y >= hInBlock) {
		    pResInfo = nullptr;
		    return eCRRes_Empty;
	    }
    }
}

int RectCache::SkipX(int xInBlock, int yInBlock, int xDest, const RectInfo*& pMinHeightInfo) const {
	int x = xInBlock;
	while (x < xDest) {
		const RectInfo* p = this->m_ppTable[x + yInBlock * this->m_BlockCountW];
		if (p) {
			if (!pMinHeightInfo || p->rcBlock.br.y < pMinHeightInfo->rcBlock.br.y) {
				pMinHeightInfo = p;
			}
			x = p->rcBlock.br.x;
		} else {
			++x;
		}
	}
	return x;
}

void RectCache::Clear() {
	for (int i = 0; i < this->m_BlockCountH * this->m_BlockCountW; ++i)
		this->m_ppTable[i] = nullptr;
	this->m_EmptyArea = this->m_BlockCountH * this->m_BlockCountW;
}

void RectCache::FillRect(const RectInfo& info) const {
	int n = info.rcBlock.tl.x + this->m_BlockCountW * info.rcBlock.tl.y;
	for (int x = info.rcBlock.tl.x; x < info.rcBlock.br.x; x++) {
		this->m_ppTable[n++] = &info;
	}
	
	n = info.rcBlock.tl.x + this->m_BlockCountW * (info.rcBlock.tl.y + 1);
	for (int y = info.rcBlock.br.y - info.rcBlock.tl.y - 1; y > 0; --y) {
		this->m_ppTable[n] = &info;
		n += this->m_BlockCountW;
	}
}

void RectCache::Init(int w, int h, int bw, int bh) {
	this->m_Width = w;
	this->m_Height = h;
	this->m_BlockWidth = bw;
	this->m_BlockHeight = bh;
	this->m_BlockCountW = w / bw;
	this->m_BlockCountH = h / bh;
	if (this->m_ppTable) {
		delete[] this->m_ppTable;
		this->m_ppTable = nullptr;
	}
	this->m_ppTable = new const RectInfo*[this->m_BlockCountH * this->m_BlockCountW];
	for (int i = 0; i < this->m_BlockCountH * this->m_BlockCountW; i++) {
		this->m_ppTable[i++] = nullptr;
	}
	this->m_EmptyArea = this->m_BlockCountH * this->m_BlockCountW;
}

bool RectCache::Add(RectInfo& info, int width, int height) {
	int wInBlock = (this->m_BlockWidth + width - 1) / this->m_BlockWidth;
	int hInBlock = (this->m_BlockHeight + height - 1) / this->m_BlockHeight;
	int blockArea = wInBlock * hInBlock;
	if (wInBlock * hInBlock > this->m_EmptyArea) {
		return false;
	}
	int xMax = this->m_BlockCountW - wInBlock + 1;
	int yMax = this->m_BlockCountH - hInBlock + 1;
	if (yMax <= 0) {
		return false;
	}
	int y = 0;
	while (true) {
		int x = 0;
		const RectInfo* pMinHeightInfo = nullptr;
		do {
			const RectInfo* pInfo = nullptr;
			switch (this->CheckRect(x, y, wInBlock, hInBlock, pInfo, pMinHeightInfo)) {
				case eCRRes_Empty:
					info.rcBlock.tl.x = x;
					info.rcBlock.tl.y = y;
					info.rcBlock.br.x = x + wInBlock;
					info.rcBlock.br.y = y + hInBlock;
					info.rcPixel.tl.x = x * this->m_BlockWidth;
					info.rcPixel.tl.y = y * this->m_BlockHeight;
					info.rcPixel.br.x = x * this->m_BlockWidth + width;
					info.rcPixel.br.y = y * this->m_BlockHeight + height;
					this->FillRect(info);
					this->m_EmptyArea -= blockArea;
					return true;
				case eCRRes_RectOnX:
					if (x >= pInfo->rcBlock.br.x) {
						return false;
					}
					x = pInfo->rcBlock.br.x;
					break;
				case eCRRes_RectOnY:
					x = this->SkipX(wInBlock + x, y, pInfo->rcBlock.br.x > xMax ? xMax : pInfo->rcBlock.br.x, pMinHeightInfo);
					break;
				case eCRRes_Error:
					return false;
				default:
					break;
			}
		} while (x < xMax);
		if (pMinHeightInfo && y < pMinHeightInfo->rcBlock.br.y) {
			y = pMinHeightInfo->rcBlock.br.y;
		} else {
			++y;
		}
		if (y >= yMax) {
			return false;
		}
	}
}
