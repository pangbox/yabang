#include "wd3d8.h"
#include <cmath>
#include <algorithm>
#include <psapi.h>
#include <wdxtc.h>
#include <windows.h>
#include <wlist.h>
#include <wvideo.h>
#include <xmmintrin.h>
#include "screencape.h"

const char* g_driverPath[4] = {
	"C:/WINNT/system/",
	"C:/WINNT/system32/",
	"C:/WINDOWS/system/",
	"C:/WINDOWS/system32/",
};

struct {
	D3DRENDERSTATETYPE state;
	uint32_t value;
} g_defRenderState[] = {
	{D3DRS_ZENABLE, 1},
	{D3DRS_FILLMODE, 3},
	{D3DRS_SHADEMODE, 2},
	{D3DRS_ZWRITEENABLE, 1},
	{D3DRS_ALPHATESTENABLE, 1},
	{D3DRS_LASTPIXEL, 1},
	{D3DRS_ZFUNC, 4},
	{D3DRS_ALPHAREF, 0},
	{D3DRS_ALPHAFUNC, 5},
	{D3DRS_DITHERENABLE, 1},
	{D3DRS_ALPHABLENDENABLE, 0},
	{D3DRS_FOGENABLE, 0},
	{D3DRS_SPECULARENABLE, 0},
	{D3DRS_FOGTABLEMODE, 0},
	{D3DRS_DEPTHBIAS, 0},
	{D3DRS_RANGEFOGENABLE, 0},
	{D3DRS_STENCILENABLE, 0},
	{D3DRS_TEXTUREFACTOR, 0},
	{D3DRS_CULLMODE, 1},
	{D3DRS_CLIPPING, 1},
	{D3DRS_LIGHTING, 0},
	{D3DRS_AMBIENT, 0},
	{D3DRS_FOGVERTEXMODE, 3},
	{D3DRS_BLENDOP, 1},
	{D3DRS_INDEXEDVERTEXBLENDENABLE, 0},
	{D3DRS_VERTEXBLEND, 0},
	{D3DRS_NORMALIZENORMALS, 0},
	{D3DRS_COLORVERTEX, 1},
	{D3DRS_SPECULARMATERIALSOURCE, 2},
	{D3DRS_COLORWRITEENABLE, 15},
};

struct {
	uint32_t stage;
	D3DTEXTURESTAGESTATETYPE type;
	uint32_t value;
} g_defTextureState[12] = {
	{0, D3DTSS_TEXCOORDINDEX, 0},
	{1, D3DTSS_TEXCOORDINDEX, 1},
	{0, D3DTSS_COLORARG1, 2},
	{0, D3DTSS_COLORARG2, 0},
	{1, D3DTSS_COLORARG1, 2},
	{1, D3DTSS_COLORARG2, 1},
	{0, D3DTSS_ALPHAARG1, 2},
	{0, D3DTSS_ALPHAARG2, 0},
	{0, D3DTSS_ALPHAOP, 4},
	{1, D3DTSS_ALPHAARG1, 1},
	{1, D3DTSS_ALPHAARG2, 2},
	{1, D3DTSS_ALPHAOP, 2},
};

struct {
	uint32_t stage;
	D3DSAMPLERSTATETYPE type;
	uint32_t value;
} g_defSamplerState[10] = {
	{0, D3DSAMP_ADDRESSU, 1},
	{0, D3DSAMP_ADDRESSV, 1},
	{1, D3DSAMP_ADDRESSU, 3},
	{1, D3DSAMP_ADDRESSV, 3},
	{0, D3DSAMP_MINFILTER, 2},
	{0, D3DSAMP_MAGFILTER, 2},
	{0, D3DSAMP_MIPFILTER, 1},
	{1, D3DSAMP_MINFILTER, 2},
	{1, D3DSAMP_MAGFILTER, 2},
	{1, D3DSAMP_MIPFILTER, 1},
};

const float g_sqrtPi = sqrt(3.141592741012573f);
unsigned int g_alphaRef;
int g_nTriangles;
D3DPRIMITIVETYPE g_primitiveType;
WindowsVersion g_winVer;

const D3DXMATRIX g_mId(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
);

struct {
	WFxParameterType wvdFxParamType;
	const char* semantic;
} g_paramList[21] = {
	{WFxParamWorldViewProjection, "WVDFXPARAM_WORLDVIEWPROJECTION"},
	{WFxParamWorld, "WVDFXPARAM_WORLD"},
	{WFxParamWorldView, "WVDFXPARAM_WORLDVIEW"},
	{WFxParamPrevView, "WVDFXPARAM_PREV_VIEW"},
	{WFxParamView, "WVDFXPARAM_VIEW"},
	{WFxParamProjection, "WVDFXPARAM_PROJECTION"},
	{WFxParamViewProjection, "WVDFXPARAM_VIEWPROJECTION"},
	{WFxParamConstColor, "WVDFXPARAM_CONSTCOLOR"},
	{WFxParamLightDiffuse, "WVDFXPARAM_LIGHT_DIFFUSE"},
	{WFxParamLightAmbient, "WVDFXPARAM_LIGHT_AMBIENT"},
	{WFxParamLightDirection, "WVDFXPARAM_LIGHT_DIRECTION"},
	{WFxParamFogRange, "WVDFXPARAM_FOG_RANGE"},
	{WFxParamFogColor, "WVDFXPARAM_FOG_COLOR"},
	{WFxParamCameraPosition, "WVDFXPARAM_CAMERAPOSITION"},
	{WFxParamSphericalHarmonicsCoefficients, "WVDFXPARAM_SHCOEFF"},
	{WFxParamExtraTexture, "WVDFXPARAM_EXTRATEXTURE"},
	{WFxParamRefTexture, "WVDFXPARAM_REFTEXTURE"},
	{WFxParamMaterialColor, "WVDFXPARAM_MATERIALCOLOR"},
	{WFxParamTextureTransform, "WVDFXPARAM_TEXTURE_TRANSFORM"},
	{WFxParamBlurUvOffset, "WVDFXPARAM_BLUR_UVOFFSET"},
	{WFxParamShadowColor, "WVDFXPARAM_SHADOW_COLOR"},
};

struct {
	WFxMacroFlag wvdFxMacroFlag;
	const char* txt;
} g_macroList[24] = {
	{WFxMacroTexture, "WVDFXMACRO_TEXTURE"},
	{WFxMacroNormal, "WVDFXMACRO_NORMAL"},
	{WFxMacroVertexColor, "WVDFXMACRO_VERTEX_COLOR"},
	{WFxMacroMaterialColor, "WVDFXMACRO_MATERIAL_COLOR"},
	{WFxMacroTextureStage2, "WVDFXMACRO_TEXTURE_STAGE2"},
	{WFxMacroTextureStage2Add, "WVDFXMACRO_TEXTURE_STAGE2_ADD"},
	{WFxMacroUseConstColor, "WVDFXMACRO_USE_CONSTCOLOR"},
	{WFxMacroFog, "WVDFXMACRO_FOG"},
	{WFxMacroNoLighting, "WVDFXMACRO_NO_LIGHTING"},
	{WFxMacroRimLighting, "WVDFXMACRO_RIMLIGHTING"},
	{WFxMacroApplySphericalHarmonics, "WVDFXMACRO_APPLY_SPHERICAL_HARMONICS"},
	{WFxMacroPostProcessingDepthOfField, "WVDFXMACRO_POSTPROCESSING_DOF"},
	{WFxMacroPostProcessingMotionBlur, "WVDFXMACRO_POSTPROCESSING_MOTIONBLUR"},
	{WFxMacroOverlay, "WVDFXMACRO_OVERLAY"},
	{WFxMacroMrtVelocity, "WVDFXMACRO_MRT_VELOCITY"},
	{WFxMacroMrtDepthOfFieldFactor, "WVDFXMACRO_MRT_DOF_FACTOR"},
	{WFxMacroProjTextureCoordinate, "WVDFXMACRO_PROJ_TEXCOORD"},
	{WFxMacroShadowCaster, "WVDFXMACRO_SHADOW_CASTER"},
	{WFxMacroShadowReceiver, "WVDFXMACRO_SHADOW_RECVER"},
	{WFxMacroBlurring, "WVDFXMACRO_BLURRING"},
	{WFxMacroPerPixelLighting, "WVDFXMACRO_PERPIXEL_LIGHTING"},
	{WFxMacroNoVertexShader, "WVDFXMACRO_NO_VERTEXSHADER"},
	{WFxMacroNoPixelShader, "WVDFXMACRO_NO_PIXELSHADER"},
	{WFxMacroParamSharing, "WVDFXMACRO_PARAM_SHARING"},
};

float g_shCoefficient[28] = {
	-0.33614999f, 0.94016099f, 0.127409f, 1.14203f, -0.33537f,
	1.16755f, 0.100581f, 1.3035001f, -0.28731999f, 1.35096f,
	0.036699001f, 1.39474f, -0.16829801f, 0.0227974f, -0.22076701f,
	-0.159484f, -0.149726f, -0.038500499f, -0.29665101f, -0.140673f,
	-0.106568f, -0.126265f, -0.36153501f, -0.102911f, 0.0286815f,
	-0.0049609598f, -0.060617f, 1.0f
};

WDirect3D8::sRtFormat WDirect3D8::m_rtFmt[4] = {
	{D3DFMT_A8R8G8B8, 4},
	{D3DFMT_A4R4G4B4, 2},
	{D3DFMT_A1R5G5B5, 2},
	{D3DFMT_A8R3G3B2, 2},
};

WDirect3D8::sRtFormat WDirect3D8::m_rtNoAlphaFmt[3] = {
	{D3DFMT_X8R8G8B8, 4},
	{D3DFMT_R5G6B5, 2},
	{D3DFMT_X1R5G5B5, 2},
};

WDirect3D8::sRtFormat WDirect3D8::m_rtDepthFmt[5] = {
	{D3DFMT_D24X8, 4},
	{D3DFMT_D16, 2},
	{D3DFMT_D24S8, 4},
	{D3DFMT_D24X4S4, 4},
	{D3DFMT_D15S1, 2},
};

WDirect3D8::FMTLIST WDirect3D8::m_fmtList[5] = {
	{D3DFMT_R5G6B5, "R5G6B5", 0x10},
	{D3DFMT_X1R5G5B5, "X1R5G5B5", 0x10},
	{D3DFMT_A1R5G5B5, "A1R5G5B5", 0x10},
	{D3DFMT_X8R8G8B8, "X8R8G8B8", 0x20},
	{D3DFMT_A8R8G8B8, "A8R8G8B8", 0x20},
};

D3DFORMAT WDirect3D8::m_fmtRecommendedList[6][4] = {
	{D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_A1R5G5B5, D3DFMT_R8G8B8},
	{D3DFMT_A1R5G5B5, D3DFMT_A4R4G4B4, D3DFMT_A8R8G8B8, D3DFMT_A8R3G3B2},
	{D3DFMT_A4R4G4B4, D3DFMT_R8G8B8, D3DFMT_A8R3G3B2, D3DFMT_A1R5G5B5},
	{D3DFMT_R8G8B8, D3DFMT_X8R8G8B8, D3DFMT_R5G6B5, D3DFMT_A1R5G5B5},
	{D3DFMT_A8R8G8B8, D3DFMT_A1R5G5B5, D3DFMT_A4R4G4B4, D3DFMT_UNKNOWN},
	{D3DFMT_A8R8G8B8, D3DFMT_A4R4G4B4, D3DFMT_A1R5G5B5, D3DFMT_UNKNOWN},
};

WDirect3D8::PixInfo WDirect3D8::m_fmtTypeList[9] = {
	{D3DFMT_R5G6B5, 2, 3, 0x0B, 2, 5, 3, 0, 8, 0},
	{D3DFMT_R8G8B8, 3, 0, 0x10, 0, 8, 0, 0, 8, 0},
	{D3DFMT_A1R5G5B5, 2, 3, 0x0A, 3, 5, 3, 0, 7, 0x0F},
	{D3DFMT_X1R5G5B5, 2, 3, 0x0A, 3, 5, 3, 0, 8, 0x10},
	{D3DFMT_R3G3B2, 1, 5, 5, 5, 2, 6, 0, 8, 0},
	{D3DFMT_A4R4G4B4, 2, 4, 8, 4, 4, 4, 0, 4, 0x0C},
	{D3DFMT_A8R8G8B8, 4, 0, 0x10, 0, 8, 0, 0, 0, 0x18},
	{D3DFMT_A8R3G3B2, 2, 5, 5, 5, 2, 6, 0, 0, 8},
	{D3DFMT_X8R8G8B8, 4, 0, 0x10, 0, 8, 0, 0, 8, 0x18},
};

sCaptureOption g_captureOption = {sCaptureOption::CM_A, false, false, true, false, D3DSWAPEFFECT(0), 0};

D3DTEXTURESTAGESTATETYPE g_texStageStateList[6] = {
	D3DTSS_COLORARG1, D3DTSS_COLORARG2, D3DTSS_COLOROP,
	D3DTSS_ALPHAARG1, D3DTSS_ALPHAARG2, D3DTSS_ALPHAOP,
};

WindowsVersion GetWindowsVersion() {
	return WinVerWindows7;
}

namespace WindowUtility {
	DwmApiDll::DwmApiDll() {
		this->m_hDWMApi = nullptr;
		this->m_fIsEnabled = nullptr;
		this->m_fEnable = nullptr;
	}

	DwmApiDll::~DwmApiDll() {
		if (this->m_hDWMApi) {
			FreeLibrary(this->m_hDWMApi);
			this->m_hDWMApi = nullptr;
		}
	}

	HMODULE DwmApiDll::LoadDwmApiDll() {
		TRACE("Loading DWM API.");
		this->m_hDWMApi = LoadLibrary(TEXT("dwmapi.dll"));
		if (!this->m_hDWMApi) {
			TRACE("Failed to load DWM API.");
			return nullptr;
		}
		this->m_fIsEnabled = reinterpret_cast<IsCompositionEnabledPtr>(GetProcAddress(
			this->m_hDWMApi, "DwmIsCompositionEnabled"));
		this->m_fEnable = reinterpret_cast<EnableCompositionPtr>(GetProcAddress(this->m_hDWMApi, "DwmEnableComposition")
		);
		TRACE("DWM Loaded: %p, %p, %p", this->m_hDWMApi, this->m_fIsEnabled, this->m_fEnable);
		return this->m_hDWMApi;
	}

	BOOL DwmApiDll::IsCompositionEnabled() const {
		BOOL bEnabled = 0;

		if (!this->m_fIsEnabled || FAILED(this->m_fIsEnabled(&bEnabled)) || bEnabled != 1) {
			return FALSE;
		}

		return TRUE;
	}

	HRESULT DwmApiDll::EnableComposition(BOOL bEnable) {
		if (this->m_fEnable && this->m_fIsEnabled) {
			return this->m_fEnable(bEnable);
		}
		return E_FAIL;
	}
}

HRESULT fnWD3DDevice_CopyRect(IDirect3DDevice9* dev, IDirect3DSurface9* srcSurf, LPRECT srcRc,
                              IDirect3DSurface9* dstSurf) {
	TRACE("dev=%p, srcSurf=%p, srcRc=%p, dstSurf=%p", dev, srcSurf, srcRc, dstSurf);
	return dev->StretchRect(srcSurf, srcRc, dstSurf, srcRc, D3DTEXF_NONE);
}

HRESULT fnWD3DDevice_SetStreamSource(IDirect3DDevice9* dev, UINT strmNo, IDirect3DVertexBuffer9* vb, UINT offset,
                                     UINT stride) {
	TRACE("dev=%p, strmNo=%d, vb=%p, offset=%d, stride=%d", dev, strmNo, vb, offset, stride);
	return dev->SetStreamSource(strmNo, vb, offset, stride);
}

HRESULT fnWD3DDevice_DrawIndexedPrimitive(IDirect3DDevice9* dev, D3DPRIMITIVETYPE primType, INT baseVtxIdx, UINT minIdx,
                                          UINT numVtxs, UINT startIdx, UINT primCount) {
	TRACE("dev=%p, primType=%d, baseVtxIdx=%d, minIdx=%d, numVtxs=%d, startIdx=%d, primCount=%d", dev, primType,
	      baseVtxIdx, minIdx, numVtxs, startIdx, primCount);
	return dev->DrawIndexedPrimitive(primType, baseVtxIdx, minIdx, numVtxs, startIdx, primCount);
}

void sCaptureOption::SetMode(eCaptureMode mode, bool windowed) {
	TRACE("mode=%d, windowed=%d", mode, windowed);
	this->currentMode = mode;
	if (mode == CM_B) {
		this->fullScreenPresentationInterval = 0x80000000;
		this->swapEffect = D3DSWAPEFFECT_COPY;
		this->useMemCopy = false;
		this->updateWholeScreen = false;
		this->useTexture = g_winVer >= WinVerWindowsVista;
	} else if (mode == CM_A) {
		this->fullScreenPresentationInterval = 0;
		this->swapEffect = D3DSWAPEFFECT_COPY;
		this->useMemCopy = true;
		this->updateWholeScreen = true;
		this->useTexture = false;
	}
}

WSplashD3D::WSplashD3D(WDirect3D8* pDriver): m_pDriver(nullptr), m_hTexs{}, m_rects{}, m_uvs{}, m_offset(),
                                             m_iBufWidth(0), m_iBufHeight(0),
                                             m_iSrcWidth(0),
                                             m_iSrcHeight(0),
                                             m_nSurfCount(0) {
	TRACE("STUB pDriver=%p", pDriver);
	// TODO: implement
}

WSplashD3D::~WSplashD3D() {
	TRACE("STUB");
	// TODO: implement
}

void WSplashD3D::Init(tagBITMAPINFO* bi, void* data, bool fitToScreen) {
	TRACE("STUB");
	// TODO: implement
}

void WSplashD3D::Reset() {
	TRACE("STUB");
	// TODO: implement
}

void WSplashD3D::SetTextureCoordinateOffset(float u, float v) {
	TRACE("STUB");
	// TODO: implement
}

void WSplashD3D::Draw(const WPoint* pSrc, const WRect* pDest, uint32_t color) {
	TRACE("STUB");
	// TODO: implement
}

void WSplashD3D::Draw(uint32_t color) {
	TRACE("STUB");
	// TODO: implement
}

void WSplashD3D::ResetScreenSize() {
	TRACE("STUB");
	// TODO: implement
}

int WSplashD3D::GetWidth() {
	TRACE("STUB");
	// TODO: implement
	return 640;
}

int WSplashD3D::GetHeight() {
	TRACE("STUB");
	// TODO: implement
	return 480;
}

int WSplashD3D::InitFromScreen(bool bCopy) {
	TRACE("STUB");
	// TODO: implement
	return 0;
}

int FillIndices(D3DPRIMITIVETYPE primitiveType, uint16_t startIndex, uint16_t numVertices, uint16_t* pIndexData) {
	TRACE("Called");

	uint16_t i = 0;
	switch (primitiveType) {
		case D3DPT_LINELIST:
			for (i = 0; i < numVertices - 1; i++) {
				pIndexData[i * 2 + 0] = startIndex + 2 * i + 0;
				pIndexData[i * 2 + 1] = startIndex + 2 * i + 1;
			}
			break;
		case D3DPT_LINESTRIP:
			for (i = 0; i < numVertices - 1; i++) {
				pIndexData[i * 2 + 0] = i + startIndex + 0;
				pIndexData[i * 2 + 1] = i + startIndex + 1;
			}
			break;
		case D3DPT_TRIANGLELIST:
			for (i = 0; i < numVertices / 3; i++) {
				pIndexData[i * 3 + 0] = i + startIndex + 2 * i + 0;
				pIndexData[i * 3 + 1] = i + startIndex + 2 * i + 1;
				pIndexData[i * 3 + 2] = i + startIndex + 2 * i + 2;
			}
			break;
		case D3DPT_TRIANGLESTRIP:
			for (i = 0; i < numVertices - 3; i++) {
				if (i & 1) {
					pIndexData[i * 3 + 0] = i + startIndex + 0;
					pIndexData[i * 3 + 1] = i + startIndex + 2;
					pIndexData[i * 3 + 2] = i + startIndex + 1;
				} else {
					pIndexData[i * 3 + 1] = i + startIndex + 1;
					pIndexData[i * 3 + 2] = i + startIndex + 2;
				}
			}
			break;
		case D3DPT_TRIANGLEFAN:
			for (i = 0; i < numVertices - 2; i++) {
				pIndexData[i * 3 + 0] = startIndex;
				pIndexData[i * 3 + 1] = i + startIndex + 1;
				pIndexData[i * 3 + 2] = i + startIndex + 2;
			}
			break;
		default:
			TRACE("Unexpected FillIndices call with PrimitiveType=%d", primitiveType);
	}
	return i;
}

WDirect3D8::WDirect3D8(char* devName, int id) {
	TRACE("Initializing: devName=%s, id=%d", devName, id);
	for (auto& i : this->m_texList) {
		i.renderTargetSizeInfo.isAbsolute = false;
		i.renderTargetSizeInfo.width = 0;
		i.renderTargetSizeInfo.height = 0;
	}
	if (g_winVer == WinVerNone) {
		g_winVer = GetWindowsVersion();
	}
	const size_t devNameBufSize = strlen(devName) + 6;
	this->m_devName = new char[devNameBufSize];
	strcpy_s(this->m_devName, devNameBufSize, "[DX8]");
	strcat_s(this->m_devName, devNameBufSize, devName);
	this->m_devId = id;
	for (int i = 0; i < 7; i++) {
		this->m_shCoefficient[i][0] = g_shCoefficient[(i << 2) + 0];
		this->m_shCoefficient[i][1] = g_shCoefficient[(i << 2) + 1];
		this->m_shCoefficient[i][2] = g_shCoefficient[(i << 2) + 2];
		this->m_shCoefficient[i][3] = g_shCoefficient[(i << 2) + 3];
	}
	this->m_dwmApiDll = new WindowUtility::DwmApiDll();
	this->m_dwmApiDll->LoadDwmApiDll();
	TRACE("Done.");
}

WDirect3D8* WDirect3D8::CreateClone(char* devName, int id) {
	TRACE("Called");
	return new WDirect3D8(devName, id);
}

void SetD3DMatrixFromWMatrix(D3DMATRIX* d3dm, const WMatrix* wm) {
	TRACE("Called");
	d3dm->m[0][0] = wm->p[0];
	d3dm->m[0][1] = wm->p[1];
	d3dm->m[0][2] = wm->p[2];
	d3dm->m[0][3] = 0.0;
	d3dm->m[1][0] = wm->p[3];
	d3dm->m[1][1] = wm->p[4];
	d3dm->m[1][2] = wm->p[5];
	d3dm->m[1][3] = 0.0;
	d3dm->m[2][0] = wm->p[6];
	d3dm->m[2][1] = wm->p[7];
	d3dm->m[2][2] = wm->p[8];
	d3dm->m[2][3] = 0.0;
	d3dm->m[3][0] = wm->p[9];
	d3dm->m[3][1] = wm->p[10];
	d3dm->m[3][2] = wm->p[11];
	d3dm->m[3][3] = 1.0;
}

WDirect3D8::WFxParamPool& WDirect3D8::GetFxParamPool() {
	TRACE("Called");
	return this->m_fxParamPool;
}

LRESULT WDirect3D8::WinProc(UINT message, WPARAM wParam, LPARAM lParam) {
	TRACE("message=%d, wParam=%d, lParam=%d", message, wParam, lParam);
	switch (message) {
		case WM_PAINT:
			if (this->m_pd3dDevice && this->m_bWindow && this->m_pd3dDevice->TestCooperativeLevel() >= 0) {
				this->m_pd3dDevice->Present(nullptr, nullptr, this->m_hWnd, nullptr);
			}

		case WM_SYSCOMMAND:
			if (wParam > 0xF030) {
				if (wParam != 0xF100 && wParam != 0xF170) {
					return 0;
				}
			} else if (wParam != 0xF030 && wParam != 0xF000 && wParam != 0xF010) {
				return 0;
			}
			if (this->m_bWindow) {
				return 0;
			}
		case WM_ACTIVATEAPP:
			break;
		default:
			return 0;
	}

	return 1;
}

HRESULT WDirect3D8::SetVertexShader(uint32_t dwVertexTypeDesc) {
	TRACE("dwVertexTypeDesc=%d", dwVertexTypeDesc);
	if (this->m_xLastVertexDecl == dwVertexTypeDesc) {
		return 0;
	}

	this->FlushRenderPrimitive();

	this->m_xLastVertexDecl = dwVertexTypeDesc;
	if ((dwVertexTypeDesc & D3DFVF_POSITION_MASK) == 2 || (dwVertexTypeDesc & D3DFVF_POSITION_MASK) == 4) {
		return this->m_pd3dDevice->SetFVF(dwVertexTypeDesc);
	}
	return this->m_pd3dDevice->SetFVF((dwVertexTypeDesc & 0xFFFFAFF3) | 2);
}

bool WDirect3D8::SetFogEnable(bool enable) {
	TRACE("enable=%d", enable);
	return WDirect3D::SetFogEnable(enable);
}

D3DFORMAT WDirect3D8::FindDepthBufferFormat(D3DFORMAT format) const {
	TRACE("format=%08x", format);
	for (auto& i : m_rtDepthFmt) {
		if (this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL, format, 2, D3DRTYPE_SURFACE,
		                                    i.fmt) != S_OK) {
			continue;
		}
		if (this->m_d3d8->CheckDepthStencilMatch(this->m_devId, D3DDEVTYPE_HAL, format, format, i.fmt) !=
			S_OK) {
			continue;
		}
		return i.fmt;
	}
	return D3DFMT_D16;
}

int WDirect3D8::GetBackBufferBpp(D3DFORMAT format) const {
	TRACE("format=%08x", format);
	for (auto& i : m_fmtList) {
		if (i.format == format) {
			return i.bitNum;
		}
	}
	return 0;
}

WDirect3D8::PixInfo* WDirect3D8::FindPixInfoByFormat(D3DFORMAT fmt) {
	TRACE("fmt=%08x", fmt);
	for (auto& i : m_fmtTypeList) {
		if (i.pixFmt == fmt) {
			return &i;
		}
	}

	return nullptr;
}

int WDirect3D8::SetTextureFormat(D3DFORMAT format) {
	TRACE("format=%08x", format);
	int result = 0;

	for (int i = 0; i < 6; i++) {
		int j;
		for (j = 0; j < 4; j++) {
			if (this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL, format, 0, D3DRTYPE_TEXTURE,
			                                    m_fmtRecommendedList[i][j]) != S_OK) {
				continue;
			}
			for (auto& k : m_fmtTypeList) {
				if (k.pixFmt != m_fmtRecommendedList[i][j]) {
					continue;
				}
				this->m_fmt[i] = &k;
				break;
			}
			break;
		}
		if (j == 4) {
			result = 1;
			TRACE("%d: no format found");
		}
	}

	return result;
}

HRESULT WDirect3D8::Present() {
	TRACE("Called");

	RECT rc;
	D3DLOCKED_RECT lr;

	if (this->m_pScreenCape) {
		this->m_pScreenCape->Render();
	}

	++this->m_renderCount;
	HRESULT result = this->m_pd3dDevice->Present(nullptr, nullptr, this->m_hWnd, nullptr);
	if (FAILED(result)) {
		return result;
	}

	if (!this->m_pEventQuery) {
		IDirect3DSurface9* pRenderTarget;
		result = this->m_pd3dDevice->GetRenderTarget(0, &pRenderTarget);
		if (SUCCEEDED(result)) {
			SetRect(&rc, 0, 0, 1, 1);
			pRenderTarget->LockRect(&lr, &rc, 16);
			pRenderTarget->UnlockRect();
			pRenderTarget->Release();
		}
		return result;
	}

	this->m_pEventQuery->Issue(1);
	while (this->m_pEventQuery->GetData(nullptr, 0, 1) == 1) {}
	return result;
}

void WDirect3D8::XResetCreateResource() {
	int width, height;
	WRenderToTextureSizeInfo sizeInfo;

	TRACE("Called");

	for (auto& i : this->m_xaVbList) {
		if (i.xdwUsage & 0x200) {
			this->m_pd3dDevice->CreateVertexBuffer(
				i.xnVtxs * i.xbStride,
				i.xdwUsage,
				i.xdwFVF,
				D3DPOOL_DEFAULT,
				&i.xpVb,
				nullptr
			);
		}
	}

	for (auto& i : this->m_xaIbList) {
		if (i.xdwUsage & 0x200) {
			this->m_pd3dDevice->CreateIndexBuffer(
				i.xnIdxs * 2,
				i.xdwUsage,
				D3DFMT_INDEX16,
				D3DPOOL_DEFAULT,
				&i.xpIb,
				nullptr
			);
		}
	}

	for (int i = 0; i < 2048; i++) {
		if (!this->m_texList[i].renderTargetType) {
			continue;
		}

		sizeInfo.width = this->m_texList[i].renderTargetSizeInfo.width;
		sizeInfo.height = this->m_texList[i].renderTargetSizeInfo.height;

		if (this->m_texList[i].renderTargetSizeInfo.isAbsolute) {
			width = this->m_texList[i].width;
			height = this->m_texList[i].height;
		} else {
			width = static_cast<int>(floor(
				static_cast<float>(this->GetWidth()) * this->m_texList[i].renderTargetSizeInfo.width + 0.5f));
			height = static_cast<int>(floor(
				static_cast<float>(this->GetHeight()) * this->m_texList[i].renderTargetSizeInfo.height + 0.5f));
		}

		this->CreateTextureSurface(width, height, this->m_texList[i].mipmaplevel, this->m_texList[i].renderTargetType,
		                           i, 0, 32);
		this->SetRenderTargetSizeInfo(i, sizeInfo);
	}
}

