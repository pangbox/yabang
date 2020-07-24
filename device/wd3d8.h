#pragma once

#include "wd3d.h"
#include "screencape.h"
#include <wproc.h>
#include <wutil.h>
#include <bitmap.h>

#include <d3d9.h>
#include <d3dx9math.h>

#include <string>
#include <list>
#include <stack>
#include <vector>
#include <map>
#include <cstdint>

class WDirect3D8;
class WSplashD3D;

enum WindowsVersion {
	WinVerNone,
	WinVerOld,
	WinVerWindowsNt,
	WinVerWindows2000,
	WinVerWindowsXp,
	WinVerWindowsVista,
	WinVerWindows7,
};

WindowsVersion GetWindowsVersion();

namespace WindowUtility {
	typedef HRESULT (WINAPI *IsCompositionEnabledPtr)(BOOL*);
	typedef HRESULT (WINAPI *EnableCompositionPtr)(int);

	class DwmApiDll {
	public:
		DwmApiDll();
		~DwmApiDll();

		HMODULE LoadDwmApiDll();
		BOOL IsCompositionEnabled() const;
		HRESULT EnableComposition(BOOL bEnable);

	private:
		HMODULE m_hDWMApi;
		IsCompositionEnabledPtr m_fIsEnabled;
		EnableCompositionPtr m_fEnable;
	};
};

struct sCaptureOption {
	enum eCaptureMode {
		CM_A = 0x0,
		CM_B = 0x1,
		CM_Q = 0x2,
	};

	void SetMode(eCaptureMode mode, bool windowed);

	eCaptureMode currentMode;
	bool useMemCopy;
	bool updateWholeScreen;
	bool dxCopyRectsOK;
	bool useTexture;
	D3DSWAPEFFECT swapEffect;
	unsigned int fullScreenPresentationInterval;
};

class WSplashD3D : public WSplash {
public:
	WSplashD3D(WDirect3D8* pDriver);
	~WSplashD3D();
	void Init(tagBITMAPINFO* bi, void* data, bool fitToScreen);
	int InitFromScreen(bool bCopy) override;
	void Reset() override;
	void Draw(const WPoint* pSrc, const WRect* pDest, uint32_t color) override;
	void Draw(uint32_t color) override;
	void ResetScreenSize() override;
	int GetWidth() override;
	int GetHeight() override;
	void SetTextureCoordinateOffset(float u, float v) override;

private:
	WDirect3D8* m_pDriver;
	int m_hTexs[64];
	tagRECT m_rects[64];
	WVector2D m_uvs[64];
	WVector2D m_offset;
	int m_iBufWidth;
	int m_iBufHeight;
	int m_iSrcWidth;
	int m_iSrcHeight;
	int m_nSurfCount;
};


class WDirect3D8 : public WDirect3D, public WProc {
	friend class WDirect3D8KR645;
	friend class WDirect3D8US852;
public:
	struct RtBackup {
		struct Rt {
			IDirect3DSurface9* surf[2] = {nullptr, nullptr};
		};

		Rt rt = {};
		IDirect3DSurface9* depthSurf = nullptr;
		D3DVIEWPORT9 viewport = {};
	};

	struct PixInfo {
		D3DFORMAT pixFmt;
		uint32_t cpp;
		uint32_t r_r_shift;
		uint32_t r_l_shift;
		uint32_t g_r_shift;
		uint32_t g_l_shift;
		uint32_t b_r_shift;
		uint32_t b_l_shift;
		uint32_t a_r_shift;
		uint32_t a_l_shift;

		uint32_t Pack(BYTE r, BYTE g, BYTE b) const {
			return static_cast<uint32_t>(r) >> LOBYTE(this->r_r_shift) << this->r_l_shift |
				static_cast<uint32_t>(g) >> LOBYTE(this->g_r_shift) << this->g_l_shift |
				static_cast<uint32_t>(b) >> LOBYTE(this->b_r_shift) << this->b_l_shift;
		}

		uint32_t Pack(BYTE a, BYTE r, BYTE g, BYTE b) const {
			return static_cast<uint32_t>(r) >> LOBYTE(this->r_r_shift) << this->r_l_shift |
				static_cast<uint32_t>(g) >> LOBYTE(this->g_r_shift) << this->g_l_shift |
				static_cast<uint32_t>(b) >> LOBYTE(this->b_r_shift) << this->b_l_shift |
				static_cast<uint32_t>(a) >> LOBYTE(this->a_r_shift) << this->a_l_shift;
		}
	};

	struct Effect {
		struct Param {
			const char* handle = nullptr;
			bool isShared = false;
		};

		ID3DXEffect* pEffect = nullptr;
		Param param[21] = {};
	};

	struct MergeBuffer {
		D3DPRIMITIVETYPE type = D3DPT_POINTLIST;
		unsigned int numVertices = 0;
		unsigned int primitiveCount = 0;
		unsigned int vertexStreamZeroStride = 0;
		int xhIb = 0;
		int xhVb = 0;
		unsigned int minIndex = 0;
		unsigned int startIndex = 0;
		int lockVertex = 0;
		int lockIndex = 0;
		D3DXMATRIX xLastMatrix;
		uint8_t* vertexStreamBuffer = nullptr;
		unsigned int vertexStreamBufferSize = 0;
		uint16_t* indexBuffer = nullptr;
		unsigned int indexBufferSize = 0;

		MergeBuffer() {
			D3DXMatrixIdentity(&xLastMatrix);
		}

		void CheckAndIncreaseVertexStreamBuffer(unsigned int size) {
			if (this->vertexStreamBufferSize < size) {
				this->vertexStreamBufferSize = size;
				auto* buffer = static_cast<uint8_t*>(realloc(this->vertexStreamBuffer, size));
				if (buffer) {
					this->vertexStreamBuffer = buffer;
				} else {
					throw std::exception("realloc failed");
				}
			}
		}

		void CheckAndIncreaseIndexBuffer(unsigned int size) {
			if (this->indexBufferSize < size) {
				this->indexBufferSize = size;
				auto* buffer = static_cast<uint16_t*>(realloc(this->indexBuffer, size));
				if (buffer) {
					this->indexBuffer = buffer;
				} else {
					throw std::exception("realloc failed");
				}
			}
		}
	};

	struct WFxParamPool {
		typedef std::map<const char*, int> StringIntMap;
		typedef std::map<const char*, D3DXVECTOR2> StringVec2Map;
		typedef std::map<const char*, D3DXVECTOR3> StringVec3Map;
		typedef std::map<const char*, D3DXVECTOR4> StringVec4Map;
		typedef std::map<const char*, D3DXMATRIX> StringMatrixMap;
		typedef std::map<const char*, IDirect3DBaseTexture9*> StringTexMap;

		typedef std::map<WFxParameterType, int> ParamIntMap;
		typedef std::map<WFxParameterType, D3DXVECTOR2> ParamVec2Map;
		typedef std::map<WFxParameterType, D3DXVECTOR3> ParamVec3Map;
		typedef std::map<WFxParameterType, D3DXVECTOR4> ParamVec4Map;
		typedef std::map<WFxParameterType, D3DXMATRIX> ParamMatrixMap;
		typedef std::map<WFxParameterType, IDirect3DBaseTexture9*> ParamTexMap;

		typedef std::pair<ParamIntMap, StringIntMap> IntCachePair;
		typedef std::pair<ParamVec2Map, StringVec2Map> Vec2CachePair;
		typedef std::pair<ParamVec3Map, StringVec3Map> Vec3CachePair;
		typedef std::pair<ParamVec4Map, StringVec4Map> Vec4CachePair;
		typedef std::pair<ParamMatrixMap, StringMatrixMap> MatrixCachePair;
		typedef std::pair<ParamTexMap, StringTexMap> TexCachePair;

		ID3DXEffectPool* pool;
		IntCachePair paramCacheListPairInt;
		Vec2CachePair paramCacheListPairVec2;
		Vec3CachePair paramCacheListPairVec3;
		Vec4CachePair paramCacheListPairVec4;
		MatrixCachePair paramCacheListPairMat;
		TexCachePair paramCacheListPairTex;

