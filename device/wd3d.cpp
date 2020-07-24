#include "wd3d.h"
#include <cmath>
#include <wutil.h>
#include <algorithm>

WDirect3D::WDirect3D() {
	this->ApplyGamma(1.3f);
}

void WDirect3D::ClearVertices() {
	this->m_dwOffset = 0;
}

void WDirect3D::Release() {
	for (int i = 0; i < 0x800; i++) {
		if (this->m_texList[i]) {
			this->DestroyTexture(i);
			this->m_texList[i] = TexStateVoid;
		}
	}
	for (int i = 1; i < 0x400; i++) {
		if (this->m_xahVbs[i]) {
			this->XReleaseVertexBuffer(i);
			this->m_xahVbs[i] = 0;
		}
	}
	for (int i = 1; i < 0x100; i++) {
		if (this->m_xahIbs[i]) {
			this->XReleaseIndexBuffer(i);
			this->m_xahIbs[i] = 0;
		}
	}
	if (this->m_chVertex) {
		delete[] this->m_chVertex;
		this->m_chVertex = nullptr;
	}
}

bool WDirect3D::SetFogEnable(bool enable) {
	return this->m_fog = enable;
}

void WDirect3D::SetFogState(float fogStart, float fogEnd, uint32_t color) {
	this->m_fogStart = fogStart;
	this->m_fogEnd = fogEnd;
	this->m_fogColor = color & 0xFFFFFF;
}

uint8_t WDirect3D::CalcFog(float depth) {

	if (this->m_clipNearScale == 0.0f) {
		this->m_clipNearScale = 0.1f;
	}
	double x = static_cast<double>(this->m_clipScaleZ) - depth;
	if (x == 0.0) {
		x = 0.0000001;
	}
	float d = this->m_clipNearScale / static_cast<float>(x);
	if (d > this->m_fogEnd) {
		return 0;
	}
	if (d > this->m_fogStart) {
		return static_cast<uint8_t>((this->m_fogEnd - d) * 255.0f / (this->m_fogEnd - this->m_fogStart));
	}
	return -1;
}

float WDirect3D::CalcDepth(WtVertex** p, int num) {
	double n = 0.0;
	for (int i = 0; i < num; i++) {
		n += p[i]->vz;
	}
	return static_cast<float>(n / static_cast<float>(num));
}

void WDirect3D::DrawBuffered(const WPolyBufferSw& bufSw) {
	this->ApplyCustomRenderState(bufSw.customRenderState);
	this->DrawPrimitive(bufSw.type, bufSw.num, bufSw.dwVertexTypeDesc, bufSw.lpvVertices, bufSw.dptPrimitiveType,
	                    bufSw.type2);
}

void WDirect3D::DrawBuffered(const WPolyBufferHw& bufHw) {
	this->ApplyCustomRenderState(bufHw.customRenderState);
	this->XDrawIndexedPrimitive(bufHw.viewState, bufHw.batchState);
}

int WDirect3D::ComparePolyDepth(const void* elem1, const void* elem2) {
	const auto* fElem1 = static_cast<const float*>(elem1);
	const auto* fElem2 = static_cast<const float*>(elem2);

	if (fElem1[4] > fElem2[4]) {
		return -1;
	}

	if (fElem1[4] < fElem2[4]) {
		return 1;
	}

	return elem1 >= elem2 ? 1 : -1;
}

int WDirect3D::GetTextureNum(int stage) {
	if (stage) {
		for (int i = 0; i < 127 && this->m_texList[this->m_texCount2]; ++i) {
			this->m_texCount2 = this->m_texCount2 < 127 ? this->m_texCount2 + 1 : 1;
		}
		this->m_texList[this->m_texCount2] = TexStateCreated;
		return this->m_texCount2;
	}
	for (int i = 0; i < 1920 && this->m_texList[this->m_texCount1]; ++i) {
		this->m_texCount1 = this->m_texCount1 < 2047 ? this->m_texCount1 + 1 : 128;
	}
	this->m_texList[this->m_texCount1] = TexStateCreated;
	return this->m_texCount1;
}

int WDirect3D::UploadCompressedTexture(void* pSrc, size_t srcSize, int type) {
	int result = this->UploadCompressedTextureSurface(pSrc, srcSize, type);
	this->m_texList[result] = TexStateUpdated;
	return result;
}

int WDirect3D::CreateTexture(LPBITMAPINFO src, int type) {
	int bitNum;
	unsigned int bChromaKey;

	int texNum = GetTextureNum(static_cast<int>(type >> 29 & 1));
	if (!(type & 0x1000)) {
		if (this->m_useHiQualityTex || (bitNum = 16, type < 0)) {
			bitNum = 24;
		}

		if (src->bmiHeader.biBitCount < 0x20) {
			bChromaKey = static_cast<unsigned int>(type) >> 11 & 1;
		} else {
			bChromaKey = 2;
		}

		this->CreateTextureSurface(src->bmiHeader.biWidth, src->bmiHeader.biHeight,
		                           ((type & 0x40000000) | 0x20000000) >> 29, type & 0x1E000, texNum,
		                           static_cast<int>(bChromaKey), bitNum);
		this->m_texList[texNum] = TexStateUpdated;
	}

	return texNum;
}

