#include "pch.h"
#include "include/Graphics.h"

namespace CubeRenderer {

	void Graphics::CreateDeviceAndSwapChain(HWND hWnd)
	{
		D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
		};

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		if (hWnd == nullptr) {
			D3D_FEATURE_LEVEL selectedFeatureLevel;
			ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &selectedFeatureLevel, &context));

			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = 1; // Match the size of the window.
			swapChainDesc.Height = 1;
			swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;  // most common
			swapChainDesc.Stereo = FALSE;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;  // double buffering
			swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
			swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
			swapChainDesc.Flags = 0;

			ComPtr<IDXGIAdapter1> dxgiAdapter;
			dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter));

			ComPtr<IDXGIFactory2> dxgiFactory2;
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory2));

			ComPtr<IDXGISwapChain1> swapChain1;
			ThrowIfFailed(dxgiFactory2->CreateSwapChainForComposition(device.Get(), &swapChainDesc, nullptr, &swapChain1));
			swapChain1.As<IDXGISwapChain>(&swapChain);
		}
		else {
			DXGI_SWAP_CHAIN_DESC sd = {};
			sd.BufferDesc.Width = 0;
			sd.BufferDesc.Height = 0;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.RefreshRate.Numerator = 0;
			sd.BufferDesc.RefreshRate.Denominator = 0;
			sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.BufferCount = 1;
			sd.OutputWindow = hWnd;

			ThrowIfFailed(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context));
		}


		ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)));

#ifdef _DEBUG
		ComPtr<ID3D11Debug> debug;
		device.As<ID3D11Debug>(&debug);
		ComPtr<ID3D11InfoQueue> infoQueue;
		debug.As<ID3D11InfoQueue>(&infoQueue);

		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_INFO, true);
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_MESSAGE, true);
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);

		/*D3D11_MESSAGE_ID hide[] = { D3D11_MESSAGE_ID_DEVICE_DRAW_RenderTargetVIEW_NOT_SET };
		D3D11_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		infoQueue->AddStorageFilterEntries(&filter);*/

