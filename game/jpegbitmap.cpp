#include "jpegbitmap.h"
#include <ijl.h>
#include <memory>

JpegBitmap::JpegBitmap(uint8_t* data, int len)
	: m_data(data), m_size(len) {}

bool JpegBitmap::DecodeHeader() {
	// Put JPEG_CORE_PROPERTIES in heap as it is rather large.
	std::unique_ptr<JPEG_CORE_PROPERTIES> jcprops{new JPEG_CORE_PROPERTIES};

	memset(jcprops.get(), 0, sizeof(jcprops));
	if (ijlInit(jcprops.get()) != IJL_OK) {
		return false;
	}

	jcprops->JPGBytes = reinterpret_cast<unsigned char*>(this->m_data);
	jcprops->JPGSizeBytes = this->m_size;

	if (ijlRead(jcprops.get(), IJL_JBUFF_READPARAMS) != IJL_OK) {
		ijlFree(jcprops.get());
		return false;
	}

	this->m_bi = reinterpret_cast<BITMAPINFO*>(new char[jcprops->JPGChannels != 1 ? 40 : 1064]);
	this->m_bi->bmiHeader.biSize = 40;
	this->m_bi->bmiHeader.biPlanes = 1;
	this->m_bi->bmiHeader.biCompression = 0;
	this->m_bi->bmiHeader.biSizeImage = 0;
	this->m_bi->bmiHeader.biClrUsed = 0;
	this->m_bi->bmiHeader.biWidth = jcprops->JPGWidth;
	this->m_bi->bmiHeader.biHeight = jcprops->JPGHeight;
	this->m_bi->bmiHeader.biBitCount = jcprops->JPGChannels != 1 ? 24 : 8;

	ijlFree(jcprops.get());
	return true;
}

bool JpegBitmap::DecodeImage() {
	// Put JPEG_CORE_PROPERTIES in heap as it is rather large.
    std::unique_ptr<JPEG_CORE_PROPERTIES> jcprops{ new JPEG_CORE_PROPERTIES };

    memset(jcprops.get(), 0, sizeof(jcprops));
	if (ijlInit(jcprops.get()) != IJL_OK) {
		return false;
	}

	jcprops->JPGBytes = reinterpret_cast<unsigned char*>(this->m_data);
    jcprops->JPGSizeBytes = this->m_size;

	if (ijlRead(jcprops.get(), IJL_JBUFF_READPARAMS) != IJL_OK) {
        ijlFree(jcprops.get());
        return false;
    }

    this->Create(jcprops->JPGWidth, jcprops->JPGHeight, jcprops->JPGChannels != 1 ? 24 : 8);

	jcprops->DIBBytes = this->m_vram;
    jcprops->DIBHeight = jcprops->JPGHeight;
    jcprops->DIBColor = jcprops->JPGChannels == 1 ? IJL_G : IJL_BGR;
    jcprops->DIBWidth = jcprops->JPGWidth;
    jcprops->DIBChannels = jcprops->JPGChannels != 1 ? 3 : 1;
    jcprops->DIBPadBytes = this->m_pitch - jcprops->JPGWidth * jcprops->DIBChannels;

	if (jcprops->JPGChannels == 1) {
        jcprops->JPGColor = IJL_G;
    } else if (jcprops->JPGChannels == 3) {
        jcprops->JPGColor = IJL_YCBCR;
    } else {
        jcprops->DIBColor = IJL_OTHER;
        jcprops->JPGColor = IJL_OTHER;
    }

	if (ijlRead(jcprops.get(), IJL_JBUFF_READWHOLEIMAGE)) {
        ijlFree(jcprops.get());
        return false;
    }

	// Create gray-scale palette if the image only has one channel.
    if (jcprops->JPGChannels == 1) {
        uint8_t pal[256 * 3];
		memset(&pal[0 * 3], 0x00, sizeof(uint8_t[8 * 3]));
    	for (int i = 8; i < 248; i++) {
			memset(&pal[i * 3], i, sizeof(uint8_t[3]));
    	}
		memset(&pal[248 * 3], 0xFF, sizeof(uint8_t[8 * 3]));
        this->SetPalette(pal);
    }
    ijlFree(jcprops.get());

	return true;
}

void JpegBitmap::Update() {
	if (!this->m_data || !this->DecodeImage()) return;
	delete[] this->m_data;
	this->m_data = nullptr;
}