void WDirect3D::UpdateTexture(int texHandle, LPBITMAPINFO src, void* data, uint32_t type) {
	if (this->m_texList[texHandle] == 1) {
		this->CreateTextureSurface(src->bmiHeader.biWidth, src->bmiHeader.biHeight, 1, 0, texHandle, 0, 16);
		this->m_texList[texHandle] = TexStateUpdated;
	}
	this->UpdateTextureSurface(texHandle, src, data, type);
}

void WDirect3D::DestroyTexture(int texHandle) {
	this->m_texList[texHandle] = TexStateVoid;
}

int WDirect3D::XGetVbHandle() {
	for (int i = 1; i < 0x400 && this->m_xahVbs[this->m_xiVbCount]; ++i) {
		this->m_xiVbCount = this->m_xiVbCount < 0x400 ? this->m_xiVbCount + 1 : 1;
	}
	this->m_xahVbs[this->m_xiVbCount] = 1;
	return this->m_xiVbCount;
}

int WDirect3D::XGetIbHandle() {
	for (int i = 1; i < 0x100 && this->m_xahIbs[this->m_xiIbCount]; ++i) {
		this->m_xiIbCount = this->m_xiIbCount < 0x100 ? this->m_xiIbCount + 1 : 1;
	}
	this->m_xahIbs[this->m_xiIbCount] = 1;
	return this->m_xiIbCount;
}

void WDirect3D::XReleaseVertexBuffer(int hVb) {
	this->m_xahVbs[hVb] = 0;
}

void WDirect3D::XReleaseIndexBuffer(int hIb) {
	this->m_xahIbs[hIb] = 0;
}

void* WDirect3D::ModifyVertices(WtVertex** vl, int n, uint32_t dwVertexTypeDesc, bool store) {
	TRACE("vl=%p, n=%d, dwVertexTypeDesc=%08x, store=%d", vl, n, dwVertexTypeDesc, store);

	uint32_t dwOffset = this->m_dwOffset;
	if (dwOffset >= 0xCCCCC) {
		this->Flush(0);
		dwOffset = 0;
	}

	char* result = &this->m_chVertex[dwOffset];
	switch (dwVertexTypeDesc) {
		case 0x00000000:
			for (int i = 0; i < n; dwOffset += 24, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 16);
				memcpy(&this->m_chVertex[dwOffset + 16], &vl[i]->lv, 8);
			}
			break;
		case 0x00000040:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 20);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i]->lv, 8);
			}
			break;
		case 0x00000042:
			for (int i = 0; i < n; dwOffset += 16, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i]->diffuse, 4);
			}
			break;
		case 0x00000044:
			for (int i = 0; i < n; dwOffset += 20, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 20);
			}
			break;
		case 0x00000102:
			for (int i = 0; i < n; dwOffset += 20, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i]->tu, 8);
			}
			break;
		case 0x00000104:
			for (int i = 0; i < n; dwOffset += 24, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 16);
				memcpy(&this->m_chVertex[dwOffset + 16], &vl[i]->tu, 8);
			}
			break;
		case 0x00000142:
			for (int i = 0; i < n; dwOffset += 24, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i]->diffuse, 12);
			}
			break;
		case 0x00000144:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 28);
			}
			break;
		case 0x00000202:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i]->tu, 16);
			}
			break;
		case 0x00000204:
			for (int i = 0; i < n; dwOffset += 32, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 16);
				memcpy(&this->m_chVertex[dwOffset + 16], &vl[i]->tu, 16);
			}
			break;
		case 0x00000242:
			for (int i = 0; i < n; dwOffset += 32, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i]->diffuse, 20);
			}
			break;
		case 0x00000244:
			for (int i = 0; i < n; dwOffset += 36, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 36);
			}
			break;
		case 0x80000080:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 16);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i]->tu, 8);
				this->m_chVertex[dwOffset + 19] = this->CalcFog(vl[i]->z);
			}
			break;
		case 0x800000C2:
			for (int i = 0; i < n; dwOffset += 20, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i]->diffuse, 4);
			}
			break;
		case 0x800000C4:
			for (int i = 0; i < n; dwOffset += 24, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 20);
				this->m_chVertex[dwOffset + 23] = this->CalcFog(vl[i]->z);
			}
			break;
		case 0x80000182:
			for (int i = 0; i < n; dwOffset += 24, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 12);
				memcpy(&this->m_chVertex[dwOffset + 16], &vl[i]->tu, 8);
			}
			break;
		case 0x80000184:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 16);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i]->tu, 8);
				this->m_chVertex[dwOffset + 19] = this->CalcFog(vl[i]->z);
			}
		case 0x800001C2:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i]->diffuse, 4);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i]->tu, 8);
			}
			break;
		case 0x800001C4:
			for (int i = 0; i < n; dwOffset += 32, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 20);
				memcpy(&this->m_chVertex[dwOffset + 24], &vl[i]->tu, 8);
				this->m_chVertex[dwOffset + 23] = this->CalcFog(vl[i]->z);
			}
			break;
		case 0x80000282:
			for (int i = 0; i < n; dwOffset += 32, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 12);
				memcpy(&this->m_chVertex[dwOffset + 16], &vl[i]->tu, 16);
			}
			break;
		case 0x80000284:
			for (int i = 0; i < n; dwOffset += 36, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 16);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i]->tu, 16);
				this->m_chVertex[dwOffset + 19] = this->CalcFog(vl[i]->z);
			}
			break;
		case 0x800002C2:
			for (int i = 0; i < n; dwOffset += 36, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i]->diffuse, 4);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i]->tu, 16);
				this->m_chVertex[dwOffset + 19] = this->CalcFog(vl[i]->z);
			}
			break;
		case 0x800002C4:
			for (int i = 0; i < n; dwOffset += 40, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i]->x, 20);
				memcpy(&this->m_chVertex[dwOffset + 24], &vl[i]->tu, 16);
				this->m_chVertex[dwOffset + 23] = this->CalcFog(vl[i]->z);
			}
			break;
		default:
			TRACE("Unexpected dwVertexTypeDesc=%08x", dwVertexTypeDesc);
			break;
	}

	if (store) {
		this->m_dwOffset = dwOffset;
	}

	return result;
}

