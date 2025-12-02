#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <Graphics.h>

#include <microsoft.ui.xaml.media.dxinterop.h>
#include <microsoft.ui.xaml.window.h>

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Microsoft.UI.Dispatching.h>

#include <d2d1_1.h>
#pragma comment(lib, "d2d1.lib")

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::CubeRendererWinUI::implementation
{
    using namespace winrt;
    using namespace Microsoft::UI::Xaml;
    using namespace CubeRenderer;
    using namespace DirectX;

    //IDXGISurface* dxgiBackBuffer;
    ID2D1Factory1* d2dFactory;
    ID2D1Device* d2dDevice;
    ID2D1DeviceContext* d2dContext;
    ID2D1SolidColorBrush* d2dbrush;
    ID2D1Bitmap1* d2dTargetBitmap1;
    //IDXGIDevice1* dxgiDevice;


    ID3D11Device* device;
    ID3D11Texture2D* backBuffer;
    IDXGISwapChain1* swapChain;
    IDXGISurface* dxgiBackBuffer;

    IDXGIDevice* dxgiDevice;

    Graphics* graphics;

    //DispatcherTimer timer;

    //ID2D1DeviceContext* d2dContext;
    
    void MainWindow::InitializeDirectX() {
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

        ID3D11DeviceContext* tempContext;

        D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, NULL, &tempContext);

        device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
    }

    void MainWindow::CreateSwapChain() {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.Width = 1000;
        swapChainDesc.Height = 1000;
        swapChainDesc.BufferCount = 2;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
        swapChainDesc.SampleDesc.Count = 1; // don't use multi-sampling
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        swapChainDesc.Flags = 0;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect

        IDXGIAdapter1* dxgiAdapter;
        dxgiDevice->GetParent(IID_PPV_ARGS(&dxgiAdapter));
        IDXGIFactory2* dxgiFactory2;
        dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory2));


        // resize window flick bug
        HRESULT hr = dxgiFactory2->CreateSwapChainForComposition(device, &swapChainDesc, NULL, &swapChain);

        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        // renderTargetView = device.CreateRenderTargetView(backBuffer);

        backBuffer->QueryInterface(IID_PPV_ARGS(&dxgiBackBuffer));

        auto nativePanel = SwapChainPanel().as<ISwapChainPanelNative>();

        nativePanel->SetSwapChain(swapChain);

        ID2D1Factory1* d2dFactory;
        D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID_PPV_ARGS(&d2dFactory));

        d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
        d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);

        auto bitmapProperties = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(
                DXGI_FORMAT_B8G8R8A8_UNORM,
                D2D1_ALPHA_MODE_PREMULTIPLIED));

        d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, bitmapProperties, &d2dTargetBitmap1);
        d2dContext->SetTarget(d2dTargetBitmap1);
        d2dContext->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 1.0f, 1.0f), &d2dbrush);

        dxgiDevice->Release();

        Draw();
    }

    void MainWindow::Draw() {
        d2dContext->BeginDraw();

        d2dContext->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f));

        /*graphics->Resize(100, 100);
		graphics->Render(0.0f, 0.0f, 0.0f, 0.0f);
		ID2D1Bitmap1* bitmap = graphics->RenderToBitmap();*/
        d2dContext->DrawLine(
            D2D1::Point2F(0.0f, 0.0f),
            D2D1::Point2F(100.0f, 100.0f),
            d2dbrush,
            10.0f);

        //auto texture = graphics->RenderToTexture(0.0f, 0.0f, 0.0f, 0.0f);
       // graphics->SaveTextureToFIle(texture, L"C:\\Users\\Lasse\\Documents\\outpdddddut.png");

        //ID2D1Bitmap1* bitmap = graphics->CreateD2DBitmapFromTexture(texture);
		/*graphics->SaveBitmapToFile(bitmap, L"C:\\Users\\Lasse\\Documents\\outp iiibarfedut.png"); */

        Texture* steveTexture = graphics->CreateTexture(GetExecutableDirectory() / L"C:\\Users\\Lasse\\Documents\\outpdddddut.png");

        ID2D1Bitmap1* bitmap = graphics->CreateD2DBitmapFromTexture(steveTexture->GetResource());

            d2dContext->DrawBitmap(bitmap, D2D1::RectF(0, 0, 1000, 1000), 1.0, D2D1_INTERPOLATION_MODE_LINEAR);

        d2dContext->DrawLine(
            D2D1::Point2F(0.0f, 0.0f),
            D2D1::Point2F(1000.0f, 1000.0f),
            d2dbrush,
            1.0f);

        d2dContext->EndDraw();

        swapChain->Present(1, 0);

        //graphics->Render(0, 0, 0, 0);
    }

    void MainWindow::ResizeSwapChain(UINT width, UINT height)
    {
        if (!d2dContext) return;

        d2dContext->SetTarget(NULL);

        backBuffer->Release();
        dxgiBackBuffer->Release();
        d2dTargetBitmap1->Release();

        swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

        backBuffer->QueryInterface(IID_PPV_ARGS(&dxgiBackBuffer));

        auto bitmapProperties = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(
                DXGI_FORMAT_B8G8R8A8_UNORM,
                D2D1_ALPHA_MODE_PREMULTIPLIED), 144, 144);

        d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, bitmapProperties, &d2dTargetBitmap1);
        d2dContext->SetTarget(d2dTargetBitmap1);

        Draw();
    }

    /*void MainWindow::OnLoaded(IInspectable const&, RoutedEventArgs const&)
    {
        CreateSwapChain();
    }*/

    MainWindow::MainWindow()
    {
        InitializeComponent();
		// Xaml objects should not call InitializeComponent during construction.
		// See

        try {
            /*winrt::
            var hwnd = WinRT.Interop.WindowNative.GetWindowHandle(this);*/

           /* auto windowNative{ this->try_as<IWindowNative>() };
            check_bool(windowNative);
            HWND hWnd{ nullptr };
            windowNative->get_WindowHandle(&hWnd);*/

            graphics = new Graphics();

            graphics->OnError = [](HRESULT hr) {
                OutputDebugString(TEXT("Oops"));
                };

            //Scene* scene = graphics->CreateScene();


            Scene* scene = graphics->Init();
			graphics->CreateD2DDeviceAndContext();

            Texture *steveTexture = graphics->CreateTexture(GetExecutableDirectory() / L"steve.png");

            scene->SetTexture(steveTexture);

            scene->AddCube(8, 8, 8, -2.0f, 22.0f, -2.0f, 0, 0, steveTexture);

            // Waist
            scene->AddCube(8, 12, 4, -2.0f, 12.0f, -2.0f, 16, 16, steveTexture);

            // Left arm
            scene->AddCube(4, 12, 4, 4.0f, 12.0f, -2.0f, 32, 48, steveTexture);

            // Right arm
            scene->AddCube(4, 12, 4, -8.0f, 12.0f, -2.0f, 40, 16, steveTexture);

            // Left leg
            scene->AddCube(4, 12, 4, -0.1, 0.0f, -2.0f, 16, 48, steveTexture);

            // Right leg
            scene->AddCube(4, 12, 4, -3.9f, 0.0f, -2.0f, 0, 16, steveTexture);

			graphics->UpdateScene();

            auto swapChain = graphics->GetSwapChain();

            auto nativePanel = SwapChainPanel().as<ISwapChainPanelNative>();

            nativePanel->SetSwapChain(swapChain);

			//graphics->Render(0.0f, 0.0f, 0.0f, 0.0f);


			//graphics->CreateRenderTexture(100, 100);

			

            InitializeDirectX();


        }
        catch (const runtime_error& e) {

        }
	}

    void MainWindow::SwapChainPanel_Loaded(IInspectable const& sender, RoutedEventArgs const& e)
    {
        CreateSwapChain();
        //timer.Start();
    }

    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}



void winrt::CubeRendererWinUI::implementation::MainWindow::SwapChainPanel_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e)
{
	auto newSize = e.NewSize();
	ResizeSwapChain(static_cast<UINT>(newSize.Width), static_cast<UINT>(newSize.Height));
}
