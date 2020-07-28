#pragma once
#include <unknwn.h>

#include "bitmap.h"
#include "vfw.h"

class WAVIEncoder {
public:
	WAVIEncoder();
	virtual ~WAVIEncoder();

	bool IsOpened() const;
	void WriteAVI(Bitmap* bitmap);
	void Flush();
	void Release();
	void CopyBitmap(Bitmap* dest, Bitmap* src, bool flip);
	int* Close(int* info);
	void SetFrameBuffer(int page);
	void ScaleBitmap(Bitmap* out, Bitmap* bitmap, int w, int h, int px, int py, bool flip);
	bool Open(const char* filename, int w, int h, int fps, HWND hwnd);
	Bitmap* ScaleBitmap(Bitmap* bitmap, int w, int h, int px, int py, bool flip);
	void Write(Bitmap* bitmap);

protected:
	IAVIFile* m_pfile = nullptr;
	IAVIStream* m_ps = nullptr;
	IAVIStream* m_psCompressed = nullptr;
	int m_cur_frame = 0;
	int m_width = 0;
	int m_height = 0;
	Bitmap** m_history = nullptr;
	int m_hisNum = 0;
	int m_cur = 0;
	bool m_bOpened = false;
	char m_filename[260] = {};
};