void WDirect3D8::CreateEventQuery() {
	TRACE("Called");

	if (this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, nullptr) >= 0) {
		this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, &this->m_pEventQuery);
	}
}

void WDirect3D8::ReleaseEventQuery() {
	TRACE("Called");

	if (this->m_pEventQuery) {
		this->m_pEventQuery->Release();
		this->m_pEventQuery = nullptr;
	}
}

void WDirect3D8::XInstantiateTexture(int hTex) {
	TRACE("hTex=%d", hTex);

	uint32_t mipLevels;
	D3DSURFACE_DESC topLvlD3dsd;
	IDirect3DSurface9 *srcSurf, *dstSurf;

	if (this->m_texList[hTex].pixFmtInfo) {
		this->m_pd3dDevice->CreateTexture(
			this->m_texList[hTex].width, this->m_texList[hTex].height,
			this->m_texList[hTex].mipmaplevel, 0,
			this->m_texList[hTex].pixFmtInfo->pixFmt, D3DPOOL_MANAGED,
			&this->m_texList[hTex].pTex, nullptr);
	} else if (this->m_ddsRes) {
		LPDIRECT3DTEXTURE9 pTex = nullptr;
		mipLevels = this->m_bUseMipmap == 0 ? this->m_ddsRes + 1 : this->m_ddsRes + this->m_iMaxMipLvl;
		D3DXCreateTextureFromFileInMemoryEx(
			this->m_pd3dDevice, this->m_texList[hTex].dxtcData,
			this->m_texList[hTex].dxtcDataSize, -1, -1, mipLevels, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 1,
			this->m_dwMipCreateFilter, 0, nullptr, nullptr, &pTex);
		if (!pTex) {
			return;
		}
		mipLevels = pTex->GetLevelCount();
		if (this->m_ddsRes + 1 > mipLevels) {
			this->m_ddsRes = mipLevels - 1;
		}
		unsigned int iTopSrcLvlToLoad = this->m_ddsRes;
		mipLevels = this->m_bUseMipmap == 0 ? 1 : mipLevels - this->m_ddsRes;
		pTex->GetLevelDesc(iTopSrcLvlToLoad, &topLvlD3dsd);
		D3DXCreateTexture(this->m_pd3dDevice, topLvlD3dsd.Width, topLvlD3dsd.Height, mipLevels, 0, topLvlD3dsd.Format,
		                  D3DPOOL_MANAGED, &this->m_texList[hTex].pTex);
		if (!this->m_texList[hTex].pTex) {
			pTex->Release();
			return;
		}
		for (UINT i = 0; i < mipLevels; i++) {
			if (pTex->GetSurfaceLevel(i + iTopSrcLvlToLoad, &srcSurf) < 0) {
				continue;
			}
			if (this->m_texList[hTex].pTex->GetSurfaceLevel(i, &dstSurf) >= 0) {
				D3DXLoadSurfaceFromSurface(dstSurf, nullptr, nullptr, srcSurf, nullptr, nullptr, 1, 0);
				dstSurf->Release();
			}
			srcSurf->Release();
		}
		pTex->Release();
		this->m_texList[hTex].height = topLvlD3dsd.Height;
		this->m_texList[hTex].width = topLvlD3dsd.Width;
		this->m_texList[hTex].mipmaplevel = mipLevels;
		this->m_texList[hTex].dxtcDataSize = 0;
		if (this->m_texList[hTex].dxtcData) {
			delete[] this->m_texList[hTex].dxtcData;
			this->m_texList[hTex].dxtcData = nullptr;
		}
	} else {
		mipLevels = this->m_bUseMipmap ? this->m_iMaxMipLvl : 1;
		D3DXCreateTextureFromFileInMemoryEx(
			this->m_pd3dDevice, this->m_texList[hTex].dxtcData,
			this->m_texList[hTex].dxtcDataSize, -1, -1, mipLevels, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 1,
			this->m_dwMipCreateFilter, 0, nullptr, nullptr, &this->m_texList[hTex].pTex);
		if (this->m_texList[hTex].pTex) {
			this->m_texList[hTex].pTex->GetLevelDesc(0, static_cast<D3DSURFACE_DESC*>(&topLvlD3dsd));
			this->m_texList[hTex].height = topLvlD3dsd.Height;
			this->m_texList[hTex].width = topLvlD3dsd.Width;
			this->m_texList[hTex].mipmaplevel = this->m_texList[hTex].pTex->GetLevelCount();
			this->m_texList[hTex].dxtcDataSize = 0;
			if (this->m_texList[hTex].dxtcData) {
				delete[] this->m_texList[hTex].dxtcData;
				this->m_texList[hTex].dxtcData = nullptr;
			}
		}
	}
}

void WDirect3D8::XFillVertexBuffer(int hVb) {
	TRACE("hVb=%d", hVb);

	void* pb;
	sVb8& vb = this->m_xaVbList[hVb];

	if (!vb.xpVb) {
		return;
	}

	uint32_t flags = 0;
	if (vb.xdwUsage & 0x200) {
		flags = 0x2000;
	}

	vb.xpVb->Lock(0, 0, &pb, flags);
	memcpy(pb, vb.xpVertexData, vb.xnVtxs * vb.xbStride);
	vb.xpVb->Unlock();

	vb.xbNeedToBeFilled = false;
}

void WDirect3D8::XFillIndexBuffer(int hIb) {
	TRACE("hIb=%d", hIb);

	void* pb;
	sIb8& ib = this->m_xaIbList[hIb];

	if (!ib.xpIb) {
		return;
	}

	uint32_t flags = 0;
	if (ib.xdwUsage & 0x200) {
		flags = 0x2000;
	}

	ib.xpIb->Lock(0, 0, &pb, flags);
	memcpy(pb, ib.xpIndexData, ib.xnIdxs * 2);
	ib.xpIb->Unlock();

	ib.xbNeedToBeFilled = false;
}

void WDirect3D8::Paint() {
	TRACE("Called");

	if (this->m_devState != WDeviceStateLost) {
		this->Present();
	}
}

bool WDirect3D8::IsTextureFilled(int texHandle) {
	TRACE("texHandle=%d", texHandle);

	return this->m_texList[texHandle].isFilled;
}

int WDirect3D8::GetTextureWidth(int hTex) {
	TRACE("hTex=%d", hTex);

	return this->m_texList[hTex].width;
}

int WDirect3D8::GetTextureHeight(int hTex) {
	TRACE("hTex=%d", hTex);

	return this->m_texList[hTex].height;
}

void WDirect3D8::SetRenderTargetSizeInfo(int hTex, const WRenderToTextureSizeInfo& sizeInfo) {
	TRACE("hTex=%d, sizeInfo.m_isAbsolute=%d, sizeInfo.m_width=%d, sizeInfo.m_height=%d", hTex, sizeInfo.isAbsolute,
	      sizeInfo.width, sizeInfo.height);

	this->m_texList[hTex].renderTargetSizeInfo = sizeInfo;
}

int WDirect3D8::LockVb(const void* vPtr, unsigned vNum, unsigned vPitch, int offset) {
	TRACE("vPtr=%p, vNum=%d, vPitch=%d, offset=%d", vPtr, vNum, vPitch, offset);

	void* pBuffer;

	if (!this->m_pLockableVb) {
		uint32_t dwUsage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		this->m_pd3dDevice->CreateVertexBuffer(0x100000, dwUsage, 0, D3DPOOL_DEFAULT, &this->m_pLockableVb, nullptr);
		this->m_pd3dDevice->CreateIndexBuffer(0x80000, dwUsage, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &this->m_pLockableIb,
		                                      nullptr);
		this->m_iVbOffset = 0x100000;
		this->m_iIbOffset = 0x80000;
	}

	if (!this->m_pLockableVb || !this->m_pLockableIb) {
		return -1;
	}

	uint32_t dwLockFlags = 4096;
	uint32_t dwPitchOff = (vPitch - this->m_iVbOffset % vPitch) % vPitch;
	uint32_t dwTotal = vPitch * (vNum + offset) + this->m_iVbOffset + dwPitchOff;
	unsigned int padBytes = (vPitch - this->m_iVbOffset % vPitch) % vPitch;
	if (dwTotal > 0x100000 || dwTotal / vPitch >= this->m_d3dCaps.MaxVertexIndex || dwTotal / vPitch >= 0x10000) {
		dwLockFlags = 0x2000;
		this->m_iVbOffset = 0;
		padBytes = 0;
		dwPitchOff = 0;
	}

	if (this->m_pLockableVb->Lock(dwPitchOff + this->m_iVbOffset + offset * vPitch, vPitch * vNum, &pBuffer,
	                              dwLockFlags) !=
		S_OK) {
		return -1;
	}

	memcpy(pBuffer, vPtr, vPitch * vNum);
	this->m_pLockableVb->Unlock();
	unsigned result = (this->m_iVbOffset + padBytes) / vPitch;
	this->m_iVbOffset = padBytes + vPitch * (vNum + offset) + this->m_iVbOffset;

	return result;
}

int WDirect3D8::LockIb(const uint16_t* pvIb, int iNum) {
	TRACE("pvIb=%p, iNum=%d", pvIb, iNum);

	void* pBuffer;
	int len = 2 * iNum;
	int lockFlags = 0x1000;
	if (len + this->m_iIbOffset > 0x80000) {
		lockFlags = 0x2000;
		this->m_iIbOffset = 0;
	}
	if (this->m_pLockableIb->Lock(this->m_iIbOffset, len, &pBuffer, lockFlags) != S_OK) {
		return -1;
	}
	memcpy(pBuffer, pvIb, len);
	this->m_pLockableIb->Unlock();
	int result = this->m_iIbOffset / 2;
	this->m_iIbOffset += len;
	return result;
}

void __fastcall WDirect3D8::FillVertex(uint8_t* dest, const uint8_t* src, unsigned int xLastVertexDecl,
                                       size_t numVertices, unsigned int vertexStreamZeroStride, const D3DXMATRIX* m) {
	TRACE("dest=%p, src=%p, xLastVertexDecl=%08x, NumVertices=%d, VertexStreamZeroStride=%d, m=%p", dest, src,
	      xLastVertexDecl, numVertices, vertexStreamZeroStride, m);

	auto* pbSrc = PBYTE(src);
	auto* pbDest = PBYTE(dest);

	switch (xLastVertexDecl & D3DFVF_POSITION_MASK) {
		case 0x0002: {
			D3DXMATRIXA16 mat = *m;
			__m128 m11 = _mm_load_ps(&mat.m[0][0]);
			__m128 m21 = _mm_load_ps(&mat.m[1][0]);
			__m128 m31 = _mm_load_ps(&mat.m[2][0]);
			__m128 m41 = _mm_load_ps(&mat.m[3][0]);
			if ((xLastVertexDecl & 0x10) != 0) {
				for (size_t i = 0; i < numVertices; i++) {
					auto* pfSrc = reinterpret_cast<float*>(pbSrc);
					auto* pfDest = reinterpret_cast<float*>(pbDest);
					__m128 v1 = _mm_add_ps(
						_mm_add_ps(_mm_add_ps(m41, _mm_mul_ps(_mm_set_ps1(pfSrc[0]), m11)),
						           _mm_mul_ps(_mm_set_ps1(pfSrc[1]), m21)), _mm_mul_ps(_mm_set_ps1(pfSrc[2]), m31));
					__m128 v2 = _mm_add_ps(
						_mm_add_ps(_mm_mul_ps(_mm_set_ps1(pfSrc[3]), m11), _mm_mul_ps(_mm_set_ps1(pfSrc[4]), m21)),
						_mm_mul_ps(_mm_set_ps1(pfSrc[5]), m31));
					pfDest[0] = v1.m128_f32[0];
					pfDest[1] = v1.m128_f32[1];
					pfDest[2] = v1.m128_f32[2];
					pfDest[3] = v2.m128_f32[0];
					pfDest[4] = v2.m128_f32[1];
					pfDest[5] = v2.m128_f32[2];
					memcpy(&pfDest[6], &pfSrc[6], vertexStreamZeroStride - 24);
					pbDest += vertexStreamZeroStride;
					pbSrc += vertexStreamZeroStride;
				}
			} else {
				for (size_t i = 0; i < numVertices; i++) {
					auto* pfSrc = reinterpret_cast<float*>(pbSrc);
					auto* pfDest = reinterpret_cast<float*>(pbDest);
					__m128 v1 = _mm_add_ps(
						_mm_add_ps(_mm_add_ps(m41, _mm_mul_ps(_mm_set_ps1(pfSrc[0]), m11)),
						           _mm_mul_ps(_mm_set_ps1(pfSrc[1]), m21)), _mm_mul_ps(_mm_set_ps1(pfSrc[2]), m31));
					pfDest[0] = v1.m128_f32[0];
					pfDest[1] = v1.m128_f32[1];
					pfDest[2] = v1.m128_f32[2];
					memcpy(&pfDest[3], &pfSrc[3], vertexStreamZeroStride - 12);
					pbDest += vertexStreamZeroStride;
					pbSrc += vertexStreamZeroStride;
				}
			}
			break;
		}
		case 0x0004:
			memcpy(dest, src, vertexStreamZeroStride * numVertices);
			break;
		case 0x0006:
			if ((xLastVertexDecl & 0x10) != 0) {
				unsigned int destStride = vertexStreamZeroStride - 4;
				for (size_t i = 0; i < numVertices; i++) {
					auto* pfSrc = PFLOAT(pbSrc);
					auto* pfDest = PFLOAT(pbDest);
					D3DXVec3TransformCoord(reinterpret_cast<D3DXVECTOR3*>(&pfDest[0]),
					                       reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[12]]);
					D3DXVec3TransformNormal(reinterpret_cast<D3DXVECTOR3*>(&pfDest[3]),
					                        reinterpret_cast<D3DXVECTOR3*>(&pfSrc[4]), &m[pbSrc[12]]);
					memcpy(pbDest + 24, pbSrc + 28, vertexStreamZeroStride - 28);
					pbDest += destStride;
					pbSrc += vertexStreamZeroStride;
				}
			} else {
				unsigned int destStride = vertexStreamZeroStride - 4;
				for (size_t i = 0; i < numVertices; i++) {
					D3DXVec3TransformCoord(reinterpret_cast<D3DXVECTOR3*>(pbDest),
					                       reinterpret_cast<D3DXVECTOR3*>(pbSrc), &m[pbSrc[12]]);
					memcpy(pbDest + 12, pbSrc + 16, vertexStreamZeroStride - 16);
					pbDest += destStride;
					pbSrc += vertexStreamZeroStride;
				}
			}
			break;
		case 0x0008:
			if ((xLastVertexDecl & 0x10) != 0) {
				D3DXVECTOR3 v[2];
				D3DXVECTOR3 n[2];
				unsigned int destStride = vertexStreamZeroStride - 8;
				for (size_t i = 0; i < numVertices; i++) {
					auto* pfSrc = PFLOAT(pbSrc);
					auto* pfDest = PFLOAT(pbDest);
					D3DXVec3TransformCoord(&v[0], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[16]]);
					D3DXVec3TransformNormal(&n[0], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[5]), &m[pbSrc[16]]);
					D3DXVec3TransformCoord(&v[1], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[17]]);
					D3DXVec3TransformNormal(&n[1], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[5]), &m[pbSrc[17]]);
					float f = 1.0f - pfSrc[3] - pfSrc[4];
					pfDest[0] = v[0].x * pfSrc[3] + v[1].x * f;
					pfDest[1] = v[0].y * pfSrc[3] + v[1].y * f;
					pfDest[2] = v[0].z * pfSrc[3] + v[1].z * f;
					pfDest[3] = n[0].x * pfSrc[3] + n[1].x * f;
					pfDest[4] = n[0].y * pfSrc[3] + n[1].y * f;
					pfDest[5] = n[0].z * pfSrc[3] + n[1].z * f;
					memcpy(pbDest + 24, pbSrc + 32, vertexStreamZeroStride - 32);
					pbDest += destStride;
					pbSrc += vertexStreamZeroStride;
				}
			} else {
				D3DXVECTOR3 v[2];
				unsigned int destStride = vertexStreamZeroStride - 8;
				for (size_t i = 0; i < numVertices; i++) {
					auto* pfSrc = PFLOAT(pbSrc);
					auto* pfDest = PFLOAT(pbDest);
					D3DXVec3TransformCoord(&v[0], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[16]]);
					D3DXVec3TransformCoord(&v[1], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[17]]);
					float f = 1.0f - pfSrc[3] - pfSrc[4];
					pfDest[0] = v[0].x * pfSrc[3] + v[1].x * f;
					pfDest[1] = v[0].y * pfSrc[3] + v[1].y * f;
					pfDest[2] = v[0].z * pfSrc[3] + v[1].z * f;
					memcpy(pbDest + 12, pbSrc + 20, vertexStreamZeroStride - 20);
					pbDest += destStride;
					pbSrc += vertexStreamZeroStride;
				}
			}
			break;
		case 0x000A:
			if ((xLastVertexDecl & 0x10) != 0) {
				D3DXVECTOR3 v[3];
				D3DXVECTOR3 n[3];
				unsigned int destStride = vertexStreamZeroStride - 12;
				for (size_t i = 0; i < numVertices; i++) {
					auto* pfSrc = PFLOAT(pbSrc);
					auto* pfDest = PFLOAT(pbDest);
					D3DXVec3TransformCoord(&v[0], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[20]]);
					D3DXVec3TransformNormal(&n[0], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[6]), &m[pbSrc[20]]);
					D3DXVec3TransformCoord(&v[1], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[21]]);
					D3DXVec3TransformNormal(&n[1], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[6]), &m[pbSrc[21]]);
					D3DXVec3TransformCoord(&v[2], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[22]]);
					D3DXVec3TransformNormal(&n[2], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[6]), &m[pbSrc[22]]);
					float f = 1.0f - pfSrc[3] - pfSrc[4];
					pfDest[0] = v[0].x * pfSrc[3] + v[1].x * pfSrc[4] + v[2].x * f;
					pfDest[1] = v[0].y * pfSrc[3] + v[1].y * pfSrc[4] + v[2].y * f;
					pfDest[2] = v[0].z * pfSrc[3] + v[1].z * pfSrc[4] + v[2].z * f;
					pfDest[3] = n[0].x * pfSrc[3] + n[1].x * pfSrc[4] + n[2].x * f;
					pfDest[4] = n[0].y * pfSrc[3] + n[1].y * pfSrc[4] + n[2].y * f;
					pfDest[5] = n[0].z * pfSrc[3] + n[1].z * pfSrc[4] + n[2].z * f;
					memcpy(pbDest + 24, pbSrc + 36, vertexStreamZeroStride - 36);
					pbDest += destStride;
					pbSrc += vertexStreamZeroStride;
				}
			} else {
				D3DXVECTOR3 v[3];
				unsigned int destStride = vertexStreamZeroStride - 12;
				for (size_t i = 0; i < numVertices; i++) {
					auto* pfSrc = PFLOAT(pbSrc);
					auto* pfDest = PFLOAT(pbDest);
					D3DXVec3TransformCoord(&v[0], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[20]]);
					D3DXVec3TransformCoord(&v[1], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[21]]);
					D3DXVec3TransformCoord(&v[2], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[22]]);
					float f = 1.0f - pfSrc[3] - pfSrc[4];
					pfDest[0] = v[0].x * pfSrc[3] + v[1].x * pfSrc[4] + v[2].x * f;
					pfDest[1] = v[0].y * pfSrc[3] + v[1].y * pfSrc[4] + v[2].y * f;
					pfDest[2] = v[0].z * pfSrc[3] + v[1].z * pfSrc[4] + v[2].z * f;
					memcpy(pbDest + 12, pbSrc + 24, vertexStreamZeroStride - 24);
					pbDest += destStride;
					pbSrc += vertexStreamZeroStride;
				}
			}
			break;
		case 0x000C:
			if ((xLastVertexDecl & 0x10) != 0) {
				D3DXVECTOR3 v[4];
				D3DXVECTOR3 n[4];
				unsigned int destStride = vertexStreamZeroStride - 16;
				for (size_t i = 0; i < numVertices; i++) {
					auto* pfSrc = PFLOAT(pbSrc);
					auto* pfDest = PFLOAT(pbDest);
					D3DXVec3TransformCoord(&v[0], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[24]]);
					D3DXVec3TransformNormal(&n[0], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[7]), &m[pbSrc[24]]);
					D3DXVec3TransformCoord(&v[1], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[25]]);
					D3DXVec3TransformNormal(&n[1], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[7]), &m[pbSrc[25]]);
					D3DXVec3TransformCoord(&v[2], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[26]]);
					D3DXVec3TransformNormal(&n[2], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[7]), &m[pbSrc[26]]);
					D3DXVec3TransformCoord(&v[3], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[27]]);
					D3DXVec3TransformNormal(&n[3], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[7]), &m[pbSrc[27]]);
					float f = 1.0f - pfSrc[3] - pfSrc[4] - pfSrc[5];
					pfDest[0] = v[0].x * pfSrc[3] + v[1].x * pfSrc[4] + v[2].x * pfSrc[5] + v[3].x * f;
					pfDest[1] = v[0].y * pfSrc[3] + v[1].y * pfSrc[4] + v[2].y * pfSrc[5] + v[3].y * f;
					pfDest[2] = v[0].z * pfSrc[3] + v[1].z * pfSrc[4] + v[2].z * pfSrc[5] + v[3].z * f;
					pfDest[3] = n[0].x * pfSrc[3] + n[1].x * pfSrc[4] + n[2].x * pfSrc[5] + n[3].x * f;
					pfDest[4] = n[0].y * pfSrc[3] + n[1].y * pfSrc[4] + n[2].y * pfSrc[5] + n[3].y * f;
					pfDest[5] = n[0].z * pfSrc[3] + n[1].z * pfSrc[4] + n[2].z * pfSrc[5] + n[3].z * f;
					memcpy(pbDest + 24, pbSrc + 40, vertexStreamZeroStride - 40);
					pbDest += destStride;
					pbSrc += vertexStreamZeroStride;
				}
			} else {
				D3DXVECTOR3 v[4];
				unsigned int destStride = vertexStreamZeroStride - 16;
				for (size_t i = 0; i < numVertices; i++) {
					PFLOAT pfSrc = PFLOAT(pbSrc);
					PFLOAT pfDest = PFLOAT(pbDest);
					D3DXVec3TransformCoord(&v[0], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[24]]);
					D3DXVec3TransformCoord(&v[1], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[25]]);
					D3DXVec3TransformCoord(&v[2], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[26]]);
					D3DXVec3TransformCoord(&v[3], reinterpret_cast<D3DXVECTOR3*>(&pfSrc[0]), &m[pbSrc[27]]);
					float f = 1.0f - pfSrc[3] - pfSrc[4] - pfSrc[5];
					pfDest[0] = v[0].x * pfSrc[3] + v[1].x * pfSrc[4] + v[2].x * pfSrc[5] + v[3].x * f;
					pfDest[1] = v[0].y * pfSrc[3] + v[1].y * pfSrc[4] + v[2].y * pfSrc[5] + v[3].y * f;
					pfDest[2] = v[0].z * pfSrc[3] + v[1].z * pfSrc[4] + v[2].z * pfSrc[5] + v[3].z * f;
					memcpy(pbDest + 24, pbSrc + 40, vertexStreamZeroStride - 40);
					pbDest += destStride;
					pbSrc += vertexStreamZeroStride;
				}
			}
		default:
			TRACE("FillVertex called with unexpected xLastVertexDecl=%08x", xLastVertexDecl);
			break;
	}
}

void WDirect3D8::DrawPrimitiveUp(D3DPRIMITIVETYPE type, UINT primitiveCount, const void* pVertexStreamZeroData,
                                 UINT vertexStreamZeroStride) {
	TRACE("Type=%d, PrimitiveCount=%d, pVertexStreamZeroData=%p, VertexStreamZeroStride=%d", type, primitiveCount,
	      pVertexStreamZeroData, vertexStreamZeroStride);

	unsigned int mergeIndex;
	unsigned int numVertices;

	if (this->m_devState == WDeviceStateLost) {
		return;
	}

	switch (type) {
		case D3DPT_LINELIST:
			numVertices = primitiveCount * 2;
			break;
		case D3DPT_LINESTRIP:
			numVertices = primitiveCount + 1;
			break;
		case D3DPT_TRIANGLELIST:
			numVertices = primitiveCount * 3;
			break;
		case D3DPT_TRIANGLESTRIP:
		case D3DPT_TRIANGLEFAN:
			numVertices = primitiveCount + 2;
			break;
		default:
			TRACE("Unexpected type=%d", type);
			return;
	}

	if (this->m_mergeBuffer.primitiveCount) {
		D3DPRIMITIVETYPE mergeType = this->m_mergeBuffer.type;
		bool canMerge = false;
		if (mergeType == D3DPT_LINELIST || mergeType == D3DPT_LINESTRIP) {
			if (type == D3DPT_LINESTRIP) {
				canMerge = true;
			}
		} else {
			if (mergeType == D3DPT_TRIANGLELIST || mergeType == D3DPT_TRIANGLESTRIP || mergeType == D3DPT_TRIANGLEFAN) {
				canMerge = true;
			}
			if (type == D3DPT_TRIANGLEFAN) {
				canMerge = true;
			}
		}
		if (!this->m_mergeBuffer.xhVb && !this->m_mergeBuffer.xhIb &&
			this->m_mergeBuffer.vertexStreamZeroStride == vertexStreamZeroStride &&
			vertexStreamZeroStride * (numVertices + this->m_mergeBuffer.numVertices) < this
			                                                                           ->m_mergeBuffer.
			                                                                           vertexStreamBufferSize &&
			canMerge) {
			int vtxPerPrimitive = 3;
			if (type == D3DPT_LINELIST || type == D3DPT_LINESTRIP) {
				vtxPerPrimitive = 2;
			} else if (type == D3DPT_TRIANGLELIST || type == D3DPT_TRIANGLESTRIP || type == D3DPT_TRIANGLEFAN) {
				vtxPerPrimitive = 3;
			}
			this->m_mergeBuffer.CheckAndIncreaseIndexBuffer(
				2 * vtxPerPrimitive * (primitiveCount + this->m_mergeBuffer.primitiveCount));
			if (!this->m_mergeBuffer.lockIndex) {
				this->m_mergeBuffer.primitiveCount = FillIndices(this->m_mergeBuffer.type, 0,
				                                                 this->m_mergeBuffer.numVertices,
				                                                 this->m_mergeBuffer.indexBuffer);
				this->m_mergeBuffer.lockIndex = 1;
				if (this->m_mergeBuffer.type == D3DPT_LINESTRIP) {
					this->m_mergeBuffer.type = D3DPT_LINELIST;
				} else {
					this->m_mergeBuffer.type = D3DPT_TRIANGLELIST;
				}
			}
			unsigned int mergePrimCount = this->m_mergeBuffer.primitiveCount;
			if (this->m_mergeBuffer.type == D3DPT_LINELIST) {
				mergeIndex = 2 * mergePrimCount;
			} else {
				mergeIndex = 3 * mergePrimCount;
			}
			FillVertex(
				&this->m_mergeBuffer.vertexStreamBuffer[vertexStreamZeroStride * this->m_mergeBuffer.numVertices],
				static_cast<const uint8_t*>(pVertexStreamZeroData),
				this->m_xLastVertexDecl,
				numVertices,
				vertexStreamZeroStride,
				this->m_xLastWorldMatrix
			);
			this->m_mergeBuffer.primitiveCount += FillIndices(
				type,
				this->m_mergeBuffer.numVertices,
				numVertices,
				&this->m_mergeBuffer.indexBuffer[mergeIndex]
			);
			this->m_mergeBuffer.numVertices += numVertices;
			return;
		}
		this->FlushRenderPrimitive();
	}

	this->m_mergeBuffer.type = type;
	this->m_mergeBuffer.primitiveCount = primitiveCount;
	this->m_mergeBuffer.numVertices = numVertices;
	this->m_mergeBuffer.vertexStreamZeroStride = vertexStreamZeroStride;
	memcpy(&this->m_mergeBuffer.xLastMatrix, this->m_xLastWorldMatrix, sizeof(D3DXMATRIX));
	this->m_mergeBuffer.xhVb = 0;
	this->m_mergeBuffer.xhIb = 0;
	this->m_mergeBuffer.lockVertex = 1;
	this->m_mergeBuffer.lockIndex = 0;
	if (this->m_mergeBuffer.vertexStreamBufferSize < vertexStreamZeroStride * numVertices) {
		this->m_mergeBuffer.vertexStreamBufferSize = vertexStreamZeroStride * numVertices;
		auto* buffer = static_cast<uint8_t*>(realloc(
			this->m_mergeBuffer.vertexStreamBuffer, vertexStreamZeroStride * numVertices));
		if (buffer) {
			this->m_mergeBuffer.vertexStreamBuffer = buffer;
		} else {
			throw std::exception("realloc failed");
		}
	}

	FillVertex(
		this->m_mergeBuffer.vertexStreamBuffer,
		static_cast<const uint8_t*>(pVertexStreamZeroData),
		this->m_xLastVertexDecl,
		numVertices,
		vertexStreamZeroStride,
		this->m_xLastWorldMatrix
	);
}