		WFxParamPool() {
			this->pool = nullptr;
		}

		[[nodiscard]] ID3DXEffectPool* GetPool() const {
			return this->pool;
		}

		void GetInt(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared, int* Value) {
			if (!ef || !hParam) {
				return;
			}
			if (isShared) {
				auto it = this->paramCacheListPairInt.first.find(wvdFxParamType);
				if (it != this->paramCacheListPairInt.first.end()) {
					*Value = it->second;
					return;
				}
			} else {
				auto it = this->paramCacheListPairInt.second.find(hParam);
				if (it != this->paramCacheListPairInt.second.end()) {
					*Value = it->second;
					return;
				}
			}
			ef->GetInt(hParam, Value);
		}

		void GetVector2(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared,
		                D3DXVECTOR2* Value) {
			if (!ef || !hParam) {
				return;
			}
			if (isShared) {
				auto it = this->paramCacheListPairVec2.first.find(wvdFxParamType);
				if (it != this->paramCacheListPairVec2.first.end()) {
					*Value = it->second;
					return;
				}
			} else {
				auto it = this->paramCacheListPairVec2.second.find(hParam);
				if (it != this->paramCacheListPairVec2.second.end()) {
					*Value = it->second;
					return;
				}
			}
			ef->GetFloatArray(hParam, reinterpret_cast<FLOAT*>(Value), 2);
		}

		void GetVector3(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared,
		                D3DXVECTOR3* Value) {
			if (!ef || !hParam) {
				return;
			}
			if (isShared) {
				auto it = this->paramCacheListPairVec3.first.find(wvdFxParamType);
				if (it != this->paramCacheListPairVec3.first.end()) {
					*Value = it->second;
					return;
				}
			} else {
				auto it = this->paramCacheListPairVec3.second.find(hParam);
				if (it != this->paramCacheListPairVec3.second.end()) {
					*Value = it->second;
					return;
				}
			}
			ef->GetFloatArray(hParam, reinterpret_cast<FLOAT*>(Value), 3);
		}

		void GetVector4(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared,
		                D3DXVECTOR4* Value) {
			if (!ef || !hParam) {
				return;
			}
			if (isShared) {
				auto it = this->paramCacheListPairVec4.first.find(wvdFxParamType);
				if (it != this->paramCacheListPairVec4.first.end()) {
					*Value = it->second;
					return;
				}
			} else {
				auto it = this->paramCacheListPairVec4.second.find(hParam);
				if (it != this->paramCacheListPairVec4.second.end()) {
					*Value = it->second;
					return;
				}
			}
			ef->GetVector(hParam, Value);
		}

		void GetMatrix(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared,
		               D3DXMATRIX* Value) {
			if (!ef || !hParam) {
				return;
			}
			if (isShared) {
				auto it = this->paramCacheListPairMat.first.find(wvdFxParamType);
				if (it != this->paramCacheListPairMat.first.end()) {
					*Value = it->second;
					return;
				}
			} else {
				auto it = this->paramCacheListPairMat.second.find(hParam);
				if (it != this->paramCacheListPairMat.second.end()) {
					*Value = it->second;
					return;
				}
			}
			ef->GetMatrix(hParam, Value);
		}

		void GetTexture(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared,
		                IDirect3DBaseTexture9** Value) {
			if (!ef || !hParam) {
				return;
			}
			if (isShared) {
				auto it = this->paramCacheListPairTex.first.find(wvdFxParamType);
				if (it != this->paramCacheListPairTex.first.end()) {
					*Value = it->second;
					return;
				}
			} else {
				auto it = this->paramCacheListPairTex.second.find(hParam);
				if (it != this->paramCacheListPairTex.second.end()) {
					*Value = it->second;
					return;
				}
			}
			ef->GetTexture(hParam, Value);
		}

		void ClearParamCache() {
			TRACE("Clearing Int caches.");
			this->paramCacheListPairInt.first.clear();
			this->paramCacheListPairInt.second.clear();
			TRACE("Clearing Vec2 caches.");
			this->paramCacheListPairVec2.first.clear();
			this->paramCacheListPairVec2.second.clear();
			TRACE("Clearing Vec3 caches.");
			this->paramCacheListPairVec3.first.clear();
			this->paramCacheListPairVec3.second.clear();
			TRACE("Clearing Vec4 caches.");
			this->paramCacheListPairVec4.first.clear();
			this->paramCacheListPairVec4.second.clear();
			TRACE("Clearing Mat caches.");
			this->paramCacheListPairMat.first.clear();
			this->paramCacheListPairMat.second.clear();
			TRACE("Clearing Tex caches.");
			this->paramCacheListPairTex.first.clear();
			this->paramCacheListPairTex.second.clear();
		}

		void Release() {
			this->ClearParamCache();
			if (this->pool) {
				TRACE("Releasing effect pool.");
				this->pool->Release();
				this->pool = nullptr;
			}
		}

		void SetInt(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared, int Value) {
			if (isShared) {
				auto it = this->paramCacheListPairInt.first.find(wvdFxParamType);
				if (it == this->paramCacheListPairInt.first.end()) {
					this->paramCacheListPairInt.first.insert(std::pair<WFxParameterType, int>(wvdFxParamType, Value));
				} else {
					if (it->second != Value) {
						it->second = Value;
					}
				}
				ef->SetInt(static_cast<D3DXHANDLE>(hParam), Value);
			} else {
				auto it = this->paramCacheListPairInt.second.find(hParam);
				if (it == this->paramCacheListPairInt.second.end()) {
					this->paramCacheListPairInt.second.insert(std::pair<const char*, int>(hParam, Value));
				} else {
					if (it->second != Value) {
						it->second = Value;
					}
				}
				ef->SetInt(static_cast<D3DXHANDLE>(hParam), Value);
			}
		}

		void SetVector2(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared,
		                const D3DXVECTOR2* Value) {
			if (isShared) {
				auto it = this->paramCacheListPairVec2.first.find(wvdFxParamType);
				if (it == this->paramCacheListPairVec2.first.end()) {
					this->paramCacheListPairVec2.first.insert(
						std::pair<WFxParameterType, D3DXVECTOR2>(wvdFxParamType, *Value));
				} else {
					if (memcmp(&it->second, Value, sizeof(D3DXVECTOR2))) {
						memcpy(&it->second, Value, sizeof(D3DXVECTOR2));
					}
				}
				ef->SetFloatArray(static_cast<D3DXHANDLE>(hParam), (FLOAT*)Value, 2);
			} else {
				auto it = this->paramCacheListPairVec2.second.find(hParam);
				if (it == this->paramCacheListPairVec2.second.end()) {
					this->paramCacheListPairVec2.second.insert(std::pair<const char*, D3DXVECTOR2>(hParam, *Value));
				} else {
					if (memcmp(&it->second, Value, sizeof(D3DXVECTOR2))) {
						memcpy(&it->second, Value, sizeof(D3DXVECTOR2));
					}
				}
				ef->SetFloatArray(static_cast<D3DXHANDLE>(hParam), (FLOAT*)Value, 2);
			}
		}

		void SetVector3(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared,
		                const D3DXVECTOR3* Value) {
			if (isShared) {
				auto it = this->paramCacheListPairVec3.first.find(wvdFxParamType);
				if (it == this->paramCacheListPairVec3.first.end()) {
					this->paramCacheListPairVec3.first.insert(
						std::pair<WFxParameterType, D3DXVECTOR3>(wvdFxParamType, *Value));
				} else {
					if (memcmp(&it->second, Value, sizeof(D3DXVECTOR3))) {
						memcpy(&it->second, Value, sizeof(D3DXVECTOR3));
					}
				}
				ef->SetFloatArray(static_cast<D3DXHANDLE>(hParam), (FLOAT*)Value, 3);
			} else {
				auto it = this->paramCacheListPairVec3.second.find(hParam);
				if (it == this->paramCacheListPairVec3.second.end()) {
					this->paramCacheListPairVec3.second.insert(std::pair<const char*, D3DXVECTOR3>(hParam, *Value));
				} else {
					if (memcmp(&it->second, Value, sizeof(D3DXVECTOR3))) {
						memcpy(&it->second, Value, sizeof(D3DXVECTOR3));
					}
				}
				ef->SetFloatArray(static_cast<D3DXHANDLE>(hParam), (FLOAT*)Value, 3);
			}
		}

