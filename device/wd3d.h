#pragma once
#include <wvideo.h>
#include <wscene.h>
#include <windows.h>
#include <d3d9.h>

#include <vector>

class WDirect3D : public WVideoDev {
public:
	WDirect3D();
	WDirect3D(const WDirect3D&) = delete;
	WDirect3D(WDirect3D&&) = delete;
	~WDirect3D();

	WDirect3D& operator=(const WDirect3D&) = delete;
	WDirect3D& operator=(WDirect3D&&) = delete;

	bool SetFogEnable(bool enable) override;
	void SetFogState(float fogStart, float fogEnd, uint32_t color) override;
	int UploadCompressedTexture(void* pSrc, size_t srcSize, int type) override;
	int CreateTexture(LPBITMAPINFO src, int type) override;
	void UpdateTexture(int texHandle, LPBITMAPINFO src, LPVOID data, uint32_t type) override;
	void XReleaseVertexBuffer(int hVb) override;
	void XReleaseIndexBuffer(int hIb) override;
	int XCreateVertexBuffer_(int numVertices, uint32_t dwFvf, uint32_t dwUsage) override;
	int XCreateIndexBuffer_(int numIndices, uint32_t dwUsage) override;
	void XDrawIndexedTriangles(const WxViewState& viewState, const WxBatchState& batchState) override;
	void DrawPolygonFan(WtVertex** p, int iType, int iNum, int iType2, uint32_t dwVertexTypeDesc) override;
	void DrawIndexedTriangles(WtVertex* p, int pNum, uint16_t* fList, int fNum, uint32_t iType,
	                          uint32_t iType2) override;
	WDeviceState GetDeviceState() override;
	int GetBackBufferBpp() const override;
	int GetBufferingMeshNum() const override;
	void DestroyTexture(int) override;

	virtual void DrawPrimitive(uint32_t iType, int iNum, uint32_t dwVertexTypeDesc, void* lpvVertices,
	                           D3DPRIMITIVETYPE dptPrimitiveType, int iType2) = 0;
	virtual void DrawPrimitiveIndexed(int iType, uint32_t dwVertexTypeDesc, void* lpvVertices, int pnum,
	                                  uint16_t* flist, int fnum, int iType2) = 0;
	virtual bool SetRenderState4Flushing(int pass) = 0;
	virtual void Flush(uint32_t flag);
	virtual void FlushEqual();
	virtual void FlushMultiPass(uint32_t flag);
	virtual void FlushOnePass(uint32_t flag);
	virtual void FlushAlways();
	virtual int UploadCompressedTextureSurface(void* pSrcData, size_t srcDataSize, int type) = 0;
	virtual void CreateTextureSurface(uint32_t dwWidth, uint32_t dwHeight, int iMipmapCount, int renderTargetType,
	                                  int iIndex, int bChromaKey, int bitNum) = 0;
	virtual void UpdateTextureSurface(int texHandle, BITMAPINFO* bi, void* data, int type) = 0;
	virtual void FlushRenderPrimitive();
	virtual void* SnapShotCustomRenderState() = 0;
	virtual void ClearCustomRenderStateSnapShotList() = 0;
	virtual void ApplyCustomRenderState(const void* customRenderState) = 0;
	virtual void XCreateVertexBuffer(int hVb, int numVertices, uint32_t dwFVF, uint32_t dwUsage) = 0;
	virtual void XCreateIndexBuffer(int hIb, int numIndices, uint32_t dwUsage) = 0;
	virtual void XDrawIndexedPrimitive(const WxViewState& viewState, const WxBatchState& batchState) = 0;

protected:

	virtual void Release();
	int GetTextureNum(int stage);
	void* ModifyVertices(WtVertex** vl, int n, uint32_t dwVertexTypeDesc, bool store);
	void* ModifyVertices(WtVertex* vl, int n, uint32_t dwVertexTypeDesc, bool store);
	void ApplyGamma(float gamma);
	void Buffering(int type, int type2, int num, uint32_t dwVertexTypeDesc, void* p, D3DPRIMITIVETYPE dptPrimitiveType,
	               float depth);
	void Buffering(const WxViewState& viewState, const WxBatchState& batchState);
	uint32_t VertexSize(uint32_t dwVertexTypeDesc) override;
	void Init();
	size_t GetSortBufferSize() const;
	size_t GetSortBufferSwSize() const;
	size_t GetSortBufferHwSize() const;

	enum TexState {
		TexStateVoid = 0x0,
		TexStateCreated = 0x1,
		TexStateUpdated = 0x2,
	};

	struct WPolyBuffer {
		bool bHw;
		float depth;
	};

	struct WPolyBufferSw : public WPolyBuffer {
		uint32_t type;
		uint32_t type2;
		int num;
		uint32_t dwVertexTypeDesc;
		void* lpvVertices;
		D3DPRIMITIVETYPE dptPrimitiveType;
		void* customRenderState;
	};

	struct WPolyBufferHw : public WPolyBuffer {
		WxViewState viewState{};
		WxBatchState batchState{};
		void* customRenderState = nullptr;

		WPolyBufferHw& operator=(const WPolyBufferHw& rhs) {
			if (this == &rhs) {
				return *this;
			}
			this->bHw = rhs.bHw;
			this->depth = rhs.depth;
			memcpy(&this->viewState.xLight, &rhs.viewState.xLight, sizeof this->viewState.xLight);
			memcpy(&this->viewState.xmView, &rhs.viewState.xmView, sizeof this->viewState.xmView);
			memcpy(&this->viewState.xmProj, &rhs.viewState.xmProj, sizeof this->viewState.xmProj);
			this->batchState = rhs.batchState;
			this->customRenderState = rhs.customRenderState;
			return *this;
		}
	};

	int m_iRefTable[256]{};
	int m_maxTextureBlendStages = 0;
	int m_backBufBpp = 0;
	char* m_chVertex = nullptr;
	uint32_t m_dwOffset = 0;
	std::vector<WPolyBuffer*> m_buf4Sort;
	std::vector<WPolyBufferSw> m_bufPolySw;
	std::vector<WPolyBufferHw> m_bufPolyHw;
	int m_texCount1 = 128;
	int m_texCount2 = 1;
	TexState m_texList[2048]{};
	int m_bufPolySwNum = 0;
	int m_bufPolyHwNum = 0;
	int m_bufSortNum = 0;
	WDeviceState m_devState = WDeviceStateNormal;
	bool m_fog = false;
	float m_fogStart = 0.0f;
	float m_fogEnd = 0.0f;
	uint32_t m_fogColor = 0;
	bool m_useHiQualityTex = false;
	int m_xiVbCount = 1;
	int m_xiIbCount = 1;
	int m_xahVbs[1024]{1};
	int m_xahIbs[256]{1};

private:
	void ClearVertices();
	uint8_t CalcFog(float depth);
	float CalcDepth(WtVertex** p, int num);
	void DrawBuffered(WPolyBufferSw const& bufSw);
	void DrawBuffered(WPolyBufferHw const& bufHw);
	static int ComparePolyDepth(void const* elem1, void const* elem2);
	int XGetVbHandle();
	int XGetIbHandle();
};