void WDirect3D8::DrawIndexedPrimitive(D3DPRIMITIVETYPE type, unsigned int minIndex, unsigned int numVertices,
                                      unsigned int startIndex, unsigned int primitiveCount) {
	TRACE("Type=%d, minIndex=%d, NumVertices=%d, StartIndex=%d, PrimitiveCount=%d", type, minIndex, numVertices,
	      startIndex, primitiveCount);

	if (this->m_devState == WDeviceStateLost) {
		return;
	}

	if (this->m_mergeBuffer.primitiveCount) {
		if (this->m_mergeBuffer.xhVb) {
			if (this->m_mergeBuffer.type == D3DPT_TRIANGLELIST) {
				sVb8& vb = this->m_xaVbList[this->m_xhLastVb];
				if (vb.xbStride == this->m_mergeBuffer.vertexStreamZeroStride && vb.xpVertexData && this->m_xaIbList[
						this->m_xhLastIb].xpIndexData && vb.xbStride * (numVertices + this->m_mergeBuffer.numVertices)
					<= this->m_mergeBuffer.vertexStreamBufferSize) {
					this->m_mergeBuffer.CheckAndIncreaseIndexBuffer(
						6 * (primitiveCount + this->m_mergeBuffer.primitiveCount));
					if (!this->m_mergeBuffer.lockVertex) {
						this->m_mergeBuffer.lockVertex = 1;
						this->m_mergeBuffer.lockIndex = 1;
						auto srcIndex = m_mergeBuffer.vertexStreamZeroStride * m_mergeBuffer.minIndex;
						FillVertex(
							this->m_mergeBuffer.vertexStreamBuffer,
							reinterpret_cast<const uint8_t*>(&m_xaVbList[m_mergeBuffer.xhVb].xpVertexData[srcIndex]),
							this->m_xLastVertexDecl,
							this->m_mergeBuffer.numVertices,
							this->m_mergeBuffer.vertexStreamZeroStride,
							&this->m_mergeBuffer.xLastMatrix
						);
						if (this->m_mergeBuffer.minIndex) {
							for (UINT i = 0; i < 3 * this->m_mergeBuffer.primitiveCount; i++) {
								this->m_mergeBuffer.indexBuffer[i] = this->m_xaIbList[this->m_mergeBuffer.xhIb].
									xpIndexData[i + this->m_mergeBuffer.startIndex] - this->m_mergeBuffer.minIndex;
							}
						} else {
							memcpy(this->m_mergeBuffer.indexBuffer,
							       &this->m_xaIbList[this->m_mergeBuffer.xhIb].xpIndexData[this
							                                                               ->m_mergeBuffer.startIndex],
							       6 * this->m_mergeBuffer.primitiveCount);
						}
					}
					FillVertex(
						&this->m_mergeBuffer.vertexStreamBuffer[this
						                                        ->m_mergeBuffer.
						                                        vertexStreamZeroStride * this
						                                                                 ->
						                                                                 m_mergeBuffer
						                                                                 .numVertices
						],
						reinterpret_cast<const uint8_t*>(&this->m_xaVbList[this->m_xhLastVb].xpVertexData[minIndex *
							this
							->
							m_mergeBuffer
							.
							vertexStreamZeroStride
						]),
						this->m_xLastVertexDecl,
						numVertices,
						this->m_mergeBuffer.vertexStreamZeroStride,
						this->m_xLastWorldMatrix
					);
					for (UINT i = 0, index = startIndex; i < 3 * primitiveCount; i++, index++) {
						this->m_mergeBuffer.indexBuffer[this->m_mergeBuffer.primitiveCount + i + 2 * this
						                                                                             ->m_mergeBuffer.
						                                                                             primitiveCount] =
							this->m_mergeBuffer.numVertices + this->m_xaIbList[this->m_xhLastIb].xpIndexData[index] -
							minIndex;
					}
					this->m_mergeBuffer.numVertices += numVertices;
					this->m_mergeBuffer.primitiveCount += primitiveCount;
					return;
				}
			}
		}
		this->FlushRenderPrimitive();
	}
	this->m_mergeBuffer.type = type;
	this->m_mergeBuffer.numVertices = numVertices;
	this->m_mergeBuffer.startIndex = startIndex;
	this->m_mergeBuffer.lockVertex = 0;
	this->m_mergeBuffer.lockIndex = 0;
	this->m_mergeBuffer.minIndex = minIndex;
	this->m_mergeBuffer.primitiveCount = primitiveCount;
	this->m_mergeBuffer.xhVb = this->m_xhLastVb;
	this->m_mergeBuffer.xhIb = this->m_xhLastIb;
	this->m_mergeBuffer.vertexStreamZeroStride = this->m_xaVbList[this->m_xhLastVb].xbStride;
	memcpy(&this->m_mergeBuffer.xLastMatrix, this->m_xLastWorldMatrix, sizeof(D3DXMATRIX));
	if (!this->m_xaVbList[this->m_xhLastVb].xpVertexData || !this->m_xaIbList[this->m_xhLastIb].xpIndexData || this->
		m_lastRenderState & 0x1000) {
		this->FlushRenderPrimitive();
	}
}

void WDirect3D8::_SetTexture(uint32_t stage, IDirect3DTexture9* pTexture) {
	TRACE("Stage=%d, pTexture=%p", stage, pTexture);

	if (stage < 2) {
		if (this->m_pTexture[stage] != pTexture) {
			this->FlushRenderPrimitive();
			this->m_pTexture[stage] = pTexture;
		}
	} else {
		this->FlushRenderPrimitive();
	}
	this->m_pd3dDevice->SetTexture(stage, pTexture);
}

void WDirect3D8::_SetLight(uint32_t index, const D3DLIGHT9* pLight) {
	TRACE("Index=%d, pLight=%p", index, pLight);

	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetLight(index, pLight);
}

bool WDirect3D8::LoadShCoefficient(const char* filename) {
	TRACE("filename=%s", filename);

	float fLight[9] = {0};
	float pGOut[9] = {0};
	float pBOut[9] = {0};

	IDirect3DCubeTexture9* pScratchEnvironmentMap = nullptr;
	if (D3DXCreateCubeTextureFromFileA(this->m_pd3dDevice, filename, &pScratchEnvironmentMap) < 0) {
		return false;
	}

	D3DXSHProjectCubeMap(3, pScratchEnvironmentMap, fLight, pGOut, pBOut);
	float fC0 = 1.0f / (g_sqrtPi + g_sqrtPi);
	float fC1 = 1.0f / (sqrt(3.0f) * g_sqrtPi);
	float fC2 = sqrt(15.0f) / (g_sqrtPi * 8.0f);
	float fC3 = sqrt(5.0f) / (g_sqrtPi * 16.0f);
	float fC4 = fC2 * 0.5f;
	this->m_shCoefficient[0][0] = -(fLight[3] * fC1);
	this->m_shCoefficient[0][1] = -(fLight[1] * fC1);
	this->m_shCoefficient[0][2] = fLight[2] * fC1;
	this->m_shCoefficient[0][3] = fLight[0] * fC0 - fLight[6] * fC3;
	this->m_shCoefficient[1][0] = -(pGOut[3] * fC1);
	this->m_shCoefficient[1][1] = -(pGOut[1] * fC1);
	this->m_shCoefficient[1][2] = pGOut[2] * fC1;
	this->m_shCoefficient[1][3] = pGOut[0] * fC0 - pGOut[6] * fC3;
	this->m_shCoefficient[2][0] = -(pBOut[3] * fC1);
	this->m_shCoefficient[2][1] = -(pBOut[1] * fC1);
	this->m_shCoefficient[2][2] = pBOut[2] * fC1;
	this->m_shCoefficient[2][3] = pBOut[0] * fC0 - pBOut[6] * fC3;
	this->m_shCoefficient[3][0] = fLight[4] * fC2;
	this->m_shCoefficient[3][1] = -(fLight[5] * fC2);
	this->m_shCoefficient[3][2] = fLight[6] * fC3 * 3.0f;
	this->m_shCoefficient[3][3] = -(fLight[7] * fC2);
	this->m_shCoefficient[4][0] = pGOut[4] * fC2;
	this->m_shCoefficient[4][1] = -(pGOut[5] * fC2);
	this->m_shCoefficient[4][2] = pGOut[6] * fC3 * 3.0f;
	this->m_shCoefficient[4][3] = -(pGOut[7] * fC2);
	this->m_shCoefficient[5][0] = pBOut[4] * fC2;
	this->m_shCoefficient[5][1] = -(pBOut[5] * fC2);
	this->m_shCoefficient[5][2] = pBOut[6] * fC3 * 3.0f;
	this->m_shCoefficient[5][3] = -(pBOut[7] * fC2);
	this->m_shCoefficient[6][3] = 1.0f;
	this->m_shCoefficient[6][0] = fLight[8] * fC4;
	this->m_shCoefficient[6][1] = pGOut[8] * fC4;
	this->m_shCoefficient[6][2] = pBOut[8] * fC4;
	pScratchEnvironmentMap->Release();
	return true;
}

void WDirect3D8::_SetTransform(D3DTRANSFORMSTATETYPE state, const D3DXMATRIX& matrix) {
	TRACE("Called");

	if (this->m_devState == WDeviceStateLost) {
		return;
	}

	switch (state) {
		case D3DTS_VIEW:
			if (!memcmp(&this->m_xLastViewMatrix, &matrix, sizeof(D3DXMATRIX))) {
				return;
			}
			this->FlushRenderPrimitive();
			memcpy(&this->m_xLastViewMatrix, &matrix, sizeof this->m_xLastViewMatrix);
			this->m_pd3dDevice->SetTransform(state, &matrix);
			break;

		case D3DTS_PROJECTION:
			if (!memcmp(&this->m_xLastProjMatrix, &matrix, sizeof(D3DXMATRIX))) {
				return;
			}
			this->FlushRenderPrimitive();
			memcpy(&this->m_xLastProjMatrix, &matrix, sizeof this->m_xLastProjMatrix);
			this->m_pd3dDevice->SetTransform(state, &matrix);
			break;

		default:
			if (state >= D3DTS_WORLDMATRIX(0) && state < D3DTS_WORLDMATRIX(256)) {
				if (memcmp(this->m_xLastWorldMatrix[state - 256], &matrix, sizeof(D3DXMATRIX)) != 0) {
					memcpy(this->m_xLastWorldMatrix[state - 256], &matrix, sizeof(D3DXMATRIX));
					if (!this->IsSupportVs() && state == D3DTS_WORLD) {
						this->m_pd3dDevice->SetTransform(state, &matrix);
					}
				}
			} else {
				this->m_pd3dDevice->SetTransform(state, &matrix);
			}
			break;
	}
}

void WDirect3D8::_SetRenderState(D3DRENDERSTATETYPE state, uint32_t value) {
	TRACE("State=%d, Value=%d", state, value);

	this->FlushRenderPrimitive();
	switch (state) {
		case D3DRS_FOGSTART:
			this->m_lastFogStart = reinterpret_cast<float&>(value);
			break;
		case D3DRS_FOGEND:
			this->m_lastFogEnd = reinterpret_cast<float&>(value);
			break;
		case D3DRS_TEXTUREFACTOR:
			this->m_lastTFactor = value;
			break;
		case D3DRS_LIGHTING:
			this->m_lightEnable = value != 0;
			break;
		default:
			break;
	}
	this->m_pd3dDevice->SetRenderState(state, value);
}

void WDirect3D8::_SetTextureStageState(uint32_t stage, D3DTEXTURESTAGESTATETYPE type, uint32_t value) {
	TRACE("Stage=%d, Type=%d, Value=%d", stage, type, value);
	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetTextureStageState(stage, type, value);
}

void WDirect3D8::_SetSamplerState(uint32_t sampler, D3DSAMPLERSTATETYPE type, uint32_t value) {
	TRACE("Sampler=%d, Type=%d, Value=%d", sampler, type, value);
	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetSamplerState(sampler, type, value);
}

bool WDirect3D8::SetRenderState4Flushing(int pass) {
	TRACE("pass=%d", pass);
	switch (pass) {
		case 0:
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_EQUAL);
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, 0xFF);
			if ((this->m_lastRenderState & 0x80000000) == 0) {
				g_alphaRef = 0;
				return false;
			}
			g_alphaRef = 0x80;
			return true;
		case 1:
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESS);
			return true;
		case 2:
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_LESSEQUAL);
			return true;
		default:
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, g_alphaRef);
			return true;
	}
}

int WDirect3D8::UploadCompressedTextureSurface(void* pSrcData, size_t srcDataSize, int type) {
	int hTex;
	if (*(static_cast<uint32_t*>(pSrcData) + 4) > this->m_d3dCaps.MaxTextureWidth || *(static_cast<uint32_t*>(pSrcData)
			+ 3) >
		this->m_d3dCaps.MaxTextureHeight) {
		BITMAPINFO bmpInfo;
		WDXTC dxtc;
		dxtc.Load(static_cast<uint8_t*>(pSrcData), srcDataSize);
		uint8_t* pDecompData = dxtc.GetDecompData();
		uint32_t texWidth = *(static_cast<uint32_t*>(pSrcData) + 4);
		bmpInfo.bmiHeader.biWidth = this->m_d3dCaps.MaxTextureWidth;
		if (LONG(texWidth) <= bmpInfo.bmiHeader.biWidth) {
			bmpInfo.bmiHeader.biWidth = texWidth;
		}
		uint32_t texHeight = *(static_cast<uint32_t*>(pSrcData) + 3);
		bmpInfo.bmiHeader.biHeight = this->m_d3dCaps.MaxTextureHeight;
		if (LONG(texHeight) <= bmpInfo.bmiHeader.biHeight) {
			bmpInfo.bmiHeader.biHeight = texHeight;
		}
		bmpInfo.bmiHeader.biBitCount = 32;
		hTex = this->CreateTexture(&bmpInfo, type);
		bmpInfo.bmiHeader.biHeight = *(static_cast<uint32_t*>(pSrcData) + 3);
		bmpInfo.bmiHeader.biWidth = *(static_cast<uint32_t*>(pSrcData) + 4);
		this->UpdateTextureSurface(hTex, &bmpInfo, pDecompData, type);
	} else {
		hTex = this->GetTextureNum(type >> 29 & 1);
		this->m_texList[hTex].width = *(static_cast<uint32_t*>(pSrcData) + 4) >> this->m_ddsRes;
		this->m_texList[hTex].height = *(static_cast<uint32_t*>(pSrcData) + 3) >> this->m_ddsRes;
		this->m_texList[hTex].pixFmtInfo = nullptr;
		this->m_texList[hTex].dxtcDataSize = srcDataSize;
		this->m_texList[hTex].dxtcData = new char[srcDataSize];
		memcpy(this->m_texList[hTex].dxtcData, pSrcData, srcDataSize);
		this->m_texList[hTex].updateType = 0;
		this->m_texList[hTex].needToBeFilled = false;
	}
	return hTex;
}

void WDirect3D8::UpdateCompressedTexture(int texHandle, void* pSrcData, size_t srcDataSize, int type) {
	TRACE("texHandle=%d, pSrcData=%p, srcDataSize=%d, type=%d", texHandle, pSrcData, srcDataSize, type);
	// (This function is not implemented in Wangreal either.)
}

void WDirect3D8::SetRenderTargetFormat() {
	TRACE("Called");

	this->m_rtFmtIdx = -1;
	this->m_rtNoAlphaFmtIdx = -1;
	this->m_rtLowMemFmtIdx = -1;
	this->m_rtNoAlphaLowMemFmtIdx = -1;
	this->m_rtDepthFmtIdx = -1;

	for (int i = 0; i < 4; i++) {
		if (this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL, this->m_d3dpp.BackBufferFormat, 1,
		                                    D3DRTYPE_SURFACE, m_rtFmt[i].fmt) != S_OK) {
			continue;
		}

		this->m_rtFmtIdx = i;
		break;
	}

	for (int i = 0; i < 3; i++) {
		if (this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL, this->m_d3dpp.BackBufferFormat, 1,
		                                    D3DRTYPE_SURFACE, m_rtNoAlphaFmt[i].fmt) != S_OK) {
			continue;
		}

		this->m_rtNoAlphaFmtIdx = i;
		break;
	}

	if (this->m_rtFmtIdx == -1 || this->m_rtNoAlphaFmtIdx == -1) {
		MessageBox(this->m_hWnd, TEXT("Failed to find valid formats for render target."), TEXT("yabang/wangreal"),
		           MB_OK);
		ExitProcess(1);
	}

	for (int i = 0; i < 5; i++) {
		if (FAILED(this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL, this->m_d3dpp.BackBufferFormat, 2,
			D3DRTYPE_SURFACE, m_rtDepthFmt[i].fmt))) {
			continue;
		}

		if (FAILED(this->m_d3d8->CheckDepthStencilMatch(this->m_devId, D3DDEVTYPE_HAL, this->m_d3dpp.BackBufferFormat,
			m_rtFmt[this->m_rtFmtIdx].fmt, m_rtDepthFmt[i].fmt))) {
			continue;
		}

		if (FAILED(this->m_d3d8->CheckDepthStencilMatch(this->m_devId, D3DDEVTYPE_HAL, this->m_d3dpp.BackBufferFormat,
			m_rtNoAlphaFmt[this->m_rtNoAlphaFmtIdx].fmt,
			m_rtDepthFmt[i].fmt))) {
			continue;
		}

		this->m_rtDepthFmtIdx = i;
		break;
	}

	for (int i = 0; i < 4; i++) {
		if (m_rtFmt[i].bpp > 2) {
			continue;
		}

		if (FAILED(this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL, this->m_d3dpp.BackBufferFormat, 1,
			D3DRTYPE_SURFACE, m_rtFmt[i].fmt))) {
			continue;
		}

		if (FAILED(this->m_d3d8->CheckDepthStencilMatch(this->m_devId, D3DDEVTYPE_HAL, this->m_d3dpp.BackBufferFormat,
			m_rtFmt[i].fmt, m_rtDepthFmt[this->m_rtDepthFmtIdx].fmt))) {
			continue;
		}

		this->m_rtLowMemFmtIdx = i;
		break;
	}

	for (int i = 0; i < 3; i++) {
		if (m_rtNoAlphaFmt[i].bpp > 2) {
			continue;
		}

		if (this->m_d3d8->CheckDeviceFormat(this->m_devId, D3DDEVTYPE_HAL, this->m_d3dpp.BackBufferFormat, 1,
		                                    D3DRTYPE_SURFACE, m_rtNoAlphaFmt[i].fmt) != S_OK) {
			continue;
		}

		if (this->m_d3d8->CheckDepthStencilMatch(this->m_devId, D3DDEVTYPE_HAL, this->m_d3dpp.BackBufferFormat,
		                                         m_rtNoAlphaFmt[i].fmt,
		                                         m_rtDepthFmt[this->m_rtDepthFmtIdx].fmt) != S_OK) {
			continue;
		}

		this->m_rtNoAlphaLowMemFmtIdx = i;
		break;
	}

	if (this->m_rtLowMemFmtIdx == -1) {
		this->m_rtLowMemFmtIdx = this->m_rtFmtIdx;
	}

	if (this->m_rtNoAlphaLowMemFmtIdx == -1) {
		this->m_rtNoAlphaLowMemFmtIdx = this->m_rtNoAlphaFmtIdx;
	}
}

void WDirect3D8::CreateTextureSurface(uint32_t dwWidth, uint32_t dwHeight, int iMipmapCount, int renderTargetType,
                                      int iIndex,
                                      int bChromaKey, int bitNum) {
	TRACE("dwWidth=%d, dwHeight=%d, iMipmapcount=%d, renderTargetType=%d, iIndex=%d, bChromakey=%d, bitnum=%d", dwWidth,
	      dwHeight, iMipmapCount, renderTargetType, iIndex, bChromaKey, bitNum);

	uint32_t i;
	uint32_t j;
	int k;
	D3DFORMAT format;
	uint32_t dwUsage;

	if (renderTargetType != 0 || !(this->m_d3dCaps.TextureCaps & 2)) {
		i = dwWidth;
		j = dwHeight;
	} else {
		for (i = 1; i < dwWidth; i *= 2) {}
		for (j = 1; j < dwHeight; j *= 2) {}
	}

	if (i >= this->m_d3dCaps.MaxTextureWidth) {
		i = this->m_d3dCaps.MaxTextureWidth;
	}
	if (j >= this->m_d3dCaps.MaxTextureHeight) {
		j = this->m_d3dCaps.MaxTextureHeight;
	}

	for (k = iMipmapCount; (dwWidth >> k <= 1 || dwHeight >> k <= 1) && k >= 0; --k) {}

	this->m_texList[iIndex].width = i;
	this->m_texList[iIndex].height = j;
	this->m_texList[iIndex].pixFmtInfo = this->m_fmt[bChromaKey + (bitNum <= 16 ? 0 : 3)];
	this->m_texList[iIndex].mipmaplevel = k;
	this->m_texList[iIndex].renderTargetType = renderTargetType;
	this->m_texList[iIndex].renderTargetSizeInfo.isAbsolute = false;
	this->m_texList[iIndex].renderTargetSizeInfo.width = 0;
	this->m_texList[iIndex].renderTargetSizeInfo.height = 0;
	this->m_texList[iIndex].isFilled = false;

	if (!renderTargetType) {
		return;
	}

	switch (renderTargetType & 0xE000) {
		case 0x2000:
			if (renderTargetType & 0x10000)
				format = m_rtFmt[this->m_rtLowMemFmtIdx].fmt;
			else
				format = m_rtFmt[this->m_rtFmtIdx].fmt;
			dwUsage = 1;
			break;
		case 0x4000:
			if (renderTargetType & 0x10000)
				format = m_rtNoAlphaFmt[this->m_rtNoAlphaLowMemFmtIdx].fmt;
			else
				format = m_rtNoAlphaFmt[this->m_rtNoAlphaFmtIdx].fmt;
			dwUsage = 1;
			break;
		case 0x8000:
			format = m_rtDepthFmt[this->m_rtDepthFmtIdx].fmt;
			dwUsage = 2;
			break;
		default:
			format = m_rtFmt[this->m_rtFmtIdx].fmt;
			dwUsage = 1;
			break;
	}
	this->m_pd3dDevice->CreateTexture(this->m_texList[iIndex].width, this->m_texList[iIndex].height, 1, dwUsage, format,
	                                  D3DPOOL_DEFAULT, &this->m_texList[iIndex].pTex, nullptr);
	if (this->m_texList[iIndex].pTex) {
		this->m_texList[iIndex].pTex->GetSurfaceLevel(0, &this->m_texList[iIndex].pSurf);
	}
}

RGBQUAD* WDirect3D8::GetPixelPtr(BITMAPINFO* bi, uint8_t* data, int x, int y) {
	TRACE("Called");
	if (bi->bmiHeader.biBitCount == 8) {
		return static_cast<RGBQUAD*>(&bi->bmiColors[data[y * (bi->bmiHeader.biWidth + 3 & ~3) + x]]);
	}
	return reinterpret_cast<RGBQUAD*>(&data[3 * x + y * (3 * (bi->bmiHeader.biWidth + 1) & ~3)]);
}

void WDirect3D8::UpdateTextureSurfaceNormal(void* buff, int pitch, PixInfo* pix, BITMAPINFO* bi, void* data) {
	TRACE("Called");

	int rgb, a;
	int w = bi->bmiHeader.biWidth;
	char* ptr = static_cast<char*>(buff);

	for (int y = bi->bmiHeader.biHeight; y >= 0; y--) {
		for (int x = 0; x < w; x++) {
			if (x % 3 >= 1) {
				rgb = 255 >> pix->r_r_shift << pix->r_l_shift | 255 >> pix->g_r_shift << pix->g_l_shift | 255 >>
					pix->b_r_shift << pix->b_l_shift;
				a = 0;
			} else {
				rgb = 255 >> pix->r_r_shift << pix->r_l_shift | 255 >> pix->g_r_shift << pix->g_l_shift | 255 >>
					pix->b_r_shift << pix->b_l_shift;
				a = 128;
			}
			uint32_t value = rgb | a >> pix->a_r_shift << pix->a_l_shift;
			memcpy(ptr, &value, pix->cpp);
			ptr += pix->cpp;
		}
	}
}

void WDirect3D8::UpdateTextureSurfaceDirect(void* buff, int pitch, PixInfo* pix, BITMAPINFO* bi, void* data) {
	uint32_t cps = bi->bmiHeader.biBitCount >> 3;
	auto* dataPtr = static_cast<uint8_t*>(data);
	auto* buffPtr = static_cast<uint8_t*>(buff);
	uint32_t nCps = (cps * bi->bmiHeader.biWidth + 3 & ~3) - cps * bi->bmiHeader.biWidth;
	uint32_t nPitch = pitch - bi->bmiHeader.biWidth * pix->cpp;

	if (bi->bmiHeader.biBitCount == 8) {
		for (int y = 0; y < bi->bmiHeader.biHeight; y++) {
			for (int x = 0; x < bi->bmiHeader.biWidth; x++) {
				BYTE index = *dataPtr;
				uint32_t color;
				if (pix->a_r_shift == 8) {
					color = bi->bmiColors[index].rgbBlue >> pix->b_r_shift << pix->b_l_shift |
						bi->bmiColors[index].rgbGreen >> pix->g_r_shift << pix->g_l_shift |
						bi->bmiColors[index].rgbRed >> pix->r_r_shift << pix->r_l_shift;
				} else {
					color = bi->bmiColors[index].rgbBlue >> pix->b_r_shift << pix->b_l_shift |
						bi->bmiColors[index].rgbGreen >> pix->g_r_shift << pix->g_l_shift |
						bi->bmiColors[index].rgbRed >> pix->r_r_shift << pix->r_l_shift |
						bi->bmiColors[index].rgbReserved >> pix->a_r_shift << pix->a_l_shift;
				}
				dataPtr += cps;
				memcpy(buffPtr, &color, pix->cpp);
				buffPtr += pix->cpp;
			}
			buffPtr += nPitch;
			dataPtr += nCps;
		}
	} else if (bi->bmiHeader.biHeight > 0) {
		for (int y = 0; y < bi->bmiHeader.biHeight; y++) {
			for (int x = 0; x < bi->bmiHeader.biWidth; x++) {
				uint32_t color;
				if (pix->a_r_shift == 8) {
					color = dataPtr[0] >> LOBYTE(pix->b_r_shift) << pix->b_l_shift |
						dataPtr[1] >> LOBYTE(pix->g_r_shift) << pix->g_l_shift |
						dataPtr[2] >> LOBYTE(pix->r_r_shift) << pix->r_l_shift;
				} else {
					color = dataPtr[0] >> LOBYTE(pix->b_r_shift) << pix->b_l_shift |
						dataPtr[1] >> LOBYTE(pix->g_r_shift) << pix->g_l_shift |
						dataPtr[2] >> LOBYTE(pix->r_r_shift) << pix->r_l_shift |
						dataPtr[3] >> LOBYTE(pix->a_r_shift) << pix->a_l_shift;
				}
				dataPtr += cps;
				memcpy(buffPtr, &color, pix->cpp);
				buffPtr += pix->cpp;
			}
			buffPtr += nPitch;
			dataPtr += nCps;
		}
	}
}

void WDirect3D8::UpdateTextureSurfaceSampling(void* buff, int width, int height, int pitch, PixInfo* pix,
                                              BITMAPINFO* bi, void* data, int type) {
	uint32_t chromaKey = 0;
	bool useChromaKey = (type & 0x800) != 0;
	if (useChromaKey) {
		chromaKey = 255 >> pix->b_r_shift << pix->b_l_shift |
			255 >> pix->a_r_shift << pix->a_l_shift;
	}
	unsigned int cps = static_cast<unsigned int>(bi->bmiHeader.biBitCount) >> 3;
	unsigned int nCps = cps * bi->bmiHeader.biWidth + 3 & ~3u;
	if (height <= 0) {
		return;
	}

	auto* pOut = static_cast<uint8_t*>(buff);
	for (unsigned int y = 0; y < static_cast<unsigned int>(height); y++) {
		unsigned int offset = bi->bmiHeader.biHeight == height
			? nCps * y
			: nCps * (y * bi->bmiHeader.biHeight / height);
		auto* pInLine = static_cast<uint8_t*>(data) + offset;
		for (unsigned int x = 0; x < static_cast<unsigned int>(width); x++) {
			unsigned int index = bi->bmiHeader.biWidth == width ? x : cps * (x * bi->bmiHeader.biWidth / width);
			RGBQUAD inVal;
			if (bi->bmiHeader.biBitCount == 8) {
				inVal = bi->bmiColors[pInLine[index]];
			} else {
				inVal = *reinterpret_cast<RGBQUAD*>(&pInLine[index]);
			}
			uint32_t outVal = 0;
			if (pix->a_r_shift == 8) {
				outVal = static_cast<unsigned int>(inVal.rgbBlue) >> pix->b_r_shift << pix->b_l_shift |
					static_cast<unsigned int>(inVal.rgbGreen) >> pix->g_r_shift << pix->g_l_shift |
					static_cast<unsigned int>(inVal.rgbRed) >> pix->r_r_shift << pix->r_l_shift;
			} else if (!useChromaKey) {
				outVal = static_cast<unsigned int>(inVal.rgbBlue) >> pix->b_r_shift << pix->b_l_shift |
					static_cast<unsigned int>(inVal.rgbGreen) >> pix->g_r_shift << pix->g_l_shift |
					static_cast<unsigned int>(inVal.rgbRed) >> pix->r_r_shift << pix->r_l_shift |
					static_cast<unsigned int>(inVal.rgbReserved) >> pix->a_r_shift << pix->a_l_shift;
			} else {
				outVal = static_cast<unsigned int>(inVal.rgbBlue) >> pix->b_r_shift << pix->b_l_shift |
					static_cast<unsigned int>(inVal.rgbGreen) >> pix->g_r_shift << pix->g_l_shift |
					static_cast<unsigned int>(inVal.rgbRed) >> pix->r_r_shift << pix->r_l_shift |
					255 >> pix->a_r_shift << pix->a_l_shift;
				if (outVal == chromaKey) {
					outVal = 0;
				}
			}
			memcpy(pOut, &outVal, pix->cpp);
			pOut += pix->cpp;
		}
	}
}

