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
	using namespace Microsoft::WRL;

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

		Scene *CreateScene();
		void SetScene(std::unique_ptr<Scene> scene);
		Scene* GetScene();
		void UpdateScene();

		void InitializeBlendState();

		void UpdateViewport(HWND hWnd);

		void LoadTexture(const std::filesystem::path& filename);

		IDXGISwapChain* GetSwapChain();

		std::function<void(HRESULT)> OnError;
	private:
		ComPtr<ID3D11Device> device;
		ComPtr<IDXGIDevice> dxgiDevice;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<IDXGISwapChain> swapChain;

		ComPtr<ID3D11Buffer> vertexBuffer;
		ComPtr<ID3D11Buffer> indexBuffer;

		ComPtr<ID3D11InputLayout> inputLayout;

		ComPtr<ID3D11PixelShader> pixelShader;
		ComPtr<ID3D11VertexShader> vertexShader;
		ComPtr<ID3DBlob> pixelShaderBlob;
		ComPtr<ID3DBlob> vertexShaderBlob;

		ComPtr<ID3D11Buffer> constantBuffer;

		ComPtr<ID3D11Texture2D> backBuffer;
		ComPtr<IDXGISurface> dxgiSurface;

		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		ComPtr<ID3D11DepthStencilState> depthStencilState;

		ComPtr<ID3D11RenderTargetView> renderTargetView;
		ComPtr<ID3D11DepthStencilView> depthStencilView;

		ComPtr<ID3D11ShaderResourceView> textureView;
		ComPtr<ID3D11SamplerState> sampler;

		std::unique_ptr<Scene> scene;

		XMMATRIX projectionMatrix;
		XMMATRIX viewMatrix;

		void UpdateViewport(UINT width, UINT height);

		void SetVertexBuffer(Vertex* vertices, size_t size);
		void SetIndexBuffer(unsigned short* indices, size_t size);

		struct ConstantBuffer {
			XMMATRIX transform;
		};

		inline void ThrowIfFailed(HRESULT hr)
		{
			if (FAILED(hr))
			{
				if (OnError) OnError(hr);
			}
		}
	};
}
