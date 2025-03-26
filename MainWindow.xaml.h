#pragma once


#include "MainWindow.g.h"
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.Foundation.h>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::Islamiyat::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        void MainMenu_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, Controls::NavigationViewSelectionChangedEventArgs const& args);
        void StartProgressRingAnimation();
        fire_and_forget SendMessageToGemini(hstring userMessage);
        void SendButton_Click(const IInspectable&, const Microsoft::UI::Xaml::RoutedEventArgs&);
        void OnCloseRequested();
        MainWindow();
    };
}

namespace winrt::Islamiyat::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
