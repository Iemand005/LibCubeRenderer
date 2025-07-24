#pragma once

#include "pch.h"

#include "include/Graphics.h"

namespace CubeRenderer {
	using namespace DirectX;
	using namespace Microsoft::WRL;

	//using namespace winrt::Windows::Foundation;
	//using namespace winrt::Microsoft::UI::Xaml;
	//using namespace winrt::Microsoft::UI::Xaml::Controls;

	inline std::filesystem::path GetExecutableDirectory()
	{
		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(nullptr, buffer, MAX_PATH);
		return std::filesystem::path(buffer).parent_path();
	}


	class Graphics::Private {
	public:
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
				//if (OnError) OnError(hr);
			}
		}

		HRESULT LoadTextureFromPNG(ID3D11Device* pDevice, const std::wstring& filename);
	};
}