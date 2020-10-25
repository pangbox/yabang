#include "wresourcemanager.h"

#include "bitmap.h"
#include "cfile.h"
#include "w3dspr.h"
#include "woverlay.h"
#include "wresource.h"
#include "wtitlefont.h"
#include "wvideo.h"

#include <png.h>
#include "cexcept.h"
#include "jpegbitmap.h"
#include "wfixedfont.h"

WResourceManager* g_resourceManager = nullptr;

WResourceManager::WResourceManager() {
	remove("files_of_the_same_name.txt");
}

WVideoDev* WResourceManager::VideoReference() const {
	return this->m_video;
}

void WResourceManager::SetVideoReference(WVideoDev* video) {
	this->m_video = video;
}

const WList<w_texlist*>& WResourceManager::GetTextureList() const {
	return this->texList;
}

void WResourceManager::Lock(rmlType_t type) {
	this->m_lock[type].Lock();
}

void WResourceManager::Unlock(rmlType_t type) {
	this->m_lock[type].Unlock();
}

void WResourceManager::UnlockAllByThread(unsigned int threadId) {
	for (auto& lock : this->m_lock) {
		lock.UnlockInThread(threadId);
	}
}

void WResourceManager::SetUseLessVideoRam(int level) {
	this->m_savemem[0] = false;
	this->m_savemem[1] = level > 0;
	this->m_savemem[2] = level > 1;
}

void WResourceManager::Release(WResource* resource) {
	delete resource;
}

void WResourceManager::Release(int hTex) {
	w_texlist* tex = this->FindTexture(hTex);
	if (!tex) {
		return;
	}
	tex->count--;
	if (tex->count <= 0) {
		this->m_nByteUsedTexture -= tex->size;
		this->m_video->DestroyTexture(hTex);
		this->texList.DelItem(tex);
		delete[] tex;
	}
}

w_texlist* WResourceManager::FindTexture(int texHandle) {
	for (auto* it = this->texList.Start(); it != nullptr; it = this->texList.Next()) {
		if (it->texhandle == texHandle) {
			return it;
		}
	}
	return nullptr;
}

w_texlist* WResourceManager::FindTexture(const char* texName) {
	char name[64];
	this->StrcpyLower(name, strchr(texName, '/') ? strrchr(texName, '/') + 1 : texName);
	return this->texList.Find(name);
}

w_texlist* WResourceManager::AddTexture(const char* texName, int texHandle, int width, int height)
{
	char name[64];
	this->StrcpyLower(name, strchr(texName, '/') ? strrchr(texName, '/') + 1 : texName);
	auto* texlist = reinterpret_cast<w_texlist*>(new uint8_t[sizeof(w_texlist) + strlen(name)]);
	texlist->count = 1;
	texlist->texhandle = texHandle;
	texlist->width = width;
	texlist->height = height;
	texlist->size = 0;
	strcpy(texlist->texname, name);
	this->texList.AddItem(texlist, texlist->texname, false);
	return texlist;
}

WOverlay* WResourceManager::GetOverlay(const char* filename, unsigned int flag) {
	auto* overlay = new WOverlay();
	overlay->m_resrcMng = this;
	if (!filename || !overlay->Load(filename, flag)) {
		return overlay;
	}
	delete overlay;
	return nullptr;
}

WOverlay* WResourceManager::GetOverlay(const char* name, Bitmap* bitmap, unsigned int flag) {
	auto* overlay = new WOverlay();
	overlay->m_resrcMng = this;
	if (!name || !overlay->Load(name, bitmap, flag)) {
		return overlay;
	}
	delete overlay;
	return nullptr;
}

WTitleFont* WResourceManager::GetTitleFont() {
	auto* font = new WTitleFont();
	font->m_resrcMng = this;
	return font;
}

void WResourceManager::Release(WTitleFont* font) {
	delete font;
}

W3dSpr* WResourceManager::Get3DSpr(const char* filename, int type) {
	W3dSpr* spr = new W3dSpr();
	spr->m_resrcMng = this;
	if (!spr->LoadSprite(filename, type)) {
		return spr;
	}
	delete spr;
	return nullptr;
}