void WDirect3D8::UpdateTextureSurfaceFiltering(void* buff, int w, int h, int pitch, PixInfo* pix, BITMAPINFO* bi,
                                               uint8_t* data) {
	TRACE("Called");

	// TODO: implement
	MessageBox(nullptr, TEXT("WDirect3D8::UpdateTextureSurfaceFiltering not implemented."),
	           TEXT("WDirect3D8::UpdateTextureSurfaceFiltering not implemented."), MB_OK);
	ExitProcess(1);
}

void WDirect3D8::UpdateTextureSurfaceAlpha(void* buff, int pitch, PixInfo* pix, const RECT& rc, BITMAPINFO* bi,
                                           void* data) {
	int nSrcBpp = bi->bmiHeader.biBitCount >> 3;
	int nDstBpp = pix->cpp;
	int nSrcPitch = bi->bmiHeader.biBitCount * bi->bmiHeader.biWidth / 8 + 3 & ~3;

	if (rc.bottom != rc.top) {
		return;
	}

	uint8_t* dataPtr = static_cast<uint8_t*>(data);
	uint8_t* buffPtr = static_cast<uint8_t*>(buff);

	for (int y = 0; y < rc.bottom - rc.top; y++) {
		uint8_t* dataLine = dataPtr;
		uint8_t* buffLine = buffPtr;

		for (int x = 0; x < rc.right - rc.left; x++) {
			BYTE pixel[4];

			// Convert into RGB.
			if (nDstBpp == 2) {
				WORD buffword = *reinterpret_cast<WORD*>(buffLine);
				pixel[0] = buffword >> pix->b_l_shift << pix->b_r_shift;
				pixel[1] = buffword >> pix->g_l_shift << pix->g_r_shift;
				pixel[2] = buffword >> pix->r_l_shift << pix->r_r_shift;
			} else {
				pixel[0] = buffLine[0];
				pixel[1] = buffLine[1];
				pixel[2] = buffLine[2];
			}

			// Premultiply alpha.
			BYTE mult[4];
			for (int i = 0; i < 3; i++) {
				mult[i] = (dataLine[3] * dataLine[i] + (255 - dataLine[3]) * pixel[i]) / 255;
			}

			// Convert into destination format.
			uint32_t code;
			if (pix->a_r_shift == 8) {
				code = mult[0] >> pix->b_r_shift << pix->b_l_shift |
					mult[1] >> pix->g_r_shift << pix->g_l_shift |
					mult[2] >> pix->r_r_shift << pix->r_l_shift;
			} else {
				code = mult[0] >> pix->b_r_shift << pix->b_l_shift |
					mult[1] >> pix->g_r_shift << pix->g_l_shift |
					mult[2] >> pix->r_r_shift << pix->r_l_shift |
					dataLine[3] >> pix->a_r_shift << pix->a_l_shift;
			}

			memcpy(buffLine, &code, nDstBpp);

			dataLine += nSrcBpp;
			buffLine += nDstBpp;
		}

		buffPtr += pitch;
		dataPtr += nSrcPitch;
	}
}

void WDirect3D8::UpdateTextureSurface(void* buff, int width, int height, int pitch, PixInfo* pix, BITMAPINFO* bi,
                                      void* data, int type) {
	if (type & 0x20000) {
		this->UpdateTextureSurfaceNormal(buff, pitch, pix, bi, data);
		return;
	}
	if (!(type & 0x800)) {
		if (type & 0x80000) {
			if (width >= bi->bmiHeader.biWidth || height >= bi->bmiHeader.biHeight) {
				this->UpdateTextureSurfaceDirect(buff, pitch, pix, bi, data);
				return;
			}
		} else if (width == bi->bmiHeader.biWidth && height == bi->bmiHeader.biHeight) {
			this->UpdateTextureSurfaceDirect(buff, pitch, pix, bi, data);
			return;
		}
	}
	if (type & 0x800 || bi->bmiHeader.biBitCount != 8 && bi->bmiHeader.biBitCount != 24) {
		this->UpdateTextureSurfaceSampling(buff, width, height, pitch, pix, bi, data, type);
	} else {
		this->UpdateTextureSurfaceFiltering(buff, width, height, pitch, pix, bi, static_cast<uint8_t*>(data));
	}
}

void WDirect3D8::UpdateTextureSurface(int texHandle, BITMAPINFO* bi, void* data, int type) {
	D3D8Texture& tex = this->m_texList[texHandle];
	if (tex.bitmap) {
		if (bi->bmiHeader.biWidth == tex.bitmap->m_bi->bmiHeader.biWidth) {
			if (bi->bmiHeader.biHeight == tex.bitmap->m_bi->bmiHeader.biHeight && bi->bmiHeader.biBitCount == tex
			                                                                                                  .bitmap->
			                                                                                                  m_bi
			                                                                                                  ->
			                                                                                                  bmiHeader.
			                                                                                                  biBitCount
			) {
				memcpy(tex.bitmap->m_vram, data, tex.bitmap->m_bi->bmiHeader.biHeight * tex.bitmap->m_pitch);
			}
		}
	} else {
		Bitmap bitmap;
		bitmap.SetBitmapinfo(bi, static_cast<uint8_t*>(data));
		tex.bitmap = new Bitmap();
		*tex.bitmap = bitmap;
	}
	tex.updateType = type;
	tex.needToBeFilled = true;
}

void WDirect3D8::DestroyTexture(int texHandle) {
	TRACE("texHandle=%d", texHandle);

	if (texHandle >= 2048) {
		TRACE("Invalid texture handle texHandle=%d", texHandle);
		return;
	}

	if (texHandle == (this->m_lastTexState & 0x7FF) || texHandle == (this->m_lastTexState >> 11 & 0x7F)) {
		this->FlushRenderPrimitive();
	}

	if (this->m_texList[texHandle].pSurf) {
		this->m_texList[texHandle].pSurf->Release();
		this->m_texList[texHandle].pSurf = nullptr;
	}

	if (this->m_texList[texHandle].pTex) {
		this->m_texList[texHandle].pTex->Release();
		this->m_texList[texHandle].pTex = nullptr;
	}

	if (this->m_texList[texHandle].bitmap) {
		delete this->m_texList[texHandle].bitmap;
		this->m_texList[texHandle].bitmap = nullptr;
	}

	if (this->m_texList[texHandle].dxtcData) {
		delete[] this->m_texList[texHandle].dxtcData;
		this->m_texList[texHandle].dxtcData = nullptr;
	}

	memset(&this->m_texList[texHandle], 0, sizeof(D3D8Texture));
	WDirect3D::DestroyTexture(texHandle);
}

void WDirect3D8::FixTexturePart(int texHandle, const RECT& rc, BITMAPINFO* src, void* data, int type) {
	D3D8Texture& tex = this->m_texList[texHandle];

	if (!tex.bitmap) {
		return;
	}

	int cps = tex.bitmap->m_bi->bmiHeader.biBitCount >> 3;
	int8_t* vramptr = (int8_t*)&tex.bitmap->m_vram[cps * rc.left + tex.bitmap->m_pitch * rc.top];
	if (type & 0x40000) {
		if (src->bmiHeader.biBitCount == 32 && (tex.bitmap->m_bi->bmiHeader.biBitCount == 24 || tex
		                                                                                        .bitmap->m_bi->bmiHeader
		                                                                                        .
		                                                                                        biBitCount == 32)) {
			PixInfo* pix = FindPixInfoByFormat(tex.bitmap->m_bi->bmiHeader.biBitCount == 32
				? D3DFMT_A8R8G8B8
				: D3DFMT_R8G8B8);
			this->UpdateTextureSurfaceAlpha(vramptr, tex.bitmap->m_pitch, pix, rc, src, data);
		}
	} else if (tex.bitmap->m_bi->bmiHeader.biBitCount == src->bmiHeader.biBitCount) {
		int srcPitch = cps * src->bmiHeader.biWidth + 3 & ~3;
		uint8_t* dataptr = static_cast<uint8_t*>(data);
		for (int i = 0; i < rc.bottom - rc.top; i++) {
			memcpy(&vramptr[i * tex.bitmap->m_pitch], dataptr, cps * src->bmiHeader.biWidth);
			dataptr += srcPitch;
		}
	}

	tex.updateType = type | (tex.updateType & 0x80000);
	tex.needToBeFilled = true;
}

char* WDirect3D8::DuplicateString(const char* str) const {
	TRACE("str=%s", str);
	const size_t bufLen = strlen(str) + 1;
	char* result = new char[bufLen];
	strcpy_s(result, bufLen, str);
	return result;
}

bool WDirect3D8::GetDevName(const D3DDISPLAYMODE& mode, char* pBuf, int bufLen) const {
	TRACE("mode=(%d x %d, refresh=%d, format=%d), pBuf=%p, bufLen=%d", mode.Width, mode.Height, mode.RefreshRate,
	      mode.Format, pBuf, bufLen);

	for (const auto& fmt : m_fmtList) {
		if (fmt.format == mode.Format) {
			sprintf_s(pBuf, bufLen, "w%d h%d b%d", mode.Width, mode.Height, fmt.bitNum);
			return false;
		}
	}

	return true;
}

bool WDirect3D8::GetWindowDisplayMode(int& iWidth, int& iHeight, int& iColor) {
	TRACE("Called");

	D3DDISPLAYMODE d3ddm;

	if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) != S_OK) {
		return false;
	}

	iWidth = d3ddm.Width;
	iHeight = d3ddm.Height;

	switch (d3ddm.Format) {
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
			iColor = 32;
			break;
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
			iColor = 16;
		default:
			TRACE("Unexpected format.");
			break;
	}
	return true;
}

bool WDirect3D8::IsSupportedDisplayMode(bool bWindowed, int iWidth, int iHeight, int iColor) {
	TRACE("bWindowed=%d, iWidth=%d, iHeight=%d, iColor=%d", bWindowed, iWidth, iHeight, iColor);

	D3DDISPLAYMODE d3ddm;
	char modeName[64];
	char temp[64];

	sprintf_s(modeName, 64, "w%d h%d b%d", iWidth, iHeight, iColor);
	if (bWindowed) {
		if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) != S_OK) {
			return false;
		}
		switch (d3ddm.Format) {
			case D3DFMT_A8R8G8B8:
			case D3DFMT_X8R8G8B8:
				if (iColor == 16) {
					return false;
				}
				break;
			case D3DFMT_R5G6B5:
			case D3DFMT_X1R5G5B5:
			case D3DFMT_A1R5G5B5:
				if (iColor == 32) {
					return false;
				}
			default:
				break;
		}
		return true;
	}
	D3DFORMAT fmt = D3DFMT_X8R8G8B8;
	if (iColor == 16) {
		if (this->m_d3d8->GetAdapterModeCount(this->m_devId, D3DFMT_R5G6B5) != 0) {
			fmt = D3DFMT_R5G6B5;
		} else {
			fmt = D3DFMT_X1R5G5B5;
		}
	}
	for (UINT i = 0; i < this->m_d3d8->GetAdapterModeCount(this->m_devId, fmt); i++) {
		this->m_d3d8->EnumAdapterModes(this->m_devId, fmt, i, &d3ddm);
		if (this->GetDevName(d3ddm, temp, 64)) {
			continue;
		}
		if (strcmp(modeName, temp) != 0) {
			continue;
		}
		if (d3ddm.RefreshRate < 0x3C && d3ddm.RefreshRate) {
			continue;
		}
		return true;
	}
	return false;
}

void WDirect3D8::ReleaseCopiedScreenResource() {
	TRACE("Called");
	if (this->m_pCopiedScreenSurface) {
		this->m_pCopiedScreenSurface->Release();
		this->m_pCopiedScreenSurface = nullptr;
	}
	if (this->m_hCopiedScreenTexture > 0) {
		this->DestroyTexture(this->m_hCopiedScreenTexture);
		this->m_hCopiedScreenTexture = 0;
	}
	this->m_useCopiedScreen = false;
}

IDirect3DSurface9* WDirect3D8::FindDepthSurf(WORD w, WORD h) {
	TRACE("w=%d, h=%d", w, h);
	int i;
	for (i = 0; i < 32; i++) {
		if (!this->m_depthSurfList[i].pDepth) {
			break;
		}
		if (w == this->m_depthSurfList[i].wWidth && h == this->m_depthSurfList[i].wHeight) {
			return this->m_depthSurfList[i].pDepth;
		}
	}
	if (i < 32) {
		this->m_depthSurfList[i].wWidth = w;
		this->m_depthSurfList[i].wHeight = h;
		this->m_pd3dDevice->CreateDepthStencilSurface(w, h, m_rtDepthFmt[this->m_rtDepthFmtIdx].fmt,
		                                              D3DMULTISAMPLE_NONE, 0, 1, &this->m_depthSurfList[i].pDepth,
		                                              nullptr);
		return this->m_depthSurfList[i].pDepth;
	}
	if (this->m_commonDepthSurf) {
		this->m_commonDepthSurf->Release();
	}
	this->m_pd3dDevice->CreateDepthStencilSurface(w, h, m_rtDepthFmt[this->m_rtDepthFmtIdx].fmt, D3DMULTISAMPLE_NONE,
	                                              0, 1, &this->m_commonDepthSurf, nullptr);
	return this->m_commonDepthSurf;
}

uint32_t* WDirect3D8::GetZBufferHistogram() {
	TRACE("Called");
	// TODO: implement
	MessageBox(nullptr, TEXT("WDirect3D8::GetZBufferHistogram not implemented."),
	           TEXT("WDirect3D8::GetZBufferHistogram not implemented."), MB_OK);
	ExitProcess(1);
}

HRESULT WDirect3D8::ScreenShot(const char* pDestFilename, D3DXIMAGE_FILEFORMAT format) const {
	TRACE("pDestFilename=%s, format=%d", pDestFilename, format);
	D3DDISPLAYMODE mode;
	IDirect3DSurface9* surf;

	HRESULT result = this->m_pd3dDevice->GetDisplayMode(0, &mode);
	if (FAILED(result)) {
		return result;
	}

	result = this->m_pd3dDevice->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
	                                                         &surf, nullptr);
	if (FAILED(result)) {
		return result;
	}

	result = this->m_pd3dDevice->GetFrontBufferData(0, surf);
	if (SUCCEEDED(result)) {
		return D3DXSaveSurfaceToFileA(pDestFilename, format, surf, nullptr, nullptr);
	}

	surf->Release();
	return result;
}

bool WDirect3D8::CaptureScreen(Bitmap* bitmap) {
	TRACE("Called");
	// TODO: implement
	MessageBox(nullptr, TEXT("WDirect3D8::CaptureScreen not implemented."),
	           TEXT("WDirect3D8::CaptureScreen not implemented."), MB_OK);
	ExitProcess(1);
}

void WDirect3D8::XGetCaps(uint32_t iItem, uint32_t* pdwCaps) {
	TRACE("iItem=%d, pdwCaps=%p", iItem, pdwCaps);
	switch (iItem) {
		case 0:
			*pdwCaps = this->m_d3dCaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION && this->m_d3dCaps.DevCaps &
				D3DDEVCAPS_HWTRANSFORMANDLIGHT;
			break;
		case 1:
			*pdwCaps = 1;
			break;
		case 2:
			*pdwCaps = (this->m_d3dCaps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) != 0;
			break;
		case 3:
			pdwCaps[0] = this->m_d3dCaps.MaxTextureWidth;
			pdwCaps[1] = this->m_d3dCaps.MaxTextureHeight;
			break;
		case 4:
			*pdwCaps = this->m_d3dCaps.MaxTextureBlendStages;
			break;
		case 5:
			*pdwCaps = this->m_d3dCaps.MaxSimultaneousTextures;
			break;
		case 6:
			*pdwCaps = this->m_d3dCaps.MaxVertexBlendMatrices;
			break;
		case 7:
			*pdwCaps = this->m_d3dCaps.MaxVertexBlendMatrixIndex;
			break;
		case 8:
			*pdwCaps = reinterpret_cast<uint32_t&>(this->m_d3dCaps.MaxPointSize);
			break;
		case 9:
			*pdwCaps = this->m_d3dCaps.VertexShaderVersion;
			break;
		case 10:
			*pdwCaps = this->m_d3dCaps.PixelShaderVersion;
			break;
		default:
			TRACE("Unexpected iItem=%d", iItem);
			break;
	}
}

uint32_t WDirect3D8::XDetermineFvf(int iDrawFlag, int iDrawFlag2, int iMaxBoneNum) {
	TRACE("%d, %d, %d", iDrawFlag, iDrawFlag2, iMaxBoneNum);

	uint32_t result;
	if (iDrawFlag2 & 0x1000) {
		switch (iMaxBoneNum) {
			case 1:
				result = D3DFVF_LASTBETA_UBYTE4 | D3DFVF_XYZB1;
				break;
			case 2:
				result = D3DFVF_LASTBETA_UBYTE4 | D3DFVF_XYZB2;
				break;
			case 3:
				result = D3DFVF_LASTBETA_UBYTE4 | D3DFVF_XYZB3;
				break;
			case 4:
				result = D3DFVF_LASTBETA_UBYTE4 | D3DFVF_XYZB4;
				break;
			default:
				result = D3DFVF_XYZ;
		}
	} else {
		result = D3DFVF_XYZ;
	}
	if (!(iDrawFlag2 & 0x14)) {
		result |= D3DFVF_NORMAL;
	}
	if (!(iDrawFlag & 0x40000000) && !(iDrawFlag2 & 0x2000)) {
		result |= D3DFVF_DIFFUSE;
	}

	if (iDrawFlag & 0x3F800) {
		result |= D3DFVF_TEX2;
	} else {
		result |= D3DFVF_TEX1;
	}

	return result;
}

uint32_t WDirect3D8::XGetFvf(int hVb) {
	TRACE("hVb=%d", hVb);

	return this->m_xaVbList[hVb].xdwFVF;
}

BYTE WDirect3D8::XGetStride(int hVb) {
	TRACE("hVb=%d", hVb);

	return this->m_xaVbList[hVb].xbStride;
}

bool WDirect3D8::XHasVertexElem(uint32_t dwFVF, WVertexElement elem) const {
	TRACE("dwFVF=%d, elem=%d", dwFVF, elem);
	switch (elem) {
		case WVertexElementPosition:
			return (dwFVF & D3DFVF_POSITION_MASK) != 0;
		case WVertexElementNormal:
			return (dwFVF & D3DFVF_NORMAL) != 0;
		case WVertexElementDiffuse:
			return (dwFVF & D3DFVF_DIFFUSE) != 0;
		case WVertexElementTexture1:
			return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX1;
		case WVertexElementTexture2:
			return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX2;
		case WVertexElementTexture3:
			return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX3;
		case WVertexElementTexture4:
			return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX4;
		case WVertexElementTexture5:
			return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX5;
		case WVertexElementTexture6:
			return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX6;
		case WVertexElementTexture7:
			return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX7;
		case WVertexElementTexture8:
			return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX8;
		default:
			return false;
	}
}

int WDirect3D8::XGetVertexElemOffset(uint32_t dwFVF, WVertexElement elem) const {
	TRACE("dwFVF=%d, elem=%d", dwFVF, elem);

	BYTE off = 0;
	if (elem == WVertexElementPosition) {
		return off;
	}
	switch (dwFVF & D3DFVF_POSITION_MASK) {
		case D3DFVF_XYZ:
			off = 12;
			break;
		case D3DFVF_XYZRHW:
		case D3DFVF_XYZB1:
			off = 16;
			break;
		case D3DFVF_XYZB2:
			off = 20;
			break;
		case D3DFVF_XYZB3:
			off = 24;
			break;
		case D3DFVF_XYZB4:
			off = 28;
			break;
		default:
			break;
	}

	if (elem == WVertexElementNormal) {
		return (dwFVF & D3DFVF_NORMAL) != 0 ? off : -1;
	}
	if ((dwFVF & D3DFVF_NORMAL) != 0) {
		off += 12;
	}

	if (elem == WVertexElementDiffuse) {
		return dwFVF & D3DFVF_DIFFUSE ? off : -1;
	}
	if (dwFVF & D3DFVF_DIFFUSE) {
		off += 4;
	}

	if (elem == WVertexElementTexture1) {
		return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX1 ? off : -1;
	}
	if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX1) {
		off += 8;
	}

	if (elem == WVertexElementTexture2) {
		return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX2 ? off : -1;
	}
	if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX2) {
		off += 8;
	}

	if (elem == WVertexElementTexture3) {
		return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX3 ? off : -1;
	}
	if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX3) {
		off += 8;
	}

	if (elem == WVertexElementTexture4) {
		return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX4 ? off : -1;
	}
	if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX4) {
		off += 8;
	}

	if (elem == WVertexElementTexture5) {
		return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX5 ? off : -1;
	}
	if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX5) {
		off += 8;
	}

	if (elem == WVertexElementTexture6) {
		return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX6 ? off : -1;
	}
	if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX6) {
		off += 8;
	}

	if (elem == WVertexElementTexture7) {
		return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX7 ? off : -1;
	}
	if ((dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX7) {
		off += 8;
	}

	if (elem == WVertexElementTexture8) {
		return (dwFVF & D3DFVF_TEXCOUNT_MASK) >= D3DFVF_TEX8 ? off : -1;
	}
	return -1;
}

int WDirect3D8::XGetBlendWeightSize(uint32_t dwFVF) const {
	TRACE("dwFVF=%d", dwFVF);

	int result = 0;
	switch (dwFVF & D3DFVF_POSITION_MASK) {
		case D3DFVF_XYZ:
		case D3DFVF_XYZRHW:
			result = 0;
			break;
		case D3DFVF_XYZB1:
			result = 4;
			break;
		case D3DFVF_XYZB2:
			result = 8;
			break;
		case D3DFVF_XYZB3:
			result = 12;
			break;
		case D3DFVF_XYZB4:
			result = 16;
			break;
		default:
			break;
	}
	if (dwFVF & D3DFVF_LASTBETA_UBYTE4) {
		result -= 4;
	}
	return result;
}

void WDirect3D8::XCreateVertexBuffer(int hVb, int numVertices, uint32_t dwFVF, uint32_t dwUsage) {
	TRACE("hVb=%d, numVertices=%d, dwFVF=%d, dwUsage=%d", hVb, numVertices, dwFVF, dwUsage);

	unsigned int vertexSize = this->VertexSize(dwFVF);
	this->m_xaVbList[hVb].xdwUsage = dwUsage;
	this->m_xaVbList[hVb].xbStride = vertexSize;
	this->m_xaVbList[hVb].xdwFVF = dwFVF;
	this->m_xaVbList[hVb].xnVtxs = numVertices;
	this->m_xaVbList[hVb].xbNeedToBeFilled = true;
	this->m_xaVbList[hVb].xpVertexData = new uint8_t[numVertices * vertexSize];
	this->m_xaVbList[hVb].xpVb = nullptr;
}

void WDirect3D8::XCreateIndexBuffer(int hIb, int numIndices, uint32_t dwUsage) {
	TRACE("hIb=%d, numIndices=%d, dwUsage=%d", hIb, numIndices, dwUsage);

	this->m_xaIbList[hIb].xdwUsage = dwUsage;
	this->m_xaIbList[hIb].xnIdxs = numIndices;
	this->m_xaIbList[hIb].xbNeedToBeFilled = true;
	this->m_xaIbList[hIb].xpIndexData = new WORD[numIndices];
	this->m_xaIbList[hIb].xpIb = nullptr;
}

uint32_t WDirect3D8::XDetermineBufferUsage(uint32_t dwFvf) {
	TRACE("dwFVF=%d", dwFvf);
	const auto maxVtxBlendMtx = this->m_d3dCaps.MaxVertexBlendMatrices;
	const auto vtxBlendMtxIndex = this->m_d3dCaps.MaxVertexBlendMatrixIndex;
	if (this->m_xdwDevBehavior & 0x80000000 && dwFvf & 0x1000 && (maxVtxBlendMtx < 4 || vtxBlendMtxIndex < 0x80)) {
		return 16;
	}
	return 0;
}

void WDirect3D8::XReleaseVertexBuffer(int hVb) {
	TRACE("hVb=%d", hVb);
	if (hVb <= 0) {
		return;
	}
	if (hVb == this->m_xhLastVb) {
		this->FlushRenderPrimitive();
	}
	this->m_xiVbSize -= this->m_xaVbList[hVb].xnVtxs * this->m_xaVbList[hVb].xbStride;
	if (this->m_xaVbList[hVb].xpVertexData) {
		delete[] this->m_xaVbList[hVb].xpVertexData;
		this->m_xaVbList[hVb].xpVertexData = nullptr;
	}
	if (this->m_xaVbList[hVb].xpVb) {
		this->m_xaVbList[hVb].xpVb->Release();
		this->m_xaVbList[hVb].xpVb = nullptr;
	}
	memset(&this->m_xaVbList[hVb], 0, sizeof(sVb8));
	WDirect3D::XReleaseVertexBuffer(hVb);
}

void WDirect3D8::XReleaseIndexBuffer(int hIb) {
	TRACE("hIb=%d", hIb);
	if (hIb <= 0) {
		return;
	}

	if (hIb == this->m_xhLastIb) {
		this->FlushRenderPrimitive();
	}

	if (this->m_xaIbList[hIb].xpIndexData) {
		delete[] this->m_xaIbList[hIb].xpIndexData;
		this->m_xaIbList[hIb].xpIndexData = nullptr;
	}

	if (this->m_xaIbList[hIb].xpIb) {
		this->m_xaIbList[hIb].xpIb->Release();
		this->m_xaIbList[hIb].xpIb = nullptr;
	}

	WDirect3D::XReleaseIndexBuffer(hIb);
}

char* WDirect3D8::XLockVertexBuffer(int hVb, UINT uiOffset, UINT uiSize) {
	TRACE("hVb=%d, uiOffset=%d, uiSize=%d", hVb, uiOffset, uiSize);
	this->m_xaVbList[hVb].xbNeedToBeFilled = true;
	return &reinterpret_cast<char*>(this->m_xaVbList[hVb].xpVertexData)[uiOffset];
}

void WDirect3D8::XUnlockVertexBuffer(int hVb) {
	TRACE("hVb=%d", hVb);
}

char* WDirect3D8::XLockIndexBuffer(int hIb, UINT uiOffset, UINT uiSize) {
	TRACE("hIb=%d, uiOffset=%d, uiSize=%d", hIb, uiOffset, uiSize);
	this->m_xaIbList[hIb].xbNeedToBeFilled = true;
	return &reinterpret_cast<char*>(this->m_xaIbList[hIb].xpIndexData)[uiOffset];
}

void WDirect3D8::XUnlockIndexBuffer(int hIb) {
	TRACE("hIb=%d", hIb);
}

void WDirect3D8::XSetTransform(WTransformStateType state, const WMatrix4& matrix) {
	TRACE("State=%d", state);
	this->_SetTransform(static_cast<D3DTRANSFORMSTATETYPE>(state), (const D3DXMATRIX&)matrix);
}

void WDirect3D8::XSetPrevViewTransform(const WMatrix4& matrix) {
	TRACE("Called");
	memcpy(&this->m_xPrevViewMatrix, &matrix, sizeof(WMatrix4));
}

void WDirect3D8::XSetLight(uint32_t index, const LightSet& wLight) {
	TRACE("Index=%d", index);
	if (!wLight.type) {
		return;
	}

	if (wLight.type == this->m_xaWLights[index].type
		&& wLight.nearOne.x == this->m_xaWLights[index].nearOne.x
		&& wLight.nearOne.y == this->m_xaWLights[index].nearOne.y
		&& wLight.nearOne.z == this->m_xaWLights[index].nearOne.z
		&& wLight.diffuse == this->m_xaWLights[index].diffuse
		&& wLight.ambient == this->m_xaWLights[index].ambient) {
		return;
	}

	this->FlushRenderPrimitive();

	this->m_xaWLights[index].type = wLight.type;
	this->m_xaWLights[index].nearOne.x = wLight.nearOne.x;
	this->m_xaWLights[index].nearOne.y = wLight.nearOne.y;
	this->m_xaWLights[index].nearOne.z = wLight.nearOne.z;
	this->m_xaWLights[index].diffuse = wLight.diffuse;
	this->m_xaWLights[index].ambient = wLight.ambient;

	D3DLIGHT9& light = this->m_xaLights[index];
	light.Type = wLight.type == 2 ? D3DLIGHT_DIRECTIONAL : D3DLIGHT_POINT;
	light.Ambient.r = static_cast<float>(wLight.ambient >> 16 & 0xFF) / 255.f;
	light.Ambient.g = static_cast<float>(wLight.ambient >> 8 & 0xFF) / 255.f;
	light.Ambient.b = static_cast<float>(wLight.ambient >> 0 & 0xFF) / 255.f;
	light.Diffuse.r = static_cast<float>(wLight.diffuse >> 16 & 0xFF) / 255.f;
	light.Diffuse.g = static_cast<float>(wLight.diffuse >> 8 & 0xFF) / 255.f;
	light.Diffuse.b = static_cast<float>(wLight.diffuse >> 0 & 0xFF) / 255.f;

	if (wLight.type == 2) {
		light.Direction.x = wLight.nearOne.x;
		light.Direction.y = wLight.nearOne.y;
		light.Direction.z = wLight.nearOne.z;
	} else {
		light.Position.x = wLight.nearOne.x;
		light.Position.y = wLight.nearOne.y;
		light.Position.z = wLight.nearOne.z;
	}

	if (!index) {
		this->m_lightDirect.x = this->m_xaLights[0].Direction.x;
		this->m_lightDirect.y = this->m_xaLights[0].Direction.y;
		this->m_lightDirect.z = this->m_xaLights[0].Direction.z;
	}

	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetLight(index, &light);
}