void* WDirect3D::ModifyVertices(WtVertex* vl, int n, uint32_t dwVertexTypeDesc, bool store) {
	TRACE("vl=%p, n=%d, dwVertexTypeDesc=%08x, store=%d", vl, n, dwVertexTypeDesc, store);

	uint32_t dwOffset = this->m_dwOffset;
	if (dwOffset >= 0xCCCCC) {
		this->Flush(0);
		dwOffset = 0;
	}

	char* result = &this->m_chVertex[dwOffset];
	switch (dwVertexTypeDesc) {
		case 0x00000000:
			for (int i = 0; i < n; dwOffset += 24, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 16);
				memcpy(&this->m_chVertex[dwOffset + 16], &vl[i].lv, 8);
			}
			break;
		case 0x00000040:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 20);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i].lv, 8);
			}
			break;
		case 0x00000042:
			for (int i = 0; i < n; dwOffset += 16, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i].diffuse, 4);
			}
			break;
		case 0x00000044:
			for (int i = 0; i < n; dwOffset += 20, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 20);
			}
			break;
		case 0x00000102:
			for (int i = 0; i < n; dwOffset += 20, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i].tu, 8);
			}
			break;
		case 0x00000104:
			for (int i = 0; i < n; dwOffset += 24, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 16);
				memcpy(&this->m_chVertex[dwOffset + 16], &vl[i].tu, 8);
			}
			break;
		case 0x00000142:
			for (int i = 0; i < n; dwOffset += 24, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i].diffuse, 12);
			}
			break;
		case 0x00000144:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 28);
			}
			break;
		case 0x00000202:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i].tu, 16);
			}
			break;
		case 0x00000204:
			for (int i = 0; i < n; dwOffset += 32, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 16);
				memcpy(&this->m_chVertex[dwOffset + 16], &vl[i].tu, 16);
			}
			break;
		case 0x00000242:
			for (int i = 0; i < n; dwOffset += 32, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i].diffuse, 20);
			}
			break;
		case 0x00000244:
			for (int i = 0; i < n; dwOffset += 36, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 36);
			}
			break;
		case 0x80000080:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 16);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i].tu, 8);
				this->m_chVertex[dwOffset + 19] = this->CalcFog(vl[i].z);
			}
			break;
		case 0x800000C2:
			for (int i = 0; i < n; dwOffset += 20, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i].diffuse, 4);
			}
			break;
		case 0x800000C4:
			for (int i = 0; i < n; dwOffset += 24, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 20);
				this->m_chVertex[dwOffset + 23] = this->CalcFog(vl[i].z);
			}
			break;
		case 0x80000182:
			for (int i = 0; i < n; dwOffset += 24, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 12);
				memcpy(&this->m_chVertex[dwOffset + 16], &vl[i].tu, 8);
			}
			break;
		case 0x80000184:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 16);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i].tu, 8);
				this->m_chVertex[dwOffset + 19] = this->CalcFog(vl[i].z);
			}
		case 0x800001C2:
			for (int i = 0; i < n; dwOffset += 28, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i].diffuse, 4);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i].tu, 8);
			}
			break;
		case 0x800001C4:
			for (int i = 0; i < n; dwOffset += 32, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 20);
				memcpy(&this->m_chVertex[dwOffset + 24], &vl[i].tu, 8);
				this->m_chVertex[dwOffset + 23] = this->CalcFog(vl[i].z);
			}
			break;
		case 0x80000282:
			for (int i = 0; i < n; dwOffset += 32, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 12);
				memcpy(&this->m_chVertex[dwOffset + 16], &vl[i].tu, 16);
			}
			break;
		case 0x80000284:
			for (int i = 0; i < n; dwOffset += 36, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 16);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i].tu, 16);
				this->m_chVertex[dwOffset + 19] = this->CalcFog(vl[i].z);
			}
			break;
		case 0x800002C2:
			for (int i = 0; i < n; dwOffset += 36, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 12);
				memcpy(&this->m_chVertex[dwOffset + 12], &vl[i].diffuse, 4);
				memcpy(&this->m_chVertex[dwOffset + 20], &vl[i].tu, 16);
				this->m_chVertex[dwOffset + 19] = this->CalcFog(vl[i].z);
			}
			break;
		case 0x800002C4:
			for (int i = 0; i < n; dwOffset += 40, i++) {
				memcpy(&this->m_chVertex[dwOffset + 0], &vl[i].x, 20);
				memcpy(&this->m_chVertex[dwOffset + 24], &vl[i].tu, 16);
				this->m_chVertex[dwOffset + 23] = this->CalcFog(vl[i].z);
			}
			break;
		default:
			TRACE("Unexpected dwVertexTypeDesc=%08x", dwVertexTypeDesc);
	}

	if (store) {
		this->m_dwOffset = dwOffset;
	}

	return result;
}