W3dAniSpr* WResourceManager::Get3DAniSpr(const char* filename, int type, float fSprSizeX, float fSprSizeY) {
	W3dAniSpr* spr = new W3dAniSpr();
	spr->m_resrcMng = this;
	if (!spr->LoadSpritesInOneTexture(filename, type, fSprSizeX, fSprSizeY)) {
		return spr;
	}
	delete spr;
	return nullptr;
}

void WResourceManager::StrcpyLower(char* out, const char* src) {
	for (; *src; src++) {
		*out++ = (*src >= 'A' && *src <= 'Z') ? (*src - 0x20) : *src;
	}
	*out = 0;
}

const char* WResourceManager::FindMatchFile(const char* filename) {
	char lowername[64] = {0};

	if (!this->m_matchDirectory) {
		return filename;
	}

	const char* basename = strrchr(filename, '/');
	if (!basename) {
		basename = strrchr(filename, '\\');
		if (!basename) {
			return filename;
		}
	}

	// Increment past the directory separator.
	basename++;

	StrcpyLower(lowername, basename);

	auto* match = this->m_matchList.Find(lowername);
	if (!match) {
		return filename;
	}

	return match->fullname;
}

void WResourceManager::AddOriginList(WResource* resource) {
	this->originList.AddItem(resource, nullptr, false);
}

WFixedFont* WResourceManager::GetFixedFont(const char* filename) {
	auto* cachedFont = this->fontList.originList.Find(filename);
	if (cachedFont && cachedFont->resource) {
		auto *font = static_cast<WFixedFont*>(cachedFont->resource->MakeClone());
		if (font != nullptr) {
			font->m_resrcMng = this;
			this->fontList.AddResource(filename, font);
			return font;
		}
	}
	
	auto *font = new WFixedFont(8);
	font->m_resrcMng = this;
	if (font->Load(filename)) {
		delete font;
		return 0;
	}
	if (this->fontList.AddResource(filename, font)) {
		this->AddOriginList(font);
	}
	return font;
}

int WResourceManager::UploadTexture(const char* texName, const Bitmap* bitmap, unsigned int type, RECT* rect) {
	if (texName) {
		w_texlist* tex = this->FindTexture(texName);
		if (tex) {
			++tex->count;
			return tex->texhandle;
		}
	}

	int texHandle;
	int width;
	int height;

	if (rect) {
		Bitmap temp;
		temp.Create(rect->right - rect->left + 1, rect->bottom - rect->top + 1, bitmap->m_bi->bmiHeader.biBitCount);
		int bpp = (static_cast<unsigned int>(bitmap->m_bi->bmiHeader.biBitCount) + 7) / 8;
		if (bpp <= 1) {
			int colors = bitmap->m_bi->bmiHeader.biClrUsed;
			if (!colors) {
				colors = 256;
			}
			for (int i = 0; i < colors; ++i)
			{
				temp.m_bi->bmiColors[i].rgbRed = bitmap->m_bi->bmiColors[i].rgbRed;
				temp.m_bi->bmiColors[i].rgbGreen = bitmap->m_bi->bmiColors[i].rgbGreen;
				temp.m_bi->bmiColors[i].rgbBlue = bitmap->m_bi->bmiColors[i].rgbBlue;
			}
		}
		int scanline = 0;
		for (int y = rect->top; y < rect->bottom; ++y) {
			memcpy(
				&temp.m_vram[scanline],
				&bitmap->m_vram[y * bitmap->m_pitch + bpp * rect->left],
				bpp * (rect->right - rect->left + 1));
			scanline += temp.m_pitch;
		}
		height = temp.m_bi->bmiHeader.biHeight;
		width = temp.m_bi->bmiHeader.biWidth;
		texHandle = this->m_video->CreateTexture(temp.m_bi, type);
		this->m_video->UpdateTexture(texHandle, temp.m_bi, temp.m_vram, type);
	} else {
		width = bitmap->m_bi->bmiHeader.biWidth;
		height = bitmap->m_bi->bmiHeader.biHeight;
		texHandle = this->m_video->CreateTexture(bitmap->m_bi, type);
		if ((type & 0x1001F000) == 0) {
			this->m_video->UpdateTexture(texHandle, bitmap->m_bi, bitmap->m_vram, type);
		}
	}

	const char* texListName = texName;
	if (!texName || rect) {
		texListName = "Unknown";
	}

	w_texlist* tex = this->AddTexture(texListName, texHandle, width, height);
	unsigned int size = width * height * ((type & 0x80000000) != 0 ? 3 : 2);
	tex->size = size;
	if ((type & 0x40000000) != 0) {
		tex->size = size + size / 2;
	}
	this->m_nByteUsedTexture += tex->size;
	if (!tex->texhandle) {
		this->AddMissingTexture(tex->texname);
	}
	return tex->texhandle;
}

