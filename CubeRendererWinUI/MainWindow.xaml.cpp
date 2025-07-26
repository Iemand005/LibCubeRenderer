#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <Graphics.h>

#include <microsoft.ui.xaml.media.dxinterop.h>


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::CubeRendererWinUI::implementation
{
    using namespace winrt;
    using namespace Microsoft::UI::Xaml;
    using namespace CubeRenderer;
    
    MainWindow::MainWindow()
    {
        InitializeComponent();
		// Xaml objects should not call InitializeComponent during construction.
		// See

        Graphics* graphics = new Graphics();

        graphics->OnError = [](HRESULT hr) {
            OutputDebugString(TEXT("Oops"));
        };

        //Scene* scene = graphics->CreateScene();


        Scene* scene = graphics->Init();
        
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
		
        graphics->LoadTexture(GetExecutableDirectory() / L"steve.png");

		auto swapChainPanel = this->SwapChainPanel();
        IDXGISwapChain *swapChain = graphics->GetSwapChain();
        auto nativePanel = swapChainPanel.as<ISwapChainPanelNative>();
        nativePanel->SetSwapChain(swapChain);

		swapChainPanel.SizeChanged([graphics](IInspectable const&, SizeChangedEventArgs const& e) {
            auto size = e.NewSize();
            graphics->Resize(size.Width, size.Height);
            graphics->Render(1.0f, -3.0f, -2.0f, 0.0f);
		});

		graphics->Render(3.0f, -3.0f, -2.0f, 0.0f);
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
