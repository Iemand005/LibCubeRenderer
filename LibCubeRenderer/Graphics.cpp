#include "pch.h"
#include "include/Graphics.h"

namespace CubeRenderer {


	//void Graphics::AttachTo(SwapChainPanel panel) {
	//	auto nativePanel = panel.as<ISwapChainPanelNative>();
	//	ThrowIfFailed(nativePanel->SetSwapChain(swapChain.Get()));

	//	//UINT width = panel.Width();
	//	//UINT height = panel.Height();

	//	panel.SizeChanged([this](IInspectable const& sender, SizeChangedEventArgs const& args) {
	//		auto panel = sender.as<SwapChainPanel>();
	//		UINT width = panel.ActualWidth();
	//		UINT height = panel.ActualHeight();
	//		Resize(width, height);
	//		});	

	//}


	//Graphics::Graphics(SwapChainPanel panel) {

	//	Init();
	//	AttachTo(panel);
	//}

	void Graphics::CreateDevice()
	{

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};

		D3D_FEATURE_LEVEL selectedFeatureLevel;
		ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &selectedFeatureLevel, &context));

		ThrowIfFailed(device->QueryInterface(__uuidof(IDXGIDevice), &dxgiDevice));


#ifdef _DEBUG
		//ComPtr<ID3D11Debug> debug = device 
		////ComPtr<ID3D11InfoQueue> infoQueue = debug.as<ID3D11InfoQueue>();

		//infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
		//infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
		//infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_INFO, true);
		//infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_MESSAGE, true);
		//infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);

		///*D3D11_MESSAGE_ID hide[] = { D3D11_MESSAGE_ID_DEVICE_DRAW_RenderTargetVIEW_NOT_SET };
		//D3D11_INFO_QUEUE_FILTER filter = {};
		//filter.DenyList.NumIDs = _countof(hide);
		//filter.DenyList.pIDList = hide;
		//infoQueue->AddStorageFilterEntries(&filter);*/