void WDirect3D::ApplyGamma(float gamma) {
	for (int i = 0; i < 256; i++) {
		this->m_iRefTable[i] = std::clamp(
			static_cast<int>(pow(static_cast<double>(i) * 0.00390625, 1.0 / gamma) * 256.0), 0, 255);
	}
}

int WDirect3D::XCreateVertexBuffer_(int numVertices, uint32_t dwFvf, uint32_t dwUsage) {
	int hVb = this->XGetVbHandle();
	this->XCreateVertexBuffer(hVb, numVertices, dwFvf, dwUsage);
	return hVb;
}

int WDirect3D::XCreateIndexBuffer_(int numIndices, uint32_t dwUsage) {
	int hIb = this->XGetIbHandle();
	this->XCreateIndexBuffer(hIb, numIndices, dwUsage);
	return hIb;
}

void WDirect3D::FlushMultiPass(uint32_t flag) {
	int typeMask, increment, endIdx, ia, typeToAdd;
	bool bLastAlphaTest = this->SetRenderState4Flushing(0);

	for (int i = this->m_bufSortNum - 1; i >= 0; --i) {
		TRACE("[%d - 1 => 0] i = %d", this->m_bufSortNum, i);
		WPolyBuffer& polyBuffer = *this->m_buf4Sort[i];
		if (polyBuffer.bHw) {
			auto& hwPolyBuf = static_cast<WPolyBufferHw&>(polyBuffer);
			if (bLastAlphaTest) {
				hwPolyBuf.batchState.xiFlag0 |= 0x80000000;
			} else {
				hwPolyBuf.batchState.xiFlag0 &= ~0x80000000;
			}
			if (!(hwPolyBuf.batchState.xiFlag0 & 0x01800000)) {
				this->ApplyCustomRenderState(hwPolyBuf.customRenderState);
				this->XDrawIndexedPrimitive(hwPolyBuf.viewState, hwPolyBuf.batchState);
			}
		} else {
			auto& swPolyBuf = static_cast<WPolyBufferSw&>(polyBuffer);
			if (bLastAlphaTest) {
				swPolyBuf.type |= 0x80000000;
			} else {
				swPolyBuf.type &= ~0x80000000;
			}
			if (!(swPolyBuf.type & 0x01800000)) {
				this->ApplyCustomRenderState(swPolyBuf.customRenderState);
				this->DrawPrimitive(swPolyBuf.type, swPolyBuf.num, swPolyBuf.dwVertexTypeDesc, swPolyBuf.lpvVertices,
				                    swPolyBuf.dptPrimitiveType, swPolyBuf.type2);
			}
		}
	}

	uint32_t lastRenderState = 0;
	this->ApplyCustomRenderState(nullptr);
	this->FlushRenderPrimitive();
	this->SetRenderState4Flushing(1);

	if (flag & 2) {
		ia = this->m_bufSortNum - 1;
		endIdx = -1;
		typeMask = 0x20000000;
		typeToAdd = 0;
		increment = -1;
	} else {
		ia = 0;
		endIdx = this->m_bufSortNum;
		typeMask = 0;
		typeToAdd = 0x20000000;
		increment = 1;
	}

	for (; ia != endIdx; ia += increment) {
		WPolyBuffer& polyBuffer = *this->m_buf4Sort[ia];
		if (polyBuffer.bHw) {
			auto& hwPolyBuffer = static_cast<WPolyBufferHw&>(polyBuffer);
			if ((hwPolyBuffer.batchState.xiFlag0 & 0x1800000) != lastRenderState) {
				if (hwPolyBuffer.batchState.xiFlag0 & 0x1800000) {
					this->SetRenderState4Flushing(2);
				} else {
					this->SetRenderState4Flushing(1);
				}
				lastRenderState = hwPolyBuffer.batchState.xiFlag0 & 0x1800000;
			}
			hwPolyBuffer.batchState.xiFlag0 = typeToAdd | (hwPolyBuffer.batchState.xiFlag0 & ~typeMask);
			this->ApplyCustomRenderState(hwPolyBuffer.customRenderState);
			this->XDrawIndexedPrimitive(hwPolyBuffer.viewState, hwPolyBuffer.batchState);
		} else {
			auto& swPolyBuffer = static_cast<WPolyBufferSw&>(polyBuffer);
			if ((swPolyBuffer.type & 0x1800000) != lastRenderState) {
				if (swPolyBuffer.type & 0x1800000) {
					this->SetRenderState4Flushing(2);
				} else {
					this->SetRenderState4Flushing(1);
				}
				lastRenderState = swPolyBuffer.type & 0x1800000;
			}
			swPolyBuffer.type = typeToAdd | (swPolyBuffer.type & ~typeMask);
			this->ApplyCustomRenderState(swPolyBuffer.customRenderState);
			this->DrawPrimitive(swPolyBuffer.type, swPolyBuffer.num, swPolyBuffer.dwVertexTypeDesc,
			                    swPolyBuffer.lpvVertices, swPolyBuffer.dptPrimitiveType, swPolyBuffer.type2);
		}
	}

	this->ApplyCustomRenderState(nullptr);
	this->FlushRenderPrimitive();
	this->SetRenderState4Flushing(3);
}

