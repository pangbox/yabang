#pragma once
#include <cstdint>
#include <wdevice.h>
#include <wscene.h>

enum WFxMacroFlag {
	WFxMacroTexture = 1 << 0,
	WFxMacroNormal = 1 << 1,
	WFxMacroVertexColor = 1 << 2,
	WFxMacroMaterialColor = 1 << 3,
	WFxMacroTextureStage2 = 1 << 4,
	WFxMacroTextureStage2Add = 1 << 5,
	WFxMacroUseConstColor = 1 << 6,
	WFxMacroFog = 1 << 7,
	WFxMacroNoLighting = 1 << 8,
	WFxMacroRimLighting = 1 << 9,
	WFxMacroApplySphericalHarmonics = 1 << 10,
	WFxMacroPostProcessingDepthOfField = 1 << 11,
	WFxMacroPostProcessingMotionBlur = 1 << 12,
	WFxMacroOverlay = 1 << 13,
	WFxMacroMrtVelocity = 1 << 14,
	WFxMacroMrtDepthOfFieldFactor = 1 << 15,
	WFxMacroProjTextureCoordinate = 1 << 16,
	WFxMacroShadowCaster = 1 << 17,
	WFxMacroShadowReceiver = 1 << 18,
	WFxMacroBlurring = 1 << 19,
	WFxMacroPerPixelLighting = 1 << 28,
	WFxMacroNoVertexShader = 1 << 29,
	WFxMacroNoPixelShader = 1 << 30,
	WFxMacroParamSharing = 1 << 31,
	WFxMacroPsExtend =
	WFxMacroApplySphericalHarmonics |
	WFxMacroPostProcessingDepthOfField |
	WFxMacroPostProcessingMotionBlur |
	WFxMacroOverlay |
	WFxMacroMrtVelocity,
	WFxMacroShaderExtend =
	WFxMacroNoLighting |
	WFxMacroRimLighting |
	WFxMacroPsExtend,
};

enum WTransformStateType : uint32_t {
	WTransformStateView = 0x2,
	WTransformStateProjection = 0x3,
	WTransformStateTexture0 = 0x10,
	WTransformStateTexture1 = 0x11,
	WTransformStateTexture2 = 0x12,
	WTransformStateTexture3 = 0x13,
	WTransformStateTexture4 = 0x14,
	WTransformStateTexture5 = 0x15,
	WTransformStateTexture6 = 0x16,
	WTransformStateTexture7 = 0x17,
};