#endif
	}

	void Graphics::CreateSwapChain() {

		/*ComPtr<IDXGIFactory2> factory;
		ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory2), factory.put_void()));*/
		//IDXGIDevice* dxgiDevice = device->QueryInterface();


		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = 1000; // Match the size of the window.
		swapChainDesc.Height = 1000;
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
		dxgiDevice->GetParent(__uuidof(IDXGIAdapter1), &dxgiAdapter);

		ComPtr<IDXGIFactory2> dxgiFactory2;
		dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory2);

		ComPtr<IDXGISwapChain1> swapChain1;
		swapChain.As<IDXGISwapChain1>(&swapChain1);
		ThrowIfFailed(dxgiFactory2->CreateSwapChainForComposition(device.Get(), &swapChainDesc, nullptr, &swapChain1));
	}

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

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#ifdef _DEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		//IDXGISwapChain* swapChain;
		//ComPtr<IDXGISwapChain> swapChainOld;
		ThrowIfFailed(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context));
		//swapChain.As<IDXGISwapChain>(&swapChainOld);

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

		ThrowIfFailed(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &RenderTargetView));
		
		backBuffer->QueryInterface(__uuidof(IDXGISurface), &dxgiSurface);

		D3D11_BUFFER_DESC cbd = {};
		cbd.ByteWidth = sizeof(ConstantBuffer);
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		ThrowIfFailed(device->CreateBuffer(&cbd, nullptr, &constantBuffer));


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

		/*D3D11_TEXTURE2D_DESC backBufferDesc = {};
		backBuffer->GetDesc(&backBufferDesc);*/

		//backBuffer->Release();

		//D3D11_VIEWPORT viewport = { 0 };
		//viewport.Width = static_cast<float>(backBufferDesc.Width);
		//viewport.Height = static_cast<float>(backBufferDesc.Height);
		//viewport.MaxDepth = 1.0f;

		//context->RSSetViewports(1, &viewport);
	}

	void Graphics::CreateTriangle() {
		// Define triangle vertices with positions and texture coordinates
		Vertex vertices[] = {
			// Top center vertex (red in UV space)
			{ { 0.0f, 0.5f, 0.0f }, { 0.5f, 0.0f } },

			// Bottom right vertex (green in UV space)
			{ { 0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } },

			// Bottom left vertex (blue in UV space)
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } }
		};

		// Create vertex buffer description
		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(vertices);
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		// Create subresource data
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };
		vertexData.pSysMem = vertices;

		// Create the vertex buffer
		HRESULT hr = device->CreateBuffer(
			&vertexBufferDesc,
			&vertexData,
			&vertexBuffer
		);

		if (FAILED(hr)) {
			throw std::exception("Failed to create vertex buffer");
		}
	}

	void Graphics::CreateInputLayout() {

		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R16G16_FLOAT, 0, offsetof(Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,  offsetof(Vertex, textureCoordinate), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		ThrowIfFailed(device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout));
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

		CreateDevice();
		CreateSwapChain();
		CreateRenderTarget();
		UpdateViewport(1000, 1000); // Initial size

		CreateVertexShader();
		CreatePixelShader();
		CreateInputLayout();
		CreateTriangle();

		// Set up initial rendering state
		ID3D11RenderTargetView* rtv = RenderTargetView.Get();
		context->OMSetRenderTargets(1, &rtv, nullptr);
		context->IASetInputLayout(inputLayout.Get());

		///*CreateDevice();
		//CreateSwapChain();
		//CreateRenderTarget();

		//CreatePixelShader();
		//CreateVertexShader();

		//CreateInputLayout();

		//CreateTriangle();*/

		//Resize(100, 100);


		//InitializeBlendState();

		///*D3D11_BUFFER_DESC cbd = {};
		//cbd.ByteWidth = sizeof(ConstantBuffer);
		//cbd.Usage = D3D11_USAGE_DYNAMIC;
		//cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		//cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		//ThrowIfFailed(device->CreateBuffer(&cbd, nullptr, constantBuffer.put()));

		//viewMatrix = XMMatrixTranslation(0.0f, 0.0f, 100.0f);

		//UpdateViewport(1000, 1000);*/

		//ComPtr<ID3DBlob> pBlob;

		//ID3D11Buffer* bufferArray[] = { constantBuffer.Get() };
		//context->VSSetConstantBuffers(0, 1, bufferArray);


		//context->IASetInputLayout(inputLayout.Get());

		//

		//context->PSSetShaderResources(0, 1, textureView.put());
		//context->PSSetSamplers(0, 1, sampler.put());

		//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	void Graphics::Init(HWND hWnd) {
		CreateDeviceAndSwapChain(hWnd);
		CreateRenderTarget();
		UpdateViewport(1000, 1000); // Initial size

		CreateVertexShader();
		CreatePixelShader();
		CreateInputLayout();
		CreateTriangle();

		// Set up initial rendering state
		ID3D11RenderTargetView* rtv = RenderTargetView.Get();
		context->OMSetRenderTargets(1, &rtv, nullptr);
		context->IASetInputLayout(inputLayout.Get());
	}

	void Graphics::CreateScene() {
		scene = std::make_unique<Scene>();
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

		context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
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

		context->OMSetRenderTargets(0, NULL, NULL);
		RenderTargetView = nullptr;

		ThrowIfFailed(swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0));

		ComPtr<ID3D11Resource> backBuffer;
		ThrowIfFailed(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));
		ThrowIfFailed(device->CreateRenderTargetView(backBuffer.Get(), NULL, &RenderTargetView));

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

		ThrowIfFailed(device->CreateTexture2D(&depthDesc, nullptr, &depthStencilBuffer));

		// 2. Create depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = depthDesc.Format;

		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		dsvDesc.Texture2D.MipSlice = 0;

		ThrowIfFailed(device->CreateDepthStencilView(depthStencilBuffer.Get(), &dsvDesc, &depthStencilView));

		// 3. Create depth stencil state
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = FALSE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

		ThrowIfFailed(device->CreateDepthStencilState(&dsDesc, &depthStencilState));

		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		vp.Width = width;
		vp.Height = height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		context->RSSetViewports(1, &vp);

		projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.1f, 500.0f);
	}

	void Graphics::Render(float angle, float x, float y, float z) {

		float clearColor[4] = { 1.0f, 0, 0.0f, 0 }; // Red
		context->ClearRenderTargetView(RenderTargetView.Get(), clearColor);

		ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC bd = {};
		D3D11_SUBRESOURCE_DATA sd = {};

		struct Vertex {
			float x, y;
		};

		struct Vertex vertices[] =
		{
			{0.0f, 2.0f},
			{ 2.5f, -2.5f },
			{-2.5f, -2.5f},
		};

		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(vertices);
		bd.StructureByteStride = sizeof(Vertex);
		//D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;

		const UINT stride = sizeof(Vertex);
		const UINT offset = 0;

		ComPtr<ID3D11VertexShader> pVertexShader;
		ComPtr<ID3DBlob> pBlob;

		std::wstring fullPath = GetExecutableDirectory() / L"VertexShader.cso";
		auto hr = D3DReadFileToBlob(fullPath.c_str(), &pBlob);

		device->CreateBuffer(&bd, &sd, &pVertexBuffer);

		ID3D11Buffer* vertexBuffer = pVertexBuffer.Get();
		context->IASetVertexBuffers(0u, 1u, &vertexBuffer, &stride, &offset);
		device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVertexShader);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		/*ID3D11InputLayout* inputLayout;

		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout);*/
		ID3D11RenderTargetView* rtv = RenderTargetView.Get();
		context->OMSetRenderTargets(1, &rtv, nullptr);

		context->IASetInputLayout(inputLayout.Get());

		D3D11_VIEWPORT viewport;

		// Set viewport dimensions
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = 100;
		viewport.Height = 100;

		// Standard depth range
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		// Bind the viewport
		context->RSSetViewports(1, &viewport);

		//dxgiSurface->

		context->Draw(std::size(vertices), 0);

		swapChain->Present(1, 0);
	}

	void Graphics::Clear() {
		//float clearColor[4] = { 1.0f, 0, 0.0f, 0 }; // Red
		//context->ClearRenderTargetView(RenderTargetView.Get(), clearColor);
		//context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}



	void Graphics::Present() {
		//ThrowIfFailed(swapChain->Present(1, 0));
	}

	void Graphics::SetScene(std::unique_ptr<Scene> scene)
	{
		this->scene = std::move(scene);

		auto indices = this->scene->GetIndices();
		auto vertices = this->scene->GetVertices();
		size_t verticesSize = this->scene->GetVerticesSize();
		size_t indicesSize = this->scene->GetIndicesSize();
		SetVertexBuffer(vertices, verticesSize);
		SetIndexBuffer(indices, indicesSize);
	}

	Scene* Graphics::GetScene()
	{
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

}
