// ReSharper disable CppVirtualFunctionInFinalClass
// ReSharper disable CppInconsistentNaming
#pragma once
#include "wd3d8.h"

class WDirect3D8KR645 final {
public:
	explicit WDirect3D8KR645(WDirect3D8* d) : m_dev(d) {}

	virtual ~WDirect3D8KR645() {
		delete m_dev;
	}

	// WDevice
	virtual const char* GetDeviceName() {
		return m_dev->GetDeviceName();
	}

	virtual const char* EnumModeName() {
		return m_dev->EnumModeName();
	}

	virtual WProc* ExternProc() {
		return m_dev->ExternProc();
	}

	// WVideoDev
	virtual WDeviceState GetDeviceState() {
		return m_dev->GetDeviceState();
	}

	virtual void SetMainThreadId(unsigned int threadId) {
		m_dev->SetMainThreadId(threadId);
	}

	virtual void DrawPolygonFan(WtVertex** p, int iType, int iNum, int iType2, DWORD dwVertexTypeDesc) {
		m_dev->DrawPolygonFan(p, iType, iNum, iType2, dwVertexTypeDesc);
	}

	virtual void DrawIndexedTriangles(WtVertex* p, int pnum, LPWORD flist, int fnum, int iType, int iType2) {
		m_dev->DrawIndexedTriangles(p, pnum, flist, fnum, iType, iType2);
	}

	virtual void DrawLine(WtVertex** p, int type) {
		m_dev->DrawLine(p, type);
	}

	virtual int Command(WDeviceMessage message, int param1, int param2) {
		return m_dev->Command(message, param1, param2);
	}

	virtual int CreateTexture(LPBITMAPINFO src, int type) {
		return m_dev->CreateTexture(src, type);
	}

	virtual void UpdateTexture(int texHandle, LPBITMAPINFO src, LPVOID data, DWORD type) {
		m_dev->UpdateTexture(texHandle, src, data, type);
	}

	virtual void DestroyTexture(int texHandle) {
		m_dev->DestroyTexture(texHandle);
	}

	virtual int UploadCompressedTexture(void* pSrc, size_t srcSize, int type) {
		return m_dev->UploadCompressedTexture(pSrc, srcSize, type);
	}

	virtual void UpdateCompressedTexture(int texHandle, void* pSrcData, size_t srcDataSize, int type) {
		m_dev->UpdateCompressedTexture(texHandle, pSrcData, srcDataSize, type);
	}

	virtual void FixTexturePart(int texHandle, const RECT& rc, BITMAPINFO* src, void* data, int type) {
		m_dev->FixTexturePart(texHandle, rc, src, data, type);
	}

	virtual bool IsTextureFilled(int texHandle) {
		return m_dev->IsTextureFilled(texHandle);
	}

	virtual int GetTextureWidth(int hTex) {
		return m_dev->GetTextureWidth(hTex);
	}

	virtual int GetTextureHeight(int hTex) {
		return m_dev->GetTextureHeight(hTex);
	}

	virtual void SetRenderTargetSizeInfo(int hTex, const WRenderToTextureSizeInfo& sizeInfo) {
		m_dev->SetRenderTargetSizeInfo(hTex, sizeInfo);
	}

	virtual bool BeginScene() {
		return m_dev->BeginScene();
	}

	virtual void EndScene() {
		m_dev->EndScene();
	}

	virtual void Paint() {
		m_dev->Paint();
	}

	virtual void SetGlobalRenderState(int rs, int rsex) {
		m_dev->SetGlobalRenderState(rs, rsex);
	}

	virtual bool IsSupportVS() {
		return m_dev->IsSupportVs();
	}

	virtual bool IsSupportPS() {
		return m_dev->IsSupportPs();
	}

	virtual bool IsSupportMRT() {
		return m_dev->IsSupportMrt();
	}

	virtual bool IsSupportClipPlane() {
		return m_dev->IsSupportClipPlane();
	}

