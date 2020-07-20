#include "bitmap.h"

#include <algorithm>
#include <cstdio>
#include <cmath>

Bitmap::Bitmap() {
	this->m_bi = nullptr;
	this->m_vram = nullptr;
	this->m_lock = false;
	this->m_pitch = 0;
}

Bitmap::Bitmap(const Bitmap& other) {
	*this = other;
}

Bitmap::Bitmap(Bitmap&& other) noexcept {
	*this = std::move(other);
}

Bitmap::Bitmap(int32_t w, int32_t h, int32_t bpp) {
	this->m_bi = nullptr;
	this->m_vram = nullptr;
	this->m_lock = false;
	this->m_pitch = 0;
	this->Create(w, h, bpp);
}

Bitmap::~Bitmap() {
	if (this->m_lock) {
		return;
	}
	if (this->m_bi) {
		delete[] reinterpret_cast<uint8_t*>(this->m_bi);
	}
}

Bitmap& Bitmap::operator=(const Bitmap& other) {
	if (this == &other) {
		return *this;
	}
	if (!this->m_bi || this->m_bi->bmiHeader.biWidth != other.m_bi->bmiHeader.biWidth || this->m_bi->bmiHeader.biHeight
		!= other.m_bi->bmiHeader.biHeight || this->m_bi->bmiHeader.biBitCount != other.m_bi->bmiHeader.biBitCount) {
		this->Create(other.m_bi->bmiHeader.biWidth, other.m_bi->bmiHeader.biHeight, other.m_bi->bmiHeader.biBitCount);
	}
	const_cast<Bitmap&>(other).Update();
	if (this->m_bi->bmiHeader.biBitCount <= 8) {
		int colorsUsed = this->m_bi->bmiHeader.biClrUsed;
		if (!colorsUsed) {
			colorsUsed = 256;
		}
		memcpy(this->m_bi->bmiColors, other.m_bi->bmiColors, 4 * colorsUsed);
	}
	memcpy(this->m_vram, other.m_vram, this->m_bi->bmiHeader.biHeight * this->m_pitch);
	this->m_lock = false;
	return *this;
}

Bitmap& Bitmap::operator=(Bitmap&& other) noexcept {
	m_bi = other.m_bi;
	m_pitch = other.m_pitch;
	m_vram = other.m_vram;
	m_lock = other.m_lock;
	return *this;
}

uint8_t* Bitmap::VMem(BITMAPINFO* bi) {
	if (bi->bmiHeader.biBitCount > 8) {
		return reinterpret_cast<uint8_t*>(bi) + sizeof(BITMAPINFOHEADER);
	}
	int colors = bi->bmiHeader.biClrUsed;
	if (!colors) {
		colors = 256;
	}
	return reinterpret_cast<uint8_t*>(bi) + sizeof(BITMAPINFOHEADER) + 4 * colors;
}

void Bitmap::Create(int32_t w, int32_t h, int32_t bpp) {

	if (!this->m_lock && this->m_bi) {
		delete[] reinterpret_cast<uint8_t*>(this->m_bi);
	}

	size_t pitch = (w * (bpp / 8) + 3) & ~3;
	size_t length = sizeof(BITMAPINFOHEADER) + (bpp > 8 ? 0 : 4 * 256) + h * pitch;

	this->m_pitch = pitch;
	this->m_bi = reinterpret_cast<BITMAPINFO*>(new uint8_t[length]);
	this->m_lock = false;
	memset(this->m_bi, 0, length);
	this->m_bi->bmiHeader.biSize = 40;
	this->m_bi->bmiHeader.biPlanes = 1;
	this->m_bi->bmiHeader.biCompression = 0;
	this->m_bi->bmiHeader.biSizeImage = 0;
	this->m_bi->bmiHeader.biClrUsed = 0;
	this->m_bi->bmiHeader.biWidth = w;
	this->m_bi->bmiHeader.biHeight = h;
	this->m_bi->bmiHeader.biBitCount = bpp;
	if (this->m_bi->bmiHeader.biBitCount <= 8) {
		DWORD colors = this->m_bi->bmiHeader.biClrUsed;
		if (!colors) {
			colors = 256;
		}
		this->m_vram = reinterpret_cast<uint8_t*>(&this->m_bi->bmiColors[colors]);
	} else {
		this->m_vram = reinterpret_cast<uint8_t*>(this->m_bi->bmiColors);
	}
}

void Bitmap::Update() {}

