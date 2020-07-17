#include "wdxtc.h"
#include <cstdio>

const DWORD g_dxt1FourCc = MAKEFOURCC('D', 'X', 'T', '1');
const DWORD g_dxt3FourCc = MAKEFOURCC('D', 'X', 'T', '3');
const DWORD g_dxt5FourCc = MAKEFOURCC('D', 'X', 'T', '5');

WDXTC::WDXTC(): m_ddsd2() {
	this->m_pCompData = nullptr;
	this->m_pDecompData = nullptr;
}

WDXTC::~WDXTC() {
	if (this->m_pDecompData) {
		delete[] this->m_pDecompData;
		this->m_pDecompData = nullptr;
	}
	if (this->m_pCompData) {
		delete[] this->m_pCompData;
		this->m_pCompData = nullptr;
	}
}

LPDDSURFACEDESC2 WDXTC::GetDdsd2() {
	return &this->m_ddsd2;
}

uint8_t* WDXTC::GetDecompData() const {
	return this->m_pDecompData;
}

void WDXTC::GetColorFromBlock(Color32* aClr32, const DxtcColorBlock* pBlock) {
	aClr32[0].a = -1;
	aClr32[0].r = 8 * (pBlock->color0 >> 11);
	aClr32[0].g = 4 * (pBlock->color0 >> 5);
	aClr32[0].b = 8 * pBlock->color0;
	aClr32[1].a = -1;
	aClr32[1].r = 8 * (pBlock->color1 >> 11);
	aClr32[1].g = 4 * (pBlock->color1 >> 5);
	aClr32[1].b = 8 * pBlock->color1;
	if (pBlock->color0 <= pBlock->color1) {
		aClr32[2].r = (static_cast<BYTE>(aClr32[0].r) + static_cast<BYTE>(aClr32[1].r)) / 2;
		aClr32[2].g = (static_cast<BYTE>(aClr32[0].g) + static_cast<BYTE>(aClr32[1].g)) / 2;
		aClr32[2].b = (static_cast<BYTE>(aClr32[1].b) + static_cast<BYTE>(aClr32[0].b)) / 2;
		aClr32[2].a = -1;
		aClr32[3].r = 0;
		aClr32[3].g = -1;
		aClr32[3].b = -1;
		aClr32[3].a = 0;
	} else {
		aClr32[2].r = (aClr32[1].r + 2 * aClr32[0].r) / 3;
		aClr32[2].g = (aClr32[1].g + 2 * aClr32[0].g) / 3;
		aClr32[2].b = (aClr32[1].b + 2 * aClr32[0].b) / 3;
		aClr32[2].b += aClr32[2].b < 0;
		aClr32[2].a = -1;
		aClr32[3].r = (aClr32[0].r + 2 * aClr32[1].r) / 3;
		aClr32[3].r += aClr32[3].r < 0;
		aClr32[3].g = (aClr32[0].g + 2 * static_cast<BYTE>(aClr32[1].g)) / 3;
		aClr32[3].b = (aClr32[0].b + 2 * aClr32[1].b) / 3;
		aClr32[3].b += aClr32[3].b < 0;
		aClr32[3].a = -1;
	}
}

void WDXTC::DecodeColor(Color32* pDecomp, const DxtcColorBlock* pBlock, Color32* aClr32) const {
	BYTE uiMasks[4] = {0x03, 0x0C, 0x30, 0xC0};
	Color32* pOut = pDecomp;
	const uint8_t* pIn = pBlock->row;

	for (int i = 0; i < 4; i++, pIn++) {
		for (int shift = 0, mask = 0; shift < 8; shift += 2, mask++, pOut++) {
			switch ((*pIn & uiMasks[mask]) >> shift) {
				case 0:
					pOut->clr = aClr32[0].clr;
					break;
				case 1:
					pOut->clr = aClr32[1].clr;
					break;
				case 2:
					pOut->clr = aClr32[2].clr;
					break;
				case 3:
					pOut->clr = aClr32[3].clr;
					break;
				default:
					break;
			}
		}
		pOut += this->m_ddsd2.dwWidth - 4;
	}
}

