#include "pch.h"
#include "include/BitmapGraphics.h"

using namespace CubeRenderer;

void BitmapGraphics::Resize(UINT width, UINT height) {
	auto graphics = (Graphics*)this;
	graphics->Resize(width, height);
	if (d2dContext) {
		auto bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D2D1_ALPHA_MODE_PREMULTIPLIED), 144, 144);

		d2dContext->CreateBitmapFromDxgiSurface(GetDXGIBackBuffer(), bitmapProperties, &d2dTargetBitmap1);
		d2dContext->SetTarget(d2dTargetBitmap1);
	}
}

void BitmapGraphics::SaveTextureToFIle(ID3D11Texture2D* texture, WCHAR* fileName) {

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	desc.BindFlags = 0;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> stagingTexture;
	HRESULT hr = GetDevice()->CreateTexture2D(&desc, nullptr, &stagingTexture);

	auto context = GetContext();
	context->CopyResource(stagingTexture.Get(), texture);


	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = context->Map(stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mapped);

	try {
		Gdiplus::Bitmap bitmap(desc.Width, desc.Height, mapped.RowPitch,
			PixelFormat32bppARGB, static_cast<BYTE*>(mapped.pData));

		CLSID clsid;

		UINT num = 0;
		UINT size = 0;

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

ID2D1Bitmap1* BitmapGraphics::CreateD2DBitmapFromTexture(ID3D11Texture2D* texture)
{
	ComPtr<IDXGISurface> dxgiSurface;
	HRESULT hr = texture->QueryInterface(IID_PPV_ARGS(&dxgiSurface));

	D3D11_TEXTURE2D_DESC textureDesc;

	texture->GetDesc(&textureDesc);


	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
		D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
		D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
	);

	ComPtr<ID2D1Bitmap1> d2dBitmap;
	hr = d2dContext->CreateBitmapFromDxgiSurface(
		dxgiSurface.Get(),
		NULL,
		&d2dBitmap
	);

	if (FAILED(hr))
	{
		return nullptr;
	}

	return d2dBitmap.Detach();
}

HRESULT BitmapGraphics::CreateD2DBitmapFromD3DTexture(ID3D11Texture2D* d3dTexture, ID2D1DeviceContext* d2dContext, ID2D1Bitmap1** outD2dBitmap)
{
	if (!d3dTexture || !d2dContext || !outD2dBitmap)
		return E_INVALIDARG;

	D3D11_TEXTURE2D_DESC desc;
	d3dTexture->GetDesc(&desc);

	if (desc.Format != DXGI_FORMAT_R8G8B8A8_UNORM &&
		desc.Format != DXGI_FORMAT_R8G8B8A8_UNORM)
	{
		return E_INVALIDARG;
	}

	Microsoft::WRL::ComPtr<IDXGISurface> dxgiSurface;
	HRESULT hr = d3dTexture->QueryInterface(IID_PPV_ARGS(&dxgiSurface));
	if (FAILED(hr)) {
		return hr;
	}

	D2D1_BITMAP_PROPERTIES1 bitmapProps = {};
	bitmapProps.pixelFormat.format = desc.Format;
	bitmapProps.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	bitmapProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	bitmapProps.dpiX = 96.0f;
	bitmapProps.dpiY = 96.0f;

	hr = d2dContext->CreateBitmapFromDxgiSurface(
		dxgiSurface.Get(),
		&bitmapProps,
		outD2dBitmap
	);

	return hr;
}

void BitmapGraphics::CreateD2DDeviceAndContext()
{
	IDXGIDevice* dxgiDevice = nullptr;
	ThrowIfFailed(GetDevice()->QueryInterface(IID_PPV_ARGS(&dxgiDevice)));

	ID2D1Factory1* d2dFactory = nullptr;
	D2D1_FACTORY_OPTIONS options = {};
	ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &options, (void**)&d2dFactory));

	ID2D1Device* d2dDevice = nullptr;
	ThrowIfFailed(d2dFactory->CreateDevice(dxgiDevice, &d2dDevice));

	ThrowIfFailed(d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &d2dContext));
}

ID3D11Texture2D* BitmapGraphics::RenderToTexture(float angle, float x, float y, float z) {

	const float color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	auto context = GetContext();
	//auto device = GetDevice();
	//context->ClearRenderTargetView(textureRTV.Get(), color);
	//if (depthStencilView)
	//	context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//D3D11_TEXTURE2D_DESC desc;
	//renderTexture->GetDesc(&desc);
	//desc.BindFlags = 0;
	//desc.Usage = D3D11_USAGE_STAGING;
	//desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	//ComPtr<ID3D11Texture2D> stagingTexture;
	//device->CreateTexture2D(&desc, nullptr, stagingTexture.GetAddressOf());

	//context->OMSetRenderTargets(1, textureRTV.GetAddressOf(), depthStencilView.Get());

	//const ConstantBuffer cb = { {
	//	XMMatrixTranspose(
	//		XMMatrixRotationZ(z) *
	//		XMMatrixRotationY(x) *
	//		XMMatrixRotationX(y) *
	//		XMMatrixTranslation(x, -y, 0) *
	//		viewMatrix * projectionMatrix
	//	)
	//} };

	//D3D11_MAPPED_SUBRESOURCE mappedResource;
	//context->Map(constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//memcpy(mappedResource.pData, &cb, sizeof(cb));
	//context->Unmap(constantBuffer.Get(), 0);


	//context->DrawIndexed(indexCount, 0, 0);

	//ID2D1Bitmap1* bitmap = CreateD2DBitmapFromTexture(renderTexture.Get());
	//SaveBitmapToFile(bitmap, L"C:\\Users\\Lasse\\Documents\\outp baoorfedut.png");

	//context->CopyResource(stagingTexture.Get(), renderTexture.Get());

	//return stagingTexture.Detach();
	return NULL;
}



ID2D1Bitmap1* BitmapGraphics::RenderToBitmap() {

	ID3D11Texture2D* pBackBuffer = RenderToTexture(0.0f, 0.0f, 0.0f, 0.0f);

	IDXGISurface* pSurface = nullptr;
	HRESULT hr = pBackBuffer->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void**>(&pSurface));

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

	if (pSurface) pSurface->Release();
	if (pBackBuffer) pBackBuffer->Release();

	const WCHAR* a = L"C:\\Users\\Lasse\\Documents\\outpuotTfedstde.png";
	SaveBitmapToFile(pBitmap, a);

	return pBitmap;
}

void BitmapGraphics::SaveBitmapToFile(ID2D1Bitmap* bitmap, const WCHAR* fileName) {

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

	D2D1_SIZE_U size = bitmap->GetPixelSize();
	WICPixelFormatGUID format = GUID_WICPixelFormat32bppPRGBA;

	hr = frame->SetSize(size.width, size.height);
	if (FAILED(hr)) return;

	hr = frame->SetPixelFormat(&format);
	if (FAILED(hr)) return;

	UINT stride = size.width * 4;
	UINT bufferSize = stride * size.height;
	std::vector<BYTE> pixels(bufferSize);

	hr = bitmap->CopyFromMemory(nullptr, pixels.data(), stride);
	if (FAILED(hr)) return;

	hr = frame->WritePixels(size.height, stride, bufferSize, pixels.data());
	if (FAILED(hr)) return;

	hr = frame->Commit();
	if (FAILED(hr)) return;

	hr = encoder->Commit();
	if (FAILED(hr)) return;

	stream->Commit(STGC_DEFAULT);
	CoUninitialize();
}