void Bitmap::GetPixel(int32_t x, int32_t y, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const {
	if (x < 0 || y < 0) {
		return;
	}

	if (!this->m_bi || x >= this->m_bi->bmiHeader.biWidth || y >= this->m_bi->bmiHeader.biHeight) {
		return;
	}

	WORD bpp = this->m_bi->bmiHeader.biBitCount;
	int xb;
	if (bpp == 32) {
		xb = 4 * x;
		a = this->m_vram[xb + y * this->m_pitch + 3];
	} else {
		if (bpp != 24) {
			return;
		}
		a = -1;
		xb = 3 * x;
	}
	r = this->m_vram[xb + y * this->m_pitch + 2];
	g = this->m_vram[xb + y * this->m_pitch + 1];
	b = this->m_vram[xb + y * this->m_pitch + 0];
}

void Bitmap::GetPixel(int32_t x, int32_t y, uint8_t& r, uint8_t& g, uint8_t& b) const {
	if (x < 0 || y < 0) {
		return;
	}

	if (!this->m_bi || x >= this->m_bi->bmiHeader.biWidth || y >= this->m_bi->bmiHeader.biHeight) {
		return;
	}

	if (this->m_bi->bmiHeader.biBitCount == 8) {
		int index = this->m_vram[y * this->m_pitch + x];
		r = this->m_bi->bmiColors[index].rgbRed;
		g = this->m_bi->bmiColors[index].rgbGreen;
		b = this->m_bi->bmiColors[index].rgbBlue;
	}

	if (this->m_bi->bmiHeader.biBitCount == 24) {
		r = this->m_vram[3 * x + 2 + y * this->m_pitch];
		g = this->m_vram[3 * x + 1 + y * this->m_pitch];
		b = this->m_vram[3 * x + 0 + y * this->m_pitch];
	}

	if (this->m_bi->bmiHeader.biBitCount == 32) {
		r = this->m_vram[4 * x + 2 + y * this->m_pitch];
		g = this->m_vram[4 * x + 1 + y * this->m_pitch];
		b = this->m_vram[4 * x + 0 + y * this->m_pitch];
	}
}

void Bitmap::SetPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) const {
	if (!this->m_bi || x < 0 || y < 0 || x >= this->m_bi->bmiHeader.biWidth || y >= this->m_bi->bmiHeader.biHeight) {
		return;
	}

	int index;
	switch (this->m_bi->bmiHeader.biBitCount) {
		case 8:
			index = this->m_vram[y * this->m_pitch + x];
			this->m_bi->bmiColors[index].rgbRed = r;
			this->m_bi->bmiColors[index].rgbGreen = g;
			this->m_bi->bmiColors[index].rgbBlue = b;
			break;
		case 24:
			this->m_vram[3 * x + 2 + y * this->m_pitch] = r;
			this->m_vram[3 * x + 1 + y * this->m_pitch] = g;
			this->m_vram[3 * x + 0 + y * this->m_pitch] = b;
			break;
		case 32:
			this->m_vram[4 * x + 3 + y * this->m_pitch] = a;
			this->m_vram[4 * x + 2 + y * this->m_pitch] = r;
			this->m_vram[4 * x + 1 + y * this->m_pitch] = g;
			this->m_vram[4 * x + 0 + y * this->m_pitch] = b;
			break;
		default:
			break;
	}
}

char Bitmap::GetAlpha(int32_t x, int32_t y) const {
	if (x >= 0 && y >= 0 && x < this->m_bi->bmiHeader.biWidth && y < this->m_bi->bmiHeader.biHeight && this
	                                                                                                   ->m_bi->bmiHeader
	                                                                                                   .biBitCount == 32
	) {
		return this->m_vram[4 * x + 3 + y * this->m_pitch];
	}
	return -1;
}

void Bitmap::SetAlpha(int32_t x, int32_t y, uint8_t a) const {
	if (x >= 0 && y >= 0 && x < this->m_bi->bmiHeader.biWidth && y < this->m_bi->bmiHeader.biHeight && this
	                                                                                                   ->m_bi->bmiHeader
	                                                                                                   .biBitCount == 32
	) {
		this->m_vram[4 * x + 3 + y * this->m_pitch] = a;
	}
}

void Bitmap::SetPalette(const uint8_t* palette) const {
	for (int i = 0; i < 256; i++) {
		this->m_bi->bmiColors[i].rgbRed = palette[2];
		this->m_bi->bmiColors[i].rgbGreen = palette[1];
		this->m_bi->bmiColors[i].rgbBlue = palette[0];
		palette += 4;
	}
}