void WDXTC::DecodeExpAlphaBlock(Color32* pDecomp, const DxtcExpAlphaBlock* pAlpha) const {

	Color32* pOut = pDecomp;
	const DxtcExpAlphaBlock* pIn = pAlpha;

	for (unsigned short i : pIn->row) {
		WORD usAlpha = i >> 8;
		pOut[0].a = ((i & 0xF) | 0x10 * (i & 0xF)) & 0xF;
		pOut[1].a = (i >> 4 & 0xFF & 0xF) | 16 * (i >> 4 & 0xFF & 0xF);
		pOut[2].a = (usAlpha & 0xF) | 16 * (usAlpha & 0xF);
		pOut[3].a = usAlpha >> 4 | 16 * (usAlpha >> 4);
		pOut += this->m_ddsd2.dwWidth;
	}
}

void WDXTC::DecodeLinearAlphaBlock3Bit(Color32* pDecomp, const DxtcLinearAlphaBlock3Bit* pAlpha) const {
	Color32 tmp[16];
	WORD gAlphas[8];
	BYTE gBits[16];

	Color32* pOut = pDecomp;
	WORD a1 = pAlpha->a1;
	WORD a0 = pAlpha->a0;

	gAlphas[0] = a0;
	gAlphas[1] = a1;

	if (a0 <= a1) {
		gAlphas[2] = ((a1 + 4 * a0) * 10 / 4 >> 1) + ((a1 + 4 * a0) * 10 / 4 >> 31);
		gAlphas[3] = ((3 * a0 + 2 * a1) * 10 / 4 >> 1) + ((3 * a0 + 2 * a1) * 10 / 4 >> 31);
		gAlphas[4] = ((3 * a1 + 2 * a0) * 10 / 4 >> 1) + ((3 * a1 + 2 * a0) * 10 / 4 >> 31);
		gAlphas[5] = (a0 + 4 * a1) / 5;
		gAlphas[6] = 0;
		gAlphas[7] = 255;
	} else {
		gAlphas[2] = ((a1 + 6 * a0 + (a1 + 6 * a0) * 3 / 7) >> 2) + ((a1 + 6 * a0 + (a1 + 6 * a0) * 3 / 7) >> 31);
		gAlphas[3] = ((5 * a0 + 2 * a1 + (5 * a0 + 2 * a1) * 3 / 7) >> 2) + ((5 * a0 + 2 * a1 + (5 * a0 + 2 * a1) * 3 /
			7) >> 31);
		gAlphas[4] = ((3 * a1 + 4 * a0 + (3 * a1 + 4 * a0) * 3 / 7) >> 2) + ((3 * a1 + 4 * a0 + (3 * a1 + 4 * a0) * 3 /
			7) >> 31);
		gAlphas[5] = ((3 * a0 + 4 * a1 + (3 * a0 + 4 * a1) * 3 / 7) >> 2) + ((3 * a0 + 4 * a1 + (3 * a0 + 4 * a1) * 3 /
			7) >> 31);
		gAlphas[6] = ((5 * a1 + 2 * a0 + (5 * a1 + 2 * a0) * 3 / 7) >> 2) + ((5 * a1 + 2 * a0 + (5 * a1 + 2 * a0) * 3 /
			7) >> 31);
		gAlphas[7] = (a0 + 6 * a1) / 7;
	}

	DWORD stuff = *reinterpret_cast<const DWORD*>(&pAlpha->stuff[0]);
	gBits[0] = BYTE(stuff >> 0 & 7);
	gBits[1] = BYTE(stuff >> 3 & 7);
	gBits[2] = BYTE(stuff >> 6 & 7);
	gBits[3] = BYTE(stuff >> 9 & 7);
	gBits[4] = BYTE(stuff >> 12 & 7);
	gBits[5] = BYTE(stuff >> 15 & 7);
	gBits[6] = BYTE(stuff >> 18 & 7);
	gBits[7] = BYTE(stuff >> 21 & 7);

	stuff = *reinterpret_cast<const DWORD*>(&pAlpha->stuff[3]);
	gBits[8] = BYTE(stuff >> 0 & 7);
	gBits[9] = BYTE(stuff >> 3 & 7);
	gBits[10] = BYTE(stuff >> 6 & 7);
	gBits[11] = BYTE(stuff >> 9 & 7);
	gBits[12] = BYTE(stuff >> 12 & 7);
	gBits[13] = BYTE(stuff >> 15 & 7);
	gBits[14] = BYTE(stuff >> 18 & 7);
	gBits[15] = BYTE(stuff >> 21 & 7);

	for (int i = 0; i < 4; i++) {
		tmp[i * 4 + 0].a = BYTE(gAlphas[gBits[i * 4 + 0]]);
		tmp[i * 4 + 1].a = BYTE(gAlphas[gBits[i * 4 + 1]]);
		tmp[i * 4 + 2].a = BYTE(gAlphas[gBits[i * 4 + 2]]);
		tmp[i * 4 + 3].a = BYTE(gAlphas[gBits[i * 4 + 3]]);
	}

	for (int i = 0; i < 4; i++) {
		pOut[0].a = tmp[i * 4 + 0].a;
		pOut[1].a = tmp[i * 4 + 1].a;
		pOut[2].a = tmp[i * 4 + 2].a;
		pOut[3].a = tmp[i * 4 + 3].a;
		pOut += this->m_ddsd2.dwWidth;
	}
}

