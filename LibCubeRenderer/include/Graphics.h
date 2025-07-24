#pragma once

//#include <functional>
//
//#include <filesystem>
//
//#include <d3d11.h>
//#include <dxgi1_6.h>
//#include <d3dcompiler.h>
//
//#include <wrl/client.h>
//
//#include <Windows.h>
//
//#include <DirectXMath.h>
//#include <DirectXPackedVector.h>


//
//#include <winrt/Windows.Foundation.h>
//#include <winrt/Microsoft.UI.Xaml.h>
//#include <winrt/Microsoft.UI.Xaml.Controls.h>
//
//#include <microsoft.ui.xaml.media.dxinterop.h>

#include "Base.h"

#include "Scene.h"

namespace CubeRenderer {
	using namespace DirectX;

	inline std::filesystem::path GetExecutableDirectory()
	{
		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(nullptr, buffer, MAX_PATH);
		return std::filesystem::path(buffer).parent_path();
	}


	class Graphics {
	public:

		void Init();
		void Init(HWND hWnd);

		void CreateDevice();
		void CreateSwapChain();
		void CreateDeviceAndSwapChain(HWND hWnd);

		void CreateRenderTarget();

		void CreateVertexShader();
		void CreatePixelShader();

		void CreateInputLayout();

		

		void CreateTriangle();

		void Clear();
		void Render(float angle, float x, float y, float z);
		void Present();

		void Resize(UINT width, UINT height);

		void CreateScene();
		void SetScene(std::unique_ptr<Scene> scene);
		Scene* GetScene();

		void InitializeBlendState();

		void UpdateViewport(HWND hWnd);

		std::function<void(HRESULT)> OnError;
	private:
		class Private;
		std::unique_ptr<Private> p;
	};
}