		void SetVector4(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared,
		                const D3DXVECTOR4* Value) {
			if (isShared) {
				auto it = this->paramCacheListPairVec4.first.find(wvdFxParamType);
				if (it == this->paramCacheListPairVec4.first.end()) {
					this->paramCacheListPairVec4.first.insert(
						std::pair<WFxParameterType, D3DXVECTOR4>(wvdFxParamType, *Value));
				} else {
					if (memcmp(&it->second, Value, sizeof(D3DXVECTOR4))) {
						memcpy(&it->second, Value, sizeof(D3DXVECTOR4));
					}
				}
				ef->SetVector(static_cast<D3DXHANDLE>(hParam), Value);
			} else {
				auto it = this->paramCacheListPairVec4.second.find(hParam);
				if (it == this->paramCacheListPairVec4.second.end()) {
					this->paramCacheListPairVec4.second.insert(std::pair<const char*, D3DXVECTOR4>(hParam, *Value));
				} else {
					if (memcmp(&it->second, Value, sizeof(D3DXVECTOR4))) {
						memcpy(&it->second, Value, sizeof(D3DXVECTOR4));
					}
				}
				ef->SetVector(static_cast<D3DXHANDLE>(hParam), Value);
			}
		}

		void SetMatrix(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared,
		               const D3DXMATRIX* Value) {
			if (isShared) {
				auto it = this->paramCacheListPairMat.first.find(wvdFxParamType);
				if (it == this->paramCacheListPairMat.first.end()) {
					this->paramCacheListPairMat.first.insert(
						std::pair<WFxParameterType, D3DXMATRIX>(wvdFxParamType, *Value));
				} else {
					if (memcmp(&it->second, Value, sizeof(D3DXMATRIX))) {
						memcpy(&it->second, Value, sizeof(D3DXMATRIX));
					}
				}
				ef->SetMatrix(static_cast<D3DXHANDLE>(hParam), Value);
			} else {
				auto it = this->paramCacheListPairMat.second.find(hParam);
				if (it == this->paramCacheListPairMat.second.end()) {
					this->paramCacheListPairMat.second.insert(std::pair<const char*, D3DXMATRIX>(hParam, *Value));
				} else {
					if (memcmp(&it->second, Value, sizeof(D3DXMATRIX))) {
						memcpy(&it->second, Value, sizeof(D3DXMATRIX));
					}
				}
				ef->SetMatrix(static_cast<D3DXHANDLE>(hParam), Value);
			}
		}

		void SetTexture(ID3DXEffect* ef, WFxParameterType wvdFxParamType, const char* hParam, bool isShared,
		                IDirect3DBaseTexture9* Value) {
			if (isShared) {
				auto it = this->paramCacheListPairTex.first.find(wvdFxParamType);
				if (it == this->paramCacheListPairTex.first.end()) {
					this->paramCacheListPairTex.first.insert(
						std::pair<WFxParameterType, IDirect3DBaseTexture9*>(wvdFxParamType, Value));
				} else {
					if (it->second != Value) {
						it->second = Value;
					}
				}
				ef->SetTexture(static_cast<D3DXHANDLE>(hParam), Value);
			} else {
				auto it = this->paramCacheListPairTex.second.find(hParam);
				if (it == this->paramCacheListPairTex.second.end()) {
					this->paramCacheListPairTex.second.insert(
						std::pair<const char*, IDirect3DBaseTexture9*>(hParam, Value));
				} else {
					if (it->second != Value) {
						it->second = Value;
					}
				}
				ef->SetTexture(static_cast<D3DXHANDLE>(hParam), Value);
			}
		}
	};

	template <typename T, typename A>
	struct TwoVarRenderState {
		TwoVarRenderState() {
			memset(&this->m_Type, 0, sizeof(this->m_Type));
			memset(&this->m_Value, 0, sizeof(this->m_Value));
			memset(&this->m_OldValue, 0, sizeof(this->m_OldValue));
		}

		TwoVarRenderState(const T& Type, const A& Value) {
			this->m_Type = Type;
			this->m_Value = Value;
			memset(&this->m_OldValue, 0, sizeof(this->m_OldValue));
		}

		virtual ~TwoVarRenderState() {}

		T m_Type;
		A m_Value, m_OldValue;
	};

	template <typename T1, typename T2, typename A>
	struct ThreeVarRenderState : public TwoVarRenderState<T1, A> {
		ThreeVarRenderState() {
			memset(&this->m_Type2, 0, sizeof(this->m_Type2));
		}

		ThreeVarRenderState(const T1& Type1, const T2& Type2, const A& Value) : TwoVarRenderState<T1, A>(Type1, Value) {
			this->m_Type2 = Type2;
		}

		T2 m_Type2;
	};

	struct ClipPlane : public TwoVarRenderState<uint32_t, D3DXPLANE> {
		D3DXPLANE m_SetValue;

		ClipPlane(uint32_t Index, D3DXPLANE Value) : TwoVarRenderState<uint32_t, D3DXPLANE>(Index, Value) {}
	};

	struct D3D8Texture {
		Bitmap* bitmap;
		int dxtcDataSize;
		char* dxtcData;
		int updateType;
		bool needToBeFilled;
		IDirect3DTexture9* pTex;
		IDirect3DSurface9* pSurf;
		int width;
		int height;
		int mipmaplevel;
		PixInfo* pixFmtInfo;
		int renderTargetType;
		WRenderToTextureSizeInfo renderTargetSizeInfo;
		bool isFilled;

		D3D8Texture() {
			this->bitmap = nullptr;
			this->dxtcDataSize = 0;
			this->dxtcData = nullptr;
			this->updateType = 0;
			this->needToBeFilled = false;
			this->pTex = nullptr;
			this->pSurf = nullptr;
			this->width = 0;
			this->height = 0;
			this->mipmaplevel = 0;
			this->pixFmtInfo = nullptr;
			this->renderTargetType = 0;
			this->isFilled = false;
			this->renderTargetSizeInfo.isAbsolute = true;
			this->renderTargetSizeInfo.width = 0.0;
			this->renderTargetSizeInfo.height = 0.0;
		}
	};

	struct sDepthSurf {
		WORD wWidth;
		WORD wHeight;
		IDirect3DSurface9* pDepth;
	};

	struct sVb8 {
		unsigned int xdwFVF = 0;
		unsigned int xdwUsage = 0;
		uint8_t xbStride = 0;
		IDirect3DVertexBuffer9* xpVb = nullptr;
		int xnVtxs = 0;
		bool xbNeedToBeFilled = false;
		uint8_t* xpVertexData = nullptr;
	};

	struct sIb8 {
		unsigned int xdwUsage = 0;
		IDirect3DIndexBuffer9* xpIb = nullptr;
		int xnIdxs = 0;
		bool xbNeedToBeFilled = false;
		uint16_t* xpIndexData = nullptr;
	};

	struct sVertexDecl {
		IDirect3DVertexDeclaration9* pDecl;
		int pitch;
	};

	class WRenderState {
	private:
		template <typename T>
		static bool listSzEq(const T& a, const T& b) {
			return a.size() != b.size();
		}

		template <typename T>
		static bool rs2ListEq(const T& a, const T& b) {
			for (class T::const_iterator i = a.begin(); i != a.end(); ++i) {
				for (class T::const_iterator j = b.begin(); j != b.end(); ++j) {
					if (i->m_Type == j->m_Type) {
						if (i->m_Value != j->m_Value) {
							return false;
						}
						goto NextKey;
					}
				}
				return false;
			NextKey:
				continue;
			}
			return true;
		}