void WResourceManager::FixTexture(int texHandle, const Bitmap* bitmap, unsigned int type, RECT* rect) const {
	if (rect) {
		this->m_video->FixTexturePart(texHandle, *rect, bitmap->m_bi, bitmap->m_vram, type);
	} else {
		this->m_video->UpdateTexture(texHandle, bitmap->m_bi, bitmap->m_vram, type);
	}
}

Bitmap* WResourceManager::Combine4Alpha(Bitmap* bitmap, Bitmap* alpha) {
	uint8_t a;
	uint8_t r;
	uint8_t g;
	uint8_t b;

	bitmap->Update();
	alpha->Update();
	auto* v4 = new Bitmap();
	v4->Create(bitmap->m_bi->bmiHeader.biWidth, bitmap->m_bi->bmiHeader.biHeight, 32);
	unsigned int y = 0;
	if (bitmap->m_bi->bmiHeader.biHeight)
	{
		unsigned int w = bitmap->m_bi->bmiHeader.biWidth;
		do
		{
			unsigned int x = 0;
			if (w)
			{
				int v25 = 0;
				do
				{
					if (bitmap->m_bi->bmiHeader.biBitCount <= 8) {
						int index = bitmap->m_vram[y * bitmap->m_pitch + x];
						r = bitmap->m_bi->bmiColors[index].rgbRed;
						b = bitmap->m_bi->bmiColors[index].rgbBlue;
						g = bitmap->m_bi->bmiColors[index].rgbGreen;
					} else {
						r = bitmap->m_vram[y * bitmap->m_pitch + x * 3 + 2];
						g = bitmap->m_vram[y * bitmap->m_pitch + x * 3 + 1];
						b = bitmap->m_vram[y * bitmap->m_pitch + x * 3 + 0];
					}
					if (alpha->m_bi->bmiHeader.biBitCount > 8) {
						a = alpha->m_vram[y * alpha->m_pitch + x * (alpha->m_bi->bmiHeader.biBitCount >> 3)];
					} else {
						a = alpha->m_bi->bmiColors[alpha->m_vram[x + y * alpha->m_pitch]].rgbBlue;
					}
					v4->m_vram[x * 4 + y * v4->m_pitch] = b;
					v4->m_vram[x * 4 + 1 + y * v4->m_pitch] = g;
					v4->m_vram[x * 4 + 2 + y * v4->m_pitch] = r;
					v4->m_vram[x * 4 + 3 + y * v4->m_pitch] = a;
					w = bitmap->m_bi->bmiHeader.biWidth;
					++x;
					v25 += 3;
				} while (x < w);
			}
			++y;
		} while (y < bitmap->m_bi->bmiHeader.biHeight);
	}
	return v4;
}

bool WResourceManager::CheckMaskTexture(Bitmap** ppBitmap, const char* filename) {
	char afname[MAX_PATH];

	if (!ppBitmap || !*ppBitmap || !filename || !*filename) {
		return false;
	}

	if (!strchr(filename, '[') && !strchr(filename, '+')) {
		return false;
	}

	strcpy(afname, filename);
	sprintf(strrchr(afname, '.'), "_mask%s", strrchr(filename, '.'));

	Bitmap* alpha = this->LoadBitmapA(afname, 0, false);
	if (!alpha) {
		return false;
	}

	if (alpha->m_bi->bmiHeader.biBitCount > 8) {
		MessageBoxA(nullptr, "마스크 텍스춰는 8비트 그레이를 이용해주세요.\n0.0a", filename, MB_OK);
	}

	if (!*ppBitmap ||
		(*ppBitmap)->m_bi->bmiHeader.biWidth !=
			alpha->m_bi->bmiHeader.biWidth ||
		(*ppBitmap)->m_bi->bmiHeader.biHeight !=
			alpha->m_bi->bmiHeader.biHeight) {
		return false;
	}

	Bitmap* combined = this->Combine4Alpha(*ppBitmap, alpha);
	if (*ppBitmap) {
		delete *ppBitmap;
		*ppBitmap = nullptr;
	}

	delete alpha;
	*ppBitmap = combined;

	return true;
}