	virtual void Clear(DWORD color, int flags, float z) {
		m_dev->Clear(color, flags, z);
	}

	virtual int GetWidth() const {
		return m_dev->GetWidth();
	}

	virtual int GetHeight() const {
		return m_dev->GetHeight();
	}

	virtual int GetBackBufferBpp() const {
		return m_dev->WDirect3D::GetBackBufferBpp();
	}

	virtual bool IsWindowed() {
		return m_dev->IsWindowed();
	}

	virtual bool IsFillScreenMode() {
		return m_dev->IsFillScreenMode();
	}

	virtual float GetMonitorSupportFPS() const {
		return m_dev->GetMonitorSupportFps();
	}

	virtual bool SetFogEnable(bool enable) {
		return m_dev->SetFogEnable(enable);
	}

	virtual void SetFogState(float fogStart, float fogEnd, DWORD fogColor) {
		m_dev->SetFogState(fogStart, fogEnd, fogColor);
	}

	virtual WDirect3D8KR645* MakeClone(char* modeName, HWND hwnd, int iTnL) {
		return new WDirect3D8KR645(m_dev->MakeClone(modeName, hwnd, iTnL));
	}

	virtual bool Reset(bool bWindowed, int iWidth, int iHeight, int iColor, int lWndStyle, int fillMode) {
		return m_dev->Reset(bWindowed, iWidth, iHeight, iColor, lWndStyle, fillMode);
	}

	virtual DWORD VertexSize(DWORD dwVertexTypeDesc) {
		return m_dev->VertexSize(dwVertexTypeDesc);
	}

	virtual BYTE xGetStride(int hVb) {
		return m_dev->XGetStride(hVb);
	}

	virtual bool xHasVertexElem(DWORD dwFVF, WVertexElement elem) const {
		return m_dev->XHasVertexElem(dwFVF, elem);
	}

	virtual int xGetVertexElemOffset(DWORD dwFVF, WVertexElement elem) const {
		return m_dev->XGetVertexElemOffset(dwFVF, elem);
	}

	virtual int xGetBlendWeightSize(DWORD dwFVF) const {
		return m_dev->XGetBlendWeightSize(dwFVF);
	}

	virtual DWORD xDetermineFVF(int iDrawFlag, int iDrawFlag2, int iMaxBoneNum) {
		return m_dev->XDetermineFvf(iDrawFlag, iDrawFlag2, iMaxBoneNum);
	}

	virtual DWORD xDetermineBufferUsage(DWORD dwFVF) {
		return m_dev->XDetermineBufferUsage(dwFVF);
	}

	virtual int xCreateVertexBuffer_(int nVtxs, DWORD dwFVF, DWORD dwUsage) {
		return m_dev->XCreateVertexBuffer_(nVtxs, dwFVF, dwUsage);
	}

	virtual int xCreateIndexBuffer_(int nIdxs, DWORD dwUsage) {
		return m_dev->XCreateIndexBuffer_(nIdxs, dwUsage);
	}

	virtual void xReleaseVertexBuffer(int hVb) {
		m_dev->XReleaseVertexBuffer(hVb);
	}

	virtual void xReleaseIndexBuffer(int hIb) {
		m_dev->XReleaseIndexBuffer(hIb);
	}

	virtual char* xLockVertexBuffer(int hVb, unsigned int uiOffset, unsigned int uiSize) {
		return m_dev->XLockVertexBuffer(hVb, uiOffset, uiSize);
	}

	virtual char* xLockIndexBuffer(int hIb, unsigned int uiOffset, unsigned int uiSize) {
		return m_dev->XLockIndexBuffer(hIb, uiOffset, uiSize);
	}

	virtual void xUnlockVertexBuffer(int hVb) {
		m_dev->XUnlockVertexBuffer(hVb);
	}

	virtual void xUnlockIndexBuffer(int hIb) {
		m_dev->XUnlockIndexBuffer(hIb);
	}