void WDXTC::DecompressBlockDxt1(uint8_t* pDecomp, const DxtcColorBlock* pBlock) const {
	Color32 aClr[4];
	this->GetColorFromBlock(aClr, pBlock);
	this->DecodeColor(reinterpret_cast<Color32*>(pDecomp), pBlock, aClr);
}

void WDXTC::DecompressBlockDxt3(uint8_t* pDecomp, const DxtcColorBlock* pBlock) const {
	Color32 aClr[4];
	this->GetColorFromBlock(aClr, pBlock + 1);
	this->DecodeColor(reinterpret_cast<Color32*>(pDecomp), pBlock + 1, aClr);
	this->DecodeExpAlphaBlock(reinterpret_cast<Color32*>(pDecomp), reinterpret_cast<const DxtcExpAlphaBlock*>(pBlock));
}

void WDXTC::DecompressBlockDxt5(uint8_t* pDecomp, const DxtcColorBlock* pBlock) const {
	Color32 aClr[4];
	this->GetColorFromBlock(aClr, pBlock + 1);
	this->DecodeColor(reinterpret_cast<Color32*>(pDecomp), pBlock + 1, aClr);
	this->DecodeLinearAlphaBlock3Bit(reinterpret_cast<Color32*>(pDecomp),
	                                 reinterpret_cast<const DxtcLinearAlphaBlock3Bit*>(pBlock));
}

void WDXTC::SaveAsBmp(char* filename) const {
	FILE* pfRgb;
	FILE* pfAlpha;
	RGBQUAD bgr;
	RGBQUAD alpha;
	char strBuf[64];

	UINT w = this->m_ddsd2.dwWidth;
	UINT h = this->m_ddsd2.dwHeight;

	BITMAPFILEHEADER bfh;
	bfh.bfType = 0x4D42;
	bfh.bfSize = 54 + 3 * w * h;
	bfh.bfReserved1 = 0;
	bfh.bfReserved2 = 0;
	bfh.bfOffBits = 54;

	BITMAPINFOHEADER bih;
	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = w;
	bih.biHeight = h;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = 0;
	bih.biSizeImage = 0;
	bih.biXPelsPerMeter = 0;
	bih.biYPelsPerMeter = 0;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	sprintf_s(strBuf, 64, "%s_RGB.bmp", filename);
	fopen_s(&pfRgb, strBuf, "wb");
	sprintf_s(strBuf, 64, "%s_Alpha.bmp", filename);
	fopen_s(&pfAlpha, strBuf, "wb");
	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, pfRgb);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, pfRgb);
	fwrite(&bfh, sizeof(BITMAPFILEHEADER), 1, pfAlpha);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, pfAlpha);

	for (int y = static_cast<int>(this->m_ddsd2.dwHeight - 1); y >= 0; y--) {
		uint8_t* ptr = &this->m_pDecompData[4 * y * w];
		for (UINT dw = 0; dw < w; dw++) {
			bgr.rgbRed = ptr[0];
			bgr.rgbGreen = ptr[1];
			bgr.rgbBlue = ptr[2];
			alpha.rgbRed = ptr[3];
			alpha.rgbGreen = alpha.rgbRed;
			alpha.rgbBlue = alpha.rgbRed;
			fwrite(&bgr, 3, 1, pfRgb);
			fwrite(&alpha, 3, 1, pfAlpha);
			ptr += 4;
		}
	}

	fclose(pfAlpha);
	fclose(pfRgb);
}