		template <typename T>
		static bool rs3ListEq(const T& a, const T& b) {
			for (class T::const_iterator i = a.begin(); i != a.end(); ++i) {
				for (class T::const_iterator j = b.begin(); j != b.end(); ++j) {
					if (i->m_Type == j->m_Type && i->m_Type2 == j->m_Type2) {
						if (i->m_Value != j->m_Value) {
							return false;
						}
						goto NextKey;
					}
				}
				return false;
			NextKey:
				continue;
			}
			return true;
		}

	public:
		std::vector<TwoVarRenderState<D3DRENDERSTATETYPE, DWORD>> m_rsList;
		std::vector<ThreeVarRenderState<DWORD, D3DTEXTURESTAGESTATETYPE, DWORD>> m_tssList;
		std::vector<TwoVarRenderState<D3DTRANSFORMSTATETYPE, D3DXMATRIX>> m_transformList;
		std::vector<TwoVarRenderState<DWORD, IDirect3DBaseTexture9*>> m_texList;
		std::vector<ClipPlane> m_clipPlaneList;
		std::vector<ThreeVarRenderState<DWORD, D3DSAMPLERSTATETYPE, DWORD>> m_ssList;
		unsigned int m_fxMacro;
		std::vector<TwoVarRenderState<WFxParameterType, int>> m_fxParamIntList;
		std::vector<TwoVarRenderState<WFxParameterType, D3DXVECTOR2>> m_fxParamVec2List;
		std::vector<TwoVarRenderState<WFxParameterType, D3DXVECTOR3>> m_fxParamVec3List;
		std::vector<TwoVarRenderState<WFxParameterType, D3DXVECTOR4>> m_fxParamVec4List;
		std::vector<TwoVarRenderState<WFxParameterType, D3DXMATRIX>> m_fxParamMatList;
		std::vector<TwoVarRenderState<WFxParameterType, IDirect3DBaseTexture9*>> m_fxParamTexList;

		WRenderState() {
			this->m_fxMacro = 0;
		}

		bool operator==(const WRenderState& rhs) {
			if (this->m_fxMacro != rhs.m_fxMacro) {
				return false;
			}

			if (!listSzEq(this->m_rsList, rhs.m_rsList)) {
				return false;
			}
			if (!listSzEq(this->m_tssList, rhs.m_tssList)) {
				return false;
			}
			if (!listSzEq(this->m_transformList, rhs.m_transformList)) {
				return false;
			}
			if (!listSzEq(this->m_texList, rhs.m_texList)) {
				return false;
			}
			if (!listSzEq(this->m_clipPlaneList, rhs.m_clipPlaneList)) {
				return false;
			}
			if (!listSzEq(this->m_ssList, rhs.m_ssList)) {
				return false;
			}
			if (!listSzEq(this->m_fxParamIntList, rhs.m_fxParamIntList)) {
				return false;
			}
			if (!listSzEq(this->m_fxParamVec2List, rhs.m_fxParamVec2List)) {
				return false;
			}
			if (!listSzEq(this->m_fxParamVec3List, rhs.m_fxParamVec3List)) {
				return false;
			}
			if (!listSzEq(this->m_fxParamVec4List, rhs.m_fxParamVec4List)) {
				return false;
			}
			if (!listSzEq(this->m_fxParamMatList, rhs.m_fxParamMatList)) {
				return false;
			}
			if (!listSzEq(this->m_fxParamTexList, rhs.m_fxParamTexList)) {
				return false;
			}
			if (!rs2ListEq(this->m_rsList, rhs.m_rsList)) {
				return false;
			}
			if (!rs3ListEq(this->m_tssList, rhs.m_tssList)) {
				return false;
			}
			if (!rs2ListEq(this->m_transformList, rhs.m_transformList)) {
				return false;
			}
			if (!rs2ListEq(this->m_texList, rhs.m_texList)) {
				return false;
			}
			if (!rs2ListEq(this->m_clipPlaneList, rhs.m_clipPlaneList)) {
				return false;
			}
			if (!rs3ListEq(this->m_ssList, rhs.m_ssList)) {
				return false;
			}
			if (!rs2ListEq(this->m_fxParamIntList, rhs.m_fxParamIntList)) {
				return false;
			}
			if (!rs2ListEq(this->m_fxParamVec2List, rhs.m_fxParamVec2List)) {
				return false;
			}
			if (!rs2ListEq(this->m_fxParamVec3List, rhs.m_fxParamVec3List)) {
				return false;
			}
			if (!rs2ListEq(this->m_fxParamVec4List, rhs.m_fxParamVec4List)) {
				return false;
			}
			if (!rs2ListEq(this->m_fxParamMatList, rhs.m_fxParamMatList)) {
				return false;
			}
			if (!rs2ListEq(this->m_fxParamTexList, rhs.m_fxParamTexList)) {
				return false;
			}

			return true;
		}

		bool operator!=(const WRenderState& rhs) {
			return !(*this == rhs);
		}

		unsigned int GetFxMacro() {
			return this->m_fxMacro;
		}

		void SetRenderState(D3DRENDERSTATETYPE state, uint32_t value) {
			for (auto it = this->m_rsList.begin(); it != this->m_rsList.end(); ++it) {
				if (it->m_Type == state) {
					it->m_Value = value;
					return;
				}
			}
			this->m_rsList.emplace_back(state, value);
		}

		void Clear() {
			this->m_rsList.clear();
			this->m_tssList.clear();
			this->m_transformList.clear();
			this->m_texList.clear();
			this->m_clipPlaneList.clear();
			this->m_ssList.clear();
			this->m_fxMacro = 0;
			this->m_fxParamIntList.clear();
			this->m_fxParamVec2List.clear();
			this->m_fxParamVec3List.clear();
			this->m_fxParamVec4List.clear();
			this->m_fxParamMatList.clear();
			this->m_fxParamTexList.clear();
		}

		void SetTransform(D3DTRANSFORMSTATETYPE state, const D3DXMATRIX& matrix) {
			std::vector<TwoVarRenderState<D3DTRANSFORMSTATETYPE, D3DXMATRIX>>::iterator it;
			for (it = this->m_transformList.begin(); it != this->m_transformList.end(); ++it) {
				if (it->m_Type == state) {
					it->m_Value = matrix;
					return;
				}
			}
			this->m_transformList.emplace_back(state, matrix);
		}

		void SetTexture(uint32_t stage, IDirect3DBaseTexture9* pTex) {
			for (auto it = this->m_texList.begin(); it != this->m_texList.end(); ++it) {
				if (it->m_Type == stage) {
					it->m_Value = pTex;
					return;
				}
			}
			this->m_texList.emplace_back(stage, pTex);
		}

		void SetFxParamInt(WFxParameterType paramType, int value) {
			for (auto it = this->m_fxParamIntList.begin(); it != this->m_fxParamIntList.end(); ++it) {
				if (it->m_Type == paramType) {
					it->m_Value = value;
					return;
				}
			}
			this->m_fxParamIntList.emplace_back(paramType, value);
		}

		void SetFxParamVector2(WFxParameterType ParamType, const D3DXVECTOR2& Value) {
			for (auto it = this->m_fxParamVec2List.begin(); it != this->m_fxParamVec2List.end(); ++it) {
				if (it->m_Type == ParamType) {
					it->m_Value = Value;
					return;
				}
			}
			this->m_fxParamVec2List.emplace_back(ParamType, Value);
		}

		void SetFxParamVector3(WFxParameterType ParamType, const D3DXVECTOR3& Value) {
			for (auto it = this->m_fxParamVec3List.begin(); it != this->m_fxParamVec3List.end(); ++it) {
				if (it->m_Type == ParamType) {
					it->m_Value = Value;
					return;
				}
			}
			this->m_fxParamVec3List.emplace_back(ParamType, Value);
		}

