#pragma once

#include "MainWindow.g.h"

namespace winrt::CubeRendererWinUI::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();
        //MainWindow()
        //{
        //    // Xaml objects should not call InitializeComponent during construction.
        //    // See https://github.com/microsoft/cppwinrt/tree/master/nuget#initializecomponent
        //}

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void InitializeDirectX();
        void CreateSwapChain();
        void ResizeSwapChain(UINT width, UINT height);
        void Draw();
        //void OnLoaded(IInspectable const&, RoutedEventArgs const&);
        void SwapChainPanel_Loaded(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void SwapChainPanel_SizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::SizeChangedEventArgs const& e);
    };
}

namespace winrt::CubeRendererWinUI::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
