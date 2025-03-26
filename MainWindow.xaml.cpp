#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Data.Json.h> 
#include <nlohmann/json.hpp>

using namespace winrt;
using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::Windows::Web::Http;
using namespace winrt::Windows::Foundation;

namespace winrt::Islamiyat::implementation
{

    void MainWindow::MainMenu_SelectionChanged(winrt::Windows::Foundation::IInspectable const& sender, Controls::NavigationViewSelectionChangedEventArgs const& args)
    {
        auto selectedItem = args.SelectedItem().try_as<Controls::NavigationViewItem>();
        if (selectedItem)
        {
            auto tag = unbox_value_or<hstring>(selectedItem.Tag(), L"");
            // Hide all pages first
            ShariaLawPage().Visibility(Visibility::Collapsed);
            FIQHPage().Visibility(Visibility::Collapsed);
            PillarsPage().Visibility(Visibility::Collapsed);
            PoliticalIslamPage().Visibility(Visibility::Collapsed);
            InfoPage().Visibility(Visibility::Collapsed);
            AbdulBariPage().Visibility(Visibility::Collapsed);

            BG1().Visibility(Visibility::Collapsed);
            BG2().Visibility(Visibility::Collapsed);
            BG3().Visibility(Visibility::Collapsed);
            BG4().Visibility(Visibility::Collapsed);
            BG5().Visibility(Visibility::Collapsed);



            ElementSoundPlayer::Play(ElementSoundKind::Show);


            if (tag == L"InfoPage")
            {
                InfoPage().Visibility(Visibility::Visible);
                BG5().Visibility(Visibility::Visible);
            }
            else if (tag == L"ShariaPage")
            {
                ShariaLawPage().Visibility(Visibility::Visible);
                BG1().Visibility(Visibility::Visible);
            }
            else if (tag == L"FiqhPage")
            {
                FIQHPage().Visibility(Visibility::Visible);
                BG2().Visibility(Visibility::Visible);
            }
            else if (tag == L"PillarsPage")
            {
                PillarsPage().Visibility(Visibility::Visible);
                BG3().Visibility(Visibility::Visible);
            }
            else if (tag == L"PoliticalIslamPage")
            {
                PoliticalIslamPage().Visibility(Visibility::Visible);
                BG4().Visibility(Visibility::Visible);
            }
            else if (tag == L"AbdulBariPage")
            {
                AbdulBariPage().Visibility(Visibility::Visible);
                BG2().Visibility(Visibility::Visible);
            }
        }
    }

    void MainWindow::StartProgressRingAnimation()
    {
        auto delayMilliseconds = 100;
        auto progressBar = ProgressBarLoad();
        auto mainbarmenu = ProgressBarMain();
        mainbarmenu.Visibility(Visibility::Visible);
        progressBar.Value(0);

        std::thread([this, progressBar, delayMilliseconds, mainbarmenu]() {
            for (int i = 0; i <= 100; ++i)
            {
                DispatcherQueue().TryEnqueue([progressBar, i]() {
                    progressBar.Value(i);
                    });
                std::this_thread::sleep_for(std::chrono::milliseconds(delayMilliseconds));
            }

            DispatcherQueue().TryEnqueue([mainbarmenu]()
                {
                    mainbarmenu.Visibility(Visibility::Collapsed);
                });

            }).detach();
    }

    fire_and_forget MainWindow::SendMessageToGemini(hstring userMessage) {
        auto lifetime = get_strong();
        try {
            HttpClient httpClient;
            HttpRequestMessage request(
                HttpMethod::Post(),
                Uri(L"https://generativelanguage.googleapis.com/v1beta/models/gemini-1.5-flash:generateContent?key=API_KEY")
            );

            std::wstring jsonBody = LR"({"contents":[{"parts":[{"text": ")" +
                std::wstring(userMessage.c_str()) + LR"("}]}]})";

            HttpStringContent jsonContent(
                hstring(jsonBody),
                Windows::Storage::Streams::UnicodeEncoding::Utf8,
                L"application/json"
            );
            request.Content(jsonContent);

            HttpResponseMessage response = co_await httpClient.SendRequestAsync(request);

            if (response.StatusCode() == HttpStatusCode::Ok) {
                hstring responseText = co_await response.Content().ReadAsStringAsync();
                OutputDebugStringW((L"Raw API Response: " + std::wstring(responseText.c_str())).c_str());

                Windows::Data::Json::JsonObject jsonResponse;
                if (Windows::Data::Json::JsonObject::TryParse(responseText, jsonResponse)) {
                    if (jsonResponse.HasKey(L"candidates")) {
                        auto candidatesArray = jsonResponse.GetNamedArray(L"candidates");
                        if (candidatesArray.Size() > 0) {
                            auto firstCandidate = candidatesArray.GetAt(0).GetObject();
                            if (firstCandidate.HasKey(L"content")) {
                                auto contentObject = firstCandidate.GetNamedObject(L"content");
                                if (contentObject.HasKey(L"parts")) {
                                    auto partsArray = contentObject.GetNamedArray(L"parts");
                                    if (partsArray.Size() > 0) {
                                        auto textResponse = partsArray.GetAt(0).GetObject().GetNamedString(L"text");
                                        ChatbotResponseText().Text(textResponse);

                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    // **If JSON parsing fails, assume it's plain text**
                    OutputDebugStringW(L"Response is not JSON. Assuming plain text.");
                    ChatbotResponseText().Text(responseText);
                }
            }
            else {
                ChatbotResponseText().Text(L"Error: HTTP Request Failed! Please Check Your API Or Connection!");
            }
        }
        catch (const hresult_error& ex) {
            ChatbotResponseText().Text(L"Critical Error: " + ex.message());
        }
        catch (const std::exception& ex) {
            ChatbotResponseText().Text(L"Exception: " + to_hstring(ex.what()));
        }
    }

    void MainWindow::SendButton_Click(const IInspectable&, const RoutedEventArgs&) {
        hstring userInput = UserInputTextBox().Text();
        if (userInput.empty()) {
            ChatbotResponseText().Text(L"Please enter a message.");
            return;
        }
        SendMessageToGemini(userInput);
    }

    void MainWindow::OnCloseRequested()
    {

        if (MyWebView1() != nullptr)
        {
            auto coreWebView1 = MyWebView1();
            if (coreWebView1)
            {
                coreWebView1.Close();
            }
            MyWebView1(nullptr);
        }
        if (MyWebView2() != nullptr)
        {
            auto coreWebView2 = MyWebView2();
            if (coreWebView2)
            {
                coreWebView2.Close();
            }
            MyWebView2(nullptr);
        }
        if (MyWebView3() != nullptr)
        {
            auto coreWebView3 = MyWebView3();
            if (coreWebView3)
            {
                coreWebView3.Close();
            }
            MyWebView3(nullptr);
        }
        if (MyWebView4() != nullptr)
        {
            auto coreWebView4 = MyWebView4();
            if (coreWebView4)
            {
                coreWebView4.Close();
            }
            MyWebView4(nullptr);
        }

    }

    MainWindow::MainWindow()
    {
        this->Closed([this](auto&&, auto&&) { OnCloseRequested(); });
        InitializeComponent();
        StartProgressRingAnimation();
        ElementSoundPlayer::State(ElementSoundPlayerState::On);
        ElementSoundPlayer::SpatialAudioMode(ElementSpatialAudioMode::On);

        ExtendsContentIntoTitleBar(true);
        SetTitleBar(nullptr);

        auto ManuItem = MainMenu().MenuItems().GetAt(1).try_as<Controls::NavigationViewItem>();
        MainMenu().SelectedItem(ManuItem);

    }

}