void WDirect3D8::XSetRenderState(uint32_t dwFlag, uint32_t dwDiffuse) {
	TRACE("dwFlag=%d, dwDiffuse=%d", dwFlag, dwDiffuse);
	if (!this->m_xbCurHwTnL) {
		return;
	}
	if (this->m_xbUseTFactor && dwDiffuse != this->m_xdwTFactor) {
		this->m_xdwTFactor = dwDiffuse;
		this->FlushRenderPrimitive();
		this->m_lastTFactor = dwDiffuse;
		this->m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, dwDiffuse);
	} else if (dwDiffuse != this->m_xdwDiffuse) {
		this->FlushRenderPrimitive();
		this->m_xdwDiffuse = dwDiffuse;
		this->m_xMaterial.Diffuse.a = static_cast<float>(dwDiffuse >> 24 & 0xFF) / 255.f;
		this->m_xMaterial.Diffuse.r = static_cast<float>(dwDiffuse >> 16 & 0xFF) / 255.f;
		this->m_xMaterial.Diffuse.g = static_cast<float>(dwDiffuse >> 8 & 0xFF) / 255.f;
		this->m_xMaterial.Diffuse.b = static_cast<float>(dwDiffuse >> 0 & 0xFF) / 255.f;
		this->m_pd3dDevice->SetMaterial(&this->m_xMaterial);
	}
}

void WDirect3D8::SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
	TRACE("x=%d, y=%d, w=%d, h=%d", x, y, w, h);

	D3DVIEWPORT9 viewport;

	if (this->m_devState == WDeviceStateLost) {
		return;
	}

	viewport.X = x;
	viewport.Y = y;
	viewport.Width = w;
	viewport.Height = h;
	viewport.MinZ = 0.0;
	viewport.MaxZ = 1.0;

	if (!memcmp(&viewport, &this->m_curRt.viewport, sizeof(D3DVIEWPORT9))) {
		return;
	}

	this->FlushRenderPrimitive();
	this->m_curRt.viewport.X = viewport.X;
	this->m_curRt.viewport.Y = viewport.Y;
	this->m_curRt.viewport.Width = viewport.Width;
	this->m_curRt.viewport.Height = viewport.Height;
	this->m_curRt.viewport.MinZ = viewport.MinZ;
	this->m_curRt.viewport.MaxZ = viewport.MaxZ;
	this->m_pd3dDevice->SetViewport(&viewport);
}

void WDirect3D8::BackupMainRenderTarget() {
	TRACE("Called");

	this->m_pd3dDevice->GetRenderTarget(0, &this->m_mainRt.rt.surf[0]);
	this->m_mainRt.rt.surf[1] = nullptr;
	this->m_pd3dDevice->GetDepthStencilSurface(&this->m_mainRt.depthSurf);
	this->m_pd3dDevice->GetViewport(&this->m_mainRt.viewport);
}

void WDirect3D8::SetCustomFxMacro(uint32_t fxMacro) {
	TRACE("FxMacro=%d", fxMacro);
	this->m_customRenderState.m_fxMacro = fxMacro;
}

void WDirect3D8::ReleaseMainRenderTarget() {
	TRACE("Called");
	for (auto& i : this->m_mainRt.rt.surf) {
		if (i) {
			i->Release();
		}
	}
	if (this->m_mainRt.depthSurf) {
		this->m_mainRt.depthSurf->Release();
	}
	this->m_mainRt = {};
}

void WDirect3D8::SetBlendState(uint32_t srcBlend, uint32_t dstBlend) {
	TRACE("srcBlend=%d, dstBlend=%d", srcBlend, dstBlend);
	if (this->m_blendEnable && srcBlend == D3DBLEND_ONE && dstBlend == D3DBLEND_ZERO) {
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		this->m_blendEnable = false;
		return;
	}
	if (!this->m_blendEnable) {
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		this->m_blendEnable = true;
	}
	if (this->m_lastBlendState[0] != srcBlend) {
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, srcBlend);
		this->m_lastBlendState[0] = srcBlend;
	}
	if (this->m_lastBlendState[1] != dstBlend) {
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, dstBlend);
		this->m_lastBlendState[1] = dstBlend;
	}
}

void WDirect3D8::SetBlendMode(uint32_t dwState) {
	TRACE("dwState=%d", dwState);
	dwState &= 0x41800000;

	if (this->m_lastBlendMode == dwState) {
		return;
	}

	this->m_lastBlendMode = dwState;

	switch (dwState) {
		case 0x00000000:
			this->SetBlendState(D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
			break;
		case 0x40800000:
			this->SetBlendState(D3DBLEND_ONE, D3DBLEND_ONE);
			break;
		case 0x01000000:
		case 0x41000000:
			this->SetBlendState(D3DBLEND_ZERO, D3DBLEND_SRCCOLOR);
			break;
		case 0x01800000:
		case 0x41800000:
			this->SetBlendState(D3DBLEND_ZERO, D3DBLEND_INVSRCCOLOR);
			break;
		case 0x40000000:
			this->SetBlendState(D3DBLEND_ONE, D3DBLEND_ZERO);
			break;
		case 0x0800000:
			this->SetBlendState(D3DBLEND_SRCALPHA, D3DBLEND_ONE);
			break;
		default:
			TRACE("Unexpected dwState=%d", dwState);
	}
}

void WDirect3D8::SetTextureStageState(int stage, int type, uint32_t value) {
	TRACE("stage=%d, type=%d, value=%d", stage, type, value);
	if (type >= 6) {
		if (stage < this->m_maxTextureBlendStages) {
			// TODO: this is definitely not quite right.
			if (this->m_texStageState[stage][type - 6] != value) {
				this->_SetSamplerState(stage, static_cast<D3DSAMPLERSTATETYPE>(g_texStageStateList[type - 6]), value);
				this->m_texStageState[stage][type - 6] = value;
			}
		}
	} else if (stage < this->m_maxTextureBlendStages) {
		if (this->m_texStageState[stage][type] != value) {
			this->_SetTextureStageState(stage, g_texStageStateList[type], value);
			this->m_texStageState[stage][type] = value;
		}
	}
}

void WDirect3D8::SetVtxMode(uint32_t dwVertexTypeDesc) {
	TRACE("dwVertexTypeDesc=%d", dwVertexTypeDesc);
	this->m_lastVtxType = dwVertexTypeDesc & 0x7FFFFFFF;
	this->SetVertexShader(dwVertexTypeDesc & 0x3FFFFFFF);
	this->m_vtxSize = this->VertexSize(this->m_lastVtxType & 0xBFFFFFFF);
	uint32_t lastVtxType = this->m_lastVtxType & 0x7FFFFF7F;

	// TODO:
	// - Reconstruct switch
	// - (Maybe) uninline whatever might be inlined
	if (lastVtxType <= 0x1108) {
		if (lastVtxType == 4360) {
			if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
				this->m_texStageState[0][2] = 4;
			}
			if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(
					1,
					g_texStageStateList[2],
					1);
				this->m_texStageState[1][2] = 1;
			}
			if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
				return;
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
			this->m_texStageState[0][5] = 4;
			return;
		}
		if (lastVtxType <= 0x144) {
			if (lastVtxType != 324) {
				if (lastVtxType <= 0x102) {
					if (lastVtxType != 258) {
						switch (lastVtxType) {
							case 0x12u:
							case 0x42u:
							case 0x44u:
							case 0x52u:
								if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 3) {
									this->FlushRenderPrimitive();
									this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 3);
									this->m_texStageState[0][2] = 3;
								}
								if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
									this->FlushRenderPrimitive();
									this->m_pd3dDevice->SetTextureStageState(1, g_texStageStateList[2], 1);
									this->m_texStageState[1][2] = 1;
								}
								if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 3)
									return;
								this->FlushRenderPrimitive();
								this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 3);
								this->m_texStageState[0][5] = 3;
								return;
							default:
								return;
						}
					}
					if (!this->m_xbUseTFactor) {
						if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 2) {
							this->FlushRenderPrimitive();
							this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 2);
							this->m_texStageState[0][2] = 2;
						}
						if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
							this->FlushRenderPrimitive();
							this->m_pd3dDevice->SetTextureStageState(
								1,
								g_texStageStateList[2],
								1);
							this->m_texStageState[1][2] = 1;
						}
						if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
							return;
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
						this->m_texStageState[0][5] = 4;
						return;
					}
					if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
						this->m_texStageState[0][2] = 4;
					}
					if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(
							1,
							g_texStageStateList[2],
							1);
						this->m_texStageState[1][2] = 1;
					}
					if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
						return;
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
					this->m_texStageState[0][5] = 4;
					return;
				}
				if (lastVtxType != 0x104) {
					if (lastVtxType != 0x112 && lastVtxType != 0x142) {
						return;
					}
					if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
						this->m_texStageState[0][2] = 4;
					}
					if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(
							1,
							g_texStageStateList[2],
							1);
						this->m_texStageState[1][2] = 1;
					}
					if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
						return;
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
					this->m_texStageState[0][5] = 4;
					return;
				}
				if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 2) {
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 2);
					this->m_texStageState[0][2] = 2;
				}
				if (this->m_maxTextureBlendStages <= 1 || this->m_texStageState[1][2] == 1) {
					if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 2)
						return;
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 2);
					this->m_texStageState[0][5] = 2;
					return;
				}
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(1, g_texStageStateList[2], 1);
				this->m_texStageState[1][2] = 1;
				if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 2)
					return;
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 2);
				this->m_texStageState[0][5] = 2;
				return;
			}
			if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
				this->m_texStageState[0][2] = 4;
			}
			if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(
					1,
					g_texStageStateList[2],
					1);
				this->m_texStageState[1][2] = 1;
			}
			if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
				return;
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
			this->m_texStageState[0][5] = 4;
			return;
		}
		if (lastVtxType > 0x242) {
			if (lastVtxType != 580) {
				if (lastVtxType != 594) {
					if (lastVtxType != 4358)
						return;
					if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
						this->m_texStageState[0][2] = 4;
					}
					if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(
							1,
							g_texStageStateList[2],
							1);
						this->m_texStageState[1][2] = 1;
					}
					if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
						return;
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
					this->m_texStageState[0][5] = 4;
					return;
				}
			}
		} else if (lastVtxType != 578) {
			switch (lastVtxType) {
				case 0x152u:
					if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
						this->m_texStageState[0][2] = 4;
					}
					if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(
							1,
							g_texStageStateList[2],
							1);
						this->m_texStageState[1][2] = 1;
					}
					if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
						return;
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
					this->m_texStageState[0][5] = 4;
					return;
				case 0x202u:
				case 0x212u:
					if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
						this->m_texStageState[0][2] = 4;
					}
					if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 4) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(
							1,
							static_cast<D3DTEXTURESTAGESTATETYPE>(g_texStageStateList[2]),
							4);
						this->m_texStageState[1][2] = 4;
					}
					if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
						return;
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
					this->m_texStageState[0][5] = 4;
					return;
				case 0x204u:
					if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 2) {
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 2);
						this->m_texStageState[0][2] = 2;
					}
					if (this->m_maxTextureBlendStages > 1) {
						if (this->m_texStageState[1][2] != 4) {
							this->FlushRenderPrimitive();
							this->m_pd3dDevice->SetTextureStageState(
								1,
								static_cast<D3DTEXTURESTAGESTATETYPE>(g_texStageStateList[2]),
								4);
							this->m_texStageState[1][2] = 4;
						}
					}
					if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 2)
						return;
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 2);
					this->m_texStageState[0][5] = 2;
					return;
				default:
					return;
			}
		}
		if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
			this->m_texStageState[0][2] = 4;
		}
		if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 4) {
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetTextureStageState(
				1,
				static_cast<D3DTEXTURESTAGESTATETYPE>(g_texStageStateList[2]),
				4);
			this->m_texStageState[1][2] = 4;
		}
		if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
			return;
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
		this->m_texStageState[0][5] = 4;
		return;
	}
	if (lastVtxType > 0x115A) {
		if (lastVtxType > 0x40000212) {
			if (lastVtxType != 0x40000242) {
				if (lastVtxType != 0x40000240) {
					if (lastVtxType != 0x4000024E)
						return;
				}
			}
		} else if (lastVtxType != 0x40000212) {
			if (lastVtxType == 0x115c) {
				if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
					this->m_texStageState[0][2] = 4;
				}
				if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(
						1,
						g_texStageStateList[2],
						1);
					this->m_texStageState[1][2] = 1;
				}
				if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
					return;
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
				this->m_texStageState[0][5] = 4;
				return;
			}
			if (lastVtxType + 0x40000200 != 0) {
				if (lastVtxType + 0x40000200 != 2)
					return;
				if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 2) {
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 2);
					this->m_texStageState[0][2] = 2;
				}
				if (this->m_maxTextureBlendStages <= 1 || this->m_texStageState[1][2] == 7) {
					if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 2)
						return;
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 2);
					this->m_texStageState[0][5] = 2;
					return;
				}
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(1, g_texStageStateList[2], 7);
				this->m_texStageState[1][2] = 7;
				if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 2)
					return;
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 2);
				this->m_texStageState[0][5] = 2;
				return;
			}
		}
		if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
			this->m_texStageState[0][2] = 4;
		}
		if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 7) {
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetTextureStageState(
				1,
				static_cast<D3DTEXTURESTAGESTATETYPE>(g_texStageStateList[2]),
				7);
			this->m_texStageState[1][2] = 7;
		}
		if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
			return;
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
		this->m_texStageState[0][5] = 4;
		return;
	}
	if (lastVtxType == 4442) {
		if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
			this->m_texStageState[0][2] = 4;
		}
		if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetTextureStageState(
				1,
				g_texStageStateList[2],
				1);
			this->m_texStageState[1][2] = 1;
		}
		if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
			return;
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
		this->m_texStageState[0][5] = 4;
		return;
	}
	switch (lastVtxType) {
		case 0x110A:
		case 0x110C:
		case 0x1116:
		case 0x1118:
		case 0x111A:
		case 0x111C:
		case 0x1156:
		case 0x1158:
			if (this->m_maxTextureBlendStages > 0 && this->m_texStageState[0][2] != 4) {
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[2], 4);
				this->m_texStageState[0][2] = 4;
			}
			if (this->m_maxTextureBlendStages > 1 && this->m_texStageState[1][2] != 1) {
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetTextureStageState(
					1,
					g_texStageStateList[2],
					1);
				this->m_texStageState[1][2] = 1;
			}
			if (this->m_maxTextureBlendStages <= 0 || this->m_texStageState[0][5] == 4)
				return;
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[5], 4);
			this->m_texStageState[0][5] = 4;
			return;
		default:
			return;
	}
}

void WDirect3D8::SetFogState(float fogStart, float fogEnd, uint32_t fogColor) {
	TRACE("fogStart=%f, fogEnd=%f, fogColor=%06x", fogStart, fogEnd, fogColor);
	if (fogStart != this->m_fogStart) {
		this->FlushRenderPrimitive();
		this->m_lastFogStart = fogStart;
		this->m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, reinterpret_cast<uint32_t&>(fogStart));
	}
	if (fogEnd != this->m_fogEnd) {
		this->FlushRenderPrimitive();
		this->m_lastFogEnd = fogEnd;
		this->m_pd3dDevice->SetRenderState(D3DRS_FOGEND, reinterpret_cast<uint32_t&>(fogEnd));
	}
	if (fogColor != this->m_fogColor) {
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, fogColor);
	}
	WDirect3D::SetFogState(fogStart, fogEnd, fogColor);
}

void WDirect3D8::ResetFogState() {
	TRACE("Called");
	this->FlushRenderPrimitive();
	this->m_lastFogStart = this->m_fogStart;
	this->m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, reinterpret_cast<DWORD&>(this->m_fogStart));
	this->FlushRenderPrimitive();
	this->m_lastFogEnd = this->m_fogEnd;
	this->m_pd3dDevice->SetRenderState(D3DRS_FOGEND, reinterpret_cast<DWORD&>(this->m_fogEnd));
	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, this->m_fogColor);
}

void WDirect3D8::SetDefaultState() {
	TRACE("Called");
	DWORD value;

	for (auto& i : g_defRenderState) {
		this->m_pd3dDevice->GetRenderState(i.state, &value);
		if (value == i.value) {
			continue;
		}
		this->m_pd3dDevice->SetRenderState(i.state, i.value);
	}

	for (auto& i : g_defTextureState) {
		this->m_pd3dDevice->GetTextureStageState(i.stage, i.type, &value);
		if (value == i.value) {
			continue;
		}
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetTextureStageState(i.stage,
		                                         static_cast<D3DTEXTURESTAGESTATETYPE>(i.type),
		                                         i.value);
	}

	for (auto& i : g_defSamplerState) {
		this->m_pd3dDevice->GetSamplerState(i.stage, i.type, &value);
		if (value == i.value) {
			continue;
		}
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetSamplerState(i.stage, i.type,
		                                    i.value);
	}

	for (int i = 0; i < 2; i++) {
		IDirect3DBaseTexture9* surf;
		this->m_pd3dDevice->GetTexture(i, &surf);
		if (!surf) {
			continue;
		}
		this->FlushRenderPrimitive();
		this->m_pTexture[i] = nullptr;
		surf->Release();
	}

	this->m_pd3dDevice->GetRenderState(D3DRS_SRCBLEND, &this->m_lastBlendState[0]);
	this->m_pd3dDevice->GetRenderState(D3DRS_DESTBLEND, &this->m_lastBlendState[1]);
	this->m_pd3dDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &this->m_blendEnable);

	for (auto& i : this->m_texStageState) {
		for (int j = 0; j < 6; j++) {
			this->m_pd3dDevice->GetTextureStageState(0, g_texStageStateList[j], &i[j]);
		}
	}

	this->m_lastBlendMode = -1;
	this->m_lastVtxType = -1;
	this->m_xLastDrawType = 1028;
	this->m_xLastRenderState = 1028;
	this->m_xLastVertexDecl = -1;
	this->m_lastTexState = 0;
	this->m_lastRenderState = 0;
	this->m_fog = false;
	this->m_xhLastVb = 0;
	this->m_xhLastIb = 0;
	this->m_xdwLastUsage = 0;
	this->m_xdwLastFvf = 0;
	this->m_xbLastHwTnL = false;
	this->m_xbUseTFactor = false;
	this->m_xbLastVertexBlend = false;
	this->m_lastEffect = 0;
	memset(this->m_xaLights, 0, sizeof this->m_xaLights);
	memset(this->m_xaWLights, 0, sizeof this->m_xaWLights);
	memset(&this->m_xMaterial, 0, sizeof this->m_xMaterial);
	this->m_xMaterial.Diffuse.r = 1.0;
	this->m_xMaterial.Diffuse.b = 1.0;
	this->m_xMaterial.Diffuse.g = 1.0;
	this->m_xMaterial.Diffuse.a = 1.0;
	this->m_xMaterial.Ambient.b = 1.0;
	this->m_xMaterial.Ambient.g = 1.0;
	this->m_xMaterial.Ambient.r = 1.0;
	this->m_xMaterial.Ambient.a = 1.0;
	this->m_xdwDiffuse = -1;
	this->m_xdwTFactor = -1;
	this->m_lightEnable = false;
	for (auto& i : this->m_xLastWorldMatrix) {
		D3DXMatrixIdentity(&i);
	}
	D3DXMatrixIdentity(&this->m_xLastViewMatrix);
	D3DXMatrixIdentity(&this->m_xLastProjMatrix);
	D3DXMatrixIdentity(&this->m_xPrevViewMatrix);
	this->m_lastTFactor = -1;
	this->ResetFogState();
	this->m_pd3dDevice->SetMaterial(&this->m_xMaterial);
	this->m_pd3dDevice->LightEnable(0, 1);
	this->FlushRenderPrimitive();
	this->m_pd3dDevice->SetRenderState(D3DRS_NORMALIZENORMALS, 1);
	this->FlushRenderPrimitive();
	this->m_lastTFactor = -1;
	this->m_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, -1);
}

void WDirect3D8::Clear(uint32_t color, int flags, float z) {
	TRACE("color=%06x, flags=%08x, z=%f", color, flags, z);
	static int count = 0;

	struct VERTEX {
		float x;
		float y;
		float z;
		float rhw;
		float tu;
		float tv;
	};

	IDirect3DSurface9* rt;

	if (this->m_devState == WDeviceStateLost) {
		return;
	}

	if (this->m_useCopiedScreen) {
		if (g_captureOption.useMemCopy || !g_captureOption.dxCopyRectsOK) {
			IDirect3DSurface9* surface = nullptr;
			if (g_captureOption.useTexture) {
				IDirect3DTexture9* texture = this->m_texList[this->m_hCopiedScreenTexture].pTex;
				texture->GetSurfaceLevel(0, &surface);
			} else {
				surface = this->m_pCopiedScreenSurface;
			}
			if (this->m_pd3dDevice->GetRenderTarget(0, &rt) == S_OK) {
				RECT rc;
				if (g_captureOption.updateWholeScreen || !count) {
					rc.left = 0;
					rc.top = 0;
					rc.right = this->m_capturedDdsd.Width;
					rc.bottom = this->m_capturedDdsd.Height;
				} else {
					rc.left = 118;
					rc.top = this->m_capturedDdsd.Height - 128;
					rc.right = 523;
					rc.bottom = this->m_capturedDdsd.Height - 68;
				}
				D3DXLoadSurfaceFromSurface(rt, nullptr, &rc, surface, nullptr, &rc, 1, 0);
				rt->Release();
			}
			if (g_captureOption.useTexture) {
				surface->Release();
			}
		} else if (g_captureOption.useTexture) {
			VERTEX rect[4];
			float rectLeft, rectRight, rectTop, rectBottom;
			float uLeft, uRight, vTop, vBottom;
			if (g_captureOption.updateWholeScreen || !count) {
				rectLeft = -0.5f;
				rectRight = this->m_capturedDdsd.Width - 0.5f;
				rectBottom = -0.5f;
				rectTop = this->m_capturedDdsd.Height - 0.5f;
				uRight = 1.0f;
				uLeft = 0.0f;
				vBottom = 0.0f;
				vTop = 1.0f;
			} else {
				rectLeft = 117.5f;
				rectRight = 522.5f;
				rectBottom = static_cast<float>(this->m_capturedDdsd.Height - 128) - 0.5f;
				rectTop = static_cast<float>(this->m_capturedDdsd.Height - 68) - 0.5f;
				uLeft = 1.0f / this->m_capturedDdsd.Width * 118.0f;
				uRight = 1.0f / this->m_capturedDdsd.Width * 523.0f;
				vBottom = (this->m_capturedDdsd.Height - 128.0f) / this->m_capturedDdsd.Height;
				vTop = (this->m_capturedDdsd.Height - 68.0f) / this->m_capturedDdsd.Height;
			}
			rect[0].x = rectLeft;
			rect[0].y = rectTop;
			rect[0].z = 0.001f;
			rect[0].tu = uLeft;
			rect[0].tv = vTop;
			rect[0].rhw = 1.0f;
			rect[1].x = rectLeft;
			rect[1].y = rectBottom;
			rect[1].z = 0.001f;
			rect[1].rhw = 1.0f;
			rect[1].tu = uLeft;
			rect[1].tv = vBottom;
			rect[2].x = rectRight;
			rect[2].y = rectTop;
			rect[2].z = 0.001f;
			rect[2].rhw = 1.0f;
			rect[2].tu = uRight;
			rect[2].tv = vTop;
			rect[3].x = rectRight;
			rect[3].y = rectBottom;
			rect[3].z = 0.001f;
			rect[3].rhw = 1.0f;
			rect[3].tv = vBottom;
			rect[3].tu = uRight;
			this->DrawPrimitive(0x60300000 | (this->m_hCopiedScreenTexture & 0x7FF), 4, 0x104, rect,
			                    D3DPT_TRIANGLESTRIP, 1028);
		} else if (this->m_pd3dDevice->GetRenderTarget(0, &rt) == S_OK) {
			if (g_captureOption.updateWholeScreen || !count) {
				fnWD3DDevice_CopyRect(this->m_pd3dDevice, this->m_pCopiedScreenSurface, nullptr, rt);
			} else {
				RECT rc;
				rc.bottom = this->m_capturedDdsd.Height - 68;
				rc.left = 118;
				rc.right = 640 - 118 + 1;
				rc.top = this->m_capturedDdsd.Height - 128;
				fnWD3DDevice_CopyRect(this->m_pd3dDevice, this->m_pCopiedScreenSurface, &rc, rt);
			}
			rt->Release();
		}
		if (!g_captureOption.updateWholeScreen) {
			++count;
		}
	} else {
		uint32_t clearFlags = 0;
		count = 0;
		if (flags & 1) {
			clearFlags |= 1;
		}
		if (flags & 2) {
			clearFlags |= 2;
		}
		this->m_pd3dDevice->Clear(0, nullptr, clearFlags, color, z, 0);
	}
}

void WDirect3D8::XFillTexture(int hTex) {
	TRACE("hTex=%d", hTex);
	D3DLOCKED_RECT rect;

	D3D8Texture& texture = this->m_texList[hTex];
	if (texture.pTex) {
		if (texture.bitmap) {
			for (int mip = 0; mip < texture.mipmaplevel; ++mip) {
				if (texture.pTex->LockRect(mip, &rect, nullptr, 0) == S_OK) {
					this->UpdateTextureSurface(rect.pBits, texture.width >> mip, texture.height >> mip, rect.Pitch,
					                           texture.pixFmtInfo, texture.bitmap->m_bi, texture.bitmap->m_vram,
					                           texture.updateType);
					texture.pTex->UnlockRect(mip);
				}
			}
		}
		texture.needToBeFilled = false;
		texture.isFilled = true;
	}
}

void WDirect3D8::XInstantiateVertexBuffer(int hVb) {
	TRACE("hVb=%d", hVb);
	sVb8& vb = this->m_xaVbList[hVb];
	D3DPOOL pool = vb.xdwUsage & 0x200 ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
	this->m_pd3dDevice->CreateVertexBuffer(vb.xnVtxs * vb.xbStride, vb.xdwUsage, vb.xdwFVF, pool, &vb.xpVb, nullptr);
	this->XFillVertexBuffer(hVb);
	this->m_xiVbSize += vb.xnVtxs * vb.xbStride;
}

void WDirect3D8::XInstantiateIndexBuffer(int hIb) {
	TRACE("hIb=%d", hIb);
	sIb8& ib = this->m_xaIbList[hIb];
	D3DPOOL pool = ib.xdwUsage & 0x200 ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
	this->m_pd3dDevice->CreateIndexBuffer(2 * ib.xnIdxs, ib.xdwUsage, D3DFMT_INDEX16, pool, &ib.xpIb, nullptr);
	this->XFillIndexBuffer(hIb);
}

bool WDirect3D8::BeginCapturedBackground() {
	TRACE("Called");
	PixInfo* pixInfo;
	int hTex;
	HRESULT hRes;
	BITMAPINFO bi;
	D3DLOCKED_RECT lrDst;
	D3DLOCKED_RECT lrSrc;
	IDirect3DSurface9* pRenderTarget;
	IDirect3DSurface9* pSurf;

	this->FlushRenderPrimitive();
	if (FAILED(this->m_pd3dDevice->GetRenderTarget(0, &pRenderTarget))) {
		return false;
	}

	if (FAILED(pRenderTarget->GetDesc(&this->m_capturedDdsd))) {
		pRenderTarget->Release();
		return false;
	}

	if (g_captureOption.useTexture) {
		if (!this->m_hCopiedScreenTexture) {
			pixInfo = FindPixInfoByFormat(this->m_capturedDdsd.Format);
			if (!pixInfo) {
				pRenderTarget->Release();
				return false;
			}
			memset(&bi, 0, sizeof bi);
			bi.bmiHeader.biHeight = this->m_capturedDdsd.Height;
			bi.bmiHeader.biWidth = this->m_capturedDdsd.Width;
			bi.bmiHeader.biBitCount = WORD(8 * pixInfo->cpp);
			hTex = this->CreateTexture(&bi, 0x4000);
			this->m_hCopiedScreenTexture = hTex;
			if (hTex <= 0) {
				pRenderTarget->Release();
				return false;
			}
			if (!this->m_texList[hTex].pTex) {
				this->DestroyTexture(hTex);
				pRenderTarget->Release();
				return false;
			}
		}
	} else if (!this->m_pCopiedScreenSurface) {
		if (g_captureOption.useMemCopy || !g_captureOption.dxCopyRectsOK) {
			hRes = this->m_pd3dDevice->CreateOffscreenPlainSurface(this->m_capturedDdsd.Width,
			                                                       this->m_capturedDdsd.Height,
			                                                       this->m_capturedDdsd.Format, D3DPOOL_DEFAULT,
			                                                       &this->m_pCopiedScreenSurface, nullptr);
		} else {
			hRes = this->m_pd3dDevice->CreateRenderTarget(this->m_capturedDdsd.Width, this->m_capturedDdsd.Height,
			                                              this->m_capturedDdsd.Format, D3DMULTISAMPLE_NONE, 0, 0,
			                                              &this->m_pCopiedScreenSurface, nullptr);
		}
		if (hRes < 0) {
			pRenderTarget->Release();
			return false;
		}
	}
	this->m_useCopiedScreen = true;
	pSurf = nullptr;
	if (g_captureOption.useTexture) {
		this->m_texList[this->m_hCopiedScreenTexture].pTex->GetSurfaceLevel(0, &pSurf);
	} else {
		pSurf = this->m_pCopiedScreenSurface;
	}
	if (!g_captureOption.useMemCopy && g_captureOption.dxCopyRectsOK) {
		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = this->m_capturedDdsd.Width;
		rc.bottom = this->m_capturedDdsd.Height;
		hRes = fnWD3DDevice_CopyRect(this->m_pd3dDevice, pRenderTarget, &rc, pSurf);
		if (hRes == S_OK) {
			pRenderTarget->Release();
			if (g_captureOption.useTexture) {
				pSurf->Release();
			}
			return true;
		}
	}
	pRenderTarget->LockRect(&lrSrc, nullptr, 0x8810);
	pSurf->LockRect(&lrDst, nullptr, 0);
	UINT smallerPitch = lrDst.Pitch;
	if (lrSrc.Pitch <= lrDst.Pitch) {
		smallerPitch = lrSrc.Pitch;
	}
	for (UINT i = 0; i < this->m_capturedDdsd.Height; i++) {
		memcpy(static_cast<char*>(lrDst.pBits) + lrDst.Pitch * i, static_cast<char*>(lrSrc.pBits) + lrSrc.Pitch * i,
		       smallerPitch);
	}
	pRenderTarget->UnlockRect();
	pSurf->UnlockRect();
	pRenderTarget->Release();
	if (g_captureOption.useTexture) {
		pSurf->Release();
	}
	return true;
}