int WResourceManager::LoadTexture(const char* filename, unsigned int type, int level, const char* texName) {
	char afname[MAX_PATH];     

	int texHandle = 0;
	this->m_lock[0].Lock();
	if (!texName || !_stricmp(filename, texName)) {
		w_texlist* tex = this->FindTexture(filename);
		if (tex) {
			++tex->count;
			texHandle = tex->texhandle;
			this->m_lock[0].Unlock();
			return texHandle;
		}
	}
	Bitmap* bitmap = this->LoadBitmapA(filename, level, false);
	if (bitmap) {
		bitmap->Update();
		if (strchr(filename, '{')) {
			type = (type & 0xBFFFFFFF) | 0x800;
		}
		if (strchr(filename, '!')) {
			type |= 0x20000000;
		}
		this->CheckMaskTexture(&bitmap, filename);
		const char* name = texName;
		if (!texName) {
			name = filename;
		}
		texHandle = this->UploadTexture(name, bitmap, type, nullptr);
		delete bitmap;
	} else {
		strcpy(afname, filename);
		_strlwr(afname);
		if (strstr(afname, ".dds"))
		{
			cFile* fp = GetCFile(this->FindMatchFile(filename), 0xFFFF);
			if (!fp) {
				this->AddMissingTexture(filename);
				this->m_lock[0].Unlock();
				return 0;
			}
			auto* buff = new uint8_t[fp->Length()];
			fp->Read(buff, fp->Length());
			CloseCFile(fp);
			texHandle = this->m_video->UploadCompressedTexture(buff, fp->Length(), type);
			const char* name = texName;
			if (!texName) {
				name = filename;
			}
			this->AddTexture(name, texHandle, this->m_video->GetTextureWidth(texHandle), this->m_video->GetTextureHeight(texHandle));
			delete[] buff;
		}
	}
	this->m_lock[0].Unlock();
	return texHandle;
}

int WResourceManager::GetTextureWidth(int texHandle) {
	for (auto* it = this->texList.Start(); it != nullptr; it = this->texList.Next()) {
		if (it->texhandle == texHandle) {
			return it->width;
		}
	}
	return 0;
}

int WResourceManager::GetTextureHeight(int texHandle) {
	for (auto* it = this->texList.Start(); it != nullptr; it = this->texList.Next()) {
		if (it->texhandle == texHandle) {
			return it->height;
		}
	}
	return 0;
}

Bitmap* WResourceManager::MakeQuarterBitmap(Bitmap* bitmap) {
	// TODO: should downsample image.
	return new Bitmap(*bitmap);
}

void WResourceManager::AddMissingTexture(const char* filename) {
	if (!this->m_missingTextureList.Find(filename)) {
		char* filenameCopied = new char[strlen(filename) + 1];
		strcpy(filenameCopied, filename);
		this->m_missingTextureList.AddItem(filenameCopied, filenameCopied, false);
	}
}

Bitmap* WResourceManager::LoadBRES(const char* zipName, const char* filename) {
	// TODO: Load zipped bitmap.
	return nullptr;
}

Bitmap* WResourceManager::LoadBMP(const char* filename) {
	cFile* fp = GetCFile(this->FindMatchFile(filename), 0xFFFF);
	if (!fp) {
		this->AddMissingTexture(filename);
		return nullptr;
	}

	bmp_head header{};
	fp->Read(&header, sizeof(bmp_head));
	if (memcmp(reinterpret_cast<void*>(&header), "bm", 2) != 0) {
		CloseCFile(fp);
		return nullptr;
	}

	int offset = header.offbits - sizeof(bmp_head);
	int len = fp->Length() - fp->Tell();
	auto* info = reinterpret_cast<bmp_info*>(new uint8_t[len]);
	fp->Read(info, len);
	auto* result = new Bitmap();
	uint8_t* vram = &reinterpret_cast<uint8_t*>(info)[offset];
	int pitch = (info->width * (info->bitcount >> 3) + 3) & ~3;
	result->Create(info->width, info->height, info->bitcount);

	if (info->bitcount <= 8) {
		int clrused = info->clrused;
		if (!clrused) {
			clrused = 1 << info->bitcount;
		}
		memcpy(result->m_bi->bmiColors, &info[1], 4 * clrused);
	}

	for (int y = 0; y < info->height; ++y) {
		memcpy(&result->m_vram[y * result->m_pitch], &vram[pitch * (info->height - y - 1)], result->m_pitch);
	}

	delete[] info;
	CloseCFile(fp);
	return result;
}