		void SetFxParamVector4(WFxParameterType ParamType, const D3DXVECTOR4& Value) {
			for (auto it = this->m_fxParamVec4List.begin(); it != this->m_fxParamVec4List.end(); ++it) {
				if (it->m_Type == ParamType) {
					it->m_Value = Value;
					return;
				}
			}
			this->m_fxParamVec4List.emplace_back(ParamType, Value);
		}

		void SetFxParamMatrix(WFxParameterType ParamType, const D3DXMATRIX& Value) {
			for (auto it = this->m_fxParamMatList.begin(); it != this->m_fxParamMatList.end(); ++it) {
				if (it->m_Type == ParamType) {
					it->m_Value = Value;
					return;
				}
			}
			this->m_fxParamMatList.emplace_back(ParamType, Value);
		}

		void SetFxParamTexture(WFxParameterType ParamType, IDirect3DBaseTexture9* pTex) {
			for (auto it = this->m_fxParamTexList.begin(); it != this->m_fxParamTexList.end(); ++it) {
				if (it->m_Type == ParamType) {
					it->m_Value = pTex;
					return;
				}
			}
			this->m_fxParamTexList.emplace_back(ParamType, pTex);
		}

		bool HasAnyState() const {
			if (this->m_fxMacro) {
				return true;
			}

			if (!this->m_rsList.empty()) {
				return true;
			}
			if (!this->m_tssList.empty()) {
				return true;
			}
			if (!this->m_transformList.empty()) {
				return true;
			}
			if (!this->m_texList.empty()) {
				return true;
			}
			if (!this->m_ssList.empty()) {
				return true;
			}
			if (!this->m_clipPlaneList.empty()) {
				return true;
			}
			if (!this->m_fxParamIntList.empty()) {
				return true;
			}
			if (!this->m_fxParamVec2List.empty()) {
				return true;
			}
			if (!this->m_fxParamVec3List.empty()) {
				return true;
			}
			if (!this->m_fxParamVec4List.empty()) {
				return true;
			}
			if (!this->m_fxParamMatList.empty()) {
				return true;
			}
			if (!this->m_fxParamTexList.empty()) {
				return true;
			}

			return false;
		}

		void Begin(WDirect3D8* wd3d) {
			D3DXMATRIX proj, view;
			Effect* effect;

			if (!this->HasAnyState()) {
				return;
			}

			for (auto it = this->m_rsList.begin(); it != this->m_rsList.end(); ++it) {
				wd3d->m_pd3dDevice->GetRenderState(it->m_Type, &it->m_OldValue);
				if (it->m_Value != it->m_OldValue) {
					wd3d->m_pd3dDevice->SetRenderState(it->m_Type, it->m_Value);
				}
			}

			for (auto it = this->m_tssList.begin(); it != this->m_tssList.end(); ++it) {
				wd3d->m_pd3dDevice->GetTextureStageState(it->m_Type, it->m_Type2, &it->m_OldValue);
				if (it->m_Value != it->m_OldValue) {
					wd3d->m_pd3dDevice->SetTextureStageState(it->m_Type, it->m_Type2, it->m_Value);
				}
			}

			for (auto it = this->m_transformList.begin(); it != this->m_transformList.end(); ++it) {
				wd3d->m_pd3dDevice->GetTransform(it->m_Type, &it->m_OldValue);
				if (memcmp(it->m_Value, it->m_OldValue, 64)) {
					wd3d->m_pd3dDevice->SetTransform(it->m_Type, &it->m_Value);
				}
			}

			for (auto it = this->m_texList.begin(); it != this->m_texList.end(); ++it) {
				wd3d->m_pd3dDevice->GetTexture(it->m_Type, &it->m_OldValue);
				if (it->m_Value != it->m_OldValue) {
					wd3d->m_pd3dDevice->SetTexture(it->m_Type, it->m_Value);
				}
				if (it->m_OldValue) {
					it->m_OldValue->Release();
				}
			}

			for (auto it = this->m_clipPlaneList.begin(); it != this->m_clipPlaneList.end(); ++it) {
				wd3d->m_pd3dDevice->GetClipPlane(it->m_Type, (float*)&it->m_OldValue);
				if (wd3d->m_lastEffect) {
					D3DXMatrixInverse(&view, nullptr, &wd3d->m_xLastViewMatrix);
					D3DXMatrixTranspose(&view, &view);
					D3DXPlaneTransform(&it->m_SetValue, &it->m_Value, &view);
					D3DXMatrixInverse(&proj, nullptr, &wd3d->m_xLastProjMatrix);
					D3DXMatrixTranspose(&proj, &proj);
					D3DXPlaneTransform(&it->m_SetValue, &it->m_SetValue, &proj);
				} else {
					it->m_SetValue.a = it->m_Value.a;
					it->m_SetValue.b = it->m_Value.b;
					it->m_SetValue.c = it->m_Value.c;
					it->m_SetValue.d = it->m_Value.d;
				}
				if (it->m_SetValue.a != it->m_OldValue.a || it->m_SetValue.b != it->m_OldValue.b || it->m_SetValue.c !=
					it->m_OldValue.c || it->m_SetValue.d != it->m_OldValue.d) {
					wd3d->m_pd3dDevice->SetClipPlane(it->m_Type, (float*)&it->m_SetValue);
				}
			}

			for (auto it = this->m_ssList.begin(); it != this->m_ssList.end(); ++it) {
				wd3d->m_pd3dDevice->GetSamplerState(it->m_Type, it->m_Type2, &it->m_OldValue);
				if (it->m_Value != it->m_OldValue) {
					wd3d->m_pd3dDevice->SetSamplerState(it->m_Type, it->m_Type2, it->m_Value);
				}
			}

			if (!wd3d->m_lastEffect) {
				return;
			}

			effect = &wd3d->m_effectTable[wd3d->m_lastEffect];
			if (!effect->pEffect) {
				return;
			}

			for (auto it = this->m_fxParamIntList.begin(); it != this->m_fxParamIntList.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.GetInt(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                           effect->param[it->m_Type].isShared, &it->m_OldValue);
				if (it->m_Value != it->m_OldValue) {
					wd3d->m_fxParamPool.SetInt(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
					                           effect->param[it->m_Type].isShared, it->m_Value);
				}
			}

			for (auto it = this->m_fxParamVec2List.begin(); it != this->m_fxParamVec2List.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.GetVector2(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                               effect->param[it->m_Type].isShared, &it->m_OldValue);
				if (it->m_Value.x != it->m_OldValue.x || it->m_Value.y != it->m_OldValue.y) {
					wd3d->m_fxParamPool.SetVector2(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
					                               effect->param[it->m_Type].isShared, &it->m_Value);
				}
			}

			for (auto it = this->m_fxParamVec3List.begin(); it != this->m_fxParamVec3List.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.GetVector3(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                               effect->param[it->m_Type].isShared, &it->m_OldValue);
				if (it->m_Value.x != it->m_OldValue.x || it->m_Value.y != it->m_OldValue.y || it->m_Value.z != it
				                                                                                               ->
				                                                                                               m_OldValue
				                                                                                               .z) {
					wd3d->m_fxParamPool.SetVector3(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
					                               effect->param[it->m_Type].isShared, &it->m_Value);
				}
			}

			for (auto it = this->m_fxParamVec4List.begin(); it != this->m_fxParamVec4List.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.GetVector4(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                               effect->param[it->m_Type].isShared, &it->m_OldValue);
				if (it->m_Value.x != it->m_OldValue.x || it->m_Value.y != it->m_OldValue.y || it->m_Value.z != it
				                                                                                               ->
				                                                                                               m_OldValue
				                                                                                               .z || it
				                                                                                                     ->
				                                                                                                     m_Value
				                                                                                                     .w
					!= it->m_OldValue.w) {
					wd3d->m_fxParamPool.SetVector4(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
					                               effect->param[it->m_Type].isShared, &it->m_Value);
				}
			}

			for (auto it = this->m_fxParamMatList.begin(); it != this->m_fxParamMatList.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				// TODO: We already check effect->pEffect above, redundant.
				if (effect->pEffect && effect->param[it->m_Type].handle) {
					if (effect->param[it->m_Type].isShared) {
						auto result = wd3d->m_fxParamPool.paramCacheListPairMat.first.find(it->m_Type);
						if (result != wd3d->m_fxParamPool.paramCacheListPairMat.first.end()) {
							memcpy(&it->m_OldValue, &result->second, sizeof it->m_OldValue);
						} else {
							effect->pEffect->GetMatrix(static_cast<const char*>(effect->param[it->m_Type].handle),
							                           &it->m_OldValue);
						}
					} else {
						auto result = wd3d->m_fxParamPool.paramCacheListPairMat.second.find(
							(char*)effect->param[it->m_Type].handle);
						if (result != wd3d->m_fxParamPool.paramCacheListPairMat.second.end()) {
							memcpy(&it->m_OldValue, &result->second, sizeof it->m_OldValue);
						} else {
							effect->pEffect->GetMatrix(static_cast<const char*>(effect->param[it->m_Type].handle),
							                           &it->m_OldValue);
						}
					}
				}
				if (memcmp(&it->m_Value, &it->m_OldValue, 64)) {
					wd3d->m_fxParamPool.SetMatrix(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
					                              effect->param[it->m_Type].isShared, &it->m_Value);
				}
			}

			for (auto it = this->m_fxParamTexList.begin(); it != this->m_fxParamTexList.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.GetTexture(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                               effect->param[it->m_Type].isShared, &it->m_OldValue);
				if (it->m_Value != it->m_OldValue) {
					wd3d->m_fxParamPool.SetTexture(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
					                               effect->param[it->m_Type].isShared, it->m_Value);
				}
				if (it->m_OldValue) {
					it->m_OldValue->Release();
				}
			}

			effect->pEffect->CommitChanges();
		}