void WDirect3D8::XSetTnLBuffer(int hVb, int hIb) {
	TRACE("hVb=%d, hIb=%d", hVb, hIb);
	bool mustCreateBuf = false;

	if (hVb && hIb) {
		if (this->m_xhLastVb != hVb) {
			if (this->m_xdwDevBehavior & 0x80000000) {
				if (this->m_xaVbList[hVb].xdwUsage != this->m_xdwLastUsage) {
					this->m_xdwLastUsage = this->m_xaVbList[hVb].xdwUsage;
					if (this->m_xaVbList[hVb].xdwUsage ^ (this->m_xdwLastUsage & 0x10)) {
						this->m_pd3dDevice->SetSoftwareVertexProcessing(this->m_xaVbList[hVb].xdwUsage >> 4 & 1);
						mustCreateBuf = true;
					}
				}
			}
			if (!this->m_xaVbList[hVb].xpVb) {
				this->XInstantiateVertexBuffer(hVb);
			}
			if (this->m_xaVbList[hVb].xbNeedToBeFilled) {
				this->XFillVertexBuffer(hVb);
			}
			this->m_pd3dDevice->SetStreamSource(0, this->m_xaVbList[hVb].xpVb, 0, this->m_xaVbList[hVb].xbStride);
			this->m_xhLastVb = hVb;
		}
		if (this->m_xhLastIb != hIb || mustCreateBuf) {
			if (!this->m_xaIbList[hIb].xpIb) {
				this->XInstantiateIndexBuffer(hIb);
			}
			if (this->m_xaIbList[hIb].xbNeedToBeFilled) {
				this->XFillIndexBuffer(hIb);
			}
			this->m_pd3dDevice->SetIndices(this->m_xaIbList[hIb].xpIb);
			this->m_xhLastIb = hIb;
		}
	} else {
		this->m_xhLastVb = hVb;
		this->m_xhLastIb = hIb;
	}
}

void WDirect3D8::SetRenderState(uint32_t dwType, uint32_t dwTypeEx) {
	TRACE("dwType=%d, dwTypeEx=%d", dwType, dwTypeEx);
	const float fDepthBias = -0.0005f;

	uint32_t newRenderState = (dwType & 0xFFFC0000) | (dwTypeEx & 0x3FFF);
	if (this->m_lastRenderState == newRenderState) {
		return;
	}

	this->FlushRenderPrimitive();
	this->SetBlendMode(newRenderState);

	uint32_t dwXor = newRenderState ^ this->m_lastRenderState;
	if (dwXor & 0x300000) {
		this->FlushRenderPrimitive();
		if ((this->m_lastRenderState & 0x300000) == 0x200000) {
			this->m_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, 0);
		}
		switch (newRenderState & 0x300000) {
			case 0x200000:
				this->m_pd3dDevice->SetRenderState(D3DRS_DEPTHBIAS, *reinterpret_cast<const uint32_t*>(&fDepthBias));
				// fallthrough
			case 0x000000:
				this->m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
				break;
			case 0x100000:
				this->m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
				break;
			case 0x300000:
				this->m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
				break;
			default:
				TRACE("Unreachable case");
		}
	}

	if (dwXor & 0x2000000) {
		this->FlushRenderPrimitive();
		if (newRenderState & 0x2000000) {
			this->m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		} else {
			this->m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		}
	}

	if (dwXor & 0x8000000) {
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, newRenderState & 0x8000000 ? TRUE : FALSE);
	}

	if (dwXor & 0x80000) {
		this->FlushRenderPrimitive();
		if (newRenderState & 0x80000) {
			this->m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		} else {
			this->m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		}
	}

	if (dwXor & 0x20000000) {
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, newRenderState & 0x20000000 ? FALSE : TRUE);
	}

	if (dwXor & 0x80000000) {
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, newRenderState & 0x80000000 ? TRUE : FALSE);
	}

	if (dwXor & 0xC00) {
		switch (newRenderState & 0xC00) {
			case 0x400:
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
				break;
			case 0x800:
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
				break;
			default:
				this->FlushRenderPrimitive();
				this->m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
				break;
		}
	}

	if (dwXor & 0x04) {
		this->FlushRenderPrimitive();
		if (!(newRenderState & 0x04)) {
			this->m_lightEnable = true;
			this->m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		} else {
			this->m_lightEnable = false;
			this->m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		}
	} else if (dwXor & 0x10) {
		this->FlushRenderPrimitive();
		if (newRenderState & 0x10) {
			this->m_lightEnable = false;
			this->m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		} else {
			this->m_lightEnable = true;
			this->m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		}
	}

	if (dwXor & 0x2000) {
		this->FlushRenderPrimitive();
		this->m_pd3dDevice->SetRenderState(D3DRS_COLORVERTEX, ~(newRenderState >> 13) & 1);
	}

	if (dwXor & 0x2014) {
		this->m_xbUseTFactor = newRenderState & 0x14 && newRenderState & 0x2000 ? 1 : 0;
		if (this->m_maxTextureBlendStages > 0) {
			uint32_t texArg = this->m_xbUseTFactor != 0 ? D3DTA_TFACTOR : D3DTA_DIFFUSE;
			if (this->m_texStageState[0][1] != texArg) {
				this->_SetTextureStageState(0, g_texStageStateList[1], texArg);
				this->m_texStageState[0][1] = texArg;
			}
			if (this->m_maxTextureBlendStages > 0) {
				if (this->m_texStageState[0][4]) {
					this->FlushRenderPrimitive();
					this->m_pd3dDevice->SetTextureStageState(0, g_texStageStateList[4], 0);
					this->m_texStageState[0][4] = 0;
				}
			}
		}
	}

	this->m_lastRenderState = newRenderState;
}

int WDirect3D8::Init(char* modName, HWND hWnd, int iTnL) {
	TRACE("modName=%s, hwnd=%p, iTnL=%d", modName, hWnd, iTnL);
	Nv::Factory::ScreenCapeFactory* screenCapeFactory;
	Nv::IScreenCape* screenCape;
	D3DDISPLAYMODE d3ddm;
	D3DFORMAT t[3];
	char temp[64];

	//D3DPERF_SetOptions(1);

	this->m_renderCount = 0;
	this->m_d3d8 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!this->m_d3d8) {
		g_error = g_msgD3DInitFailed;
		return 1;
	}
	this->m_d3d8->GetDeviceCaps(this->m_devId, D3DDEVTYPE_HAL, &this->m_d3dCaps);
	this->m_maxTextureBlendStages = this->m_d3dCaps.MaxSimultaneousTextures;
	memset(&this->m_d3dpp, 0, sizeof this->m_d3dpp);
	if (SUCCEEDED(this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm))) {
		this->m_fmtWindowed = d3ddm.Format;
	}
	if (!strstr(modName, "Window")) {
		D3DFORMAT fmt = D3DFMT_X8R8G8B8;
		t[0] = D3DFMT_R5G6B5;
		t[1] = D3DFMT_X1R5G5B5;
		t[2] = D3DFMT_X8R8G8B8;
		for (auto& i : t) {
			if (this->m_d3d8->GetAdapterModeCount(0, i)) {
				fmt = i;
				break;
			}
		}
		if (!this->m_d3d8->GetAdapterModeCount(this->m_devId, fmt)) {
			g_error = "No graphics adapters found.";
			return 2;
		}
		int m = -1;
		for (UINT i = 0; i < this->m_d3d8->GetAdapterModeCount(this->m_devId, fmt); i++) {
			this->m_d3d8->EnumAdapterModes(this->m_devId, fmt, i, &d3ddm);
			for (int j = 4; j >= 0; j--) {
				if (m_fmtList[i].format == d3ddm.Format) {
					sprintf_s(temp, 64, "w%d h%d b%d", d3ddm.Width, d3ddm.Height, m_fmtList[i].bitNum);
					if (!strcmp(modName, temp) && (m == -1 || d3ddm.RefreshRate != 60)) {
						m = i;
					}
				}
			}
		}
		if (m == -1) {
			g_error = "Valid graphics mode not found.";
			return 2;
		}
		if (d3ddm.RefreshRate) {
			this->m_fps = static_cast<float>(d3ddm.RefreshRate);
		} else {
			this->m_fps = 120.0;
		}
		this->m_bWindow = false;
		for (int j = 4; j >= 0; j--) {
			if (m_fmtList[j].format == d3ddm.Format) {
				this->m_backBufBpp = m_fmtList[j].bitNum;
				break;
			}
		}
		if (!this->m_backBufBpp) {
			g_error = "Valid graphics mode not found.";
			return 4;
		}
		this->m_d3dpp.BackBufferHeight = d3ddm.Height;
		this->m_d3dpp.FullScreen_RefreshRateInHz = d3ddm.RefreshRate;
		this->m_d3dpp.Windowed = 0;
		this->m_d3dpp.BackBufferWidth = d3ddm.Width;
		this->m_d3dpp.BackBufferFormat = t[2];
		this->m_d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
		this->m_d3dpp.PresentationInterval = 0;
	} else {
		if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) != S_OK) {
			g_error = "Error getting current display mode.";
			return 4;
		}
		RECT clientRect;
		GetClientRect(hWnd, &clientRect);
		if (d3ddm.RefreshRate) {
			this->m_fps = static_cast<float>(d3ddm.RefreshRate);
		} else {
			this->m_fps = 120.0;
		}
		this->m_bWindow = true;
		this->m_backBufBpp = this->GetBackBufferBpp(d3ddm.Format);
		if (!this->m_backBufBpp) {
			g_error = "Valid graphics mode not found.";
			return 4;
		}
		this->m_d3dpp.BackBufferHeight = clientRect.bottom - clientRect.top;
		this->m_d3dpp.Windowed = 1;
		this->m_d3dpp.BackBufferWidth = clientRect.right - clientRect.left;
		this->m_d3dpp.BackBufferFormat = d3ddm.Format;
		this->m_d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
		this->m_d3dpp.FullScreen_RefreshRateInHz = 0;
		this->m_d3dpp.PresentationInterval = 0;
	}

	this->m_d3dpp.hDeviceWindow = hWnd;
	this->m_d3dpp.BackBufferCount = 1;
	this->m_d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	this->m_d3dpp.EnableAutoDepthStencil = TRUE;
	this->m_d3dpp.AutoDepthStencilFormat = this->FindDepthBufferFormat(this->m_d3dpp.BackBufferFormat);
	this->m_d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	this->m_hWnd = hWnd;
	this->m_lWndStyle = ~0;

	if (this->m_d3dCaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION && this->m_d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT
	) {
		this->m_xdwDevBehavior = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	} else {
		this->m_xdwDevBehavior = D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED;
	}

	if (this->m_d3d8->CreateDevice(this->m_devId, D3DDEVTYPE_HAL, hWnd, this->m_xdwDevBehavior, &this->m_d3dpp,
	                               &this->m_pd3dDevice) != S_OK) {
		g_error = "Could not create Direct3D 9 device.";
		return 5;
	}

	if (this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, nullptr) >= 0) {
		this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, &this->m_pEventQuery);
	}

	this->m_pd3dDevice->GetDeviceCaps(&this->m_d3dCaps);
	if (!this->IsSupportVs() || !this->IsSupportPs()) {
		this->m_d3dCaps.VertexShaderVersion = 0xFFFE0000;
		this->m_d3dCaps.PixelShaderVersion = 0xFFFF0000;
	}

	screenCapeFactory = new Nv::Factory::ScreenCapeFactory();
	screenCape = nullptr;
	if (screenCapeFactory->m_pProc) {
		screenCapeFactory->m_pProc(&screenCape, g_screencapeVersion);
	}
	this->m_pScreenCape = screenCape;
	this->m_pScreenCapeFactory = screenCapeFactory;
	if (screenCape) {
		screenCape->Initialize(hWnd, reinterpret_cast<void**>(&this->m_pd3dDevice), Nv::GraphicDeviceType::Direct3D9);
	}
	this->m_pd3dDevice->ShowCursor(TRUE);
	this->SetTextureFormat(this->m_d3dpp.BackBufferFormat);
	this->SetRenderTargetFormat();
	this->SetDefaultState();
	this->m_pd3dDevice->BeginScene();
	this->m_pd3dDevice->Clear(0, nullptr, 3u, 0, 0.0, 0);
	this->m_pd3dDevice->EndScene();
	this->Present();
	WDirect3D::Init();

	if (this->m_pCopiedScreenSplash) {
		delete this->m_pCopiedScreenSplash;
		this->m_pCopiedScreenSplash = nullptr;
	}

	this->m_pd3dDevice->SetVertexShader(nullptr);
	this->m_pd3dDevice->SetPixelShader(nullptr);
	this->BackupMainRenderTarget();

	memcpy(&this->m_curRt, &this->m_mainRt, sizeof this->m_curRt);

	if (this->m_mergeBuffer.vertexStreamBufferSize < 144000) {
		this->m_mergeBuffer.vertexStreamBufferSize = 144000;
		auto* buffer = static_cast<uint8_t*>(realloc(
			this->m_mergeBuffer.vertexStreamBuffer, 144000));
		if (buffer) {
			this->m_mergeBuffer.vertexStreamBuffer = buffer;
		} else {
			throw std::exception("realloc failed");
		}
	}

	if (this->m_mergeBuffer.indexBufferSize < 18000) {
		this->m_mergeBuffer.indexBufferSize = 18000;
		auto* buffer = static_cast<uint16_t*>(realloc(
			this->m_mergeBuffer.indexBuffer, 18000));
		if (buffer) {
			this->m_mergeBuffer.indexBuffer = buffer;
		} else {
			throw std::exception("realloc failed");
		}
	}

	return 0;
}

void WDirect3D8::XInstantiateAndFillTexture(int hTex) {
	TRACE("hTex=%d", hTex);

	if (!hTex) {
		return;
	}

	D3D8Texture& tex = this->m_texList[hTex];

	if (!tex.pTex) {
		this->XInstantiateTexture(hTex);
	}

	if (tex.needToBeFilled) {
		this->XFillTexture(hTex);
	}
}

int WDirect3D8::Command(WDeviceMessage message, int param1, int param2) {
	int numTextures;
	int numVb;
	int numIb;
	int devId;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	FILETIME ftAccess;
	FILETIME ftWrite;
	FILETIME ftCreate;
	SYSTEMTIME stCreate;
	FILETIME ftLocal;
	D3DADAPTER_IDENTIFIER9 identifier;
	char filename[MAX_PATH];

	switch (message) {
		case WDeviceMessageCaptureScreen:
			TRACE("message=W_VDEV_CAPTURE_SCREEN, param1=%d, param2=%d", param1, param2);
			return this->CaptureScreen((Bitmap*)param2) != 0;
		case WDeviceMessageGetFrontSurface:
			TRACE("message=W_VDEV_GET_FRONTSURFACE, param1=%d, param2=%d", param1, param2);
			return 0;
		case WDeviceMessageGetBackSurface:
			TRACE("message=W_VDEV_GET_BACKSURFACE, param1=%d, param2=%d", param1, param2);
			return 0;
		case WDeviceMessageDrawBox:
			TRACE("message=W_VDEV_DRAWBOX, param1=%d, param2=%d", param1, param2);
			return 0;
		case WDeviceMessageSetRect:
			TRACE("message=W_VDEV_SETRECT, param1=%d, param2=%d", param1, param2);
			return 0;
		case WDeviceMessageFlipFrontSurface:
			TRACE("message=W_VDEV_FLIPFRONTSURFACE, param1=%d, param2=%d", param1, param2);
			return 0;
		case WDeviceMessageOverdrawAnalyze:
			TRACE("message=W_VDEV_OVERDRAW_ANALYZE, param1=%d, param2=%d", param1, param2);
			return 0;
		case WDeviceMessageGetZBufferHistogram:
			TRACE("message=W_VDEV_GET_ZBUFF_HISTOGRAM, param1=%d, param2=%d", param1, param2);
			return 0;
		case WDeviceMessageUpdateLodTexture:
			TRACE("message=W_VDEV_UPDATE_LOD_TEXTURE, param1=%d, param2=%d", param1, param2);
			return 0;
		case WDeviceMessageGetAvailableVram:
			TRACE("message=W_VDEV_GETAVAIL_VRAM, param1=%d, param2=%d", param1, param2);
			return 0;
		case WDeviceMessageSetHWnd:
			TRACE("message=W_VDEV_SETHWND, param1=%d, param2=%d", param1, param2);
			this->m_hWnd = (HWND)param1;
			return 0;
		case WDeviceMessageFlush:
			TRACE("message=W_VDEV_FLUSH, param1=%d, param2=%d", param1, param2);
			this->Flush(param1);
			return 0;
		case WDeviceMessageGetCaptureMode:
			TRACE("message=W_VDEV_GET_CAPTURE_MODE, param1=%d, param2=%d", param1, param2);
			*(uint32_t*)param1 = g_captureOption.currentMode;
			return 0;
		case WDeviceMessageSetCaptureMode:
			TRACE("message=W_VDEV_SET_CAPTURE_MODE, param1=%d, param2=%d", param1, param2);
			g_captureOption.SetMode(static_cast<sCaptureOption::eCaptureMode>(param1), param2 == 1);
			// fallthrough
		case WDeviceMessageCaptureResource:
			if (message == WDeviceMessageCaptureResource)
				TRACE("message=W_VDEV_RELEASE_CAPTURERESOURCE, param1=%d, param2=%d", param1, param2);
			this->ReleaseCopiedScreenResource();
			return 0;
		case WDeviceMessageCapturedBg:
			TRACE("message=W_VDEV_CAPTURED_BG, param1=%d, param2=%d", param1, param2);
			if (param1 != 1) {
				this->m_useCopiedScreen = false;
				return 0;
			}
			if (this->m_useCopiedScreen) {
				return 0;
			}
			return this->BeginCapturedBackground();
		case WDeviceMessageScreenShot:
			TRACE("message=W_VDEV_SCREEN_SHOT, param1=%d, param2=%d", param1, param2);
			return this->ScreenShot((const char*)param1, static_cast<D3DXIMAGE_FILEFORMAT>(param2));
		case WDeviceMessageGetSplash:
			TRACE("message=W_VDEV_GET_SPLASH, param1=%d, param2=%d", param1, param2);
			return reinterpret_cast<int>(new WSplashD3D(this));
		case WDeviceMessageGetCopiedScreenSplash:
			TRACE("message=W_VDEV_GET_COPIEDSCREENSPLASH, param1=%d, param2=%d", param1, param2);
			return (int)this->m_pCopiedScreenSplash;
		case WDeviceMessageSetDdsRes:
			TRACE("message=W_VDEV_SET_DDS_RES, param1=%d, param2=%d", param1, param2);
			this->m_ddsRes = std::clamp(param1, 0, 5);
			this->m_useHiQualityTex = this->m_ddsRes == 0;
			return 0;
		case WDeviceMessageUseMipmap:
			TRACE("message=W_VDEV_USE_MIPMAP, param1=%d, param2=%d", param1, param2);
			this->m_bUseMipmap = param1 != 0;
			return 0;
		case WDeviceMessageMaxMipLevel:
			TRACE("message=W_VDEV_MAX_MIPLVL, param1=%d, param2=%d", param1, param2);
			this->m_iMaxMipLvl = param1 <= 0 ? 0 : param1;
			return 0;
		case WDeviceMessageMipCreateFilter:
			TRACE("message=W_VDEV_MIP_CREATE_FILTER, param1=%d, param2=%d", param1, param2);
			if (param1 >= 1 && param1 <= 5)
				this->m_dwMipCreateFilter = param1;
			return 0;
		case WDeviceMessageMipTextureStageFilter:
			TRACE("message=W_VDEV_MIP_TEXSTAGE_FILTER, param1=%d, param2=%d", param1, param2);
			if (param1 >= 0 && param1 <= 3) {
				this->m_dwMipTexStateFilter = param1;
				this->_SetSamplerState(0, D3DSAMP_MIPFILTER, param1);
			}
			return 0;
		case WDeviceMessageNumTextures:
			TRACE("message=W_VDEV_NUM_TEXTURES, param1=%d, param2=%d", param1, param2);
			numTextures = 0;
			for (int i = 0; i < 512; i++) {
				if (this->m_texList[i].pTex) {
					numTextures++;
				}
			}
			*reinterpret_cast<uint32_t*>(param1) = numTextures;
			return 0;
		case WDeviceMessageNumTnLBuffers:
			TRACE("message=W_VDEV_NUM_TNL_BUFFERS, param1=%d, param2=%d", param1, param2);
			numVb = 0;
			for (int i = 0; i < 256; i++) {
				if (this->m_xaVbList[i].xpVb) {
					numVb++;
				}
			}
			numIb = 0;
			for (int i = 0; i < 64; i++) {
				if (this->m_xaIbList[i].xpIb) {
					numIb++;
				}
			}
			*reinterpret_cast<uint32_t*>(param1) = numVb;
			*reinterpret_cast<uint32_t*>(param2) = numIb;
			return 0;
		case WDeviceMessageGetStatistics:
			TRACE("message=WX_VDEV_GET_STATISTICS, param1=%d, param2=%d", param1, param2);
			*reinterpret_cast<uint32_t*>(param1 + 0) = this->m_xnTotalTriangles;
			*reinterpret_cast<uint32_t*>(param1 + 4) = this->m_xnDPUPs;
			*reinterpret_cast<uint32_t*>(param1 + 8) = this->m_xnDIPUPs;
			*reinterpret_cast<uint32_t*>(param1 + 12) = this->m_xnDPs;
			*reinterpret_cast<uint32_t*>(param1 + 16) = this->m_xnDIPs;
			return 0;
		case WDeviceMessageGetCaps:
			TRACE("message=WX_VDEV_GET_CAPS, param1=%d, param2=%d", param1, param2);
			this->XGetCaps(param1, reinterpret_cast<uint32_t*&>(param2));
			return 0;
		case WDeviceMessageFillMode:
			TRACE("message=WX_VDEV_FILLMODE, param1=%d, param2=%d", param1, param2);
			if (this->m_pd3dDevice) {
				switch (param1) {
					case 0:
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
						break;
					case 1:
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
						break;
					case 2:
						this->FlushRenderPrimitive();
						this->m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT);
						break;
					default:
						TRACE("unexpected param1=%d", param1);
						break;
				}
			}
			return 0;
		case WDeviceMessageDriverInfo:
			TRACE("message=WX_VDEV_DRIVER_INFO, param1=%d, param2=%d", param1, param2);
			devId = this->m_devId;
			memset(&identifier, 0, sizeof identifier);
			if (this->m_d3d8->GetAdapterIdentifier(devId, 0, &identifier) != S_OK) {
				return 0;
			}
			if (!*identifier.Description) {
				return 0;
			}
			strcpy_s(reinterpret_cast<char*>(param1), MAX_DEVICE_IDENTIFIER_STRING, identifier.Description);
			for (auto& i : g_driverPath) {
				sprintf_s(filename, MAX_PATH, "%s%s", i, identifier.Driver);
				hFile = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
				                    FILE_ATTRIBUTE_NORMAL, nullptr);
				if (hFile != static_cast<HANDLE>(INVALID_HANDLE_VALUE)) {
					break;
				}
			}
			GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
			CloseHandle(hFile);
			FileTimeToLocalFileTime(&ftCreate, &ftLocal);
			FileTimeToSystemTime(&ftLocal, &stCreate);
			*reinterpret_cast<uint32_t*>(param2 + 0) = identifier.WHQLLevel;
			*reinterpret_cast<uint32_t*>(param2 + 4) = stCreate.wYear;
			*reinterpret_cast<uint32_t*>(param2 + 8) = stCreate.wMonth;
			*reinterpret_cast<uint32_t*>(param2 + 12) = stCreate.wDay;
			return 0;
		case WDeviceMessageGetSortBufferSize:
			TRACE("message=WX_VDEV_GET_SORTBUFFER_SIZE, param1=%d, param2=%d", param1, param2);
			*reinterpret_cast<uint32_t*>(param1 + 0) = this->GetSortBufferSize();
			*reinterpret_cast<uint32_t*>(param1 + 4) = this->GetSortBufferSwSize();
			*reinterpret_cast<uint32_t*>(param1 + 8) = this->GetSortBufferHwSize();
			return 0;
		case WDeviceMessageGetMergeBufferSize:
			TRACE("message=WX_VDEV_GET_MERGEBUFFER_SIZE, param1=%d, param2=%d", param1, param2);
			*reinterpret_cast<uint32_t*>(param1) = this->m_mergeBuffer.vertexStreamBufferSize;
			*reinterpret_cast<uint32_t*>(param1 + 4) = this->m_mergeBuffer.indexBufferSize;
			return 0;
		case WDeviceMessageUnknown26:
			TRACE("message=0x26, param1=%d, param2=%d", param1, param2);
			*reinterpret_cast<uint32_t*>(param1) = 50;
			return 0;
		default:
			TRACE("message=%08x, param1=%d, param2=%d", message, param1, param2);
			return 0;
	}
}

const char* WDirect3D8::EnumModeName() {
	TRACE("Called");
	char *srcStr, *dstStr;
	D3DCAPS9 caps;
	D3DDISPLAYMODE mode;
	D3DFORMAT formatList[3] = {D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_X8R8G8B8};
	IDirect3D9* d3d8;
	WList<char*> list(16, 16);
	size_t len = 0;
	D3DFORMAT fmt;
	char* modeName;
	char temp[128];

	if (this->m_devId < 0) {
		return nullptr;
	}

	if (this->m_modList) {
		return this->m_modList;
	}

	d3d8 = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d8) {
		g_error = g_msgD3DInitFailed;
		return nullptr;
	}

	if (FAILED(d3d8->GetDeviceCaps(this->m_devId, D3DDEVTYPE_HAL, &caps))) {
		d3d8->Release();
		return nullptr;
	}

	fmt = D3DFMT_X8R8G8B8;
	for (auto& i : formatList) {
		if (d3d8->GetAdapterModeCount(0, i)) {
			fmt = i;
			break;
		}
	}

	for (UINT i = 0; i < d3d8->GetAdapterModeCount(this->m_devId, fmt); i++) {
		d3d8->EnumAdapterModes(this->m_devId, fmt, i, &mode);
		if (!this->GetDevName(mode, temp, 128) && !list.Find(temp)) {
			modeName = this->DuplicateString(temp);
			list.AddItem(modeName, modeName, false);
			len += strlen(modeName) + 1;
		}
	}

	this->m_modList = new char[len + 1];
	size_t i = 0;
	for (char* it = list.Start(); it; it = list.Next()) {
		srcStr = it;
		dstStr = &this->m_modList[i];
		do {
			*dstStr++ = *srcStr++;
		} while (srcStr);
		i += strlen(&this->m_modList[i]) + 1;
		delete it;
	}

	this->m_modList[i] = 0;
	d3d8->Release();
	return this->m_modList;
}

void WDirect3D8::SetTexture(uint32_t iType) {
	TRACE("iType=%d", iType);

	if (this->m_lastTexState != iType) {
		if ((iType & 0x7FF) != (this->m_lastTexState & 0x7FF)) {
			this->XInstantiateAndFillTexture(static_cast<int>(iType & 0x7FF));
			IDirect3DTexture9* tex = this->m_texList[iType & 0x7FF].pTex;
			if (this->m_pTexture[0] != tex) {
				this->FlushRenderPrimitive();
				this->m_pTexture[0] = tex;
			}
			this->m_pd3dDevice->SetTexture(0, tex);
		}
		if ((iType >> 11 & 0x7F) != (this->m_lastTexState >> 11 & 0x7F)) {
			this->XInstantiateAndFillTexture(static_cast<int>(iType >> 11 & 0x7F));
			IDirect3DTexture9* tex = this->m_texList[iType >> 11 & 0x7F].pTex;
			if (this->m_pTexture[1] != tex) {
				this->FlushRenderPrimitive();
				this->m_pTexture[1] = tex;
			}
			this->m_pd3dDevice->SetTexture(1, tex);
		}
		this->m_lastTexState = iType;
	}
}

void WDirect3D8::ReleaseAllRenderTargetBackupResource() {
	TRACE("Called");
	while (!this->m_rtBackupList.empty()) {
		this->m_rtBackupList.pop();
	}
	this->m_curRt = RtBackup();
	this->ReleaseMainRenderTarget();
}