	virtual void xDrawIndexedTriangles(const WxViewState& VState, const WxBatchState& BState) {
		m_dev->XDrawIndexedTriangles(VState, BState);
	}

	virtual void xSetTransform(WTransformStateType State, const WMatrix4& Matrix) {
		return m_dev->XSetTransform(State, Matrix);
	}

	virtual void xSetPrevViewTransform(const WMatrix4& Matrix) {
		m_dev->XSetPrevViewTransform(Matrix);
	}

	virtual void SetShaderSource(const char* shaderSrc) {
		m_dev->SetShaderSource(shaderSrc);
	}

	virtual void BeginUsingCustomRenderState() {
		m_dev->BeginUsingCustomRenderState();
	}

	virtual void SetCustomRenderState(WRenderStateType State, unsigned int Value) {
		m_dev->SetCustomRenderState(State, Value);
	}

	virtual void SetCustomTextureStageState(DWORD Stage, WTextureStageStateType Type, DWORD Value) {
		m_dev->SetCustomTextureStageState(Stage, Type, Value);
	}

	virtual void SetCustomTransform(WTransformStateType State, const WMatrix4& Matrix) {
		m_dev->SetCustomTransform(State, Matrix);
	}

	virtual void SetCustomTexture(unsigned int Stage, int hTex) {
		m_dev->SetCustomTexture(Stage, hTex);
	}

	virtual void SetCustomClipPlane(DWORD Index, const WPlane& Value) {
		m_dev->SetCustomClipPlane(Index, Value);
	}

	virtual void SetCustomSamplerState(DWORD Sampler, WSamplerStateType Type, DWORD Value) {
		m_dev->SetCustomSamplerState(Sampler, Type, Value);
	}

	virtual void SetCustomFxMacro(DWORD FxMacro) {
		m_dev->SetCustomFxMacro(FxMacro);
	}

	virtual void SetCustomFxParamInt(WFxParameterType ParamType, int Value) {
		m_dev->SetCustomFxParamInt(ParamType, Value);
	}

	virtual void SetCustomFxParamVector2(WFxParameterType ParamType, const WVector2D& Value) {
		m_dev->SetCustomFxParamVector2(ParamType, Value);
	}

	virtual void SetCustomFxParamVector3(WFxParameterType ParamType, const WVector& Value) {
		m_dev->SetCustomFxParamVector3(ParamType, Value);
	}

	virtual void SetCustomFxParamVector4(WFxParameterType ParamType, const WVector4& Value) {
		m_dev->SetCustomFxParamVector4(ParamType, Value);
	}

	virtual void SetCustomFxParamMatrix(WFxParameterType ParamType, const WMatrix4& Value) {
		m_dev->SetCustomFxParamMatrix(ParamType, Value);
	}

	virtual void SetCustomFxParamTexture(WFxParameterType ParamType, int hTex) {
		m_dev->SetCustomFxParamTexture(ParamType, hTex);
	}

	virtual void EndUsingCustomRenderState() {
		m_dev->EndUsingCustomRenderState();
	}

	virtual bool BeginRenderToTexture(const WRenderToTextureParam& param) {
		return m_dev->BeginRenderToTexture(param);
	}

	virtual void EndRenderToTexture(const WRenderToTextureParam& param) {
		m_dev->EndRenderToTexture(param);
	}

	virtual bool SupportRenderTargetFormat() {
		return m_dev->SupportRenderTargetFormat();
	}

	virtual bool IsSupportedDisplayMode(bool bWindowed, int iWidth, int iHeight, int iColor) {
		return m_dev->IsSupportedDisplayMode(bWindowed, iWidth, iHeight, iColor);
	}

	virtual bool GetWindowDisplayMode(int& iWidth, int& iHeight, int& iColor) {
		return m_dev->GetWindowDisplayMode(iWidth, iHeight, iColor);
	}

	virtual int GetBufferingMeshNum() const {
		return m_dev->GetBufferingMeshNum();
	}

	virtual void SetViewPort(DWORD x, DWORD y, DWORD w, DWORD h) {
		m_dev->SetViewPort(x, y, w, h);
	}