		void End(WDirect3D8* wd3d) {
			D3DXMATRIX proj, view;
			Effect* effect;

			if (!this->HasAnyState()) {
				return;
			}

			for (auto it = this->m_rsList.begin(); it != this->m_rsList.end(); ++it) {
				wd3d->m_pd3dDevice->SetRenderState(it->m_Type, it->m_OldValue);
			}

			for (auto it = this->m_tssList.begin(); it != this->m_tssList.end(); ++it) {
				wd3d->m_pd3dDevice->SetTextureStageState(it->m_Type, it->m_Type2, it->m_OldValue);
			}

			for (auto it = this->m_transformList.begin(); it != this->m_transformList.end(); ++it) {
				wd3d->m_pd3dDevice->SetTransform(it->m_Type, &it->m_OldValue);
			}

			for (auto it = this->m_texList.begin(); it != this->m_texList.end(); ++it) {
				wd3d->m_pd3dDevice->SetTexture(it->m_Type, it->m_OldValue);
			}

			for (auto it = this->m_clipPlaneList.begin(); it != this->m_clipPlaneList.end(); ++it) {
				wd3d->m_pd3dDevice->SetClipPlane(it->m_Type, (float*)&it->m_OldValue);
			}

			for (auto it = this->m_ssList.begin(); it != this->m_ssList.end(); ++it) {
				wd3d->m_pd3dDevice->SetSamplerState(it->m_Type, it->m_Type2, it->m_OldValue);
			}

			if (!wd3d->m_lastEffect) {
				return;
			}

			effect = &wd3d->m_effectTable[wd3d->m_lastEffect];
			if (!effect->pEffect) {
				return;
			}

			for (auto it = this->m_fxParamIntList.begin(); it != this->m_fxParamIntList.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.SetInt(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                           effect->param[it->m_Type].isShared, it->m_OldValue);
			}

			for (auto it = this->m_fxParamVec2List.begin(); it != this->m_fxParamVec2List.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.SetVector2(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                               effect->param[it->m_Type].isShared, &it->m_OldValue);
			}

			for (auto it = this->m_fxParamVec3List.begin(); it != this->m_fxParamVec3List.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.SetVector3(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                               effect->param[it->m_Type].isShared, &it->m_OldValue);
			}

			for (auto it = this->m_fxParamVec4List.begin(); it != this->m_fxParamVec4List.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.SetVector4(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                               effect->param[it->m_Type].isShared, &it->m_OldValue);
			}

			for (auto it = this->m_fxParamMatList.begin(); it != this->m_fxParamMatList.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.SetMatrix(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                              effect->param[it->m_Type].isShared, &it->m_OldValue);
			}

			for (auto it = this->m_fxParamTexList.begin(); it != this->m_fxParamTexList.end(); ++it) {
				if (!effect->param[it->m_Type].handle) {
					continue;
				}
				wd3d->m_fxParamPool.SetTexture(effect->pEffect, it->m_Type, effect->param[it->m_Type].handle,
				                               effect->param[it->m_Type].isShared, it->m_OldValue);
			}

			effect->pEffect->CommitChanges();
		}

		void SetTextureStageState(uint32_t stage, D3DTEXTURESTAGESTATETYPE type, uint32_t value) {
			for (auto it = this->m_tssList.begin(); it != this->m_tssList.end(); ++it) {
				if (it->m_Type == stage && it->m_Type2 == type) {
					it->m_Value = value;
					return;
				}
			}

			this->m_tssList.emplace_back(stage, type, value);
		}

		void SetClipPlane(uint32_t Index, const D3DXPLANE& Value) {
			for (auto it = this->m_clipPlaneList.begin(); it != this->m_clipPlaneList.end(); ++it) {
				if (it->m_Type == Index) {
					it->m_Value = Value;
					return;
				}
			}
			this->m_clipPlaneList.emplace_back(Index, Value);
		}

		void SetSamplerState(uint32_t sampler, D3DSAMPLERSTATETYPE type, uint32_t value) {
			for (auto it = this->m_ssList.begin(); it != this->m_ssList.end(); ++it) {
				if (it->m_Type == sampler && it->m_Type2 == type) {
					it->m_Value = value;
					return;
				}
			}
			this->m_ssList.emplace_back(sampler, type, value);
		}
	};

	struct sRtFormat {
		D3DFORMAT fmt;
		int bpp;
	};

	struct FMTLIST {
		D3DFORMAT format;
		const char* dispaly;
		int bitNum;
	};