void WDirect3D::FlushOnePass(uint32_t flag) {
	int i, dir, n;
	uint32_t typeMask, typeToAdd;

	if (flag & 2) {
		i = this->m_bufSortNum - 1;
		n = -1;
		typeMask = 0xA0000000;
		typeToAdd = 0;
		dir = -1;
	} else {
		n = this->m_bufSortNum;
		typeMask = 0x80000000;
		typeToAdd = 0x20000000;
		i = 0;
		dir = 1;
	}

	for (; i != n; i += dir) {
		WPolyBuffer& polyBuffer = *this->m_buf4Sort[i];
		if (polyBuffer.bHw) {
			auto& hwPolyBuffer = static_cast<WPolyBufferHw&>(polyBuffer);
			hwPolyBuffer.batchState.xiFlag0 = typeToAdd | (hwPolyBuffer.batchState.xiFlag0 & ~typeMask);
			this->ApplyCustomRenderState(hwPolyBuffer.customRenderState);
			this->XDrawIndexedPrimitive(hwPolyBuffer.viewState, hwPolyBuffer.batchState);
		} else {
			auto& swPolyBuffer = static_cast<WPolyBufferSw&>(polyBuffer);
			swPolyBuffer.type = typeToAdd | (swPolyBuffer.type & ~typeMask);
			this->ApplyCustomRenderState(swPolyBuffer.customRenderState);
			this->DrawPrimitive(swPolyBuffer.type, swPolyBuffer.num, swPolyBuffer.dwVertexTypeDesc,
			                    swPolyBuffer.lpvVertices, swPolyBuffer.dptPrimitiveType, swPolyBuffer.type2);
		}
	}

	this->ApplyCustomRenderState(nullptr);
	this->FlushRenderPrimitive();
}

void WDirect3D::FlushEqual() {
	for (int i = 0; i < this->m_bufPolySwNum; ++i) {
		WPolyBufferSw& swPolyBuf = this->m_bufPolySw[i];
		if ((swPolyBuf.type & 0x300000) == 0x100000) {
			this->ApplyCustomRenderState(swPolyBuf.customRenderState);
			this->DrawPrimitive(swPolyBuf.type, swPolyBuf.num, swPolyBuf.dwVertexTypeDesc, swPolyBuf.lpvVertices,
			                    swPolyBuf.dptPrimitiveType, swPolyBuf.type2);
		}
	}
	for (int i = 0; i < this->m_bufPolyHwNum; ++i) {
		WPolyBufferHw& hwPolyBuf = this->m_bufPolyHw[i];
		if ((hwPolyBuf.batchState.xiFlag0 & 0x300000) == 0x100000) {
			this->ApplyCustomRenderState(hwPolyBuf.customRenderState);
			this->XDrawIndexedPrimitive(hwPolyBuf.viewState, hwPolyBuf.batchState);
		}
	}
	this->ApplyCustomRenderState(nullptr);
}

void WDirect3D::FlushAlways() {
	TRACE("Called");
	for (int i = 0; i < this->m_bufPolySwNum; i++) {
		WPolyBufferSw& swPolyBuf = this->m_bufPolySw[i];
		if ((swPolyBuf.type & 0x300000) == 0x300000) {
			this->ApplyCustomRenderState(swPolyBuf.customRenderState);
			this->DrawPrimitive(swPolyBuf.type, swPolyBuf.num, swPolyBuf.dwVertexTypeDesc, swPolyBuf.lpvVertices,
			                    swPolyBuf.dptPrimitiveType, swPolyBuf.type2);
		}
	}

	for (int i = 0; i < this->m_bufPolyHwNum; i++) {
		WPolyBufferHw& hwPolyBuf = this->m_bufPolyHw[i];
		if ((hwPolyBuf.batchState.xiFlag0 & 0x300000) == 0x300000) {
			this->ApplyCustomRenderState(hwPolyBuf.customRenderState);
			this->XDrawIndexedPrimitive(hwPolyBuf.viewState, hwPolyBuf.batchState);
		}
	}

	this->ApplyCustomRenderState(nullptr);
}

