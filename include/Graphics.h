#pragma once

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
			camera.Attach(new Camera()) = ComPtr<Camera>();
		};

		Scene* Init(HWND window = NULL);

		void CreateDevice();
		void CreateDevice(D3D_DRIVER_TYPE driverType);
		void CreateSwapChain(HWND window = NULL);


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
		void Present();

		void Resize(HWND window);
		void Resize(UINT width, UINT height);

		Scene* CreateScene();
		void SetScene(unique_ptr<Scene> scene);
		Scene* GetScene();
		void UpdateScene();

		void InitializeBlendState();


		Texture* CreateTexture(const path& filename);

		ID3D11Device* GetDevice();
		void MouseDown(int x, int y = 0);
		void MouseUp();
		void MouseMove(int x, int y = 0);
		ID3D11DeviceContext* GetContext();
		IDXGISwapChain* GetSwapChain();
		IDXGISurface* GetDXGIBackBuffer();

		void CreateRenderTexture(UINT width, UINT height);


		function<VOID(HRESULT)> OnError;
	private:
		ComPtr<ID3D11Device> device;
		ComPtr<IDXGIDevice> dxgiDevice;
		ComPtr<ID3D11DeviceContext> context;
		ComPtr<IDXGISwapChain1> swapChain;

		ComPtr<Camera> camera;

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
