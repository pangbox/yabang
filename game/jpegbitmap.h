#pragma once
#include "bitmap.h"

class JpegBitmap : public Bitmap {
public:
	JpegBitmap(uint8_t* data, int len);
	bool DecodeHeader();
	bool DecodeImage();
	void Update() override;

protected:
	uint8_t* m_data = nullptr;
	int m_size = 0;
};