	// WDirect3D
	virtual void DrawPrimitive(int iType, int iNum, DWORD dwVertexTypeDesc, void* lpvVertices,
	                           D3DPRIMITIVETYPE dptPrimitiveType, int iType2) {
		m_dev->DrawPrimitive(iType, iNum, dwVertexTypeDesc, lpvVertices, dptPrimitiveType, iType2);
	}

	virtual void DrawPrimitiveIndexed(int iType, DWORD dwVertexTypeDesc, void* lpvVertices, int pnum, LPWORD flist,
	                                  int fnum, int iType2) {
		m_dev->DrawPrimitiveIndexed(iType, dwVertexTypeDesc, lpvVertices, pnum, flist, fnum, iType2);
	}

	virtual bool SetRenderState4Flushing(int pass) {
		return m_dev->SetRenderState4Flushing(pass);
	}

	virtual void Flush(DWORD flag) {
		m_dev->Flush(flag);
	}

	virtual void FlushEqual() {
		m_dev->FlushEqual();
	}

	virtual void FlushMultiPass(DWORD flag) {
		return m_dev->FlushMultiPass(flag);
	}

	virtual void FlushOnePass(DWORD flag) {
		return m_dev->FlushOnePass(flag);
	}

	virtual void FlushAlways() {
		return m_dev->FlushAlways();
	}

	virtual int UploadCompressedTextureSurface(void* pSrcData, size_t srcDataSize, int type) {
		return m_dev->UploadCompressedTextureSurface(pSrcData, srcDataSize, type);
	}

	virtual void CreateTextureSurface(DWORD dwWidth, DWORD dwHeight, int iMipmapcount, int renderTargetType, int iIndex,
	                                  int bChromakey, int bitnum) {
		m_dev->CreateTextureSurface(dwWidth, dwHeight, iMipmapcount, renderTargetType, iIndex, bChromakey, bitnum);
	}

	virtual void UpdateTextureSurface(int texHandle, tagBITMAPINFO* bi, void* data, int type) {
		m_dev->UpdateTextureSurface(texHandle, bi, data, type);
	}

	virtual void FlushRenderPrimitive() {
		return m_dev->FlushRenderPrimitive();
	}

	virtual void* SnapShotCustomRenderState() {
		throw m_dev->SnapShotCustomRenderState();
	}

	virtual void ClearCustomRenderStateSnapShotList() {
		m_dev->ClearCustomRenderStateSnapShotList();
	}

	virtual void ApplyCustomRenderState(const void* customRenderState) {
		m_dev->ApplyCustomRenderState(customRenderState);
	}

	virtual void xCreateVertexBuffer(int hVb, int nVtxs, DWORD dwFVF, DWORD dwUsage) {
		return m_dev->XCreateVertexBuffer(hVb, nVtxs, dwFVF, dwUsage);
	}

	virtual void xCreateIndexBuffer(int hIb, int nIdxs, DWORD dwUsage) {
		return m_dev->XCreateIndexBuffer(hIb, nIdxs, dwUsage);
	}

	virtual void xDrawIndexedPrimitive(const WxViewState& VState, const WxBatchState& BState) {
		return m_dev->XDrawIndexedPrimitive(VState, BState);
	}

	// WDirect3D8
	virtual WDirect3D8KR645* CreateClone(char* devName, int id) {
		return new WDirect3D8KR645(m_dev->CreateClone(devName, id));
	}

private:
	WDirect3D8* m_dev;
};


class WDirect3D8US852 {
public:
	WDirect3D8US852(WDirect3D8* d) : m_dev(d) {}

	virtual ~WDirect3D8US852() {
		delete m_dev;
	}

	// WDevice
	virtual const char* GetDeviceName() {
		return m_dev->GetDeviceName();
	}

	virtual const char* EnumModeName() {
		return m_dev->EnumModeName();
	}