void WDXTC::DecompressDxt1() const {
	int nXBlocks = this->m_ddsd2.dwWidth >> 2;
	int nYBlocks = this->m_ddsd2.dwHeight >> 2;
	for (int y = 0; y < nYBlocks; y++) {
		const auto* pIn = reinterpret_cast<const DxtcColorBlock*>(&this->m_pCompData[8 * nXBlocks * y]);
		auto* pOut = reinterpret_cast<Color32*>(&this->m_pDecompData[16 * y * this->m_ddsd2.dwWidth]);
		for (int x = 0; x < nXBlocks; x++) {
			Color32 aClr32{};
			this->GetColorFromBlock(&aClr32, pIn);
			this->DecodeColor(pOut, pIn, &aClr32);
			++pIn;
			pOut += 4;
		}
	}
}

void WDXTC::DecompressDxt3() const {
	int nXBlocks = this->m_ddsd2.dwWidth >> 2;
	int nYBlocks = this->m_ddsd2.dwHeight >> 2;

	for (int y = 0; y < nYBlocks; y++) {
		auto* pIn = static_cast<uint8_t*>(&this->m_pCompData[16 * y * nXBlocks]);
		auto* pOut = reinterpret_cast<Color32*>(&this->m_pDecompData[16 * y * this->m_ddsd2.dwWidth]);
		auto* pBlock = reinterpret_cast<DxtcColorBlock*>(pIn + 8);
		for (int x = 0; x < nXBlocks; x++) {
			Color32 aClr32{};
			this->GetColorFromBlock(&aClr32, pBlock);
			this->DecodeColor(pOut, pBlock, &aClr32);
			this->DecodeExpAlphaBlock(pOut, reinterpret_cast<const DxtcExpAlphaBlock*>(pIn));
			pBlock += 2;
			pIn += 16;
			pOut += 4;
		}
	}
}

void WDXTC::DecompressDxt5() const {
	int nXBlocks = this->m_ddsd2.dwWidth >> 2;
	int nYBlocks = this->m_ddsd2.dwHeight >> 2;

	for (int y = 0; y < nYBlocks; y++) {
		auto* pIn = &this->m_pCompData[16 * y * nXBlocks];
		auto* pOut = reinterpret_cast<Color32*>(&this->m_pDecompData[16 * y * this->m_ddsd2.dwWidth]);
		auto* pBlock = reinterpret_cast<DxtcColorBlock*>(pIn + 8);
		for (int x = 0; x < nXBlocks; x++) {
			Color32 aClr32{};
			this->GetColorFromBlock(&aClr32, pBlock);
			this->DecodeColor(pOut, pBlock, &aClr32);
			this->DecodeLinearAlphaBlock3Bit(pOut, reinterpret_cast<const DxtcLinearAlphaBlock3Bit*>(pIn));
			pBlock += 2;
			pIn += 16;
			pOut += 4;
		}
	}
}

bool WDXTC::DecompressDxtc() {
	if (this->m_pDecompData) {
		delete[] this->m_pDecompData;
		this->m_pDecompData = nullptr;
	}
	this->m_pDecompData = new BYTE[4 * this->m_ddsd2.dwHeight * this->m_ddsd2.dwWidth];
	switch (this->m_ddsd2.ddpfPixelFormat.dwFourCC) {
		case g_dxt1FourCc:
			this->DecompressDxt1();
			return true;
		case g_dxt3FourCc:
			this->DecompressDxt3();
			return true;
		case g_dxt5FourCc:
			this->DecompressDxt5();
			return true;
		default:
			return false;
	}
}

bool WDXTC::Load(uint8_t* pData, int iSize) {
	if (pData[0] != 'D' || pData[1] != 'D' || pData[2] != 'S') {
		return false;
	}

	memcpy(&this->m_ddsd2, &pData[4], sizeof(DDSURFACEDESC2));
	if (!(this->m_ddsd2.dwFlags & 0x80000)) {
		return false;
	}

	this->m_pCompData = new BYTE[this->m_ddsd2.lPitch];
	memcpy(this->m_pCompData, &pData[128], this->m_ddsd2.dwLinearSize);
	return this->DecompressDxtc();
}