enum WRenderStateType : uint32_t {
	WRenderStateZEnable = 0x7,
	WRenderStateFillMode = 0x8,
	WRenderStateShadeMode = 0x9,
	WRenderStateZWriteEnable = 0xE,
	WRenderStateAlphaTestEnable = 0xF,
	WRenderStateLastPixel = 0x10,
	WRenderStateSrcBlend = 0x13,
	WRenderStateDestBlend = 0x14,
	WRenderStateCullMode = 0x16,
	WRenderStateZFunc = 0x17,
	WRenderStateAlphaRef = 0x18,
	WRenderStateAlphaFunc = 0x19,
	WRenderStateDitherEnable = 0x1A,
	WRenderStateAlphaBlendEnable = 0x1B,
	WRenderStateFogEnable = 0x1C,
	WRenderStateSpecularEnable = 0x1D,
	WRenderStateFogColor = 0x22,
	WRenderStateFogTableMode = 0x23,
	WRenderStateFogStart = 0x24,
	WRenderStateFogEnd = 0x25,
	WRenderStateFogDensity = 0x26,
	WRenderStateRangeFogEnable = 0x30,
	WRenderStateStencilEnable = 0x34,
	WRenderStateStencilFail = 0x35,
	WRenderStateStencilZFail = 0x36,
	WRenderStateStencilPass = 0x37,
	WRenderStateStencilFunc = 0x38,
	WRenderStateStencilRef = 0x39,
	WRenderStateStencilMask = 0x3A,
	WRenderStateStencilWriteMask = 0x3B,
	WRenderStateTextureFactor = 0x3C,
	WRenderStateWrap0 = 0x80,
	WRenderStateWrap1 = 0x81,
	WRenderStateWrap2 = 0x82,
	WRenderStateWrap3 = 0x83,
	WRenderStateWrap4 = 0x84,
	WRenderStateWrap5 = 0x85,
	WRenderStateWrap6 = 0x86,
	WRenderStateWrap7 = 0x87,
	WRenderStateClipping = 0x88,
	WRenderStateLighting = 0x89,
	WRenderStateAmbient = 0x8B,
	WRenderStateFogVertexMode = 0x8C,
	WRenderStateColorVertex = 0x8D,
	WRenderStateLocalViewer = 0x8E,
	WRenderStateNormalizedNormals = 0x8F,
	WRenderStateDiffuseMaterialSource = 0x91,
	WRenderStateSpecularMaterialSource = 0x92,
	WRenderStateAmbientMaterialSource = 0x93,
	WRenderStateEmissiveMaterialSource = 0x94,
	WRenderStateVertexBlend = 0x97,
	WRenderStateClipPlaneEnable = 0x98,
	WRenderStatePointSize = 0x9A,
	WRenderStatePointSizeMin = 0x9B,
	WRenderStatePointSpriteEnable = 0x9C,
	WRenderStatePointScaleEnable = 0x9D,
	WRenderStatePointScaleA = 0x9E,
	WRenderStatePointScaleB = 0x9F,
	WRenderStatePointScaleC = 0xA0,
	WRenderStateMultiSampleAntiAlias = 0xA1,
	WRenderStateMultiSampleMask = 0xA2,
	WRenderStatePatchEdgeStyle = 0xA3,
	WRenderStateDebugMonitorToken = 0xA5,
	WRenderStatePointSizeMax = 0xA6,
	WRenderStateIndexedVertexBlendEnable = 0xA7,
	WRenderStateColorWriteEnable = 0xA8,
	WRenderStateTweenFactor = 0xAA,
	WRenderStateBlendOp = 0xAB,
	WRenderStatePositionDegree = 0xAC,
	WRenderStateNormalDegree = 0xAD,
	WRenderStateScissorTestEnable = 0xAE,
	WRenderStateSlopScaleDepthBias = 0xAF,
	WRenderStateAntiAliasedLineEnable = 0xB0,
	WRenderStateMinTessellationLevel = 0xB2,
	WRenderStateMaxTessellationLevel = 0xB3,
	WRenderStateAdaptiveTessellationX = 0xB4,
	WRenderStateAdaptiveTessellationY = 0xB5,
	WRenderStateAdaptiveTessellationZ = 0xB6,
	WRenderStateAdaptiveTessellationW = 0xB7,
	WRenderStateEnableAdaptiveTessellation = 0xB8,
	WRenderStateTwoSidedStencilMode = 0xB9,
	WRenderStateCcwStencilFail = 0xBA,
	WRenderStateCcwStencilZFail = 0xBB,
	WRenderStateCcwStencilPass = 0xBC,
	WRenderStateCcwStencilFunc = 0xBD,
	WRenderStateColorWriteEnable1 = 0xBE,
	WRenderStateColorWriteEnable2 = 0xBF,
	WRenderStateColorWriteEnable3 = 0xC0,
	WRenderStateBlendFactor = 0xC1,
	WRenderStateSrgbWriteEnable = 0xC2,
	WRenderStateDepthBias = 0xC3,
	WRenderStateWrap8 = 0xC6,
	WRenderStateWrap9 = 0xC7,
	WRenderStateWrap10 = 0xC8,
	WRenderStateWrap11 = 0xC9,
	WRenderStateWrap12 = 0xCA,
	WRenderStateWrap13 = 0xCB,
	WRenderStateWrap14 = 0xCC,
	WRenderStateWrap15 = 0xCD,
	WRenderStateSeparateAlphaBlendEnable = 0xCE,
	WRenderStateSrcBlendAlpha = 0xCF,
	WRenderStateDestBlendAlpha = 0xD0,
	WRenderStateBlendOpAlpha = 0xD1,
};