	virtual WProc* ExternProc() {
		return m_dev->ExternProc();
	}

	// WVideoDev
	virtual WDeviceState GetDeviceState() {
		return m_dev->GetDeviceState();
	}

	virtual void SetMainThreadId(unsigned int threadId) {
		m_dev->SetMainThreadId(threadId);
	}

	virtual void DrawPolygonFan(WtVertex** p, int iType, int iNum, int iType2, DWORD dwVertexTypeDesc, int unk1,
	                            int unk2) {
		m_dev->DrawPolygonFan(p, iType, iNum, iType2, dwVertexTypeDesc);
	}

	virtual void DrawIndexedTriangles(WtVertex* p, int pnum, LPWORD flist, int fnum, int iType, int iType2, int unk1,
	                                  int unk2) {
		m_dev->DrawIndexedTriangles(p, pnum, flist, fnum, iType, iType2);
	}

	virtual void DrawLine(WtVertex** p, int type) {
		m_dev->DrawLine(p, type);
	}

	virtual int Command(WDeviceMessage message, int param1, int param2) {
		return m_dev->Command(message, param1, param2);
	}

	virtual int CreateTexture(LPBITMAPINFO src, int type) {
		return m_dev->CreateTexture(src, type);
	}

	virtual void UpdateTexture(int texHandle, LPBITMAPINFO src, LPVOID data, DWORD type) {
		m_dev->UpdateTexture(texHandle, src, data, type);
	}

	virtual void DestroyTexture(int texHandle) {
		m_dev->DestroyTexture(texHandle);
	}

	virtual int UploadCompressedTexture(void* pSrc, size_t srcSize, int type) {
		return m_dev->UploadCompressedTexture(pSrc, srcSize, type);
	}

	virtual void UpdateCompressedTexture(int texHandle, void* pSrcData, size_t srcDataSize, int type) {
		m_dev->UpdateCompressedTexture(texHandle, pSrcData, srcDataSize, type);
	}

	virtual void FixTexturePart(int texHandle, const RECT& rc, BITMAPINFO* src, void* data, int type) {
		m_dev->FixTexturePart(texHandle, rc, src, data, type);
	}

	virtual bool IsTextureFilled(int texHandle) {
		return m_dev->IsTextureFilled(texHandle);
	}

	virtual int GetTextureWidth(int hTex) {
		return m_dev->GetTextureWidth(hTex);
	}

	virtual int GetTextureHeight(int hTex) {
		return m_dev->GetTextureHeight(hTex);
	}

	virtual void SetRenderTargetSizeInfo(int hTex, const WRenderToTextureSizeInfo& sizeInfo) {
		m_dev->SetRenderTargetSizeInfo(hTex, sizeInfo);
	}

	virtual void sub_100185D0(float a) { }

	virtual bool BeginScene() {
		return m_dev->BeginScene();
	}

	virtual void EndScene() {
		m_dev->EndScene();
	}

	virtual void Paint() {
		m_dev->Paint();
	}

	virtual void SetGlobalRenderState(int rs, int rsex) {
		m_dev->SetGlobalRenderState(rs, rsex);
	}

	virtual void sub_100445B0(int unk) { }
	virtual void sub_10044640(int unk) { }
	virtual void sub_10044730() { }
	virtual void sub_10044750() { }

	virtual bool IsSupportVS() {
		return m_dev->IsSupportVs();
	}

	virtual bool IsSupportPS() {
		return m_dev->IsSupportPs();
	}

	virtual bool IsSupportMRT() {
		return m_dev->IsSupportMrt();
	}

	virtual bool IsSupportClipPlane() {
		return m_dev->IsSupportClipPlane();
	}

	virtual void Clear(DWORD color, int flags, float z) {
		m_dev->Clear(color, flags, z);
	}

	virtual int GetWidth() const {
		return m_dev->GetWidth();
	}

	virtual int GetHeight() const {
		return m_dev->GetHeight();
	}

	virtual int GetBackBufferBpp() const {
		return m_dev->WDirect3D::GetBackBufferBpp();
	}

