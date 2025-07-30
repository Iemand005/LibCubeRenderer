#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <Graphics.h>

#include <microsoft.ui.xaml.media.dxinterop.h>
#include <microsoft.ui.xaml.window.h>

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

    IDXGISurface* dxgiBackBuffer;
    ID2D1Factory1* d2dFactory;
    ID2D1Device* d2dDevice;
    ID2D1DeviceContext* d2dContext;
    ID2D1SolidColorBrush* d2dbrush;
    ID2D1Bitmap1* d2dTargetBitmap1;
    IDXGIDevice1* dxgiDevice;
    //ID2D1DeviceContext* d2dContext;
    
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

            Graphics* graphics = new Graphics();

            graphics->OnError = [](HRESULT hr) {
                OutputDebugString(TEXT("Oops"));
                };

            //Scene* scene = graphics->CreateScene();


            Scene* scene = graphics->Init();

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

            /*auto canvas = Canvas();
            canvas.*/

            auto swapChainPanel = this->SwapChainPanel();
            IDXGISwapChain* swapChain = graphics->GetSwapChain();
            auto nativePanel = swapChainPanel.as<ISwapChainPanelNative>();
            //nativePanel->SetSwapChain(swapChain);

            swapChainPanel.SizeChanged([graphics](IInspectable const&, SizeChangedEventArgs const& e) {
                try {
                    auto size = e.NewSize();
                    graphics->Resize(size.Width, size.Height);
                    //graphics->Render(1.0f, -3.0f, -2.0f, 0.0f);
                }
                catch (const runtime_error& e) {

                }
                });

            //graphics->Render(3.0f, -3.0f, -2.0f, 0.0f);



			D2D1_FACTORY_OPTIONS options = {};
			options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
			D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &d2dFactory);

			ID3D11Device* d3dDevice = graphics->GetDevice();

			d3dDevice->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
             
			d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);

			d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);

			IDXGIAdapter* dxgiAdapter = nullptr;
			dxgiDevice->GetAdapter(&dxgiAdapter);
			IDXGIFactory2* dxgiFactory = nullptr;
			dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
            swapChainDesc.Width = 1;
            swapChainDesc.Height = 1;
            swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
            swapChainDesc.Stereo = false;
            swapChainDesc.SampleDesc.Count = 1;                // don't use multi-sampling
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = 2;                     // use double buffering to enable flip
            swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // all apps must use this SwapEffect       
            swapChainDesc.Flags = 0;

			IDXGISwapChain1* swapChain1 = nullptr;
            dxgiFactory->CreateSwapChainForComposition(dxgiDevice, &swapChainDesc, NULL, &swapChain1);

			nativePanel->SetSwapChain(swapChain1);

            //dxgiFactory
            D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
				0, 0, D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT);
            
			ID2D1RenderTarget* d2dRenderTarget = nullptr;
			IDXGISurface* dxgiSurface = nullptr;

			//d2dFactory->CreateDxgiSurfaceRenderTarget(swapChain1, &renderTargetProperties, &d2dRenderTarget);
            //d2dFactory->CreateDxgiSurfaceRenderTarget(dxgiSurface, &renderTargetProperties, &d2dRenderTarget);

			d2dContext->CreateBitmapFromDxgiSurface()


			d2dDevice->Release();

            d2dRenderTarget->BeginDraw();
            d2dRenderTarget->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f));
            d2dRenderTarget->EndDraw();
            swapChain1->Present(1, 0);
        }
        catch (const runtime_error& e) {

        }
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