	WDirect3D8(char* devName, int id);
	LRESULT WinProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	virtual WDirect3D8* CreateClone(char* devName, int id);
	bool SetFogEnable(bool enable) override;
	void Clear(uint32_t color, int flags, float z) override;
	int Init(char* modName, HWND hWnd, int iTnL);
	int Command(WDeviceMessage message, int param1, int param2) override;
	const char* EnumModeName() override;
	WDirect3D8* MakeClone(char* modeName, HWND hWnd, int iTnL) override;
	void EndRenderToTexture(const WRenderToTextureParam& param) override;
	bool BeginScene() override;
	bool Reset(bool bWindowed, int iWidth, int iHeight, int iColor, int lWndStyle, int fillMode) override;
	bool BeginRenderToTexture(const WRenderToTextureParam& param) override;
	void SetCustomRenderState(WRenderStateType state, unsigned int value) override;
	void SetCustomTransform(WTransformStateType state, const WMatrix4& matrix) override;
	void SetCustomTexture(unsigned int stage, int hTex) override;
	void SetCustomFxParamInt(WFxParameterType paramType, int value) override;
	void SetCustomFxParamVector2(WFxParameterType paramType, const WVector2D& value) override;
	void SetCustomFxParamVector3(WFxParameterType paramType, const WVector& value) override;
	void SetCustomFxParamVector4(WFxParameterType paramType, const WVector4& value) override;
	void SetCustomFxParamMatrix(WFxParameterType paramType, const WMatrix4& value) override;
	void SetCustomFxParamTexture(WFxParameterType paramType, int hTex) override;
	void SetCustomTextureStageState(uint32_t stage, WTextureStageStateType type, uint32_t value) override;
	void SetCustomClipPlane(uint32_t index, const WPlane& value) override;
	void SetCustomSamplerState(uint32_t sampler, WSamplerStateType type, uint32_t value) override;
	void EndScene() override;
	void DrawPrimitive(uint32_t iType, int iNum, uint32_t dwVertexTypeDesc, void* lpvVertices,
	                   D3DPRIMITIVETYPE dptPrimitiveType, int iType2) override;
	void DrawPrimitiveIndexed(int iType, uint32_t dwVertexTypeDesc, void* lpvVertices, int pNum, uint16_t* fList,
	                          int fNum, int iType2) override;
	void XDrawIndexedPrimitive(const WxViewState& viewState, const WxBatchState& batchState) override;
	void BeginUsingCustomRenderState() override;
	void EndUsingCustomRenderState() override;
	WProc* ExternProc() override;
	void DrawLine(WtVertex** p, int type) override;
	void SetGlobalRenderState(int rs, int rsEx) override;
	bool IsSupportVs() override;
	bool IsSupportPs() override;
	bool IsSupportMrt() override;
	bool IsSupportClipPlane() override;
	int GetWidth() const override;
	int GetHeight() const override;
	bool IsWindowed() override;
	bool IsFillScreenMode() override;
	int GetBackBufferBpp(D3DFORMAT format) const;
	float GetMonitorSupportFps() const override;
	bool SupportRenderTargetFormat() override;
	void Paint() override;
	bool IsTextureFilled(int texHandle) override;
	int GetTextureWidth(int hTex) override;
	int GetTextureHeight(int hTex) override;
	void SetRenderTargetSizeInfo(int hTex, const WRenderToTextureSizeInfo& sizeInfo) override;
	bool LoadShCoefficient(const char* filename);
	bool SetRenderState4Flushing(int pass) override;
	int UploadCompressedTextureSurface(void* pSrcData, size_t srcDataSize, int type) override;
	void UpdateCompressedTexture(int texHandle, void* pSrcData, size_t srcDataSize, int type) override;
	void CreateTextureSurface(uint32_t dwWidth, uint32_t dwHeight, int iMipmapCount, int renderTargetType, int iIndex,
	                          int bChromaKey, int bitNum) override;
	RGBQUAD* GetPixelPtr(BITMAPINFO* bi, uint8_t* data, int x, int y);
	void UpdateTextureSurface(int texHandle, BITMAPINFO* bi, void* data, int type) override;
	void DestroyTexture(int texHandle) override;
	bool GetWindowDisplayMode(int& iWidth, int& iHeight, int& iColor) override;
	bool IsSupportedDisplayMode(bool bWindowed, int iWidth, int iHeight, int iColor) override;
	void XGetCaps(uint32_t iItem, uint32_t* pdwCaps);
	uint32_t XDetermineFvf(int iDrawFlag, int iDrawFlag2, int iMaxBoneNum) override;
	uint32_t XGetFvf(int hVb);
	BYTE XGetStride(int hVb) override;
	bool XHasVertexElem(uint32_t dwFVF, WVertexElement elem) const override;
	int XGetVertexElemOffset(uint32_t dwFVF, WVertexElement elem) const override;
	int XGetBlendWeightSize(uint32_t dwFVF) const override;
	void XCreateVertexBuffer(int hVb, int numVertices, uint32_t dwFVF, uint32_t dwUsage) override;
	void XCreateIndexBuffer(int hIb, int numIndices, uint32_t dwUsage) override;
	uint32_t XDetermineBufferUsage(uint32_t dwFvf) override;
	void XReleaseVertexBuffer(int hVb) override;
	void XReleaseIndexBuffer(int hIb) override;
	char* XLockVertexBuffer(int hVb, UINT uiOffset, UINT uiSize) override;
	void XUnlockVertexBuffer(int hVb) override;
	char* XLockIndexBuffer(int hIb, UINT uiOffset, UINT uiSize) override;
	void XUnlockIndexBuffer(int hIb) override;
	void SetCustomFxMacro(uint32_t fxMacro) override;
	void SetViewPort(uint32_t x, uint32_t y, uint32_t w, uint32_t h) override;
	void SetFogState(float fogStart, float fogEnd, uint32_t fogColor) override;
	void ResetFogState();
	void FixTexturePart(int texHandle, const RECT& rc, BITMAPINFO* src, void* data, int type) override;
	void SetShaderSource(const char* shaderSrc) override;
	const char* GetDeviceName() override;

protected:
	WFxParamPool& GetFxParamPool();
	HRESULT SetVertexShader(uint32_t dwVertexTypeDesc);
	D3DFORMAT FindDepthBufferFormat(D3DFORMAT format) const;
	PixInfo* FindPixInfoByFormat(D3DFORMAT fmt);
	int SetTextureFormat(D3DFORMAT format);
	HRESULT Present();
	bool BeginCapturedBackground();
	void SetRenderState(uint32_t dwType, uint32_t dwTypeEx);
	void SetTexture(uint32_t iType);
	void ReleaseAllRenderTargetBackupResource();
	void RestoreBackedUpRenderTarget();
	void Release() override;
	void BackupRenderTarget(const WRenderToTextureParam& param, IDirect3DSurface9* depthSurf,
	                        const D3DVIEWPORT9& viewport);
	void ApplyCustomRenderState(const void* customRenderState) override;
	void SetShader(uint32_t flags);
	void SetVtxType(uint32_t type, uint32_t type2, unsigned int vertexType, unsigned int diffuse);
	void DrawIndexedPrimitiveUp(D3DPRIMITIVETYPE type, UINT numVertices, UINT primitiveCount, const void* pIndexData,
	                            const void* pVertexStreamZeroData, UINT vertexStreamZeroStride);
	void* SnapShotCustomRenderState() override;
	void ClearCustomRenderStateSnapShotList() override;
	void DrawPrimitiveUp(D3DPRIMITIVETYPE type, UINT primitiveCount, const void* pVertexStreamZeroData,
	                     UINT vertexStreamZeroStride);
	void DrawIndexedPrimitive(D3DPRIMITIVETYPE type, unsigned int minIndex, unsigned int numVertices,
	                          unsigned int startIndex, unsigned int primitiveCount);
	void _SetTexture(uint32_t stage, IDirect3DTexture9* pTexture);
	void _SetLight(uint32_t index, const D3DLIGHT9* pLight);
	void _SetTransform(D3DTRANSFORMSTATETYPE state, const D3DXMATRIX& matrix);
	void _SetRenderState(D3DRENDERSTATETYPE state, uint32_t value);
	void _SetTextureStageState(uint32_t stage, D3DTEXTURESTAGESTATETYPE type, uint32_t value);
	void _SetSamplerState(uint32_t sampler, D3DSAMPLERSTATETYPE type, uint32_t value);
	void SetRenderTargetFormat();
	void UpdateTextureSurfaceNormal(void* buff, int pitch, PixInfo* pix, BITMAPINFO* bi, void* data);
	void UpdateTextureSurfaceDirect(void* buff, int pitch, PixInfo* pix, BITMAPINFO* bi, void* data);
	void UpdateTextureSurfaceSampling(void* buff, int width, int height, int pitch, PixInfo* pix, BITMAPINFO* bi,
	                                  void* data, int type);
	void UpdateTextureSurfaceFiltering(void* buff, int w, int h, int pitch, PixInfo* pix, BITMAPINFO* bi,
	                                   uint8_t* data);
	void UpdateTextureSurfaceAlpha(void* buff, int pitch, PixInfo* pix, const tagRECT& rc, BITMAPINFO* bi, void* data);
	void UpdateTextureSurface(void* buff, int width, int height, int pitch, PixInfo* pix, BITMAPINFO* bi, void* data,
	                          int type);
	char* DuplicateString(const char* str) const;
	bool GetDevName(const D3DDISPLAYMODE& mode, char* pBuf, int bufLen) const;
	void ReleaseCopiedScreenResource();
	IDirect3DSurface9* FindDepthSurf(WORD w, WORD h);
	uint32_t* GetZBufferHistogram();
	HRESULT ScreenShot(const char* pDestFilename, D3DXIMAGE_FILEFORMAT format) const;
	bool CaptureScreen(Bitmap* bitmap);
	void BackupMainRenderTarget();
	void ReleaseMainRenderTarget();
	void SetBlendState(uint32_t srcBlend, uint32_t dstBlend);
	void SetBlendMode(uint32_t dwState);
	void SetTextureStageState(int stage, int type, uint32_t value);
	void SetVtxMode(uint32_t dwVertexTypeDesc);
	void SetDefaultState();

private:
	void XResetCreateResource();
	void CreateEventQuery();
	void ReleaseEventQuery();
	void XInstantiateTexture(int hTex);
	void XFillVertexBuffer(int hVb);
	void XFillIndexBuffer(int hIb);
	void XFillTexture(int hTex);
	void XInstantiateVertexBuffer(int hVb);
	void XInstantiateIndexBuffer(int hIb);
	void XSetTnLBuffer(int hVb, int hIb);
	void XInstantiateAndFillTexture(int hTex);
	void XResetReleaseResource();
	void ReleaseShaderResource();
	bool XReset(bool switchWinMode);
	void UpdateShaderValue(unsigned long h);
	bool CreateEffect(uint32_t flags);
	bool ApplyShader();
	void ReloadShader();
	bool DrawIndexedPrimitiveLockable(D3DPRIMITIVETYPE primitiveType, const void* vPtr, int vNum, int vPitch,
	                                  uint16_t* iPtr, int primNum);
	bool DrawPrimitiveLockable(D3DPRIMITIVETYPE primitiveType, size_t primitiveCount, const void* pVertexStreamZeroData,
	                           unsigned int vertexStreamZeroStride);
	void FlushRenderPrimitive() override;
	int LockVb(const void* vPtr, unsigned vNum, unsigned vPitch, int offset);
	int LockIb(const uint16_t* pvIb, int iNum);
	void XSetTransform(WTransformStateType state, const WMatrix4& matrix) override;
	void XSetPrevViewTransform(const WMatrix4& matrix) override;
	void XSetLight(uint32_t index, const LightSet& wLight);
	void XSetRenderState(uint32_t dwFlag, uint32_t dwDiffuse);