	virtual int sub_100447E0() const {
		return 0;
	}

	virtual int sub_10015C40() const {
		return 0;
	}

	virtual bool IsWindowed() {
		return m_dev->IsWindowed();
	}

	virtual bool IsFillScreenMode() {
		return m_dev->IsFillScreenMode();
	}

	virtual float GetMonitorSupportFPS() const {
		return m_dev->GetMonitorSupportFps();
	}

	virtual bool SetFogEnable(bool enable) {
		return m_dev->SetFogEnable(enable);
	}

	virtual void SetFogState(float fogStart, float fogEnd, DWORD fogColor) {
		m_dev->SetFogState(fogStart, fogEnd, fogColor);
	}

	virtual WDirect3D8KR645* MakeClone(char* modeName, HWND hwnd, int iTnL) {
		return new WDirect3D8KR645(m_dev->MakeClone(modeName, hwnd, iTnL));
	}

	virtual bool Reset(bool bWindowed, int iWidth, int iHeight, int iColor, int lWndStyle, int fillMode) {
		return m_dev->Reset(bWindowed, iWidth, iHeight, iColor, lWndStyle, fillMode);
	}

	virtual DWORD VertexSize(DWORD dwVertexTypeDesc) {
		return m_dev->VertexSize(dwVertexTypeDesc);
	}

	virtual BYTE xGetStride(int hVb) {
		return m_dev->XGetStride(hVb);
	}

	virtual int sub_10015C70(int a) {
		return 0;
	}

	virtual int sub_1001CB80(int a) {
		return 0;
	}

	virtual bool xHasVertexElem(DWORD dwFVF, WVertexElement elem) const {
		return m_dev->XHasVertexElem(dwFVF, elem);
	}

	virtual int xGetVertexElemOffset(DWORD dwFVF, WVertexElement elem) const {
		return m_dev->XGetVertexElemOffset(dwFVF, elem);
	}

	virtual int xGetBlendWeightSize(DWORD dwFVF) const {
		return m_dev->XGetBlendWeightSize(dwFVF);
	}

	virtual DWORD xDetermineFVF(int iDrawFlag, int iDrawFlag2, int iMaxBoneNum) {
		return m_dev->XDetermineFvf(iDrawFlag, iDrawFlag2, iMaxBoneNum);
	}

	virtual int sub_1001CAE0(DWORD dwFVF) {
		return 0;
	}

	virtual DWORD xDetermineBufferUsage(DWORD dwFVF) {
		return m_dev->XDetermineBufferUsage(dwFVF);
	}

	virtual int xCreateVertexBuffer_(int nVtxs, DWORD dwFVF, DWORD dwUsage) {
		return m_dev->XCreateVertexBuffer_(nVtxs, dwFVF, dwUsage);
	}

	virtual int xCreateIndexBuffer_(int nIdxs, DWORD dwUsage) {
		return m_dev->XCreateIndexBuffer_(nIdxs, dwUsage);
	}

	virtual void xReleaseVertexBuffer(int hVb) {
		m_dev->XReleaseVertexBuffer(hVb);
	}

	virtual void xReleaseIndexBuffer(int hIb) {
		m_dev->XReleaseIndexBuffer(hIb);
	}

	virtual char* xLockVertexBuffer(int hVb, unsigned int uiOffset, unsigned int uiSize) {
		return m_dev->XLockVertexBuffer(hVb, uiOffset, uiSize);
	}

	virtual char* xLockIndexBuffer(int hIb, unsigned int uiOffset, unsigned int uiSize) {
		return m_dev->XLockIndexBuffer(hIb, uiOffset, uiSize);
	}

	virtual void xUnlockVertexBuffer(int hVb) {
		m_dev->XUnlockVertexBuffer(hVb);
	}

	virtual void xUnlockIndexBuffer(int hIb) {
		m_dev->XUnlockIndexBuffer(hIb);
	}

