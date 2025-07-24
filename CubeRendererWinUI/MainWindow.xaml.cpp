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
        graphics->CreateScene();
		graphics->Init();
		graphics->LoadTexture(GetExecutableDirectory() / L"steve.png");

		auto swapChainPanel = this->SwapChainPanel();
        IDXGISwapChain *swapChain = graphics->GetSwapChain();
        auto nativePanel = swapChainPanel.as<ISwapChainPanelNative>();
        nativePanel->SetSwapChain(swapChain);

		swapChainPanel.SizeChanged([graphics](IInspectable const&, SizeChangedEventArgs const& e) {
            auto size = e.NewSize();
            graphics->Resize(static_cast<UINT>(size.Width), static_cast<UINT>(size.Height));
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