enum WTextureStageStateType : uint32_t {
	WTextureStateColorOp = 0x1,
	WTextureStateColorArg1 = 0x2,
	WTextureStateColorArg2 = 0x3,
	WTextureStateAlphaOp = 0x4,
	WTextureStateAlphaArg1 = 0x5,
	WTextureStateAlphaArg2 = 0x6,
	WTextureStateBumpEnvMat00 = 0x7,
	WTextureStateBumpEnvMat01 = 0x8,
	WTextureStateBumpEnvMat10 = 0x9,
	WTextureStateBumpEnvMat11 = 0xA,
	WTextureStateTextureCoordinateIndex = 0xB,
	WTextureStateBumpEnvLScale = 0x16,
	WTextureStateBumpEnvLOffset = 0x17,
	WTextureStateTextureTransformFlags = 0x18,
	WTextureStateColorArg0 = 0x1A,
	WTextureStateAlphaArg0 = 0x1B,
	WTextureStateResultArg = 0x1C,
	WTextureStateConstant = 0x20,
};

enum WSamplerStateType : uint32_t {
	WSamplerStateAddressU = 0x1,
	WSamplerStateAddressV = 0x2,
	WSamplerStateAddressW = 0x3,
	WSamplerStateBorderColor = 0x4,
	WSamplerStateMagFilter = 0x5,
	WSamplerStateMinFilter = 0x6,
	WSamplerStateMipFilter = 0x7,
	WSamplerStateMipmapLodBias = 0x8,
	WSamplerStateMipmapLevel = 0x9,
	WSamplerStateMaxAnisotropy = 0xA,
	WSamplerStateSrgbTexture = 0xB,
	WSamplerStateElementIndex = 0xC,
	WSamplerStateDisplacementMapOffset = 0xD,
};

enum WFxParameterType {
	WFxParamWorldViewProjection = 0x0,
	WFxParamWorld = 0x1,
	WFxParamWorldView = 0x2,
	WFxParamPrevView = 0x3,
	WFxParamView = 0x4,
	WFxParamProjection = 0x5,
	WFxParamViewProjection = 0x6,
	WFxParamConstColor = 0x7,
	WFxParamLightDiffuse = 0x8,
	WFxParamLightAmbient = 0x9,
	WFxParamLightDirection = 0xA,
	WFxParamFogRange = 0xB,
	WFxParamFogColor = 0xC,
	WFxParamCameraPosition = 0xD,
	WFxParamSphericalHarmonicsCoefficients = 0xE,
	WFxParamExtraTexture = 0xF,
	WFxParamRefTexture = 0x10,
	WFxParamMaterialColor = 0x11,
	WFxParamTextureTransform = 0x12,
	WFxParamBlurUvOffset = 0x13,
	WFxParamShadowColor = 0x14,
	WFxParamNum = 0x15,
};

enum WDeviceState {
	WDeviceStateNormal = 0x0,
	WDeviceStateLost = 0x1,
};