	virtual void xDrawIndexedTriangles(const WxViewState& VState, const WxBatchState& BState) {
		m_dev->XDrawIndexedTriangles(VState, BState);
	}

	virtual void xSetTransform(WTransformStateType State, const WMatrix4& Matrix) {
		return m_dev->XSetTransform(State, Matrix);
	}

	virtual void xSetPrevViewTransform(const WMatrix4& Matrix) {
		m_dev->XSetPrevViewTransform(Matrix);
	}

	virtual void SetShaderSource(const char* shaderSrc) {
		m_dev->SetShaderSource(shaderSrc);
	}

	virtual void BeginUsingCustomRenderState() {
		m_dev->BeginUsingCustomRenderState();
	}

	virtual void SetCustomRenderState(WRenderStateType State, unsigned int Value) {
		m_dev->SetCustomRenderState(State, Value);
	}

	virtual void SetCustomTextureStageState(DWORD Stage, WTextureStageStateType Type, DWORD Value) {
		m_dev->SetCustomTextureStageState(Stage, Type, Value);
	}

	virtual void SetCustomTransform(WTransformStateType State, const WMatrix4& Matrix) {
		m_dev->SetCustomTransform(State, Matrix);
	}

	virtual void SetCustomTexture(unsigned int Stage, int hTex) {
		m_dev->SetCustomTexture(Stage, hTex);
	}

	virtual void SetCustomClipPlane(DWORD Index, const WPlane& Value) {
		m_dev->SetCustomClipPlane(Index, Value);
	}

	virtual void SetCustomSamplerState(DWORD Sampler, WSamplerStateType Type, DWORD Value) {
		m_dev->SetCustomSamplerState(Sampler, Type, Value);
	}

	virtual void SetCustomFxMacro(DWORD FxMacro) {
		m_dev->SetCustomFxMacro(FxMacro);
	}

	virtual void SetCustomFxParamInt(WFxParameterType ParamType, int Value) {
		m_dev->SetCustomFxParamInt(ParamType, Value);
	}

	virtual void SetCustomFxParamVector2(WFxParameterType ParamType, const WVector2D& Value) {
		m_dev->SetCustomFxParamVector2(ParamType, Value);
	}

	virtual void SetCustomFxParamVector3(WFxParameterType ParamType, const WVector& Value) {
		m_dev->SetCustomFxParamVector3(ParamType, Value);
	}

	virtual void SetCustomFxParamVector4(WFxParameterType ParamType, const WVector4& Value) {
		m_dev->SetCustomFxParamVector4(ParamType, Value);
	}

	virtual void SetCustomFxParamMatrix(WFxParameterType ParamType, const WMatrix4& Value) {
		m_dev->SetCustomFxParamMatrix(ParamType, Value);
	}

	virtual void SetCustomFxParamTexture(WFxParameterType ParamType, int hTex) {
		m_dev->SetCustomFxParamTexture(ParamType, hTex);
	}

	virtual int sub_1002BEC0(int a) {
		return 0;
	}

	virtual int sub_10040750(int a1, int a2) {
		return 0;
	}

	virtual int sub_10040760(int a1, int a2) {
		return 0;
	}

	virtual void EndUsingCustomRenderState() {
		m_dev->EndUsingCustomRenderState();
	}

	virtual bool BeginRenderToTexture(const WRenderToTextureParam& param) {
		return m_dev->BeginRenderToTexture(param);
	}

	virtual void EndRenderToTexture(const WRenderToTextureParam& param) {
		m_dev->EndRenderToTexture(param);
	}

	virtual bool SupportRenderTargetFormat() {
		return m_dev->SupportRenderTargetFormat();
	}

	virtual bool GetWindowDisplayMode(int& iWidth, int& iHeight, int& iColor) {
		return m_dev->GetWindowDisplayMode(iWidth, iHeight, iColor);
	}

	virtual int sub_1001BE60(int a1, int a2, int a3) {
		return 0;
	}

