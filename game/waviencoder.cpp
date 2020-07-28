#include "waviencoder.h"

#include <algorithm>
#include <io.h>
#include <cstdio>

WAVIEncoder::WAVIEncoder() = default;

WAVIEncoder::~WAVIEncoder() {
	this->Release();
}

bool WAVIEncoder::IsOpened() const {
	return this->m_bOpened;
}

void WAVIEncoder::WriteAVI(Bitmap* bitmap) {
	AVIStreamWrite(
		this->m_psCompressed,
		this->m_cur_frame++,
		1,
		bitmap->m_vram,
		bitmap->m_pitch * bitmap->m_bi->bmiHeader.biHeight,
		0x10,
		nullptr,
		nullptr);
}

void WAVIEncoder::Flush() {
	for (int i = 0; i < this->m_cur; i++) {
		this->WriteAVI(this->m_history[i]);
	}
	this->m_cur = 0;
}

void WAVIEncoder::Release() {
	if (this->m_history) {
		for (int i = 0; i < this->m_hisNum; ++i) {
			delete this->m_history[i];
		}
		delete this->m_history;
		this->m_history = nullptr;
	}
	this->m_hisNum = 0;
}

void WAVIEncoder::CopyBitmap(Bitmap* dest, Bitmap* src, bool flip) {
	for (int i = 0; i < dest->m_bi->bmiHeader.biHeight; ++i) {
		int scanline = flip ? dest->m_pitch * (dest->m_bi->bmiHeader.biHeight - i - 1) : i * dest->m_pitch;
		memcpy(&dest->m_vram[scanline], &src->m_vram[i * src->m_pitch], src->m_pitch);
	}
}

int* WAVIEncoder::Close(int* info) {
	if (this->m_bOpened) {
		this->Flush();
		AVIStreamRelease(this->m_ps);
		AVIStreamRelease(this->m_psCompressed);
		AVIFileRelease(this->m_pfile);
		AVIFileExit();
	}
	this->m_bOpened = false;

	FILE* f;
	fopen_s(&f, this->m_filename, "rb");
	int len = f ? _filelength(_fileno(f)) : 0;
	fclose(f);

	int* result = info;
	result[0] = this->m_cur_frame;
	result[1] = len;
	return result;
}

void WAVIEncoder::SetFrameBuffer(int page) {
	this->Release();
	this->m_hisNum = page;

	if (this->m_height <= 0 || this->m_width <= 0) {
		return;
	}

	this->m_history = new Bitmap*[page];
	this->m_cur = 0;

	for (int i = 0; i < this->m_hisNum; i++) {
		this->m_history[i] = new Bitmap(this->m_width, this->m_height, 24);
	}
}

void WAVIEncoder::ScaleBitmap(Bitmap* out, Bitmap* bitmap, int w, int h, int px, int py, bool flip) {
	if (out->m_bi->bmiHeader.biWidth == bitmap->m_bi->bmiHeader.biWidth
		&& out->m_bi->bmiHeader.biHeight == bitmap->m_bi->bmiHeader.biHeight
		&& out->m_bi->bmiHeader.biBitCount == bitmap->m_bi->bmiHeader.biBitCount) {
		this->CopyBitmap(out, bitmap, flip);
		return;
	}
	int scaleW = ((bitmap->m_bi->bmiHeader.biWidth - px) << 8) / out->m_bi->bmiHeader.biWidth;
	int scaleH = ((bitmap->m_bi->bmiHeader.biHeight - py) << 8) / out->m_bi->bmiHeader.biHeight;
	int extentDiv = 0x1000000 / (scaleW / 16 * (scaleH / 16));
	int v = py * scaleH;
	for (int y = 0; y < out->m_bi->bmiHeader.biHeight; y++) {
		int outline = flip ? out->m_bi->bmiHeader.biHeight - y - 1 : y;
		uint8_t* vram = &out->m_vram[outline * out->m_pitch];
		int u = px * scaleW;
		int x = 0;
		for (; x < out->m_bi->bmiHeader.biWidth; x++) {
			int r = 0;
			int g = 0;
			int b = 0;
			for (int y1 = v; y1 < v + scaleH;) {
				int dv = v + scaleH - y1;
				if (0x100 - static_cast<uint8_t>(y1) <= v + scaleH - y1) {
					dv = 0x100 - static_cast<uint8_t>(y1);
				}
				int x1 = u;
				int scanline = bitmap->m_pitch * (y1 >> 8);
				for (; x1 < u + scaleW;) {
					int dx = u + scaleW - x1;
					if (0x100 - static_cast<uint8_t>(x1) <= u + scaleW - x1) {
						dx = 0x100 - static_cast<uint8_t>(x1);
					}
					int index = scanline + 3 * (x1 >> 8);
					int f = extentDiv * dv * dx >> 16;
					r += f * bitmap->m_vram[index + 2];
					g += f * bitmap->m_vram[index + 1];
					b += f * bitmap->m_vram[index + 0];
					x1 += dx;
				}
				y1 += dv;
			}
			vram[2] = std::min(static_cast<int>(static_cast<float>(r) * 1.2f), 0xFF0000) >> 16;
			vram[1] = std::min(static_cast<int>(static_cast<float>(g) * 1.2f), 0xFF0000) >> 16;
			vram[0] = std::min(static_cast<int>(static_cast<float>(b) * 1.2f), 0xFF0000) >> 16;
			vram += 3;
			u += scaleW;
		}
		v += scaleH;
	}
}

