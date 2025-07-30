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
	using namespace std;
	using namespace filesystem;

	using namespace DirectX;
	using namespace Microsoft::WRL;

	inline path GetExecutableDirectory()
	{
		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(nullptr, buffer, MAX_PATH);
		return path(buffer).parent_path();
	}


	class Graphics {
	public:

		Scene* Init(HWND window = NULL);

		void CreateDevice();
		void CreateDevice(D3D_DRIVER_TYPE driverType);
		void CreateSwapChain(HWND window = NULL, UINT sampleCount = 8, UINT sampleQuality = 0);

		void CreateDeviceAndSwapChain(HWND window = NULL);
		void CreateDeviceAndSwapChain(D3D_DRIVER_TYPE driverType, HWND window = NULL);

		void CreateRenderTarget();

		void CreateVertexShader();
		void CreatePixelShader();

		void CreateInputLayout();

		void CreateTriangle();

		void Clear();
		void Render(float angle, float x, float y, float z);
		void Present();

		void Resize(HWND window);
		void Resize(UINT width, UINT height);

		Scene *CreateScene();
		void SetScene(unique_ptr<Scene> scene);
		Scene* GetScene();
		void UpdateScene();

		void InitializeBlendState();

		Texture* CreateTexture(const path& filename);

		IDXGISwapChain* GetSwapChain();

		function<void(HRESULT)> OnError;
	private:
		ComPtr<ID3D11Device> device;
		ComPtr<IDXGIDevice> dxgiDevice;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<IDXGISwapChain1> swapChain;

		ComPtr<ID3D11Buffer> vertexBuffer;
		ComPtr<ID3D11Buffer> indexBuffer;

		ComPtr<ID3D11InputLayout> inputLayout;

		ComPtr<ID3D11PixelShader> pixelShader;
		ComPtr<ID3D11VertexShader> vertexShader;
		ComPtr<ID3DBlob> pixelShaderBlob;
		ComPtr<ID3DBlob> vertexShaderBlob;

		ComPtr<ID3D11Buffer> constantBuffer;

		ComPtr<ID3D11Texture2D> depthStencilBuffer;
		ComPtr<ID3D11DepthStencilState> depthStencilState;

		ComPtr<ID3D11RenderTargetView> renderTargetView;
		ComPtr<ID3D11DepthStencilView> depthStencilView;

		ComPtr<ID3D11ShaderResourceView> textureView;
		ComPtr<ID3D11SamplerState> sampler;

		unique_ptr<Scene> scene;

		XMMATRIX projectionMatrix;
		XMMATRIX viewMatrix;

		INT indexCount;

		bool antiAliasing = true;
		int antiAliasingSamples = 8;

		void UpdateViewport(FLOAT width, FLOAT height);

		void SetVertexBuffer(Vertex* vertices, size_t size);
		void SetIndexBuffer(unsigned short* indices, size_t size);

		struct ConstantBuffer {
			XMMATRIX transform;
		};

		inline void ThrowIfFailed(HRESULT hr)
		{
			if (FAILED(hr))
			{
				//if (OnError) OnError(hr);
				throw runtime_error("HRESULT failed: " + to_string(hr));
			}
		}
	};
}