enum WDeviceMessage {
	WDeviceMessageSetClipper = 0x0,
	WDeviceMessageActivateApp = 0x1,
	WDeviceMessageMove = 0x2,
	WDeviceMessageSize = 0x3,
	WDeviceMessageSetSize = 0x4,
	WDeviceMessageCaptureScreen = 0x5,
	WDeviceMessageGetFrontSurface = 0x6,
	WDeviceMessageGetBackSurface = 0x7,
	WDeviceMessageDrawBox = 0x8,
	WDeviceMessageSetHWnd = 0x9,
	WDeviceMessageSetRect = 0xA,
	WDeviceMessageFlush = 0xB,
	WDeviceMessageFlipFrontSurface = 0xC,
	WDeviceMessageGetCaptureMode = 0xD,
	WDeviceMessageSetCaptureMode = 0xE,
	WDeviceMessageCaptureResource = 0xF,
	WDeviceMessageCapturedBg = 0x10,
	WDeviceMessageOverdrawAnalyze = 0x11,
	WDeviceMessageGetZBufferHistogram = 0x12,
	WDeviceMessageScreenShot = 0x13,
	WDeviceMessageGetSplash = 0x14,
	WDeviceMessageGetCopiedScreenSplash = 0x15,
	WDeviceMessageUpdateLodTexture = 0x16,
	WDeviceMessageSetDdsRes = 0x17,
	WDeviceMessageUseMipmap = 0x18,
	WDeviceMessageMaxMipLevel = 0x19,
	WDeviceMessageMipCreateFilter = 0x1A,
	WDeviceMessageMipTextureStageFilter = 0x1B,
	WDeviceMessageGetAvailableVram = 0x1C,
	WDeviceMessageNumTextures = 0x1D,
	WDeviceMessageNumTnLBuffers = 0x1E,
	WDeviceMessageGetStatistics = 0x1F,
	WDeviceMessageGetCaps = 0x20,
	WDeviceMessageFillMode = 0x21,
	WDeviceMessageDriverInfo = 0x22,
	WDeviceMessageGetSortBufferSize = 0x23,
	WDeviceMessageGetMergeBufferSize = 0x24,
	WDeviceMessageUnknown26 = 0x26,
};

enum WVertexElement {
	WVertexElementPosition = 0x0,
	WVertexElementNormal = 0x1,
	WVertexElementDiffuse = 0x2,
	WVertexElementTexture1 = 0x3,
	WVertexElementTexture2 = 0x4,
	WVertexElementTexture3 = 0x5,
	WVertexElementTexture4 = 0x6,
	WVertexElementTexture5 = 0x7,
	WVertexElementTexture6 = 0x8,
	WVertexElementTexture7 = 0x9,
	WVertexElementTexture8 = 0xa,
};

enum WPolyFunction : uint32_t {
	WPolyZFuncLessEqual = 0x0 << 20,
	WPolyZFuncEqual = 0x1 << 20,
	WPolyZFuncMostLessEqual = 0x2 << 20,
	WPolyZFuncAlways = 0x3 << 20,
	WFxParamNoDepthCheck = WPolyZFuncAlways,
};

struct WRenderToTextureSizeInfo {
	void Reset() {
		*this = WRenderToTextureSizeInfo{};
	}

	bool isAbsolute = true;
	float width = 0.0f;
	float height = 0.0f;
};

struct WRenderToTextureParam {
	struct RtTexInfo {
		int hTex;
		bool needToClear;
		unsigned int clearColor;
	};

	struct DepthSurfInfo {
		enum SurfaceUsage {
			UseNotNeeded = 0x0,
			UseMainSurface = 0x1,
			UseSharedSurface = 0x2,
			UseExclusiveSurface = 0x3,
		};

		SurfaceUsage surfUsage;
		int hTex;
		bool needToClear;
		float clearZ;
	};

	RtTexInfo rtTexInfo[2];
	DepthSurfInfo depthSurfInfo;

	[[nodiscard]] bool CanClearAtOnce() const {
		for (int i = 1; i < 2; i++) {
			if (!this->rtTexInfo[i].hTex) {
				continue;
			}
			if (this->rtTexInfo[i].needToClear != this->rtTexInfo[0].needToClear) {
				return false;
			}
			if (this->rtTexInfo[i].clearColor != this->rtTexInfo[0].clearColor) {
				return false;
			}
		}
		return true;
	}
};


struct WtVertex {
	WtVertex() {};

	union {
		struct {
			float x;
			float y;
			float z;
		};

		WVector pos{};
	};

	float rhw{};
	unsigned int diffuse{};
	float tu{};
	float tv{};
	float lu{};
	float lv{};
	float vz{};
};

class WVideoDev : public WDevice {
public:
	WVideoDev() {
		this->m_mainThreadId = 0;
		this->m_renderCount = 0;
		this->m_clipScaleZ = 0.f;
		this->m_clipNearScale = 0.f;
	}