bool WAVIEncoder::Open(const char* filename, int w, int h, int fps, HWND hwnd) {
	AVICOMPRESSOPTIONS opts;
	AVISTREAMINFOA streamInfo;
	this->m_cur_frame = 0;
	this->m_width = w;
	this->m_height = h;
	AVIFileInit();
	if (FAILED(AVIFileOpenA(&this->m_pfile, filename, OF_CREATE|OF_WRITE, nullptr))) {
		return false;
	}
	memset(&streamInfo, 0, sizeof(streamInfo));
	memcpy(&streamInfo.fccType, "vids", 4);
	memcpy(&streamInfo.fccHandler, "MSVC", 4);
	streamInfo.dwSuggestedBufferSize = 0;
	streamInfo.rcFrame.left = 0;
	streamInfo.rcFrame.top = 0;
	streamInfo.dwRate = fps;
	streamInfo.rcFrame.right = w;
	streamInfo.dwScale = 1;
	streamInfo.dwQuality = -1;
	streamInfo.rcFrame.bottom = h;
	if (FAILED(AVIFileCreateStreamA(this->m_pfile, &this->m_ps, &streamInfo))) {
		return false;
	}
	memset(&opts, 0, sizeof(opts));
	if (FAILED(AVIMakeCompressedStream(&this->m_psCompressed, this->m_ps, &opts, nullptr))) {
		return false;
	}
	Bitmap bitmap(w, h, 24);
	if (FAILED(AVIStreamSetFormat(this->m_psCompressed, 0, bitmap.m_bi, 40))) {
		return false;
	}
	if (this->m_hisNum > 0) {
		this->SetFrameBuffer(this->m_hisNum);
	}
	this->m_bOpened = true;
	strcpy_s(this->m_filename, MAX_PATH, filename);
	return true;
}

Bitmap* WAVIEncoder::ScaleBitmap(Bitmap* bitmap, int w, int h, int px, int py, bool flip) {
	auto* out = new Bitmap(w, h, 24);
	if (w == bitmap->m_bi->bmiHeader.biWidth && h == bitmap->m_bi->bmiHeader.biHeight
		&& bitmap->m_bi->bmiHeader.biBitCount == out->m_bi->bmiHeader.biBitCount) {
		this->CopyBitmap(out, bitmap, flip);
	} else {
		this->ScaleBitmap(out, bitmap, w, h, px, py, flip);
	}
	return out;
}

void WAVIEncoder::Write(Bitmap* bitmap) {
	if (this->m_history) {
		this->ScaleBitmap(this->m_history[this->m_cur++], bitmap, this->m_width, this->m_height, 0, 0, true);
		if (this->m_cur >= this->m_hisNum) {
			this->Flush();
		}
		return;
	}
	Bitmap* scaled = this->ScaleBitmap(bitmap, this->m_width, this->m_height, 0, 0, true);
	AVIStreamWrite(
		this->m_psCompressed,
		this->m_cur_frame++,
		1,
		scaled->m_vram,
		scaled->m_pitch * scaled->m_bi->bmiHeader.biHeight,
		0x10u,
		nullptr,
		nullptr);
	delete scaled;
}
