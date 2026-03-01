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
#include "Camera.h"

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

		Graphics() {
			camera = new Camera();
		};

		Scene* Init(HWND window = NULL);

		void CreateDevice();
		void CreateDevice(D3D_DRIVER_TYPE driverType);
		void CreateSwapChain(HWND window = NULL);

		void CreateD2DDeviceAndContext();

		void CreateDeviceAndSwapChain(HWND window = NULL);
		void CreateDeviceAndSwapChain(D3D_DRIVER_TYPE driverType, HWND window = NULL);

		void CreateRenderTarget(UINT sampleCount = 1, UINT sampleQuality = 0);
		void CreateDepthStencil();

		void CreateVertexShader();
		void CreatePixelShader();

		void CreateInputLayout();

		void CreateTriangle();

		void Clear();
		void Render(float angle, float x, float y, float z);
		void Render(Camera* camera = nullptr);
		ID3D11Texture2D* RenderToTexture(float angle, float x, float y, float z);
		void Present();

		void Resize(HWND window);
		void Resize(UINT width, UINT height);

		Scene *CreateScene();
		void SetScene(unique_ptr<Scene> scene);
		Scene* GetScene();
		void UpdateScene();

		void InitializeBlendState();

		ID3D11Device* GetDevice();

		Texture* CreateTexture(const path& filename);

		IDXGISwapChain* GetSwapChain();

		void CreateRenderTexture(UINT width, UINT height);

		ID2D1Bitmap1* RenderToBitmap();

		void SaveBitmapToFile(ID2D1Bitmap* bitmap, const WCHAR* fileName);
		void SaveTextureToFIle(ID3D11Texture2D* texture, WCHAR* fileName);

		HRESULT CreateD2DBitmapFromD3DTexture(
			ID3D11Texture2D* d3dTexture,
			ID2D1DeviceContext* d2dContext,
			ID2D1Bitmap1** outD2dBitmap);
		ID2D1Bitmap1* CreateD2DBitmapFromTexture(ID3D11Texture2D* texture);

		function<VOID(HRESULT)> OnError;
	private:
		ComPtr<ID3D11Device> device;
		ComPtr<IDXGIDevice> dxgiDevice;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<IDXGISwapChain1> swapChain;

		ComPtr<Camera> camera;

		ComPtr<ID2D1Device> d2dDevice;
		ComPtr<ID2D1DeviceContext> d2dContext;


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

		ComPtr< ID3D11Texture2D> msaaTexture;

		ComPtr<ID3D11ShaderResourceView> textureView;
		ComPtr<ID3D11SamplerState> sampler;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTexture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> textureRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV;

		ID3D11Texture2D* backBuffer;
		IDXGISurface* dxgiBackBuffer;
		ID2D1Bitmap1* d2dTargetBitmap1;


		unique_ptr<Scene> scene;

		XMMATRIX projectionMatrix;
		XMMATRIX viewMatrix;

		INT indexCount;

		BOOL antiAliasing = TRUE;
		DXGI_SAMPLE_DESC sampleDesc;

		BOOL superSampling = FALSE;
		INT scalingFactor = 2;

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
				if (OnError) OnError(hr);
				else throw runtime_error("HRESULT failed: " + to_string(hr));
			}
		}
	};
}