void WDirect3D::Buffering(int type, int type2, int num, uint32_t dwVertexTypeDesc, void* p,
                          D3DPRIMITIVETYPE dptPrimitiveType, float depth) {
	WPolyBufferSw* swPolyBuf = &this->m_bufPolySw[this->m_bufPolySwNum++];
	type2 &= ~0x10;
	swPolyBuf->num = num;
	swPolyBuf->bHw = false;
	swPolyBuf->type = type;
	swPolyBuf->dwVertexTypeDesc = dwVertexTypeDesc;
	swPolyBuf->dptPrimitiveType = dptPrimitiveType;
	swPolyBuf->type2 = type2;
	swPolyBuf->lpvVertices = p;
	swPolyBuf->customRenderState = this->SnapShotCustomRenderState();

	if ((type & 0x300000) != 0x300000 && (type & 0x300000) != 0x100000) {
		swPolyBuf->depth = depth;
		this->m_buf4Sort[++this->m_bufSortNum] = swPolyBuf;
	}

	int bufCapacity = this->m_bufPolySw.capacity();
	if (this->m_bufPolySwNum == bufCapacity) {
		this->Flush(1);
	}
}

void WDirect3D::Buffering(const WxViewState& viewState, const WxBatchState& batchState) {
	WPolyBufferHw& hwPolyBuf = this->m_bufPolyHw[this->m_bufPolyHwNum++];
	hwPolyBuf.bHw = true;
	memcpy(&hwPolyBuf.viewState, &viewState, sizeof hwPolyBuf.viewState);
	memcpy(&hwPolyBuf.batchState, &batchState, sizeof hwPolyBuf.batchState);
	hwPolyBuf.customRenderState = this->SnapShotCustomRenderState();

	if ((batchState.xiFlag0 & 0x300000) != 0x300000 && (batchState.xiFlag0 & 0x100000) != 0x100000) {
		hwPolyBuf.depth = batchState.xfDepth;
		this->m_buf4Sort[this->m_bufSortNum++] = &hwPolyBuf;
	}

	if (this->m_bufPolyHwNum == static_cast<int>(this->m_bufPolyHw.capacity())) {
		this->Flush(1);
	}
}

void WDirect3D::XDrawIndexedTriangles(const WxViewState& viewState, const WxBatchState& batchState) {
	if (this->m_devState == WDeviceStateLost) {
		return;
	}
	if (batchState.xiFlag0 & 0x400000) {
		this->Buffering(viewState, batchState);
	} else {
		this->XDrawIndexedPrimitive(viewState, batchState);
	}
}