	virtual int GetBufferingMeshNum() const {
		return m_dev->GetBufferingMeshNum();
	}

	virtual void SetViewPort(DWORD x, DWORD y, DWORD w, DWORD h) {
		m_dev->SetViewPort(x, y, w, h);
	}

	virtual char sub_10044830(char a) {
		return a;
	}

	virtual void sub_10044840(float a) { }
	virtual void sub_10044850(float a) { }
	virtual void sub_10044860(float a) { }

	virtual int sub_10044870(int a) {
		return a;
	}

	virtual char sub_10044880() {
		return 0;
	}

	// WDirect3D
	virtual void DrawPrimitive(int iType, int iNum, DWORD dwVertexTypeDesc, void* lpvVertices,
	                           D3DPRIMITIVETYPE dptPrimitiveType, int iType2, int unk) {
		m_dev->DrawPrimitive(iType, iNum, dwVertexTypeDesc, lpvVertices, dptPrimitiveType, iType2);
	}

	virtual void DrawPrimitiveIndexed(int iType, DWORD dwVertexTypeDesc, void* lpvVertices, int pnum, LPWORD flist,
	                                  int fnum, int iType2, int unk) {
		m_dev->DrawPrimitiveIndexed(iType, dwVertexTypeDesc, lpvVertices, pnum, flist, fnum, iType2);
	}

	virtual bool SetRenderState4Flushing(int pass) {
		return m_dev->SetRenderState4Flushing(pass);
	}

	virtual void Flush(DWORD flag, int unk) {
		m_dev->Flush(flag);
	}

	virtual void FlushEqual() {
		m_dev->FlushEqual();
	}

	virtual void FlushMultiPass(DWORD flag, int unk) {
		return m_dev->FlushMultiPass(flag);
	}

	virtual void FlushOnePass(DWORD flag) {
		return m_dev->FlushOnePass(flag);
	}

	virtual int UploadCompressedTextureSurface(void* pSrcData, size_t srcDataSize, int type) {
		return m_dev->UploadCompressedTextureSurface(pSrcData, srcDataSize, type);
	}

	virtual void CreateTextureSurface(DWORD dwWidth, DWORD dwHeight, int iMipmapcount, int renderTargetType, int iIndex,
	                                  int bChromakey, int bitnum, int unk) {
		m_dev->CreateTextureSurface(dwWidth, dwHeight, iMipmapcount, renderTargetType, iIndex, bChromakey, bitnum);
	}

	virtual void UpdateTextureSurface(int texHandle, tagBITMAPINFO* bi, void* data, int type) {
		m_dev->UpdateTextureSurface(texHandle, bi, data, type);
	}

	virtual void FlushRenderPrimitive() {
		return m_dev->FlushRenderPrimitive();
	}

	virtual void* SnapShotCustomRenderState() {
		throw m_dev->SnapShotCustomRenderState();
	}

	virtual void ClearCustomRenderStateSnapShotList() {
		m_dev->ClearCustomRenderStateSnapShotList();
	}

	virtual void ApplyCustomRenderState(const void* customRenderState) {
		m_dev->ApplyCustomRenderState(customRenderState);
	}

	virtual void xCreateVertexBuffer(int hVb, int nVtxs, DWORD dwFVF, DWORD dwUsage) {
		return m_dev->XCreateVertexBuffer(hVb, nVtxs, dwFVF, dwUsage);
	}

	virtual void xCreateIndexBuffer(int hIb, int nIdxs, DWORD dwUsage) {
		return m_dev->XCreateIndexBuffer(hIb, nIdxs, dwUsage);
	}

	virtual void xDrawIndexedPrimitive(const WxViewState& VState, const WxBatchState& BState) {
		return m_dev->XDrawIndexedPrimitive(VState, BState);
	}

	// WDirect3D8
	virtual WDirect3D8KR645* CreateClone(char* devName, int id) {
		return new WDirect3D8KR645(m_dev->CreateClone(devName, id));
	}

private:
	WDirect3D8* m_dev;
};