void WDirect3D8::XResetReleaseResource() {
	TRACE("Called");

	for (auto& vb : this->m_xaVbList) {
		if ((vb.xdwUsage & 0x200) == 0) {
			continue;
		}

		vb.xbNeedToBeFilled = true;

		if (!vb.xpVb) {
			continue;
		}

		vb.xpVb->Release();
		vb.xpVb = nullptr;
	}

	for (auto& ib : this->m_xaIbList) {
		if ((ib.xdwUsage & 0x200) == 0) {
			continue;
		}

		ib.xbNeedToBeFilled = true;

		if (!ib.xpIb) {
			continue;
		}

		ib.xpIb->Release();
		ib.xpIb = nullptr;
	}

	for (auto& i : this->m_texList) {
		if (!i.pSurf) {
			continue;
		}

		i.isFilled = false;
		i.pSurf->Release();
		i.pSurf = nullptr;

		if (!i.pTex) {
			continue;
		}

		i.pTex->Release();
		i.pTex = nullptr;
	}

	if (this->m_commonDepthSurf) {
		this->m_commonDepthSurf->Release();
		this->m_commonDepthSurf = nullptr;
	}

	for (auto& i : this->m_depthSurfList) {
		if (!i.pDepth) {
			continue;
		}

		i.pDepth->Release();
		i.pDepth = nullptr;
	}
	memset(this->m_depthSurfList, 0, sizeof this->m_depthSurfList);

	if (this->m_pCopiedScreenSurface) {
		this->m_pCopiedScreenSurface->Release();
		this->m_pCopiedScreenSurface = nullptr;
	}

	if (this->m_hCopiedScreenTexture > 0) {
		this->DestroyTexture(this->m_hCopiedScreenTexture);
		this->m_hCopiedScreenTexture = 0;
	}

	this->m_useCopiedScreen = false;
	if (this->m_pLockableVb) {
		this->m_pLockableVb->Release();
		this->m_pLockableVb = nullptr;
	}

	if (this->m_pLockableIb) {
		this->m_pLockableIb->Release();
		this->m_pLockableIb = nullptr;
	}

	this->ReleaseAllRenderTargetBackupResource();
}

void WDirect3D8::ReleaseShaderResource() {
	TRACE("Called");

	for (std::map<unsigned long, Effect>::iterator it = this->m_effectTable.begin(); it != this->m_effectTable.end();
	     ++it) {
		if (it->second.pEffect) {
			TRACE("Releasing effect %p", it->second.pEffect);
			it->second.pEffect->Release();
		}
	}
	TRACE("Clearing effect table.");
	this->m_effectTable.clear();
	TRACE("Clearing param cache.");
	this->m_fxParamPool.Release();
}

void WDirect3D8::RestoreBackedUpRenderTarget() {
	TRACE("Called");
	// TODO: IMPLEMENTATION
	MessageBox(nullptr, TEXT("WDirect3D8::RestoreBackedupRenderTarget not implemented."),
	           TEXT("WDirect3D8::RestoreBackedupRenderTarget not implemented."), MB_OK);
	ExitProcess(1);
}

void WDirect3D8::Release() {
	TRACE("Called");
	// TODO: IMPLEMENTATION
	MessageBox(nullptr, TEXT("WDirect3D8::Release not implemented."), TEXT("WDirect3D8::Release not implemented."),
	           MB_OK);
	ExitProcess(1);
}

WDirect3D8* WDirect3D8::MakeClone(char* modeName, HWND hWnd, int iTnL) {
	TRACE("modeName=%s, hWnd=%p, iTnL=%d", modeName, hWnd, iTnL);

	if (this->m_devId < 0) {
		return nullptr;
	}

	WDirect3D8* clone = this->CreateClone(this->m_devName, this->m_devId);

	if (clone->Init(modeName, hWnd, iTnL) != 0) {
		clone->Release();
		delete clone;
		return nullptr;
	}

	return clone;
}

bool WDirect3D8::XReset(bool switchWinMode) {
	this->m_renderCount = 0;
	this->XResetReleaseResource();

	for (auto it = this->m_effectTable.begin(); it != this->m_effectTable.end();
	     ++it) {
		if (it->second.pEffect) {
			it->second.pEffect->OnLostDevice();
		}
	}

	if (this->m_pEventQuery) {
		this->m_pEventQuery->Release();
		this->m_pEventQuery = nullptr;
	}

	if (FAILED(this->m_pd3dDevice->Reset(&this->m_d3dpp))) {
		D3DDISPLAYMODE d3ddm;
		if (!this->m_d3dpp.Windowed) {
			return false;
		}
		if (this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm) != S_OK) {
			g_error = "...";
			return false;
		}
		if (d3ddm.Format == this->m_d3dpp.BackBufferFormat) {
			return false;
		}
		this->m_d3dpp.BackBufferFormat = d3ddm.Format;
		this->m_d3dpp.AutoDepthStencilFormat = this->FindDepthBufferFormat(d3ddm.Format);
		if (FAILED(this->m_pd3dDevice->Reset(&this->m_d3dpp))) {
			return false;
		}
	}
	this->m_devState = WDeviceStateLost;
	this->m_clientRcCheckedAfterReset = false;
	if (SUCCEEDED(this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, nullptr))) {
		this->m_pd3dDevice->CreateQuery(D3DQUERYTYPE_EVENT, &this->m_pEventQuery);
	}
	this->SetTextureFormat(this->m_d3dpp.BackBufferFormat);
	this->SetRenderTargetFormat();
	this->SetDefaultState();
	if (this->m_d3dpp.Windowed) {
		if (this->m_lWndStyle != -1) {
			SetWindowLongA(this->m_hWnd, GWL_STYLE, this->m_lWndStyle | WS_VISIBLE);
		}
		RECT rc, old_rc;
		int windowWidth = this->m_d3dpp.BackBufferWidth;
		int windowHeight = this->m_d3dpp.BackBufferHeight;
		if (this->m_fillScrMode) {
			windowWidth = GetSystemMetrics(SM_CXSCREEN);
			windowHeight = GetSystemMetrics(SM_CYSCREEN);
		}
		SetRect(&rc, 0, 0, windowWidth, windowHeight);
		GetClientRect(this->m_hWnd, &old_rc);
		if (switchWinMode || old_rc.right - old_rc.left != windowWidth || old_rc.bottom - old_rc.top != windowHeight
		) {
			AdjustWindowRectEx(&rc, GetWindowLongA(this->m_hWnd, GWL_STYLE), 0, 0);
			int dx = (rc.left - rc.right + GetSystemMetrics(SM_CXSCREEN)) / 2;
			int dy = (rc.top - rc.bottom + GetSystemMetrics(SM_CYSCREEN)) / 2;
			OffsetRect(&rc, dx, dy);
			SetWindowPos(this->m_hWnd, HWND_NOTOPMOST, dx, dy, rc.right - rc.left, rc.bottom - rc.top, 0x40u);
			UpdateWindow(this->m_hWnd);
			g_formatChanged = false;
		}
		if (this->m_dwmApiDll && this->m_fillScrMode) {
			if (static_cast<double>(this->m_d3dpp.BackBufferWidth) / static_cast<double>(this->m_d3dpp.BackBufferHeight)
				>= 1.4 || static_cast<double>(windowWidth) / static_cast<double>(windowHeight) <= 1.4) {
				if (this->m_dwmApiDll->IsCompositionEnabled()) {
					this->m_dwmApiDll->EnableComposition(false);
				}
			} else if (!this->m_dwmApiDll->IsCompositionEnabled()) {
				this->m_dwmApiDll->EnableComposition(true);
			}
		}
	}
	this->XResetCreateResource();
	if (this->m_pCopiedScreenSplash) {
		this->m_pCopiedScreenSplash->Reset();
	}
	this->m_pd3dDevice->GetRenderTarget(0, &this->m_mainRt.rt.surf[0]);
	this->m_mainRt.rt.surf[1] = nullptr;
	this->m_pd3dDevice->GetDepthStencilSurface(&this->m_mainRt.depthSurf);
	this->m_pd3dDevice->GetViewport(&this->m_mainRt.viewport);
	memcpy(&this->m_curRt, &this->m_mainRt, sizeof this->m_curRt);

	for (std::map<unsigned long, Effect>::iterator it = this->m_effectTable.begin(); it != this->m_effectTable.end();
	     ++it) {
		if (it->second.pEffect) {
			it->second.pEffect->OnResetDevice();
		}
	}

	return true;
}

bool WDirect3D8::Reset(bool bWindowed, int iWidth, int iHeight, int iColor, int lWndStyle, int fillMode) {
	TRACE("bWindowed=%d, iWidth=%d, iHeight=%d, iColor=%d, lWndStyle=%d, fillMode=%d", bWindowed, iWidth, iHeight,
	      iColor, lWndStyle, fillMode);
	D3DDISPLAYMODE d3ddm;
	char modeName[64];
	char temp[64];

	if (!bWindowed) {
		fillMode = 0;
	}

	bool switchWinMode = (this->m_d3dpp.Windowed ? true : false) != bWindowed;
	this->m_lWndStyle = lWndStyle;
	this->m_fillScrMode = fillMode > 0;
	SetWindowLongA(this->m_hWnd, GWL_STYLE, lWndStyle);
	SetWindowLongA(this->m_hWnd, GWL_STYLE, GetWindowLongA(this->m_hWnd, GWL_STYLE) | WS_VISIBLE);
	if (bWindowed) {
		int newBpp = this->m_backBufBpp;
		if (this->m_d3dpp.Windowed) {
			if (FAILED(this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm))) {
				g_error = "Could not find a valid display mode.";
				return false;
			}
			this->m_fmtWindowed = d3ddm.Format;
			newBpp = GetBackBufferBpp(d3ddm.Format);
		}
		g_formatChanged = false;
		if (iWidth != this->m_d3dpp.BackBufferWidth || iHeight != this->m_d3dpp.BackBufferHeight || this->m_backBufBpp
			!= newBpp || this->m_d3dpp.Windowed == 0) {
			g_formatChanged = true;
		}
		this->m_backBufBpp = newBpp;
		this->m_d3dpp.Windowed = 1;
		this->m_bWindow = true;
		this->m_d3dpp.BackBufferWidth = iWidth;
		this->m_d3dpp.BackBufferHeight = iHeight;
		this->m_d3dpp.BackBufferFormat = this->m_fmtWindowed;
		this->m_d3dpp.SwapEffect = g_captureOption.swapEffect;
		this->m_d3dpp.FullScreen_RefreshRateInHz = 0;
	} else {
		unsigned int width = 0;
		unsigned int height = 0;
		unsigned int refresh = 0;
		D3DFORMAT format = {};

		sprintf_s(modeName, 64, "w%d h%d b%d", iWidth, iHeight, iColor);
		int m = -1;
		for (const auto& j : {D3DFMT_X8R8G8B8, D3DFMT_R5G6B5, D3DFMT_X1R5G5B5}) {
			for (UINT i = 0; i < this->m_d3d8->GetAdapterModeCount(this->m_devId, j); i++) {
				this->m_d3d8->EnumAdapterModes(this->m_devId, j, i, &d3ddm);
				if (!GetDevName(d3ddm, temp, 64)) {
					if (!strcmp(modeName, temp) && (d3ddm.RefreshRate >= 60 || !d3ddm.RefreshRate)) {
						refresh = d3ddm.RefreshRate;
						width = d3ddm.Width;
						height = d3ddm.Height;
						format = d3ddm.Format;
						m = i;
						break;
					}
				}
			}
			if (m != -1) {
				break;
			}
		}

		if (m == -1) {
			g_error = "Could not find a valid display mode.";
			return false;
		}

		this->m_d3dpp.Windowed = 0;
		this->m_bWindow = false;
		this->m_backBufBpp = iColor;

		float fps;
		if (refresh == 0) {
			fps = 120.0f;
		} else {
			fps = static_cast<float>(refresh);
		}

		this->m_fps = fps;
		this->m_d3dpp.BackBufferWidth = width;
		this->m_d3dpp.BackBufferHeight = height;
		this->m_d3dpp.BackBufferFormat = format;
		this->m_d3dpp.SwapEffect = g_captureOption.swapEffect;
		this->m_d3dpp.FullScreen_RefreshRateInHz = refresh;
	}
	D3DFORMAT backBufferFormat = this->m_d3dpp.BackBufferFormat;
	this->m_d3dpp.PresentationInterval = g_captureOption.fullScreenPresentationInterval;
	D3DFORMAT stencilFormat = this->FindDepthBufferFormat(backBufferFormat);
	this->m_d3dpp.AutoDepthStencilFormat = stencilFormat;

	if (!this->XReset(switchWinMode)) {
		Sleep(100);
		if (!this->XReset(switchWinMode)) {
			return false;
		}
	}
	if (bWindowed) {
		if (FAILED(this->m_d3d8->GetAdapterDisplayMode(this->m_devId, &d3ddm))) {
			g_error = "Could not find a valid display mode.";
			return false;
		}
		float fps;
		if (d3ddm.RefreshRate) {
			fps = static_cast<float>(d3ddm.RefreshRate);
		} else {
			fps = 120.0f;
		}
		this->m_fps = fps;
		this->m_fmtWindowed = d3ddm.Format;
	}

	if (g_captureOption.updateWholeScreen) {
		this->m_pd3dDevice->BeginScene();
		this->m_pd3dDevice->Clear(0, nullptr, 3, 0, 0.0, 0);
		this->m_pd3dDevice->EndScene();
		this->Present();
	} else {
		for (int i = 0; i < 2; i++) {
			this->m_pd3dDevice->BeginScene();
			this->m_pd3dDevice->Clear(0, nullptr, 3, 0, 0.0, 0);
			this->m_pd3dDevice->EndScene();
			this->Present();
		}
	}
	return true;
}

void WDirect3D8::EndRenderToTexture(const WRenderToTextureParam& param) {
	TRACE("Called");
	if (this->m_devState != 1 && param.rtTexInfo[0].hTex > 0 && this->m_texList[param.rtTexInfo[0].hTex].pSurf
	) {
		this->RestoreBackedUpRenderTarget();
		if (param.rtTexInfo[0].hTex > 0) {
			this->m_texList[param.rtTexInfo[0].hTex].isFilled = true;
		}
		if (param.rtTexInfo[1].hTex > 0) {
			this->m_texList[param.rtTexInfo[1].hTex].isFilled = true;
		}
	}
}

void WDirect3D8::BackupRenderTarget(const WRenderToTextureParam& param, IDirect3DSurface9* depthSurf,
                                    const D3DVIEWPORT9& viewport) {
	TRACE("Called");
	RtBackup newRt;
	newRt.rt.surf[0] = param.rtTexInfo[0].hTex > 0 ? this->m_texList[param.rtTexInfo[0].hTex].pSurf : nullptr;
	newRt.rt.surf[1] = param.rtTexInfo[1].hTex > 0 ? this->m_texList[param.rtTexInfo[1].hTex].pSurf : nullptr;
	newRt.depthSurf = depthSurf;
	newRt.viewport.X = viewport.X;
	newRt.viewport.Y = viewport.Y;
	newRt.viewport.Width = viewport.Width;
	newRt.viewport.Height = viewport.Height;
	newRt.viewport.MinZ = viewport.MinZ;
	newRt.viewport.MaxZ = viewport.MaxZ;
	this->m_rtBackupList.push(this->m_curRt);
	memcpy(&this->m_curRt, &newRt, sizeof(RtBackup));
}

bool WDirect3D8::BeginScene() {
	TRACE("Called");

	HRESULT hResult = this->m_pd3dDevice->TestCooperativeLevel();
	if (FAILED(hResult)) {
		this->m_devState = WDeviceStateLost;

		if (hResult == D3DERR_DEVICELOST || hResult != D3DERR_DEVICENOTRESET) {
			return false;
		}

		if (!this->XReset(false)) {
			return false;
		}
	}
	if (this->m_d3dpp.Windowed && !this->m_fillScrMode && !this->m_clientRcCheckedAfterReset) {
		this->m_clientRcCheckedAfterReset = true;
		RECT rcClient;
		GetClientRect(this->m_hWnd, &rcClient);
		if (rcClient.right - rcClient.left != this->m_d3dpp.BackBufferWidth ||
			rcClient.bottom - rcClient.top != this->m_d3dpp.BackBufferHeight) {
			SetRect(&rcClient, 0, 0, this->m_d3dpp.BackBufferWidth, this->m_d3dpp.BackBufferHeight);
			AdjustWindowRectEx(&rcClient, GetWindowLongA(this->m_hWnd, GWL_STYLE), 0, 0);
			if (rcClient.right - rcClient.left > GetSystemMetrics(SM_CXFULLSCREEN) ||
				rcClient.bottom - rcClient.top > GetSystemMetrics(SM_CYFULLSCREEN)) {
				this->m_devState = WDeviceStateLost;
				if (!this->XReset(false)) {
					return false;
				}
			}
		}
	}

	this->m_devState = WDeviceStateNormal;
	this->m_xhLastIb = 0;
	this->m_xhLastVb = 0;
	this->m_xnDIPs = 0;
	this->m_xnDPs = 0;
	this->m_xnDIPUPs = 0;
	this->m_xnDPUPs = 0;
	this->m_xnTotalTriangles = 0;
	this->m_pd3dDevice->BeginScene();
	this->m_mergeBuffer.primitiveCount = 0;

	return true;
}

void WDirect3D8::UpdateShaderValue(unsigned long h) {
	TRACE("h=%d", h);

	Effect& fx = this->m_effectTable[h];
	D3DXMATRIX mat;
	D3DXMATRIX temp;

	if (!fx.pEffect) {
		return;
	}

	if (fx.param[0].handle) {
		D3DXMatrixMultiply(&temp, this->m_xLastWorldMatrix, &this->m_xLastViewMatrix);
		D3DXMatrixMultiply(&mat, &temp, &this->m_xLastProjMatrix);
		this->m_fxParamPool.SetMatrix(
			fx.pEffect,
			WFxParamWorldViewProjection,
			fx.param[0].handle,
			fx.param[0].isShared,
			&mat);
	}
	if (fx.param[1].handle) {
		this->m_fxParamPool.SetMatrix(
			fx.pEffect,
			WFxParamWorld,
			fx.param[1].handle,
			fx.param[1].isShared,
			this->m_xLastWorldMatrix
		);
	}
	if (fx.param[2].handle) {
		D3DXMatrixMultiply(&mat, this->m_xLastWorldMatrix, &this->m_xLastViewMatrix);
		this->m_fxParamPool.SetMatrix(
			fx.pEffect,
			WFxParamWorldView,
			fx.param[2].handle,
			fx.param[2].isShared,
			&mat
		);
	}
	if (fx.param[3].handle) {
		this->m_fxParamPool.SetMatrix(
			fx.pEffect,
			WFxParamPrevView,
			fx.param[3].handle,
			fx.param[3].isShared,
			&this->m_xPrevViewMatrix
		);
	}
	if (fx.param[4].handle) {
		this->m_fxParamPool.SetMatrix(
			fx.pEffect,
			WFxParamView,
			fx.param[4].handle,
			fx.param[4].isShared,
			&this->m_xPrevViewMatrix
		);
	}
	if (fx.param[5].handle) {
		this->m_fxParamPool.SetMatrix(
			fx.pEffect,
			WFxParamProjection,
			fx.param[5].handle,
			fx.param[5].isShared,
			&this->m_xLastProjMatrix
		);
	}
	if (fx.param[6].handle) {
		D3DXMatrixMultiply(&mat, &this->m_xLastViewMatrix, &this->m_xLastProjMatrix);
		this->m_fxParamPool.SetMatrix(
			fx.pEffect,
			WFxParamViewProjection,
			fx.param[6].handle,
			fx.param[6].isShared,
			&mat
		);
	}
	if (fx.param[7].handle) {
		D3DXVECTOR4 c;
		c.w = static_cast<float>(this->m_lastTFactor >> 24 & 0xFF) / 255.f;
		c.x = static_cast<float>(this->m_lastTFactor >> 16 & 0xFF) / 255.f;
		c.y = static_cast<float>(this->m_lastTFactor >> 8 & 0xFF) / 255.f;
		c.z = static_cast<float>(this->m_lastTFactor >> 0 & 0xFF) / 255.f;
		this->m_fxParamPool.SetVector4(
			fx.pEffect,
			WFxParamConstColor,
			fx.param[7].handle,
			fx.param[7].isShared,
			&c
		);
	}
	if (fx.param[8].handle) {
		this->m_fxParamPool.SetVector4(
			fx.pEffect,
			WFxParamLightDiffuse,
			fx.param[8].handle,
			fx.param[8].isShared,
			(const D3DXVECTOR4*)&this->m_xaLights[0].Diffuse
		);
	}
	if (fx.param[9].handle) {
		this->m_fxParamPool.SetVector4(
			fx.pEffect,
			WFxParamLightAmbient,
			fx.param[9].handle,
			fx.param[9].isShared,
			(const D3DXVECTOR4*)&this->m_xaLights[0].Ambient
		);
	}
	if (fx.param[10].handle) {
		D3DXVECTOR4 c;
		c.x = this->m_xaLights[0].Direction.x;
		c.z = this->m_xaLights[0].Direction.z;
		c.y = this->m_xaLights[0].Direction.y;
		c.w = 1.0;
		this->m_fxParamPool.SetVector4(
			fx.pEffect,
			WFxParamLightDirection,
			fx.param[10].handle,
			fx.param[10].isShared,
			&c
		);
	}
	if (fx.param[11].handle) {
		D3DXVECTOR4 c;
		c.x = this->m_lastFogStart;
		c.y = this->m_lastFogEnd;
		c.z = 0.0;
		c.w = 1.0;
		this->m_fxParamPool.SetVector4(
			fx.pEffect,
			WFxParamFogRange,
			fx.param[11].handle,
			fx.param[11].isShared,
			&c
		);
	}
	if (fx.param[12].handle) {
		D3DXVECTOR3 c;
		c.x = static_cast<float>(this->m_fogColor >> 16 & 0xFF) / 255.f;
		c.y = static_cast<float>(this->m_fogColor >> 8 & 0xFF) / 255.f;
		c.z = static_cast<float>(this->m_fogColor >> 0 & 0xFF) / 255.f;
		this->m_fxParamPool.SetVector3(
			fx.pEffect,
			WFxParamFogColor,
			fx.param[12].handle,
			fx.param[12].isShared,
			&c
		);
	}
	if (fx.param[13].handle) {
		D3DXVECTOR4 c;
		D3DXMatrixInverse(&temp, nullptr, &this->m_xLastViewMatrix);
		c.x = temp.m[3][0];
		c.y = temp.m[3][1];
		c.z = temp.m[3][2];
		c.w = 1.0;
		this->m_fxParamPool.SetVector4(
			fx.pEffect,
			WFxParamCameraPosition,
			fx.param[13].handle,
			fx.param[13].isShared,
			&c
		);
	}
	if (fx.param[14].handle) {
		fx.pEffect->SetVectorArray(fx.param[14].handle, reinterpret_cast<D3DXVECTOR4*>(this->m_shCoefficient), 7);
	}
	if (fx.param[17].handle) {
		D3DXVECTOR4 c;
		c.w = static_cast<float>(this->m_xdwDiffuse >> 24 & 0xFF) / 255.f;
		c.x = static_cast<float>(this->m_xdwDiffuse >> 16 & 0xFF) / 255.f;
		c.y = static_cast<float>(this->m_xdwDiffuse >> 8 & 0xFF) / 255.f;
		c.z = static_cast<float>(this->m_xdwDiffuse >> 0 & 0xFF) / 255.f;
		this->m_fxParamPool.SetVector4(
			fx.pEffect,
			WFxParamMaterialColor,
			fx.param[17].handle,
			fx.param[17].isShared,
			&c
		);
	}
}

bool WDirect3D8::CreateEffect(uint32_t flags) {
	TRACE("flags=%d", flags);
	D3DXMACRO macro[24];
	D3DXPARAMETER_DESC paramDesc;
	ID3DXBuffer* err;

	D3DXMACRO* ptr = macro;
	for (auto& i : g_macroList) {
		if (flags & i.wvdFxMacroFlag) {
			ptr->Name = i.txt;
			ptr->Definition = "";
			ptr++;
		}
	}

	err = nullptr;
	Effect* effect = &this->m_effectTable[flags];
	memset(effect, 0, sizeof(Effect));
	if (FAILED(
		D3DXCreateEffect(this->m_pd3dDevice, this->m_shaderSource.c_str(), this->m_shaderSource.size(), macro, nullptr,
			0,
			this->m_fxParamPool.pool, &effect->pEffect, &err))) {
		return false;
	}

	for (auto& i : g_paramList) {
		D3DXHANDLE handle = effect->pEffect->GetParameterBySemantic(nullptr, i.semantic);
		if (handle && effect->pEffect->IsParameterUsed(handle, nullptr) == TRUE) {
			effect->param[i.wvdFxParamType].handle = handle;
			effect->pEffect->GetParameterDesc(handle, &paramDesc);
			if (paramDesc.Flags & 1) {
				effect->param[i.wvdFxParamType].isShared = true;
			}
		}
	}

	return true;
}

bool WDirect3D8::BeginRenderToTexture(const WRenderToTextureParam& param) {
	TRACE("Called");
	IDirect3DSurface9* rt;
	D3DVIEWPORT9 viewport;

	if (this->m_devState == WDeviceStateLost) {
		return false;
	}

	if (param.rtTexInfo[0].hTex <= 0) {
		return false;
	}

	const D3D8Texture& tex0 = this->m_texList[param.rtTexInfo[0].hTex];
	if (!tex0.pSurf) {
		return false;
	}

	if (!param.CanClearAtOnce()) {
		for (auto i : param.rtTexInfo) {
			if (i.hTex > 0 && i.needToClear) {
				this->m_pd3dDevice->SetRenderTarget(0, this->m_texList[i.hTex].pSurf);
				this->m_pd3dDevice->Clear(0, nullptr, 1u, i.clearColor, 1.0, 0);
			}
		}
	}

	IDirect3DSurface9* pDepth = nullptr;
	switch (param.depthSurfInfo.surfUsage) {
		case WRenderToTextureParam::DepthSurfInfo::UseMainSurface:
			pDepth = this->m_mainRt.depthSurf;
			break;
		case WRenderToTextureParam::DepthSurfInfo::UseSharedSurface:
			pDepth = this->FindDepthSurf(tex0.width, tex0.height);
			break;
		case WRenderToTextureParam::DepthSurfInfo::UseExclusiveSurface:
			pDepth = this->m_texList[param.depthSurfInfo.hTex].pSurf;
			break;
		default:
			break;
	}

	for (int i = 0; i < 2; i++) {
		if (param.rtTexInfo[i].hTex <= 0) {
			rt = nullptr;
		} else {
			rt = this->m_texList[param.rtTexInfo[i].hTex].pSurf;
		}

		if (rt != this->m_curRt.rt.surf[i]) {
			this->m_pd3dDevice->SetRenderTarget(i, rt);
		}
	}

	if (pDepth != this->m_curRt.depthSurf) {
		this->m_pd3dDevice->SetDepthStencilSurface(pDepth);
	}

	viewport.X = 0;
	viewport.Y = 0;
	viewport.Width = tex0.width;
	viewport.Height = tex0.height;
	viewport.MinZ = 0.0;
	viewport.MaxZ = 1.0;

	if (memcmp(&viewport, &this->m_curRt.viewport, 0x18) != 0) {
		this->m_pd3dDevice->SetViewport(&viewport);
	}

	uint32_t clearFlags = 0;
	if (param.CanClearAtOnce() && param.rtTexInfo[0].needToClear) {
		clearFlags = D3DCLEAR_TARGET;
	}

	if (pDepth && param.depthSurfInfo.needToClear) {
		clearFlags |= D3DCLEAR_ZBUFFER;
	}

	if (clearFlags) {
		this->m_pd3dDevice->Clear(0, nullptr, clearFlags, param.rtTexInfo[0].clearColor,
		                          param.depthSurfInfo.clearZ, 0);
	}

	this->BackupRenderTarget(param, pDepth, viewport);

	return true;
}

void WDirect3D8::SetCustomRenderState(WRenderStateType state, unsigned int value) {
	TRACE("state=%d, value=%d", state, value);
	this->m_customRenderState.SetRenderState(static_cast<D3DRENDERSTATETYPE>(state), value);
}

void WDirect3D8::SetCustomTransform(WTransformStateType state, const WMatrix4& matrix) {
	TRACE("state=%d", state);
	this->m_customRenderState.SetTransform(static_cast<D3DTRANSFORMSTATETYPE>(state),
	                                       reinterpret_cast<const D3DXMATRIX&>(matrix));
}

void WDirect3D8::SetCustomTexture(unsigned int stage, int hTex) {
	TRACE("stage=%d, hTex=%d", stage, hTex);
	if (hTex < 0) {
		return;
	}
	if (hTex) {
		if (this->m_texList[hTex].pTex) {
			this->XInstantiateTexture(hTex);
		}

		if (this->m_texList[hTex].needToBeFilled) {
			this->XFillTexture(hTex);
		}
	}
	this->m_customRenderState.SetTexture(stage, hTex > 0 ? this->m_texList[hTex].pTex : nullptr);
}

void WDirect3D8::SetCustomFxParamInt(WFxParameterType paramType, int value) {
	TRACE("ParamType=%d, Value=%d", paramType, value);
	this->m_customRenderState.SetFxParamInt(paramType, value);
}

