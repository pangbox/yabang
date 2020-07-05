#pragma once
#include <windows.h>

constexpr DWORD g_screencapeVersion = 20100329u;

namespace Nv {
	namespace GraphicDeviceType {
		enum Enum {
			Direct3D8 = 0x0,
			Direct3D9 = 0x1,
		};
	}

	// ReSharper disable once CppPolymorphicClassWithNonVirtualPublicDestructor
	class IScreenCape {
	public:
		virtual bool Initialize(HWND, void **, Nv::GraphicDeviceType::Enum) = 0;
		virtual void Release() = 0;
		virtual void Render() = 0;
		virtual void Pause() = 0;
		virtual void Resume() = 0;
	};

	namespace Factory {
		typedef HRESULT (__stdcall *CreateScreenCapeLayerProc)(IScreenCape **pInterface, DWORD dwVersion);
		class ScreenCapeFactory {
		public:
			ScreenCapeFactory() {
				this->m_pProc = nullptr;
				this->m_hModule = LoadLibraryA("ScreenCape.dll");
				if (this->m_hModule) {
					this->m_pProc = reinterpret_cast<CreateScreenCapeLayerProc>(GetProcAddress(this->m_hModule, "CreateScreenCape"));
				} else {
					MessageBoxW(nullptr, L"Load failed 'ScreenCape.dll'", L"Warning!", 0);
				}
			}

			~ScreenCapeFactory() {
				if (this->m_hModule) {
					FreeLibrary(this->m_hModule);
				}
			}

			IScreenCape *CreateScreenCapeLayer() const {
				IScreenCape *pInterface = nullptr;

				if (!this->m_pProc) {
					return pInterface;
				}

				this->m_pProc(&pInterface, g_screencapeVersion);
				return pInterface;
			}

			HINSTANCE m_hModule;
			CreateScreenCapeLayerProc m_pProc;
		};
	}
}