Bitmap* WResourceManager::LoadJPG(const char* filename) {
	cFile* fp = GetCFile(FindMatchFile(filename), 0xFFFF);
	if (!fp) {
		AddMissingTexture(filename);
		return nullptr;
	}

	auto *buf = new uint8_t[fp->Length()];
	fp->Read(buf, fp->Length());
	CloseCFile(fp);
	auto* bitmap = new JpegBitmap(buf, fp->Length());
	if (!bitmap->DecodeHeader()) {
		delete bitmap;
		return nullptr;
	}
	return bitmap;
}

define_exception_type(const char*);

extern struct exception_context the_exception_context[1];
struct exception_context the_exception_context[1];

static void png_cexcept_error(png_structp png_ptr, png_const_charp msg) {
	UNREFERENCED_PARAMETER(png_ptr);
	Throw msg;
}

void __cdecl png_read_data(png_struct_def* png_ptr, uint8_t* data, unsigned int length) {
	cFile* fp = static_cast<cFile*>(png_get_io_ptr(png_ptr));
	if (fp->Read(data, length) != length) {
		png_error(png_ptr, "Read Error");
	}
}

void __cdecl png_read_data2(png_structp png_ptr, uint8_t* data, unsigned int length) {
	uint8_t*& memData = *static_cast<uint8_t**>(png_get_io_ptr(png_ptr));
	memcpy(data, memData, length);
	memData += length;
}

Bitmap* WResourceManager::LoadPNG(const char* filename, bool bFromMem, bool bNet) {
	Bitmap* bitmap;
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned int iWidth;
	int iColorType;
	unsigned int iHeight;
	int iBitDepth;
	uint8_t pngSig[8];
	png_const_charp msg;

	cFile* fp = nullptr;
	const uint8_t* memData = nullptr;
	const uint8_t* memPtr;

	if (bFromMem) {
		memData = reinterpret_cast<const uint8_t*>(filename);
	} else {
		const char* match = this->FindMatchFile(filename);
		fp = GetCFile(match, 0xFFFF);
		if (!fp) {
			this->AddMissingTexture(filename);
			return nullptr;
		}
	}

	if (bFromMem) {
		memcpy(pngSig, memData, 8);
	} else {
		fp->Read(pngSig, 8);
	}

	if (!png_check_sig(pngSig, 8)) {
		if (fp) {
			CloseCFile(fp);
		}
		return nullptr;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, png_cexcept_error, nullptr);
	if (png_ptr == nullptr) {
		if (fp) {
			CloseCFile(fp);
		}
		return nullptr;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		if (fp) {
			CloseCFile(fp);
		}
		return nullptr;
	}

	Try {
		if (!bFromMem) {
			png_set_read_fn(png_ptr, fp, png_read_data);
			png_set_sig_bytes(png_ptr, 8);
		} else {
			memPtr = memData + 8;
			png_set_read_fn(png_ptr, reinterpret_cast<png_voidp>(&memPtr), png_read_data2);
			png_set_sig_bytes(png_ptr, 8);
		}

		png_read_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr, &iWidth, &iHeight, &iBitDepth, &iColorType, nullptr, nullptr, nullptr);
		if (iBitDepth == 16) {
			png_set_strip_16(png_ptr);
		}

		if (bNet && png_get_bit_depth(png_ptr, info_ptr) == 24 && iBitDepth == 8) {
			png_set_add_alpha(png_ptr, 0xFF, 1);
		}

		if (iColorType == 3) {
			png_set_expand(png_ptr);
		}

		if (iBitDepth < 8) {
			png_set_expand(png_ptr);
		}

		if (png_get_valid(png_ptr, info_ptr, 16)) {
			png_set_expand(png_ptr);
		}

		if (!iColorType || iColorType == 4) {
			png_set_gray_to_rgb(png_ptr);
		}

		png_set_bgr(png_ptr);
		png_read_update_info(png_ptr, info_ptr);
		png_get_IHDR(png_ptr, info_ptr, &iWidth, &iHeight, &iBitDepth, &iColorType, nullptr, nullptr, nullptr);
		int pitch = png_get_rowbytes(png_ptr, info_ptr);
		int channels = png_get_channels(png_ptr, info_ptr);

		bitmap = new Bitmap();
		if (!bitmap) {
			if (fp) {
				CloseCFile(fp);
			}
			return nullptr;
		}

		bitmap->Create(iWidth, iHeight, 8 * channels);
		auto** ppbRowPointers = static_cast<uint8_t**>(png_malloc(png_ptr, 4 * iHeight));
		if (!ppbRowPointers)
			png_error(png_ptr, "Visual PNG: out of memory");
		for (unsigned int y = 0; y < iHeight; ++y) {
			ppbRowPointers[y] = &bitmap->m_vram[pitch * y];
		}
		png_read_image(png_ptr, ppbRowPointers);
		png_read_end(png_ptr, nullptr);
		free(ppbRowPointers);
		ppbRowPointers = nullptr;
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
	}
	Catch(msg) {
		if (fp) {
			CloseCFile(fp);
		}
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		return nullptr;
	}

	if (fp) {
		CloseCFile(fp);
	}

	return bitmap;
}