void WDirect3D8::SetCustomFxParamVector2(WFxParameterType paramType, const WVector2D& value) {
	TRACE("ParamType=%d", paramType);
	this->m_customRenderState.SetFxParamVector2(paramType, reinterpret_cast<const D3DXVECTOR2&>(value));
}

void WDirect3D8::SetCustomFxParamVector3(WFxParameterType paramType, const WVector& value) {
	TRACE("ParamType=%d", paramType);
	this->m_customRenderState.SetFxParamVector3(paramType, reinterpret_cast<const D3DXVECTOR3&>(value));
}

void WDirect3D8::SetCustomFxParamVector4(WFxParameterType paramType, const WVector4& value) {
	TRACE("ParamType=%d", paramType);
	this->m_customRenderState.SetFxParamVector4(paramType, reinterpret_cast<const D3DXVECTOR4&>(value));
}

void WDirect3D8::SetCustomFxParamMatrix(WFxParameterType paramType, const WMatrix4& value) {
	TRACE("ParamType=%d", paramType);
	this->m_customRenderState.SetFxParamMatrix(paramType, reinterpret_cast<const D3DXMATRIX&>(value));
}

void WDirect3D8::SetCustomFxParamTexture(WFxParameterType paramType, int hTex) {
	TRACE("ParamType=%d, hTex=%d", paramType, hTex);
	if (hTex < 0) {
		return;
	}
	if (hTex) {
		if (this->m_texList[hTex].pTex) {
			this->XInstantiateTexture(hTex);
		}

		if (this->m_texList[hTex].needToBeFilled) {
			this->XFillTexture(hTex);
		}
	}
	this->m_customRenderState.SetFxParamTexture(paramType, hTex > 0 ? this->m_texList[hTex].pTex : nullptr);
}

void WDirect3D8::ApplyCustomRenderState(const void* customRenderState) {
	TRACE("customRenderState = %p", customRenderState);
	if (customRenderState) {
		const WRenderState& crs = *static_cast<const WRenderState*>(customRenderState);
		if (crs != this->m_customRenderState) {
			this->FlushRenderPrimitive();
		}
		this->m_customRenderState = crs;
	} else if (this->m_customRenderState.HasAnyState()) {
		this->FlushRenderPrimitive();
		this->m_customRenderState.Clear();
	}
}

void WDirect3D8::SetShader(uint32_t flags) {
	TRACE("flags=%08x", flags);

	this->m_effect = flags;
	if (!flags) {
		return;
	}

	auto result = this->m_effectTable.find(flags);
	if (result == this->m_effectTable.end()) {
		if (this->CreateEffect(flags) == 0) {
			this->m_effect = 0;
		}
	} else {
		if (result->second.pEffect == nullptr) {
			this->m_effect = 0;
		}
	}
}

bool WDirect3D8::ApplyShader() {
	TRACE("Called");
	bool ret = true;
	if (this->m_lastEffect) {
		if (this->m_lastEffect != this->m_effect) {
			Effect& effect = this->m_effectTable[this->m_lastEffect];
			if (effect.pEffect) {
				effect.pEffect->EndPass();
				effect.pEffect->End();
			}
		}
	}
	if (!this->m_effect) {
		if (this->m_lastEffect) {
			this->m_pd3dDevice->SetVertexShader(nullptr);
			this->m_pd3dDevice->SetPixelShader(nullptr);
			this->m_lastEffect = this->m_effect;
			return ret;
		}
		this->m_lastEffect = this->m_effect;
		return ret;
	}
	this->UpdateShaderValue(this->m_effect);
	Effect& effect = this->m_effectTable[this->m_effect];
	if (!effect.pEffect) {
		ret = false;
		this->m_lastEffect = this->m_effect;
		return ret;
	}
	if (this->m_lastEffect == this->m_effect) {
		effect.pEffect->CommitChanges();
	} else {
		unsigned int passCount;
		effect.pEffect->Begin(&passCount, 7);
		effect.pEffect->BeginPass(0);
	}
	this->m_lastEffect = this->m_effect;
	return ret;
}

void WDirect3D8::ReloadShader() {
	TRACE("Called");
	this->m_effect = NULL;

	this->ApplyShader();
	this->ReleaseShaderResource();
}

void WDirect3D8::SetShaderSource(const char* shaderSrc) {
	TRACE("shaderSrc=%s", shaderSrc);
	this->m_effect = NULL;
	this->ApplyShader();
	this->ReleaseShaderResource();
	if (shaderSrc) {
		this->m_shaderSource = shaderSrc;
	}
}

void WDirect3D8::SetCustomTextureStageState(uint32_t stage, WTextureStageStateType type, uint32_t value) {
	TRACE("Stage=%d, Type=%d, Value=%d", stage, type, value);
	this->m_customRenderState.SetTextureStageState(stage, static_cast<D3DTEXTURESTAGESTATETYPE>(type), value);
}

void WDirect3D8::SetCustomClipPlane(uint32_t index, const WPlane& value) {
	TRACE("Index=%d", index);
	this->m_customRenderState.SetClipPlane(index, reinterpret_cast<const D3DXPLANE&>(value));
}

void WDirect3D8::SetCustomSamplerState(uint32_t sampler, WSamplerStateType type, uint32_t value) {
	TRACE("Sampler=%d, Type=%d, Value=%d", sampler, type, value);
	this->m_customRenderState.SetSamplerState(sampler, static_cast<D3DSAMPLERSTATETYPE>(type), value);
}

void WDirect3D8::SetVtxType(uint32_t type, uint32_t type2, unsigned int vertexType, unsigned int diffuse) {
	TRACE("type=0x%08x, type2=0x%08x, vertexType=0x%08x, diffuse=0x%08x", type, type2, vertexType, diffuse);

	uint32_t dwTypeEx = this->m_globalRs[1] | type2;
	uint32_t dwType = this->m_globalRs[0] | type;
	this->SetRenderState(dwType, dwTypeEx);
	this->SetTexture(dwType & 0x3FFFF);
	this->m_xbCurHwTnL = dwTypeEx >> 22 != 0;
	this->XSetRenderState(dwTypeEx, diffuse);
	this->XSetTnLBuffer(static_cast<int>(dwTypeEx >> 22), static_cast<int>(dwTypeEx >> 14 & 0xFF));
	if (this->m_xbCurHwTnL != this->m_xbLastHwTnL) {
		if (this->m_fog && dwType & 0x8000000) {
			this->FlushRenderPrimitive();
			this->m_pd3dDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, this->m_xbCurHwTnL ? 2 : 0);
		}
		this->m_xbLastHwTnL = this->m_xbCurHwTnL;
	}
	this->SetVtxMode(vertexType | (dwType & 0x40000) << 12);

	uint32_t flags = 0;
	if (this->IsSupportVs() || this->IsSupportPs()) {
		flags = this->m_customRenderState.m_fxMacro;
		if (dwType & 0x07FF) {
			flags |= 0x01;
		}
		if (!(dwType & 0x40000000) && vertexType & 0x40) {
			flags |= 0x04;
		}
		if ((vertexType & D3DFVF_POSITION_MASK) == 4) {
			flags |= 0x2000;
			if ((dwTypeEx & 0x60) == 0x20) {
				flags |= 0x0801;
			} else if ((dwTypeEx & 0x60) == 0x40) {
				flags |= 0x1001;
			}
		} else {
			if (dwType & 0x3F800) {
				flags |= 0x0010;
			}
			if (dwType & 0x40000) {
				flags |= 0x0020;
			}
			if (dwTypeEx & 0x100 && !(dwType & 0x1800000)) {
				flags |= 0x4000;
			}
			if ((dwTypeEx & 0x80u) != 0 && !(dwType & 0x1800000)) {
				flags |= 0x8000;
			}
			if (this->m_lightEnable == 1 && this->m_xdwDiffuse != -1) {
				flags |= 0x0008;
			}
			if (dwType & 0x8000000) {
				flags |= 0x0080;
			}
			if (vertexType & 0x10) {
				flags |= 0x0002;
			}
			if (this->m_xbUseTFactor == 1) {
				flags |= 0x0040;
			}
			if (dwTypeEx & 0x04) {
				flags |= 0x0100;
			}
			if (dwTypeEx & 0x0200 && !(dwTypeEx & 0x04)) {
				flags |= 0x0400;
			}
		}
		if (!this->IsSupportVs()) {
			flags |= 0x20000000;
		}
		if (!this->IsSupportPs()) {
			flags |= 0x40000000;
		}
	}
	this->SetShader(flags);
}

void WDirect3D8::EndScene() {
	TRACE("Called");

	this->Flush(0);
	this->FlushRenderPrimitive();
	if (this->m_devState != WDeviceStateLost) {
		this->m_effect = NULL;
		this->ApplyShader();
		this->m_pd3dDevice->EndScene();
	}
}

bool WDirect3D8::DrawIndexedPrimitiveLockable(D3DPRIMITIVETYPE primitiveType, const void* vPtr, int vNum, int vPitch,
                                              uint16_t* iPtr, int primNum) {
	TRACE("PrimitiveType=%d, vptr=%p, vnum=%d, vpitch=%d, iptr=%p, primnum=%d", primitiveType, vPtr, vNum, vPitch, iPtr,
	      primNum);

	UINT index;
	INT off;
	int hVB;
	int hIb;

	off = this->LockVb(vPtr, vNum, vPitch, 0);
	if (off == -1) {
		return false;
	}
	if (primitiveType == 2) {
		index = this->LockIb(iPtr, 2 * primNum);
	} else {
		index = this->LockIb(iPtr, 3 * primNum);
	}
	if (index == -1) {
		return false;
	}
	hIb = this->m_xhLastIb;
	hVB = this->m_xhLastVb;
	this->m_xhLastVb = NULL;
	this->m_xhLastIb = NULL;
	this->m_pd3dDevice->SetStreamSource(0, this->m_pLockableVb, 0, vPitch);
	this->m_pd3dDevice->SetIndices(this->m_pLockableIb);
	if (this->ApplyShader()) {
		this->m_customRenderState.Begin(this);
		this->m_pd3dDevice->DrawIndexedPrimitive(primitiveType, off, 0, vNum, index, primNum);
		++this->m_xnDIPs;
		this->m_xnTotalTriangles += primNum;
		this->m_customRenderState.End(this);
	}
	this->XSetTnLBuffer(hVB, hIb);
	return true;
}

bool WDirect3D8::DrawPrimitiveLockable(D3DPRIMITIVETYPE primitiveType, size_t primitiveCount,
                                       const void* pVertexStreamZeroData, unsigned int vertexStreamZeroStride) {
	TRACE("PrimitiveType=%d, PrimitiveCount=%d", primitiveType, primitiveCount);

	unsigned int vNum;

	switch (primitiveType) {
		case D3DPT_LINELIST:
			vNum = primitiveCount * 2;
			break;
		case D3DPT_LINESTRIP:
			vNum = primitiveCount + 1;
			break;
		case D3DPT_TRIANGLELIST:
			vNum = primitiveCount * 3;
			break;
		case D3DPT_TRIANGLESTRIP:
		case D3DPT_TRIANGLEFAN:
			vNum = primitiveCount + 2;
			break;
		default:
			vNum = primitiveCount;
			break;
	}
	int off = this->LockVb(pVertexStreamZeroData, vNum, vertexStreamZeroStride, 0);
	if (off == -1) {
		return false;
	}
	int hIb = this->m_xhLastIb;
	int hVb = this->m_xhLastVb;
	this->m_xhLastVb = NULL;
	this->m_pd3dDevice->SetStreamSource(0, this->m_pLockableVb, 0, vertexStreamZeroStride);
	if (this->ApplyShader()) {
		this->m_customRenderState.Begin(this);
		this->m_pd3dDevice->DrawPrimitive(primitiveType, off, primitiveCount);
		++this->m_xnDPs;
		this->m_xnTotalTriangles += primitiveCount;
		this->m_customRenderState.End(this);
	}
	this->XSetTnLBuffer(hVb, hIb);
	return true;
}

void WDirect3D8::FlushRenderPrimitive() {
	TRACE("Called");

	D3DXMATRIX bak;
	int restoreTm;

	if (this->m_devState == WDeviceStateLost) {
		this->m_mergeBuffer.primitiveCount = 0;
		return;
	}

	if (!this->m_mergeBuffer.primitiveCount || GetCurrentThreadId() != this->m_mainThreadId) {
		return;
	}

	this->XInstantiateAndFillTexture(static_cast<int>(this->m_lastTexState & 0x7FF));
	this->XInstantiateAndFillTexture(static_cast<int>(this->m_lastTexState >> 11 & 0x7F));

	if (this->m_lastRenderState & 0x1000) {
		this->m_mergeBuffer.CheckAndIncreaseVertexStreamBuffer(
			this->m_mergeBuffer.numVertices * this->m_mergeBuffer.vertexStreamZeroStride);
		auto srcIndex = this->m_mergeBuffer.vertexStreamZeroStride * m_mergeBuffer.minIndex;
		auto* src = &this->m_xaVbList[this->m_xhLastVb].xpVertexData[srcIndex];
		FillVertex(m_mergeBuffer.vertexStreamBuffer, src, m_xLastVertexDecl, m_mergeBuffer.numVertices,
		           m_mergeBuffer.vertexStreamZeroStride, m_xLastWorldMatrix);
		unsigned int vPitch = this->m_mergeBuffer.vertexStreamZeroStride;
		switch (this->m_xLastVertexDecl & D3DFVF_POSITION_MASK) {
			case 12:
				vPitch -= 16;
				break;
			case 10:
				vPitch -= 12;
				break;
			case 8:
				vPitch -= 8;
				break;
			case 6:
				vPitch -= 4;
				break;
			default:
				break;
		}
		int off = this->LockVb(this->m_mergeBuffer.vertexStreamBuffer, this->m_mergeBuffer.numVertices, vPitch,
		                       this->m_mergeBuffer.minIndex);
		if (off == -1) {
			this->m_mergeBuffer.primitiveCount = 0;
			return;
		}
		int hIb = this->m_xhLastIb;
		int hVb = this->m_xhLastVb;
		fnWD3DDevice_SetStreamSource(this->m_pd3dDevice, 0, this->m_pLockableVb, 0, vPitch);
		restoreTm = memcmp(this->m_xLastWorldMatrix, &g_mId, sizeof(D3DXMATRIX)) != 0;
		if (restoreTm) {
			memcpy(&bak, this->m_xLastWorldMatrix, sizeof bak);
			memcpy(this->m_xLastWorldMatrix, &g_mId, sizeof(D3DXMATRIX));
			this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0), this->m_xLastWorldMatrix);
		}
		if (this->ApplyShader()) {
			this->m_customRenderState.Begin(this);
			fnWD3DDevice_DrawIndexedPrimitive(
				this->m_pd3dDevice,
				this->m_mergeBuffer.type,
				off,
				0,
				this->m_mergeBuffer.numVertices + this->m_mergeBuffer.minIndex,
				this->m_mergeBuffer.startIndex,
				this->m_mergeBuffer.primitiveCount
			);
			this->m_xnTotalTriangles += this->m_mergeBuffer.primitiveCount;
			++this->m_xnDIPs;
			this->m_customRenderState.End(this);
		}
		this->m_xhLastVb = 0;
		this->m_xhLastIb = hIb;
		this->XSetTnLBuffer(hVb, hIb);
	} else {
		if (this->m_mergeBuffer.lockVertex) {
			restoreTm = memcmp(this->m_xLastWorldMatrix, &g_mId, sizeof(D3DXMATRIX)) != 0;
			if (restoreTm) {
				memcpy(&bak, this->m_xLastWorldMatrix, sizeof bak);
				memcpy(this->m_xLastWorldMatrix, &g_mId, sizeof(D3DXMATRIX));
				this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0), this->m_xLastWorldMatrix);
			}
			if (this->m_mergeBuffer.lockIndex == 1) {
				this->DrawIndexedPrimitiveLockable(
					this->m_mergeBuffer.type,
					this->m_mergeBuffer.vertexStreamBuffer,
					this->m_mergeBuffer.numVertices,
					this->m_mergeBuffer.vertexStreamZeroStride,
					this->m_mergeBuffer.indexBuffer,
					this->m_mergeBuffer.primitiveCount
				);
			} else {
				this->DrawPrimitiveLockable(
					this->m_mergeBuffer.type,
					this->m_mergeBuffer.primitiveCount,
					this->m_mergeBuffer.vertexStreamBuffer,
					this->m_mergeBuffer.vertexStreamZeroStride
				);
			}
		} else {
			restoreTm = memcmp(this->m_xLastWorldMatrix, &this->m_mergeBuffer.xLastMatrix, sizeof(D3DXMATRIX)) != 0;
			if (restoreTm) {
				memcpy(&bak, this->m_xLastWorldMatrix, sizeof bak);
				memcpy(this->m_xLastWorldMatrix, &this->m_mergeBuffer.xLastMatrix, sizeof(D3DXMATRIX));
				this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0), this->m_xLastWorldMatrix);
			}
			this->XSetTnLBuffer(this->m_mergeBuffer.xhVb, this->m_mergeBuffer.xhIb);
			if (this->ApplyShader()) {
				this->m_customRenderState.Begin(this);
				fnWD3DDevice_DrawIndexedPrimitive(
					this->m_pd3dDevice,
					this->m_mergeBuffer.type,
					0,
					this->m_mergeBuffer.minIndex,
					this->m_mergeBuffer.numVertices,
					this->m_mergeBuffer.startIndex,
					this->m_mergeBuffer.primitiveCount
				);
				this->m_xnTotalTriangles += this->m_mergeBuffer.primitiveCount;
				++this->m_xnDIPs;
				this->m_customRenderState.End(this);
			}
			this->XSetTnLBuffer(this->m_xhLastVb, this->m_xhLastIb);
		}
	}
	if (restoreTm) {
		memcpy(this->m_xLastWorldMatrix, &bak, sizeof(D3DXMATRIX));
		this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0), this->m_xLastWorldMatrix);
	}
	this->m_mergeBuffer.primitiveCount = 0;
}

void WDirect3D8::DrawPrimitive(uint32_t iType, int iNum, uint32_t dwVertexTypeDesc, void* lpvVertices,
                               D3DPRIMITIVETYPE dptPrimitiveType, int iType2) {
	TRACE("iType=%d, iNum=%d, dwVertexTypeDesc=%d, lpvVertices=%p, dptPrimitiveType=%d, iType2=%d", iType, iNum,
	      dwVertexTypeDesc, lpvVertices, dptPrimitiveType, iType2);

	if (this->m_devState == WDeviceStateLost) {
		return;
	}
	if (memcmp(this->m_xLastWorldMatrix, &g_mId, sizeof(D3DXMATRIX)) != 0) {
		memcpy(this->m_xLastWorldMatrix, &g_mId, sizeof(D3DXMATRIX));
		if (!this->IsSupportVs()) {
			this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0), &g_mId);
		}
	}
	this->SetVtxType(iType, iType2 | 4, dwVertexTypeDesc, 0xFFFFFFFF);
	int primitiveCount;
	switch (dptPrimitiveType) {
		case D3DPT_LINELIST:
			primitiveCount = iNum / 2;
			break;
		case D3DPT_LINESTRIP:
			primitiveCount = iNum - 1;
			break;
		case D3DPT_TRIANGLELIST:
			primitiveCount = iNum / 3;
			break;
		case D3DPT_TRIANGLESTRIP:
		case D3DPT_TRIANGLEFAN:
			primitiveCount = iNum - 2;
			break;
		default:
			primitiveCount = iNum;
			break;
	}
	this->DrawPrimitiveUp(dptPrimitiveType, primitiveCount, lpvVertices, this->m_vtxSize);
}

void WDirect3D8::DrawIndexedPrimitiveUp(D3DPRIMITIVETYPE type, UINT numVertices, UINT primitiveCount,
                                        const void* pIndexData, const void* pVertexStreamZeroData,
                                        UINT vertexStreamZeroStride) {
	TRACE(
		"Type=%d, NumVertices=%d, PrimitiveCount=%d, pIndexData=%p, pVertexStreamZeroData=%p, VertexStreamZeroStride=%p",
		type, numVertices, primitiveCount, pIndexData, pVertexStreamZeroData, vertexStreamZeroStride);
	if (this->m_devState == WDeviceStateLost) {
		return;
	}
	if (type == 4) {
		this->DrawIndexedPrimitiveLockable(
			D3DPT_TRIANGLELIST,
			pVertexStreamZeroData,
			numVertices,
			vertexStreamZeroStride,
			(uint16_t*)pIndexData,
			primitiveCount
		);
	} else {
		this->m_customRenderState.Begin(this);
		this->m_pd3dDevice->DrawIndexedPrimitiveUP(
			type,
			0,
			numVertices,
			primitiveCount,
			pIndexData,
			D3DFMT_INDEX16,
			pVertexStreamZeroData,
			vertexStreamZeroStride
		);
		++this->m_xnDIPUPs;
		this->m_xnTotalTriangles += primitiveCount;
		this->m_customRenderState.End(this);
	}
}

void WDirect3D8::DrawPrimitiveIndexed(int iType, uint32_t dwVertexTypeDesc, void* lpvVertices, int pNum,
                                      uint16_t* fList,
                                      int fNum, int iType2) {
	TRACE("iType=%d, dwVertexTypeDesc=%d, lpvVertices=%p, pNum=%d, fList=%p, fNum=%d, iType2=%d", iType,
	      dwVertexTypeDesc, lpvVertices, pNum, fList, fNum, iType2);

	if (this->m_devState == WDeviceStateLost) {
		return;
	}
	if (memcmp(this->m_xLastWorldMatrix, &g_mId, 64) != 0) {
		memcpy(this->m_xLastWorldMatrix, &g_mId, sizeof(D3DXMATRIX));
		if (!this->IsSupportVs()) {
			this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0), &g_mId);
		}
	}
	this->SetVtxType(iType, iType2 | 4, dwVertexTypeDesc, 0xFFFFFFFF);
	if (this->m_devState == WDeviceStateLost) {
		return;
	}
	this->DrawIndexedPrimitiveLockable(D3DPT_TRIANGLELIST, lpvVertices, pNum, this->m_vtxSize, fList, fNum / 3);
}

void WDirect3D8::XDrawIndexedPrimitive(const WxViewState& viewState, const WxBatchState& batchState) {
	TRACE("Called");

	int primitiveCount;
	D3DXMATRIX mTmp;
	D3DXMATRIX mInv;
	D3DXMATRIX m;

	if (this->m_devState == WDeviceStateLost) {
		return;
	}
	if (!(batchState.xiFlag1 & 0x14)) {
		this->XSetLight(0, viewState.xLight);
	}
	if (this->m_devState != WDeviceStateLost && memcmp(&this->m_xLastViewMatrix, &viewState.xmView, sizeof(D3DXMATRIX))
	) {
		this->FlushRenderPrimitive();
		memcpy(&this->m_xLastViewMatrix, &viewState.xmView, sizeof this->m_xLastViewMatrix);
		this->m_pd3dDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&viewState.xmView);
	}
	if (this->m_devState != WDeviceStateLost && memcmp(&this->m_xLastProjMatrix, &viewState.xmProj, sizeof(D3DXMATRIX))
	) {
		this->FlushRenderPrimitive();
		memcpy(&this->m_xLastProjMatrix, &viewState.xmProj, sizeof this->m_xLastProjMatrix);
		this->m_pd3dDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&viewState.xmProj);
	}
	if (batchState.xnmTransfs) {
		for (int i = 0; i < batchState.xnmTransfs; i++) {
			SetD3DMatrixFromWMatrix(&m, batchState.xpapmW[i]);
			if (batchState.xpapmO) {
				SetD3DMatrixFromWMatrix(&mInv, batchState.xpapmO[i]);
				D3DXMatrixInverse(&mInv, nullptr, &mInv);
				D3DXMatrixMultiply(&mTmp, &mInv, &m);
				memcpy(&m, &mTmp, sizeof m);
			}
			this->_SetTransform(D3DTS_WORLDMATRIX(i), m);
		}
	} else {
		SetD3DMatrixFromWMatrix(&m, &batchState.xmW);
		if (this->m_devState != WDeviceStateLost) {
			if (memcmp(this->m_xLastWorldMatrix, &m, sizeof(D3DXMATRIX)) != 0) {
				memcpy(this->m_xLastWorldMatrix, &m, sizeof(D3DXMATRIX));
				if (!this->IsSupportVs()) {
					this->m_pd3dDevice->SetTransform(D3DTS_WORLDMATRIX(0), &m);
				}
			}
		}
	}
	int vtxType = batchState.xiFlag0;
	if (!this->m_fog) {
		vtxType &= ~0x8000000u;
	}
	this->SetVtxType(vtxType, batchState.xiFlag1, this->m_xaVbList[batchState.xiFlag1 >> 22].xdwFVF,
	                 batchState.xdwDiffuse);
	switch (batchState.xiFlag1 & 3) {
		case 0:
			primitiveCount = batchState.xNumIndices / 3;
			g_primitiveType = D3DPT_TRIANGLELIST;
			g_nTriangles = batchState.xNumIndices / 3;
			break;
		case 1:
			primitiveCount = batchState.xNumIndices - 2;
			g_primitiveType = D3DPT_TRIANGLESTRIP;
			g_nTriangles = primitiveCount;
			break;
		case 2:
			primitiveCount = batchState.xNumIndices;
			g_primitiveType = D3DPT_POINTLIST;
			g_nTriangles = primitiveCount;
			break;
		default:
			primitiveCount = g_nTriangles;
			break;
	}
	this->DrawIndexedPrimitive(g_primitiveType, batchState.xiBaseVtxIdx, batchState.xNumVertices,
	                           batchState.xiBaseIdxIdx, primitiveCount);
}

void WDirect3D8::BeginUsingCustomRenderState() {
	TRACE("Called");

	this->FlushRenderPrimitive();
	if (!this->m_customRenderState.HasAnyState()) {
		return;
	}
	this->m_customRenderStateBackupList.push(this->m_customRenderState);
	this->m_customRenderState.Clear();
}

void* WDirect3D8::SnapShotCustomRenderState() {
	TRACE("Called");

	if (!this->m_customRenderState.HasAnyState()) {
		return nullptr;
	}

	if (!this->m_customRenderStateSnapShotList.empty()) {
		if (this->m_customRenderState != this->m_customRenderStateSnapShotList.back()) {
			this->m_customRenderStateSnapShotList.push_back(this->m_customRenderState);
		}
		return static_cast<void*>(&this->m_customRenderStateSnapShotList.back());
	}
	this->m_customRenderStateSnapShotList.push_back(this->m_customRenderState);
	return static_cast<void*>(&this->m_customRenderStateSnapShotList.back());
}

void WDirect3D8::EndUsingCustomRenderState() {
	TRACE("Called");

	if (this->m_customRenderState.HasAnyState()) {
		this->FlushRenderPrimitive();
		this->m_customRenderState.Clear();
	}

	if (this->m_customRenderStateBackupList.empty()) {
		return;
	}

	this->m_customRenderState = this->m_customRenderStateBackupList.top();
	this->m_customRenderStateBackupList.pop();
}

void WDirect3D8::ClearCustomRenderStateSnapShotList() {
	TRACE("Called");

	this->m_customRenderStateSnapShotList.clear();
}

WProc* WDirect3D8::ExternProc() {
	TRACE("Called");

	return this;
}

void WDirect3D8::DrawLine(WtVertex** p, int type) {
	TRACE("Called");
}

void WDirect3D8::SetGlobalRenderState(int rs, int rsEx) {
	TRACE("rs=%d, rsex=%d", rs, rsEx);

	this->m_globalRs[0] = rs;
	this->m_globalRs[1] = rsEx;
}

bool WDirect3D8::IsSupportVs() {
	TRACE("result=%d", this->m_d3dCaps.VertexShaderVersion >= 0xFFFE0101);

	return this->m_d3dCaps.VertexShaderVersion >= 0xFFFE0101;
}

bool WDirect3D8::IsSupportPs() {
	TRACE("result=%d", this->m_d3dCaps.PixelShaderVersion >= 0xFFFF0200);

	return this->m_d3dCaps.PixelShaderVersion >= 0xFFFF0200;
}

bool WDirect3D8::IsSupportMrt() {
	TRACE("result=%d", this->m_d3dCaps.NumSimultaneousRTs > 1);

	return this->m_d3dCaps.NumSimultaneousRTs > 1;
}

bool WDirect3D8::IsSupportClipPlane() {
	TRACE("result=%d", this->m_d3dCaps.MaxUserClipPlanes >= 1);
	return this->m_d3dCaps.MaxUserClipPlanes >= 1;
}

int WDirect3D8::GetWidth() const {
	TRACE("result=%d", this->m_d3dpp.BackBufferWidth);

	return this->m_d3dpp.BackBufferWidth;
}

int WDirect3D8::GetHeight() const {
	TRACE("result=%d", this->m_d3dpp.BackBufferHeight);

	return this->m_d3dpp.BackBufferHeight;
}

bool WDirect3D8::IsWindowed() {
	TRACE("result=%d", this->m_d3dpp.Windowed != 0);

	return this->m_d3dpp.Windowed != 0;
}

bool WDirect3D8::IsFillScreenMode() {
	TRACE("result=%d", this->m_fillScrMode);

	return this->m_fillScrMode;
}

float WDirect3D8::GetMonitorSupportFps() const {
	TRACE("result=%d", this->m_fps);

	return this->m_fps;
}

bool WDirect3D8::SupportRenderTargetFormat() {
	TRACE("result=%d", this->m_rtFmtIdx >= 0);

	return this->m_rtFmtIdx >= 0;
}

const char* WDirect3D8::GetDeviceName() {
	TRACE("Called");
	if (this->m_devName) {
		return this->m_devName;
	}
	return "";
}
