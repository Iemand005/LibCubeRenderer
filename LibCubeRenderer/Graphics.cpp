#include "pch.h"
#include "include/Graphics.h"
//#include "Camera.h"

namespace CubeRenderer {

	//Graphics::Graphics() 

	Scene* Graphics::Init(HWND window) {

		Scene* scene = CreateScene();

		CreateDeviceAndSwapChain(window);

		InitializeBlendState();

		//CreateRenderTarget();

		if (window) Resize(window);
		else Resize(100, 100);

		CreatePixelShader();
		CreateVertexShader();

		CreateInputLayout();

		context->PSSetSamplers(0, 1, sampler.GetAddressOf());

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		return scene;
	}

	void Graphics::CreateDeviceAndSwapChain(HWND window) {
		D3D_DRIVER_TYPE driverTypes[] = {
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE
		};

		bool succeeded = false;
		runtime_error* error = NULL;
		for (UINT i = 0; i < ARRAYSIZE(driverTypes); i++)
			try {
				CreateDeviceAndSwapChain(driverTypes[i], window);
				succeeded = true;
				break;
			}
			catch (const runtime_error& e) {
				OutputDebugStringA(e.what());
				*error = e;
			}
		if (!succeeded) {
			throw *error;
		}
	}

	void Graphics::CreateDevice() {
		D3D_DRIVER_TYPE driverTypes[] = {
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE
		};

		bool succeeded = false;
		runtime_error* error = NULL;
		for (UINT i = 0; i < ARRAYSIZE(driverTypes); i++)
			try {
			CreateDevice(driverTypes[i]);
			succeeded = true;
			break;
		}
		catch (const runtime_error& e) {
			OutputDebugStringA(e.what());
			*error = e;
		}
		if (!succeeded) {
			throw* error;
		}
	}

	void Graphics::CreateDevice(D3D_DRIVER_TYPE driverType) {
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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

		D3D_FEATURE_LEVEL selectedFeatureLevel;
		ThrowIfFailed(D3D11CreateDevice(nullptr, driverType, 0, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &selectedFeatureLevel, &context));

		ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)));

#ifdef _DEBUG
		ComPtr<ID3D11Debug> debug;
		device.As<ID3D11Debug>(&debug);
		ComPtr<ID3D11InfoQueue> infoQueue;
		debug.As<ID3D11InfoQueue>(&infoQueue);

		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_INFO, true);
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_MESSAGE, true);
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);

		D3D11_MESSAGE_ID hide[] = { D3D11_MESSAGE_ID_DEVICE_DRAW_RENDERTARGETVIEW_NOT_SET };
		D3D11_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		infoQueue->AddStorageFilterEntries(&filter);