	static void __fastcall FillVertex(uint8_t* dest, const uint8_t* src, unsigned int xLastVertexDecl,
	                                  size_t numVertices, unsigned int vertexStreamZeroStride, const D3DXMATRIX* m);

	static sRtFormat m_rtFmt[4];
	static sRtFormat m_rtNoAlphaFmt[3];
	static sRtFormat m_rtDepthFmt[5];
	static FMTLIST m_fmtList[5];
	static D3DFORMAT m_fmtRecommendedList[6][4];
	static PixInfo m_fmtTypeList[9];

	IDirect3DTexture9* m_pTexture[2]{};
	std::stack<RtBackup> m_rtBackupList{};
	RtBackup m_mainRt{};
	RtBackup m_curRt{};
	int m_rtFmtIdx = -1;
	int m_rtNoAlphaFmtIdx = -1;
	int m_rtLowMemFmtIdx = -1;
	int m_rtNoAlphaLowMemFmtIdx = -1;
	int m_rtDepthFmtIdx = -1;
	IDirect3DSurface9* m_pCopiedScreenSurface = nullptr;
	int m_hCopiedScreenTexture = 0;
	WSplashD3D* m_pCopiedScreenSplash = nullptr;
	bool m_useCopiedScreen = false;
	D3DSURFACE_DESC m_capturedDdsd{};
	uint32_t m_lastTexState = 0;
	unsigned m_lastRenderState = 0;
	unsigned m_lastBlendMode = 0;
	DWORD m_texStageState[2][6]{};
	DWORD m_lastBlendState[2]{};
	DWORD m_blendEnable = 0;
	unsigned m_lastVtxType = 0;
	unsigned m_vtxSize = 0;
	D3D8Texture m_texList[2048]{};
	sDepthSurf m_depthSurfList[32]{};
	IDirect3DSurface9* m_commonDepthSurf = nullptr;
	float m_fps = 60.0f;
	bool m_bWindow = false;
	PixInfo* m_fmt[6]{};
	IDirect3D9* m_d3d8 = nullptr;
	IDirect3DDevice9* m_pd3dDevice = nullptr;
	D3DCAPS9 m_d3dCaps{};
	D3DPRESENT_PARAMETERS m_d3dpp{};
	D3DFORMAT m_fmtWindowed = D3DFMT_X8R8G8B8;
	char* m_devName = nullptr;
	char* m_modList = nullptr;
	int m_devId = 0;
	HWND m_hWnd = nullptr;
	int m_lWndStyle = 0;
	bool m_clientRcCheckedAfterReset = true;
	IDirect3DQuery9* m_pEventQuery = nullptr;
	Nv::IScreenCape* m_pScreenCape = nullptr;
	Nv::Factory::ScreenCapeFactory* m_pScreenCapeFactory = nullptr;
	unsigned int m_xdwDevBehavior = 0;
	int m_xiVbSize = 0;
	sVb8 m_xaVbList[1024]{};
	sIb8 m_xaIbList[256]{};
	bool m_xbCurHwTnL = false;
	bool m_xbLastHwTnL = false;
	unsigned m_xLastDrawType = 0;
	unsigned m_xLastRenderState = 0;
	unsigned m_xLastVertexDecl = 0;
	D3DXMATRIX m_xLastWorldMatrix[256]{};
	D3DXMATRIX m_xLastViewMatrix{};
	D3DXMATRIX m_xLastProjMatrix{};
	D3DXMATRIX m_xPrevViewMatrix{};
	unsigned int m_lastTFactor = 0;
	float m_lastFogStart = 0.0f;
	float m_lastFogEnd = 0.0f;
	D3DVECTOR m_lightDirect{};
	int m_xhLastVb = 0;
	int m_xhLastIb = 0;
	unsigned m_xdwLastUsage = 0;
	unsigned m_xdwLastFvf = 0;
	unsigned m_xdwTFactor = 0;
	bool m_xbUseTFactor = false;
	bool m_xbLastVertexBlend = false;
	int m_xnTotalTriangles = 0;
	int m_xnDPUPs = 0;
	int m_xnDIPUPs = 0;
	int m_xnDPs = 0;
	int m_xnDIPs = 0;
	LightSet m_xaWLights[4]{};
	D3DLIGHT9 m_xaLights[4]{};
	D3DMATERIAL9 m_xMaterial{};
	bool m_lightEnable = false;
	unsigned m_xdwDiffuse = 0;
	unsigned m_ddsRes = 0;
	bool m_bUseMipmap = false;
	unsigned m_iMaxMipLvl = 1;
	unsigned m_dwMipCreateFilter = 2;
	unsigned m_dwMipTexStateFilter = 1;
	std::map<int, sVertexDecl> m_vertexDecl{};
	std::string m_shaderSource{};
	unsigned m_effect = -1;
	unsigned m_lastEffect = 0;
	std::map<unsigned long, Effect> m_effectTable{};
	float m_shCoefficient[7][4]{};
	unsigned m_globalRs[2]{};
	MergeBuffer m_mergeBuffer{};
	IDirect3DVertexBuffer9* m_pLockableVb = nullptr;
	IDirect3DIndexBuffer9* m_pLockableIb = nullptr;
	unsigned m_iVbOffset = 0;
	unsigned m_iIbOffset = 0;
	WFxParamPool m_fxParamPool{};
	WRenderState m_customRenderState{};
	std::stack<WRenderState> m_customRenderStateBackupList{};
	std::list<WRenderState> m_customRenderStateSnapShotList{};
	WindowUtility::DwmApiDll* m_dwmApiDll = nullptr;
	bool m_fillScrMode = false;
};
