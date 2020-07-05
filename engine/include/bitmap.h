#pragma once
#include <cstdint>
#include <windows.h>

class Bitmap final {
public:
	Bitmap();
	Bitmap(const Bitmap& other);
	Bitmap(Bitmap&& other) noexcept;
	Bitmap(int32_t w, int32_t h, int32_t bpp);

	Bitmap& operator=(const Bitmap& other);
	Bitmap& operator=(Bitmap&& other) noexcept;

	~Bitmap();
	void Create(int32_t w, int32_t h, int32_t bpp);
	void Update();
	void GetPixel(int32_t x, int32_t y, uint8_t &r, uint8_t &g, uint8_t &b, uint8_t &a) const;
	void GetPixel(int32_t x, int32_t y, uint8_t &r, uint8_t &g, uint8_t &b) const;
	void SetPixel(int32_t x, int32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a) const;
	[[nodiscard]] char GetAlpha(int32_t x, int32_t y) const;
	void SetAlpha(int32_t x, int32_t y, uint8_t a) const;
	void SetPalette(const uint8_t *palette) const;
	void SetGrayPalette(const uint8_t *palette) const;
	void SetBitmapinfo(BITMAPINFO *bi, uint8_t *vram);
	void Save(char *filename) const;
	void PaintStretch(HWND hWnd, RECT *rect) const;
	void Paint(HWND hWnd, POINT *p) const;
	[[nodiscard]] uint8_t *GetVram(int32_t i) const;
	[[nodiscard]] int32_t Width() const;
	[[nodiscard]] int32_t Height() const;
	[[nodiscard]] int32_t BitsPerPixel() const;

	BITMAPINFO* m_bi;
	int m_pitch;
	uint8_t* m_vram;

private:
	uint8_t *VMem(BITMAPINFO *bi);

	bool m_lock;
};