void Bitmap::SetGrayPalette(const uint8_t* palette) const {
	if (palette) {
		for (int i = 0; i < 256; i++) {
			this->m_bi->bmiColors[i].rgbRed = palette[2] + i >= 255 ? 255 : palette[2] + i;
			this->m_bi->bmiColors[i].rgbGreen = palette[1] + i >= 255 ? 255 : palette[1] + i;
			this->m_bi->bmiColors[i].rgbBlue = palette[0] + i >= 255 ? 255 : palette[0] + i;
		}
	} else {
		for (int i = 0; i < 256; i++) {
			this->m_bi->bmiColors[i].rgbRed = i;
			this->m_bi->bmiColors[i].rgbGreen = i;
			this->m_bi->bmiColors[i].rgbBlue = i;
		}
	}
}

void Bitmap::SetBitmapinfo(BITMAPINFO* bi, uint8_t* vram) {
	this->m_vram = vram;
	this->m_bi = bi;
	this->m_lock = true;
	this->m_pitch = (bi->bmiHeader.biWidth * bi->bmiHeader.biBitCount / 8 + 3) & ~3;
}

void Bitmap::Save(char* filename) const {
	#pragma pack(push, 1)

	struct BmpInfo {
		int32_t size;
		int32_t width;
		int32_t height;
		INT16 planes;
		INT16 bitCount;
		int32_t compression;
		int32_t sizeImage;
		int32_t xPixelsPerMeter;
		int32_t yPixelsPerMeter;
		int32_t colorUsed;
		int32_t colorImportant;
	};

	struct BmpHead {
		char signature[2];
		int32_t size;
		INT16 reserved1;
		INT16 reserved2;
		int32_t offsetOfBits;
	};

	#pragma pack(pop)

	FILE* f;
	BmpInfo info{};
	BmpHead header{};

	info.size = sizeof(info);
	info.planes = 1;
	info.bitCount = 24;

	header.signature[0] = 'B';
	header.signature[1] = 'M';
	header.size = sizeof(header);
	header.offsetOfBits = sizeof(header) + sizeof(info);

	if (fopen_s(&f, filename, "wb") != 0) {
		return;
	}

	info.width = this->m_bi->bmiHeader.biWidth;
	info.height = this->m_bi->bmiHeader.biHeight;
	fwrite(&header, 1u, sizeof(header), f);
	fwrite(&info, 1u, sizeof(info), f);
	int y = info.height - 1;
	if (info.height == 0) {
		fclose(f);
		return;
	}

	int pitch = (3 * info.width + 3) & ~3;
	int gutter = pitch - 3 * info.width;
	do {
		int i = 0;
		if (info.width > 0) {
			int scan = 0;
			do {
				uint8_t* ptr = &this->m_vram[scan + y * this->m_pitch];
				fputc(ptr[0], f);
				fputc(ptr[1], f);
				fputc(ptr[2], f);
				scan += 3;
				++i;
			} while (i < info.width);
		}
		if (gutter > 0) {
			i = gutter;
			do {
				fputc(0, f);
				--i;
			} while (i);
		}
		--y;
	} while (y >= 0);

	fclose(f);
}

void Bitmap::PaintStretch(HWND hWnd, RECT* rect) const {
	HDC dc = GetDC(hWnd);
	StretchDIBits(
		dc,
		rect->left,
		rect->top,
		rect->right - rect->left,
		rect->bottom - rect->top,
		0,
		0,
		this->m_bi->bmiHeader.biWidth,
		std::abs(this->m_bi->bmiHeader.biHeight),
		this->m_vram,
		this->m_bi,
		0,
		SRCCOPY
	);
	ReleaseDC(hWnd, dc);
}

void Bitmap::Paint(HWND hWnd, POINT* p) const {
	HDC dc = GetDC(hWnd);
	DWORD w = this->m_bi->bmiHeader.biWidth;
	DWORD h = this->m_bi->bmiHeader.biHeight;
	if (p)
		SetDIBitsToDevice(dc, p->x, p->y, w, h, 0, h, h, h, this->m_vram, this->m_bi, 0);
	else
		SetDIBitsToDevice(dc, 0, 0, w, h, 0, h, h, h, this->m_vram, this->m_bi, 0);
	ReleaseDC(hWnd, dc);
}

uint8_t* Bitmap::GetVram(int32_t i) const {
	return &this->m_vram[i * this->m_pitch];
}

int32_t Bitmap::Width() const {
	return this->m_bi->bmiHeader.biWidth;
}

int32_t Bitmap::Height() const {
	return this->m_bi->bmiHeader.biHeight;
}

int32_t Bitmap::BitsPerPixel() const {
	return this->m_bi->bmiHeader.biBitCount;
}