#endif
	}

	

	void Graphics::CreateRenderTarget()
	{
		ThrowIfFailed(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer));

		ThrowIfFailed(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView));
		
		backBuffer->QueryInterface(__uuidof(IDXGISurface), &dxgiSurface);

		D3D11_BUFFER_DESC cbd = {};
		cbd.ByteWidth = sizeof(ConstantBuffer);
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		ThrowIfFailed(device->CreateBuffer(&cbd, nullptr, &constantBuffer));

		context->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());


		D3D11_SAMPLER_DESC sampDesc = {};
		sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT; // Use linear filtering for smoother textures
		//sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = 0/*D3D11_FLOAT32_MAX*/;

		ThrowIfFailed(device->CreateSamplerState(&sampDesc, &sampler));
		context->PSSetSamplers(0, 1, sampler.GetAddressOf());

		D3D11_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_BACK;  // Culling back faces
		rasterDesc.FrontCounterClockwise = false;  // Set this to true if your front faces are counter-clockwise
		rasterDesc.DepthBias = 0;
		rasterDesc.SlopeScaledDepthBias = 0.0f;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.AntialiasedLineEnable = true;

		ID3D11RasterizerState* pRasterizerState = nullptr;
		ThrowIfFailed(device->CreateRasterizerState(&rasterDesc, &pRasterizerState));

		context->RSSetState(pRasterizerState);
	}

	void Graphics::CreateInputLayout() {

		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURECOORDINATE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, textureCoordinate), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		ComPtr<ID3D11InputLayout> pInputLayout;

		ThrowIfFailed(device->CreateInputLayout(layout, std::size(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &pInputLayout));

		context->IASetInputLayout(pInputLayout.Get());
	}

	void Graphics::CreateVertexShader() {

		std::wstring fullPath = GetExecutableDirectory() / L"VertexShader.cso";
		ThrowIfFailed(D3DReadFileToBlob(fullPath.c_str(), &vertexShaderBlob));
		ThrowIfFailed(device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &vertexShader));
		context->VSSetShader(vertexShader.Get(), nullptr, 0);
	}

	void Graphics::CreatePixelShader() {

		std::wstring fullPath = GetExecutableDirectory() / L"PixelShader.cso";
		ThrowIfFailed(D3DReadFileToBlob(fullPath.c_str(), &pixelShaderBlob));
		ThrowIfFailed(device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader));
		context->PSSetShader(pixelShader.Get(), nullptr, 0);
	}

	void Graphics::Init() {

		CreateScene();
		
		scene->AddCube(8, 8, 8, -2.0f, 22.0f, -2.0f, 0, 0, 64, 64);

		// Waist
		scene->AddCube(8, 12, 4, -2.0f, 12.0f, -2.0f, 16, 16, 64, 64);

		// Left arm
		scene->AddCube(4, 12, 4, 4.0f, 12.0f, -2.0f, 32, 48, 64, 64);

		// Right arm
		scene->AddCube(4, 12, 4, -8.0f, 12.0f, -2.0f, 40, 16, 64, 64);

		// Left leg
		scene->AddCube(4, 12, 4, -0.1, 0.0f, -2.0f, 16, 48, 64, 64);

		// Right leg
		scene->AddCube(4, 12, 4, -3.9f, 0.0f, -2.0f, 0, 16, 64, 64);

		CreateDevice();
		CreateSwapChain();

		InitializeBlendState();

		CreateRenderTarget();

		//UpdateViewport(2000, 1000);

		CreatePixelShader();
		CreateVertexShader();

		CreateInputLayout();

		UpdateScene();

		context->PSSetShaderResources(0, 1, textureView.GetAddressOf());
		context->PSSetSamplers(0, 1, sampler.GetAddressOf());

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Graphics::Init(HWND hWnd) {

		CreateScene();

		scene->AddCube(8, 8, 8, -2.0f, 22.0f, -2.0f, 0, 0, 64, 64);

		// Waist
		scene->AddCube(8, 12, 4, -2.0f, 12.0f, -2.0f, 16, 16, 64, 64);

		// Left arm
		scene->AddCube(4, 12, 4, 4.0f, 12.0f, -2.0f, 32, 48, 64, 64);

		// Right arm
		scene->AddCube(4, 12, 4, -8.0f, 12.0f, -2.0f, 40, 16, 64, 64);

		// Left leg
		scene->AddCube(4, 12, 4, -0.1, 0.0f, -2.0f, 16, 48, 64, 64);

		// Right leg
		scene->AddCube(4, 12, 4, -3.9f, 0.0f, -2.0f, 0, 16, 64, 64);

		CreateDeviceAndSwapChain(hWnd);

		InitializeBlendState();

		CreateRenderTarget();

		UpdateViewport(hWnd);

		CreatePixelShader();
		CreateVertexShader();

		CreateInputLayout();
		
		UpdateScene();

		context->PSSetShaderResources(0, 1, textureView.GetAddressOf());
		context->PSSetSamplers(0, 1, sampler.GetAddressOf());

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Graphics::SetScene(std::unique_ptr<Scene> newScene) {
		scene = std::move(newScene);
		UpdateScene();
	}

	void Graphics::UpdateScene() {
		if (scene) {
			auto vertices = scene->GetVertices();
			auto verticesSize = scene->GetVerticesSize();
			auto indices = scene->GetIndices();
			auto indicesSize = scene->GetIndicesSize();
			SetVertexBuffer(vertices, verticesSize);
			SetIndexBuffer(indices, indicesSize);
		}
	}

	Scene *Graphics::CreateScene() {
		scene = std::make_unique<Scene>();
		return GetScene();
	}

	void Graphics::SetVertexBuffer(Vertex* vertices, size_t size) {
		vertexBuffer = nullptr;
		D3D11_BUFFER_DESC bufferDescription = {};
		D3D11_SUBRESOURCE_DATA subresourceData = {};

		bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDescription.Usage = D3D11_USAGE_DEFAULT;
		bufferDescription.CPUAccessFlags = 0;
		bufferDescription.MiscFlags = 0u;
		bufferDescription.ByteWidth = size;
		bufferDescription.StructureByteStride = sizeof(Vertex);
		subresourceData.pSysMem = vertices;
		ThrowIfFailed(device->CreateBuffer(&bufferDescription, &subresourceData, &vertexBuffer));

		const UINT stride = sizeof(Vertex);
		const UINT offset = 0;

		context->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	}

	void Graphics::SetIndexBuffer(unsigned short* indices, size_t size) {
		indexBuffer = nullptr;

		D3D11_BUFFER_DESC indexBufferDescription = {};
		indexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDescription.ByteWidth = size;
		indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDescription.CPUAccessFlags = 0;
		indexBufferDescription.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA indexSubresourceData = {};
		indexSubresourceData.pSysMem = indices;

		ThrowIfFailed(device->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer));
		context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	}

	void Graphics::Resize(UINT width, UINT height) {
		//RenderTargetView.c(); reset this one
		if (width == 0 || height == 0) return;

		context->OMSetRenderTargets(0, nullptr, nullptr);
		renderTargetView.Reset();
		depthStencilView.Reset();
		context->Flush();

		UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		ThrowIfFailed(swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, flags));

		ComPtr<ID3D11Resource> backBuffer;
		ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
		ThrowIfFailed(device->CreateRenderTargetView(backBuffer.Get(), NULL, &renderTargetView));

		UpdateViewport(width, height);
	}

	void Graphics::UpdateViewport(UINT width, UINT height) {

		if (width == 0 || height == 0) return;

		float aspectRatio = width / height;

		// Depth map stuff
		D3D11_TEXTURE2D_DESC depthDesc = {};

		depthDesc.Width = width;
		depthDesc.Height = height;

		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		device->CreateTexture2D(&depthDesc, nullptr, &depthStencilBuffer);

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, &depthStencilView);

		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		device->CreateDepthStencilState(&dsDesc, &depthStencilState);

		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		context->RSSetViewports(1, &viewport);

		projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.1f, 500.0f);

		viewMatrix = XMMatrixTranslation(0.0f, 0.0f, 100.0f);
	}

	void Graphics::UpdateViewport(HWND hWnd) {
		RECT rect;
		GetClientRect(hWnd, &rect);

		UpdateViewport(rect.right - rect.left, rect.bottom - rect.top);
	}

	void Graphics::Clear() {
		const float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		context->ClearRenderTargetView(renderTargetView.Get(), color);
		if (depthStencilView)
			context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Graphics::Render(float angle, float x, float y, float z) {

		Clear();

		const ConstantBuffer cb = {{
			XMMatrixTranspose(
				XMMatrixRotationZ(z) *
				XMMatrixRotationY(x) *
				XMMatrixRotationX(y) *
				XMMatrixTranslation(x, -y, 0) *
				viewMatrix * projectionMatrix
			)
		}};

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &cb, sizeof(cb));
		context->Unmap(constantBuffer.Get(), 0);

		context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		int indexCount = scene->GetIndicesSize() / sizeof(unsigned short);
		context->DrawIndexed(indexCount, 0, 0);

		Present();
	}

	void Graphics::Present() {
		ThrowIfFailed(swapChain->Present(1, 0));
	}

	Scene* Graphics::GetScene() {
		return scene.get();
	}

	void Graphics::InitializeBlendState() {
		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].BlendEnable = FALSE;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		ID3D11BlendState* pBlendState = nullptr;
		ThrowIfFailed(device->CreateBlendState(&blendDesc, &pBlendState));

		context->OMSetBlendState(pBlendState, nullptr, 0xFFFFFFFF);
	}

	IDXGISwapChain* Graphics::GetSwapChain() {
		return swapChain.Get();
	}

	void Graphics::LoadTexture(const std::filesystem::path& filename) {
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromFile(filename.c_str());
		if (pBitmap == nullptr || pBitmap->GetLastStatus() != Gdiplus::Ok) {
			Gdiplus::GdiplusShutdown(gdiplusToken);
			ThrowIfFailed(E_FAIL);
			return;
		}

		UINT width = pBitmap->GetWidth();
		UINT height = pBitmap->GetHeight();

		UINT bufferSize = width * height * 4;
		BYTE* pPixels = new BYTE[bufferSize];

		Gdiplus::BitmapData bitmapData;
		Gdiplus::Rect rect(0, 0, width, height);
		pBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData);

		memcpy(pPixels, bitmapData.Scan0, bufferSize);

		pBitmap->UnlockBits(&bitmapData);

		delete pBitmap;

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = pPixels;
		subresourceData.SysMemPitch = width * 4;
		subresourceData.SysMemSlicePitch = 0;

		ID3D11Texture2D* pTexture = nullptr;
		ThrowIfFailed(device->CreateTexture2D(&textureDesc, &subresourceData, &pTexture));

		ThrowIfFailed(device->CreateShaderResourceView(pTexture, nullptr, textureView.GetAddressOf()));
		pTexture->Release();
		delete[] pPixels;

		Gdiplus::GdiplusShutdown(gdiplusToken);
	}
}