Bitmap* WResourceManager::LoadTGA(const char* filename) {
	const char* match = this->FindMatchFile(filename);
	cFile* fp = GetCFile(match, 0xFFFF);
	if (!fp) {
		this->AddMissingTexture(filename);
		return nullptr;
	}

	sTgaHeader header{};
	fp->Read(&header, sizeof(sTgaHeader));
	if (header.iType != 2) {
		CloseCFile(fp);
		return nullptr;
	}
	fp->Seek(header.id_size, SEEK_CUR);

	auto* bitmap = new Bitmap();
	bitmap->Create(header.width, header.height, header.bitcount);
	for (int y = 0; y < header.height; ++y) {
		fp->Read(
			&bitmap->m_vram[bitmap->m_pitch * (header.height - y - 1)],
			header.bitcount * header.width / 8
		);
	}

	CloseCFile(fp);
	return bitmap;
}

Bitmap* WResourceManager::LoadBitmapA(const char* filename, int level, bool bNet) {
	char temp[256];
	Bitmap* result = nullptr;
	this->m_lock[1].Lock();
	const char* extension = strrchr(filename, '.');
	const char* zipName = strrchr(filename, '?');
	if (zipName) {
		memset(temp, 0, 0x100);
		memcpy(temp, filename, zipName - filename);
		result = this->LoadBRES(zipName + 1, temp);
	} else if (extension) {
		if (!_strcmpi(extension, ".bmp")) {
			result = this->LoadBMP(filename);
		} else if (!_strcmpi(extension, ".jpg")) {
			result = this->LoadJPG(filename);
		} else if (!_strcmpi(extension, ".png")) {
			result = this->LoadPNG(filename, false, bNet);
		} else if (!_strcmpi(extension, ".tga")) {
			result = this->LoadTGA(filename);
		}
		if (!result && (!_strcmpi(extension, ".jpg") || !_strcmpi(extension, ".bmp"))) {
			strcpy(&temp[4], filename);
			strcpy(strrchr(temp + 4, '.'), ".png");
			result = this->LoadPNG(temp + 4, false, false);
		}
	}
	if (result && result->m_bi->bmiHeader.biWidth > 8 && result->m_bi->bmiHeader.biHeight > 8 && this->m_savemem[level]
	) {
		Bitmap* downsampled = this->MakeQuarterBitmap(result);
		delete result;
		result = downsampled;
	}
	if (!level && result) {
		result->Update();
	}
	this->m_lock[1].Unlock();
	return result;
}

cFile* WResourceManager::GetCFile(const char* filename, int len) {
	return ::GetCFile(this->FindMatchFile(filename), len, 0);
}