void WDirect3D::DrawPolygonFan(WtVertex** p, int iType, int iNum, int iType2, uint32_t dwVertexTypeDesc) {
	TRACE("p=%p, iType=%08x, iNum=%08x, iType2=%08x, dwVertexTypeDesc=%08x", p, iType, iNum, iType2, dwVertexTypeDesc);

	WtVertex** vertexList;
	void* lpvVertices;
	WtVertex* t[64];
	float pa;
	D3DPRIMITIVETYPE dptPrimitiveType;

	if (this->m_devState == WDeviceStateLost) {
		return;
	}

	if (!(iType & 0x4000000)) {
		dptPrimitiveType = D3DPT_TRIANGLEFAN;
		vertexList = p;
	} else if (iNum == 2) {
		dptPrimitiveType = D3DPT_LINESTRIP;
		vertexList = p;
	} else {
		if (iNum > 0) {
			memcpy(t, p, 4 * iNum);
			t[iNum] = *p;
		} else {
			t[0] = *p;
		}
		vertexList = t;
		++iNum;
		dptPrimitiveType = D3DPT_LINESTRIP;
	}

	if (iType & 0x3F800) {
		dwVertexTypeDesc |= 512;
	} else {
		dwVertexTypeDesc |= (iType & 0x7FF) != 0 ? 0x100 : 0;
	}
	dwVertexTypeDesc |= ~HIBYTE(iType) & 0x40;

	if (iType & 0x8000000) {
		if (this->m_fog) {
			dwVertexTypeDesc |= 0x80000080;
		} else {
			iType &= ~0x8000000;
		}
	}
	if (iType & 0x400000) {
		if ((iType & 0x300000) == 0x300000 || (iType & 0x300000) == 0x100000) {
			pa = 0.0;
		} else {
			pa = this->CalcDepth(vertexList, iNum);
		}
		lpvVertices = this->ModifyVertices(vertexList, iNum, dwVertexTypeDesc, true);
		this->Buffering(iType, iType2, iNum, dwVertexTypeDesc, lpvVertices, dptPrimitiveType, pa);
	} else if (iType & 0x3F800 && this->m_maxTextureBlendStages == 1) {
		if (dwVertexTypeDesc & 0x80000000) {
			if (iType & 0x3FFFF) {
				lpvVertices = this->ModifyVertices(vertexList, iNum, 0x80000184, false);
				this->DrawPrimitive(
					static_cast<int>(iType & 0xFFFC07FF),
					iNum,
					388u,
					lpvVertices,
					dptPrimitiveType,
					iType2);
			}
			lpvVertices = this->ModifyVertices(vertexList, iNum, 0x80000080, true);
			int i = 0;
			if (iNum > 0) {
				uint8_t* fieldPtr = static_cast<uint8_t*>(lpvVertices) + 19;
				while (*fieldPtr == 0xFF) {
					++i;
					fieldPtr += 28;
					if (i >= iNum) {
						return;
					}
				}
				this->Buffering(
					(iType >> 11 & 0x7F) | 0x69100000,
					0,
					iNum,
					0x184,
					lpvVertices,
					dptPrimitiveType,
					0.0);
			}
		} else if (iType & 0x7FF) {
			if (dwVertexTypeDesc & 0x40) {
				lpvVertices = this->ModifyVertices(vertexList, iNum, 0x144, false);
				this->DrawPrimitive(
					iType & 0xFFF807FF,
					iNum,
					324u,
					lpvVertices,
					dptPrimitiveType,
					iType2);
			} else {
				lpvVertices = this->ModifyVertices(vertexList, iNum, 0x104u, false);
				this->DrawPrimitive(
					iType & 0xFFF807FF,
					iNum,
					260u,
					lpvVertices,
					dptPrimitiveType,
					iType2);
			}
			if (iType & 0x40000) {
				if (dwVertexTypeDesc & 0x40) {
					lpvVertices = this->ModifyVertices(vertexList, iNum, 0, true);
					this->Buffering(
						(iType >> 11 & 0x7F) | 0x20900000,
						0,
						iNum,
						0x104u,
						lpvVertices,
						dptPrimitiveType,
						0.0);
				} else {
					lpvVertices = this->ModifyVertices(vertexList, iNum, 0x40u, true);
					this->Buffering(
						(iType >> 11 & 0x7F) | 0x60900000,
						0,
						iNum,
						0x144u,
						lpvVertices,
						dptPrimitiveType,
						0.0);
				}
			} else {
				lpvVertices = this->ModifyVertices(vertexList, iNum, 0, true);
				this->Buffering(
					(iType >> 11 & 0x7F) | 0x61100000,
					0,
					iNum,
					0x104u,
					lpvVertices,
					dptPrimitiveType,
					0.0);
			}
		} else {
			if (dwVertexTypeDesc & 0x40) {
				lpvVertices = this->ModifyVertices(vertexList, iNum, 0, true);
				this->DrawPrimitive(
					iType >> 11 & 0x7F,
					iNum,
					260u,
					lpvVertices,
					dptPrimitiveType,
					iType2);
			} else {
				lpvVertices = this->ModifyVertices(vertexList, iNum, 0x40u, true);
				this->DrawPrimitive(
					(iType >> 11 & 0x7F) | 0x40000000,
					iNum,
					0x144u,
					lpvVertices,
					dptPrimitiveType,
					iType2);
			}
		}
	} else {
		lpvVertices = this->ModifyVertices(vertexList, iNum, dwVertexTypeDesc, false);
		this->DrawPrimitive(iType, iNum, dwVertexTypeDesc, lpvVertices, dptPrimitiveType, iType2);
	}
}

void WDirect3D::DrawIndexedTriangles(WtVertex* p, int pNum, uint16_t* fList, int fNum, uint32_t iType,
                                     uint32_t iType2) {
	int v9;
	void* vtxPtr;
	WtVertex* vl;
	WtVertex* t;
	WtVertex* v24;

	if (this->m_devState != WDeviceStateLost) {
		if (iType & 0x3F800)
			v9 = 0x200;
		else
			v9 = (iType & 0x7FF) != 0 ? 0x100 : 0;
		unsigned int v10 = v9 | (~HIBYTE(iType) & 0x40) | 2;
		unsigned int dwVertexTypeDesc = v9 | (~HIBYTE(iType) & 0x40) | 2;
		if (iType & 0x8000000) {
			if (this->m_fog) {
				v10 = v9 | (~HIBYTE(iType) & 0x40) | 0x80000082;
				dwVertexTypeDesc = v9 | (~HIBYTE(iType) & 0x40) | 0x80000082;
			} else {
				iType &= 0xF7FFFFFF;
				iType &= 0xF7FFFFFF;
			}
		}
		if (iType & 0x400000) {
			int v11 = 0;
			if (fNum > 0) {
				dwVertexTypeDesc = iType & 0x300000;
				do {
					t = &p[fList[v11]];
					int v12 = fList[v11 + 1];
					int v13 = v11 + 2;
					v24 = &p[v12];
					v11 = v13 + 1;
					vl = &p[fList[v13]];
					float depth;
					if (dwVertexTypeDesc == 0x300000 || dwVertexTypeDesc == 0x100000)
						depth = 0.0;
					else
						depth = (t->vz + v24->vz + p[fList[v13]].vz) * 0.33333334f;
					vtxPtr = this->ModifyVertices(&t, 3, v10, true);
					this->Buffering(iType, iType2, 3, v10, vtxPtr, D3DPT_TRIANGLEFAN, depth);
				} while (v11 < fNum);
			}
		} else if (iType & 0x4000000) {
			int v18 = 0;
			while (v18 < fNum) {
				vl = &p[fList[v18]];
				int v19 = fList[v18 + 1];
				int v20 = v18 + 1;
				t = &p[v19];
				v24 = &p[fList[v20 + 1]];
				v18 = v20 + 2;
				vtxPtr = this->ModifyVertices(&vl, 4, v10, false);
				v10 = dwVertexTypeDesc;
				this->DrawPrimitive(iType, 4, dwVertexTypeDesc, vtxPtr, D3DPT_LINESTRIP, iType2);
			}
		} else {
			vtxPtr = this->ModifyVertices(p, pNum, v10, false);
			this->DrawPrimitiveIndexed(iType, v10, vtxPtr, pNum, fList, fNum, iType2);
		}
	}
}