	virtual WDeviceState GetDeviceState() = 0;

	virtual void SetMainThreadId(unsigned int threadId) {
		this->m_mainThreadId = threadId;
	}

	virtual void DrawPolygonFan(WtVertex** p, int iType, int iNum, int iType2, uint32_t dwVertexTypeDesc) = 0;

	virtual void DrawIndexedTriangles(WtVertex* p, int pNum, uint16_t* fList, int fNum, uint32_t iType,
	                                  uint32_t iType2) {
		static WtVertex* t[4];

		if (fNum > 0) {
			fNum = (fNum - 1) / 3 + 1;
			do {
				t[0] = &p[fList[0]];
				t[1] = &p[fList[1]];
				t[2] = &p[fList[2]];
				fList += 3;
				this->DrawPolygonFan(t, iType, 3, iType2, 0);
				--fNum;
			} while (fNum);
		}
	}

	virtual void DrawLine(WtVertex** p, int type) = 0;
	virtual int Command(WDeviceMessage message, int param1, int param2) = 0;
	virtual int CreateTexture(LPBITMAPINFO src, int type) = 0;
	virtual void UpdateTexture(int texHandle, LPBITMAPINFO src, LPVOID data, uint32_t type) = 0;
	virtual void DestroyTexture(int texHandle) = 0;
	virtual int UploadCompressedTexture(void* pSrc, size_t srcSize, int type) = 0;
	virtual void UpdateCompressedTexture(int texHandle, void* pSrcData, size_t srcDataSize, int type) = 0;
	virtual void FixTexturePart(int texHandle, const RECT& rc, BITMAPINFO* src, void* data, int type) = 0;
	virtual bool IsTextureFilled(int texHandle) = 0;
	virtual int GetTextureWidth(int hTex) = 0;
	virtual int GetTextureHeight(int hTex) = 0;
	virtual void SetRenderTargetSizeInfo(int hTex, const WRenderToTextureSizeInfo& sizeInfo) = 0;
	virtual bool BeginScene() = 0;
	virtual void EndScene() = 0;
	virtual void Paint() = 0;
	virtual void SetGlobalRenderState(int rs, int rsEx) = 0;
	virtual bool IsSupportVs() = 0;
	virtual bool IsSupportPs() = 0;
	virtual bool IsSupportMrt() = 0;
	virtual bool IsSupportClipPlane() = 0;
	virtual void Clear(uint32_t color, int flags, float z) = 0;
	[[nodiscard]] virtual int GetWidth() const = 0;
	[[nodiscard]] virtual int GetHeight() const = 0;
	[[nodiscard]] virtual int GetBackBufferBpp() const = 0;
	virtual bool IsWindowed() = 0;
	virtual bool IsFillScreenMode() = 0;
	[[nodiscard]] virtual float GetMonitorSupportFps() const = 0;
	virtual bool SetFogEnable(bool enable) = 0;
	virtual void SetFogState(float fogStart, float fogEnd, uint32_t fogColor) = 0;
	virtual WVideoDev* MakeClone(char* modeName, HWND hWnd, int iTnL) = 0;
	virtual bool Reset(bool bWindowed, int iWidth, int iHeight, int iColor, int lWndStyle, int fillMode) = 0;

protected:
	virtual uint32_t VertexSize(uint32_t dwVertexTypeDesc) = 0;

public:
	virtual BYTE XGetStride(int hVb) = 0;
	[[nodiscard]] virtual bool XHasVertexElem(uint32_t fvf, WVertexElement elem) const = 0;
	[[nodiscard]] virtual int XGetVertexElemOffset(uint32_t fvf, WVertexElement elem) const = 0;
	[[nodiscard]] virtual int XGetBlendWeightSize(uint32_t fvf) const = 0;
	virtual uint32_t XDetermineFvf(int iDrawFlag, int iDrawFlag2, int iMaxBoneNum) = 0;
	virtual uint32_t XDetermineBufferUsage(uint32_t fvf) = 0;
	virtual int XCreateVertexBuffer_(int numVertices, uint32_t fvf, uint32_t dwUsage) = 0;
	virtual int XCreateIndexBuffer_(int numIndices, uint32_t dwUsage) = 0;
	virtual void XReleaseVertexBuffer(int hVb) = 0;
	virtual void XReleaseIndexBuffer(int hIb) = 0;
	virtual char* XLockVertexBuffer(int hVb, unsigned int uiOffset, unsigned int uiSize) = 0;
	virtual char* XLockIndexBuffer(int hIb, unsigned int uiOffset, unsigned int uiSize) = 0;
	virtual void XUnlockVertexBuffer(int hVb) = 0;
	virtual void XUnlockIndexBuffer(int hIb) = 0;
	virtual void XDrawIndexedTriangles(const WxViewState& viewState, const WxBatchState& batchState) = 0;
	virtual void XSetTransform(WTransformStateType state, const WMatrix4& matrix) = 0;
	virtual void XSetPrevViewTransform(const WMatrix4& matrix) = 0;
	virtual void SetShaderSource(const char* shaderSrc) = 0;
	virtual void BeginUsingCustomRenderState() = 0;
	virtual void SetCustomRenderState(WRenderStateType state, unsigned int value) = 0;
	virtual void SetCustomTextureStageState(uint32_t stage, WTextureStageStateType type, uint32_t value) = 0;
	virtual void SetCustomTransform(WTransformStateType state, const WMatrix4& matrix) = 0;
	virtual void SetCustomTexture(unsigned int stage, int hTex) = 0;
	virtual void SetCustomClipPlane(uint32_t index, const WPlane& value) = 0;
	virtual void SetCustomSamplerState(uint32_t sampler, WSamplerStateType type, uint32_t value) = 0;
	virtual void SetCustomFxMacro(uint32_t fxMacro) = 0;
	virtual void SetCustomFxParamInt(WFxParameterType paramType, int value) = 0;
	virtual void SetCustomFxParamVector2(WFxParameterType paramType, const WVector2D& value) = 0;
	virtual void SetCustomFxParamVector3(WFxParameterType paramType, const WVector& value) = 0;
	virtual void SetCustomFxParamVector4(WFxParameterType paramType, const WVector4& value) = 0;
	virtual void SetCustomFxParamMatrix(WFxParameterType paramType, const WMatrix4& value) = 0;
	virtual void SetCustomFxParamTexture(WFxParameterType paramType, int hTex) = 0;
	virtual void EndUsingCustomRenderState() = 0;
	virtual bool BeginRenderToTexture(const WRenderToTextureParam& param) = 0;
	virtual void EndRenderToTexture(const WRenderToTextureParam& param) = 0;
	virtual bool SupportRenderTargetFormat() = 0;
	virtual bool IsSupportedDisplayMode(bool bWindowed, int iWidth, int iHeight, int iColor) = 0;
	virtual bool GetWindowDisplayMode(int& iWidth, int& iHeight, int& iColor) = 0;
	[[nodiscard]] virtual int GetBufferingMeshNum() const = 0;
	virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) = 0;

	unsigned int m_mainThreadId = 0;
	unsigned int m_renderCount;
	float m_clipScaleZ;
	float m_clipNearScale;
};

class WSplash {
public:
	WSplash() = default;
	virtual ~WSplash() = default;
	WSplash(const WSplash&) = delete;
	WSplash(WSplash&&) = delete;
	WSplash& operator=(const WSplash&) = delete;
	WSplash& operator=(const WSplash&&) = delete;

	virtual int InitFromScreen(bool bCopy) = 0;
	virtual void Reset() = 0;
	virtual void Draw(const WPoint* pSrc, const WRect* pDest, uint32_t color) = 0;
	virtual void Draw(uint32_t color) = 0;
	virtual void ResetScreenSize() = 0;
	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;
	virtual void SetTextureCoordinateOffset(float u, float v) = 0;
};