#endif
	}

	void Graphics::CreateSwapChain(HWND window) {

		ComPtr<IDXGIAdapter1> dxgiAdapter;
		dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter));

		ComPtr<IDXGIFactory2> dxgiFactory2;
		dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory2));

		if (swapChain)
		{
			swapChain->Release();
			swapChain = nullptr;
		}

		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width = 1000;
		sd.Height = 1000;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.Stereo = FALSE;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		sd.BufferCount = 2;
		sd.Scaling = DXGI_SCALING_STRETCH;
		sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		sd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
		sd.Flags = 0;

		if (!window) {
			ThrowIfFailed(dxgiFactory2->CreateSwapChainForComposition(device.Get(), &sd, nullptr, &swapChain));
		}
		else {
			sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
			ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(device.Get(), window, &sd, nullptr, nullptr, &swapChain));
		}
	}

	void Graphics::CreateD2DDeviceAndContext()
	{
		IDXGIDevice* dxgiDevice = nullptr;
		ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&dxgiDevice)));

		ID2D1Factory1* d2dFactory = nullptr;
		D2D1_FACTORY_OPTIONS options = {};
		ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, (void**)&d2dFactory));

		ID2D1Device* d2dDevice = nullptr;
		ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice, &d2dDevice));

		ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &d2dContext));
	}

	void Graphics::CreateDeviceAndSwapChain(D3D_DRIVER_TYPE driverType, HWND window) {
		CreateDevice();
		CreateSwapChain(window);
	}

	void Graphics::CreateRenderTarget(UINT sampleCount, UINT sampleQuality)
	{

		sampleCount = max(min(sampleCount, 8), 1);


		if (sampleCount > 1)
		{
			if (sampleQuality == 0)
				sampleQuality = sampleCount - 1;
			UINT maxQuality = 0;
			device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, sampleCount, &maxQuality);
			if (maxQuality != 0) {
				if (sampleQuality >= maxQuality)
					sampleQuality = maxQuality - 1;
			} else sampleQuality = 0, sampleCount = 1;
		}

		if (renderTargetView) {
			renderTargetView.Reset();
		}

		sampleDesc.Count = sampleCount;
		sampleDesc.Quality = sampleQuality; // or D3D11_STANDARD_MULTISAMPLE_PATTERN

		antiAliasing = sampleDesc.Count > 1;

		ID3D11Texture2D* backBuffer;

		if (antiAliasing || superSampling) {
			DXGI_SWAP_CHAIN_DESC swapChainDesc;
			swapChain->GetDesc(&swapChainDesc);

			UINT width = swapChainDesc.BufferDesc.Width;
			UINT height = swapChainDesc.BufferDesc.Height;

			D3D11_TEXTURE2D_DESC texDesc = {};
			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.MipLevels = 1;
			texDesc.ArraySize = 1;
			texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			texDesc.SampleDesc = sampleDesc;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

			if (superSampling) {
				texDesc.Width *= scalingFactor;
				texDesc.Height *= scalingFactor;
			}

			ThrowIfFailed(device->CreateTexture2D(&texDesc, NULL, &backBuffer));

			if (superSampling) {
				texDesc.Width = width;
				texDesc.Height = height;
				texDesc.SampleDesc.Count = 1;
				texDesc.SampleDesc.Quality = 0;
				texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

				ID3D11Texture2D* pResolveTexture = nullptr;
				device->CreateTexture2D(&texDesc, nullptr, &pResolveTexture);
			}

			if (msaaTexture) {
				msaaTexture->Release();
				msaaTexture = nullptr;
			}
			msaaTexture = backBuffer;
		} else ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));


		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = antiAliasing ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

		ThrowIfFailed(device->CreateRenderTargetView(backBuffer, &rtvDesc, &renderTargetView));

		if (!antiAliasing) backBuffer->Release();
		

		D3D11_BUFFER_DESC cbd = {};
		cbd.ByteWidth = sizeof(ConstantBuffer);
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		ThrowIfFailed(device->CreateBuffer(&cbd, NULL, &constantBuffer));

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

	void Graphics::CreateRenderTexture(UINT width, UINT height) {
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		device->CreateTexture2D(&textureDesc, nullptr, renderTexture.GetAddressOf());
		device->CreateRenderTargetView(renderTexture.Get(), nullptr, textureRTV.GetAddressOf());
		device->CreateShaderResourceView(renderTexture.Get(), nullptr, textureSRV.GetAddressOf());
	}

	void Graphics::CreateDepthStencil() {

		if (depthStencilBuffer) {
			depthStencilBuffer.Reset();
			depthStencilView.Reset();
			depthStencilState.Reset();
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChain->GetDesc(&swapChainDesc);

		D3D11_TEXTURE2D_DESC depthDesc = {};

		depthDesc.Width = swapChainDesc.BufferDesc.Width;
		depthDesc.Height = swapChainDesc.BufferDesc.Height;

		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc = sampleDesc;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		ThrowIfFailed(device->CreateTexture2D(&depthDesc, nullptr, &depthStencilBuffer));

		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = antiAliasing ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;

		ThrowIfFailed(device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, &depthStencilView));

		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		device->CreateDepthStencilState(&dsDesc, &depthStencilState);
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
			indexCount = scene->GetIndexCount();

			Texture* texture = scene->GetTexture();
			ThrowIfFailed(device->CreateShaderResourceView(texture->GetResource(), nullptr, textureView.GetAddressOf()));

			context->PSSetShaderResources(0, 1, textureView.GetAddressOf());
		}
	}

	Scene* Graphics::CreateScene() {
		scene = make_unique<Scene>();
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

	void Graphics::Resize(HWND window) {
		RECT rect;
		GetClientRect(window, &rect);

		Resize(rect.right - rect.left, rect.bottom - rect.top);
	}

	void Graphics::Resize(UINT width, UINT height) {
		//RenderTargetView.c(); reset this one
		if (width == 0 || height == 0) return;

		context->OMSetRenderTargets(0, NULL, NULL);
		renderTargetView.Reset();
		depthStencilView.Reset();
		//backBuffer.Reset();
		//constantBuffer.Reset();
		if (backBuffer) {
			backBuffer->Release();
			dxgiBackBuffer->Release();
			if (d2dTargetBitmap1) d2dTargetBitmap1->Release();
		}

		ThrowIfFailed(swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

		CreateRenderTarget();
		CreateDepthStencil();
		
		swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

		backBuffer->QueryInterface(IID_PPV_ARGS(&dxgiBackBuffer));

		auto bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED), 144, 144);

		if (d2dContext) {

			d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, bitmapProperties, &d2dTargetBitmap1);
			d2dContext->SetTarget(d2dTargetBitmap1);
		}

		/*ComPtr<ID3D11Resource> backBuffer;
		ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
		ThrowIfFailed(device->CreateRenderTargetView(backBuffer.Get(), NULL, &renderTargetView));*/

		UpdateViewport(width, height);
	}

	void Graphics::UpdateViewport(FLOAT width, FLOAT height) {

		if (width == 0 || height == 0) return;

		FLOAT aspectRatio = width / height;

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

	void Graphics::Clear() {
		const float color[] = { 1.0f, 1.0f, 1.0f, 0.0f };
		context->ClearRenderTargetView(renderTargetView.Get(), color);
		if (depthStencilView)
			context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	ID3D11Texture2D* Graphics::RenderToTexture(float angle, float x, float y, float z) {

		const float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		context->ClearRenderTargetView(textureRTV.Get(), color);
		if (depthStencilView)
			context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		D3D11_TEXTURE2D_DESC desc;
		renderTexture->GetDesc(&desc);
		desc.BindFlags = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		ComPtr<ID3D11Texture2D> stagingTexture;
		device->CreateTexture2D(&desc, nullptr, stagingTexture.GetAddressOf());

		context->OMSetRenderTargets(1, textureRTV.GetAddressOf(), depthStencilView.Get());

		const ConstantBuffer cb = { {
			XMMatrixTranspose(
				XMMatrixRotationZ(z) *
				XMMatrixRotationY(x) *
				XMMatrixRotationX(y) *
				XMMatrixTranslation(x, -y, 0) *
				viewMatrix * projectionMatrix
			)
		} };

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &cb, sizeof(cb));
		context->Unmap(constantBuffer.Get(), 0);


		context->DrawIndexed(indexCount, 0, 0);

		ID2D1Bitmap1* bitmap = CreateD2DBitmapFromTexture(renderTexture.Get());
		SaveBitmapToFile(bitmap, L"C:\\Users\\Lasse\\Documents\\outp baoorfedut.png");

		context->CopyResource(stagingTexture.Get(), renderTexture.Get());

		return stagingTexture.Detach();
	}


	void Graphics::SaveTextureToFIle(ID3D11Texture2D* texture, WCHAR* fileName) {
		/*texture->GetPrivateData()*/

		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


		D3D11_TEXTURE2D_DESC desc;
		texture->GetDesc(&desc);

		// Create staging texture
		desc.BindFlags = 0;
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture;
		HRESULT hr = device->CreateTexture2D(&desc, nullptr, &stagingTexture);

		// Copy to staging texture
		context->CopyResource(stagingTexture.Get(), texture);


		D3D11_MAPPED_SUBRESOURCE mapped;
		hr = context->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mapped);

		try {
			// Create GDI+ bitmap
			Gdiplus::Bitmap bitmap(desc.Width, desc.Height, mapped.RowPitch,
				PixelFormat32bppARGB, static_cast<BYTE*>(mapped.pData));

			// Determine file format from extension if needed
			// (Alternative to passing the GUID parameter)

			// Save to file
			CLSID clsid;
				
			//Gdiplus::GetImageEncoders

			UINT num = 0;          // number of image encoders
			UINT size = 0;         // size of the image encoder array in bytes

			Gdiplus::GetImageEncodersSize(&num, &size);

			Gdiplus::ImageCodecInfo* pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));\

			Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

			for (UINT j = 0; j < num; ++j)
			{
				if (wcscmp(pImageCodecInfo[j].MimeType, L"image/png") == 0)
				{
					clsid = pImageCodecInfo[j].Clsid;
				}
			}
			free(pImageCodecInfo);

			bitmap.Save(fileName, &clsid, nullptr);
		}
		catch (...) {
			context->Unmap(stagingTexture.Get(), 0);
		}

		context->Unmap(stagingTexture.Get(), 0);

		Gdiplus::GdiplusShutdown(gdiplusToken);
	}

	ID2D1Bitmap1* Graphics::CreateD2DBitmapFromTexture(ID3D11Texture2D* texture)
	{
		// Get the DXGI surface from the D3D11 texture
		ComPtr<IDXGISurface> dxgiSurface;
		HRESULT hr = texture->QueryInterface(IID_PPV_ARGS(&dxgiSurface));

		D3D11_TEXTURE2D_DESC textureDesc;

		texture->GetDesc(&textureDesc);


		// Bitmap properties
		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
		);

		// Create the D2D bitmap
		ComPtr<ID2D1Bitmap1> d2dBitmap;
		hr = d2dContext->CreateBitmapFromDxgiSurface(
			dxgiSurface.Get(),
			NULL,
			&d2dBitmap
		);

		if (FAILED(hr))
		{
			// Handle error
			return nullptr;
		}

		return d2dBitmap.Detach();
	}

	HRESULT Graphics::CreateD2DBitmapFromD3DTexture(ID3D11Texture2D* d3dTexture, ID2D1DeviceContext* d2dContext, ID2D1Bitmap1** outD2dBitmap)
	{
		if (!d3dTexture || !d2dContext || !outD2dBitmap)
			return E_INVALIDARG;

		// 1. Get texture description
		D3D11_TEXTURE2D_DESC desc;
		d3dTexture->GetDesc(&desc);

		// 2. Verify the texture format is compatible with D2D
		// D2D typically works best with BGRA or RGBA formats
		if (desc.Format != DXGI_FORMAT_R8G8B8A8_UNORM &&
			desc.Format != DXGI_FORMAT_R8G8B8A8_UNORM)
		{
			return E_INVALIDARG;
		}

		// 3. Get DXGI surface from the texture
		Microsoft::WRL::ComPtr<IDXGISurface> dxgiSurface;
		HRESULT hr = d3dTexture->QueryInterface(IID_PPV_ARGS(&dxgiSurface));
		if (FAILED(hr)) {
			return hr;
		}

		// 4. Set up bitmap properties
		D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
		bitmapProps.pixelFormat.format = desc.Format;
		bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
		bitmapProps.dpiX = 96.0f;
		bitmapProps.dpiY = 96.0f;

		// 5. Create the D2D bitmap
		hr = d2dContext->CreateBitmapFromDxgiSurface(
			dxgiSurface.Get(),
			&bitmapProps,
			outD2dBitmap
		);

		return hr;
	}

	void Graphics::Render(float angle, float x, float y, float z) {
		//Render(camera)

		camera->rotation.x = x;
		camera->rotation.y = y;
		camera->rotation.z = z
			;
	}

	void Graphics::Render(Camera *camera) {

		Clear();
		{

			/*swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

			backBuffer->QueryInterface(IID_PPV_ARGS(&dxgiBackBuffer));

			auto bitmapProperties = D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(
					DXGI_FORMAT_B8G8R8A8_UNORM,
					D2D1_ALPHA_MODE_PREMULTIPLIED), 144, 144);

			d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, bitmapProperties, &d2dTargetBitmap1);
			d2dContext->SetTarget(d2dTargetBitmap1);

			d2dContext->BeginDraw();

			d2dContext->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));

			d2dContext->EndDraw();*/

		}
		if (!camera) camera = this->camera.Get();

		//XMMatrixRotation()

		context->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		//Camera *camera = new Camera();
		/*camera->rotation.x = x;
		camera->rotation.y = y;
		camera->rotation.z = z;*/
		
		const ConstantBuffer cb = { {
			XMMatrixTranspose(
				XMMatrixRotationY(camera->rotation.x) *
				XMMatrixRotationX(camera->rotation.y) *
				XMMatrixRotationZ(camera->rotation.z) *
				XMMatrixTranslation(camera->rotation.x, -camera->rotation.y, 0) *
				viewMatrix * projectionMatrix
			)
		} };

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &cb, sizeof(cb));
		context->Unmap(constantBuffer.Get(), 0);


		context->DrawIndexed(indexCount, 0, 0);

		if (antiAliasing) {

			ID3D11Texture2D* pBackBuffer = nullptr;
			swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

			context->ResolveSubresource(pBackBuffer, 0, msaaTexture.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);

			pBackBuffer->Release();
		}

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

		ID3D11BlendState* blendState = nullptr;
		ThrowIfFailed(device->CreateBlendState(&blendDesc, &blendState));

		float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		context->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);
	}

	IDXGISwapChain* Graphics::GetSwapChain() {
		return swapChain.Get();
	}

	ID3D11Device* Graphics::GetDevice() {
		return device.Get();
	}

	Texture* Graphics::CreateTexture(const path& filename) {
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		Gdiplus::Bitmap* pBitmap = Gdiplus::Bitmap::FromFile(filename.c_str());
		if (pBitmap == nullptr || pBitmap->GetLastStatus() != Gdiplus::Ok) {
			Gdiplus::GdiplusShutdown(gdiplusToken);
			ThrowIfFailed(E_FAIL);
			return NULL;
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

		Gdiplus::GdiplusShutdown(gdiplusToken);

		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		D3D11_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pSysMem = pPixels;
		subresourceData.SysMemPitch = width * 4;
		subresourceData.SysMemSlicePitch = 0;

		ID3D11Texture2D* texture = nullptr;
		ThrowIfFailed(device->CreateTexture2D(&textureDesc, &subresourceData, &texture));

		return new Texture(texture);

		ThrowIfFailed(device->CreateShaderResourceView(texture, nullptr, textureView.GetAddressOf()));
		texture->Release();
		delete[] pPixels;


		context->PSSetShaderResources(0, 1, textureView.GetAddressOf());
	}

	ID2D1Bitmap1* Graphics::RenderToBitmap() {

		ID3D11Texture2D* pBackBuffer = RenderToTexture(0.0f, 0.0f, 0.0f, 0.0f);

		// Create a DXGI surface from the back buffer
		IDXGISurface* pSurface = nullptr;
		HRESULT hr = pBackBuffer->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void**>(&pSurface));

		// Create the D2D bitmap
		ID2D1Bitmap1* pBitmap = nullptr;
		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET,
				D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
			);

			hr = d2dContext->CreateBitmapFromDxgiSurface(
				pSurface,
				&props,
				&pBitmap
			);
		}

		// Cleanup
		if (pSurface) pSurface->Release();
		if (pBackBuffer) pBackBuffer->Release();

		const WCHAR* a = L"C:\\Users\\Lasse\\Documents\\outpuotTfedstde.png";
		SaveBitmapToFile(pBitmap, a);

		return pBitmap;
	}

	void Graphics::SaveBitmapToFile(ID2D1Bitmap* bitmap, const WCHAR* fileName) {
		
		HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		if (FAILED(hr)) return;

		Microsoft::WRL::ComPtr<IWICImagingFactory> wicFactory;
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wicFactory)
		);
		if (FAILED(hr)) return;

		// 2. Create WIC components
		Microsoft::WRL::ComPtr<IWICStream> stream;
		hr = wicFactory->CreateStream(&stream);
		if (FAILED(hr)) return;

		hr = stream->InitializeFromFilename(fileName, GENERIC_WRITE);
		if (FAILED(hr)) return;

		Microsoft::WRL::ComPtr<IWICBitmapEncoder> encoder;
		hr = wicFactory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder);
		if (FAILED(hr)) return;

		hr = encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache);
		if (FAILED(hr)) return;

		Microsoft::WRL::ComPtr<IWICBitmapFrameEncode> frame;
		hr = encoder->CreateNewFrame(&frame, nullptr);
		if (FAILED(hr)) return;

		hr = frame->Initialize(nullptr);
		if (FAILED(hr)) return;

		// 3. Get bitmap properties
		D2D1_SIZE_U size = bitmap->GetPixelSize();
		WICPixelFormatGUID format = GUID_WICPixelFormat32bppPRGBA;

		hr = frame->SetSize(size.width, size.height);
		if (FAILED(hr)) return;

		hr = frame->SetPixelFormat(&format);
		if (FAILED(hr)) return;

		// 4. Copy bitmap data
		UINT stride = size.width * 4; // 4 bytes per pixel (BGRA)
		UINT bufferSize = stride * size.height;
		std::vector<BYTE> pixels(bufferSize);

		hr = bitmap->CopyFromMemory(nullptr, pixels.data(), stride);
		if (FAILED(hr)) return;

		// 5. Write to file
		hr = frame->WritePixels(size.height, stride, bufferSize, pixels.data());
		if (FAILED(hr)) return;

		hr = frame->Commit();
		if (FAILED(hr)) return;

		hr = encoder->Commit();
		if (FAILED(hr)) return;

		// 6. Cleanup
		stream->Commit(STGC_DEFAULT);
		CoUninitialize();
	}
}