WDeviceState WDirect3D::GetDeviceState() {
	return this->m_devState;
}

int WDirect3D::GetBackBufferBpp() const {
	return this->m_backBufBpp;
}

int WDirect3D::GetBufferingMeshNum() const {
	return this->m_bufPolySwNum;
}

void WDirect3D::FlushRenderPrimitive() {}

uint32_t WDirect3D::VertexSize(uint32_t dwVertexTypeDesc) {
	switch (dwVertexTypeDesc & 0x7FFFFFFF) {
		case 0x0000:
			return 24;
		case 0x0042:
			return 16;
		case 0x0044:
			return 20;
		case 0x0052:
		case 0x0080:
			return 28;
		case 0x00C2:
			return 20;
		case 0x00C4:
			return 24;
		case 0x0102:
			return 20;
		case 0x0104:
			return 24;
		case 0x0112:
			return 32;
		case 0x0142:
			return 24;
		case 0x0144:
			return 28;
		case 0x0152:
			return 36;
		case 0x0182:
			return 24;
		case 0x0184:
		case 0x01C2:
			return 28;
		case 0x01C4:
			return 32;
		case 0x0202:
			return 28;
		case 0x0204:
			return 32;
		case 0x0212:
			return 40;
		case 0x0242:
			return 32;
		case 0x0244:
			return 36;
		case 0x0252:
			return 44;
		case 0x0282:
			return 32;
		case 0x0284:
		case 0x02C2:
			return 36;
		case 0x02C4:
			return 40;
		case 0x1106:
			return 24;
		case 0x1108:
			return 28;
		case 0x110A:
			return 32;
		case 0x110C:
		case 0x1116:
			return 36;
		case 0x1118:
			return 40;
		case 0x111A:
			return 44;
		case 0x111C:
			return 48;
		default:
			return 0;
	}
}

WDirect3D::~WDirect3D() {
	this->WDirect3D::Release();
}

void WDirect3D::Init() {
	this->m_chVertex = new char[0x100000];
	this->m_devState = WDeviceStateNormal;
	this->m_buf4Sort.resize(0x1200);
	this->m_bufPolySw.resize(0x1000);
	this->m_bufPolyHw.resize(0x200);
}

void WDirect3D::Flush(uint32_t flag) {
	this->FlushRenderPrimitive();
	this->m_dwOffset = 0;
	if (this->m_devState == 0 && this->m_bufSortNum > 0) {
		this->BeginUsingCustomRenderState();
		this->FlushEqual();
		this->FlushRenderPrimitive();
		qsort(&this->m_buf4Sort[0], this->m_bufSortNum, 4, ComparePolyDepth);
		if (flag & 1) {
			this->FlushMultiPass(flag);
		} else {
			this->FlushOnePass(flag);
		}
		this->FlushRenderPrimitive();
		this->FlushAlways();
		this->FlushRenderPrimitive();
		this->EndUsingCustomRenderState();
	}
	this->ClearCustomRenderStateSnapShotList();

	int swCapacity = this->m_bufPolySw.capacity();
	if (this->m_bufPolySwNum == swCapacity) {
		this->m_bufPolySw.clear();
		this->m_bufPolySw.resize(static_cast<int>(this->m_bufPolySwNum * 1.5));
	}

	int hwCapacity = this->m_bufPolyHw.capacity();
	if (this->m_bufPolyHwNum == hwCapacity) {
		this->m_bufPolyHw.clear();
		this->m_bufPolyHw.resize(static_cast<int>(this->m_bufPolyHwNum * 1.5));
	}

	int ptrCapacity = this->m_buf4Sort.capacity();
	if (swCapacity + hwCapacity != ptrCapacity) {
		this->m_buf4Sort.clear();
		swCapacity = this->m_bufPolySw.capacity();
		hwCapacity = this->m_bufPolyHw.capacity();
		this->m_buf4Sort.resize(swCapacity + hwCapacity);
	}
	this->m_bufSortNum = 0;
	this->m_bufPolyHwNum = 0;
	this->m_bufPolySwNum = 0;
}

size_t WDirect3D::GetSortBufferSize() const {
	return this->m_buf4Sort.size();
}

size_t WDirect3D::GetSortBufferSwSize() const {
	return this->m_bufPolySw.size();
}

size_t WDirect3D::GetSortBufferHwSize() const {
	return this->m_bufPolyHw.size();
}
