#include <wx/wx.h>
#include <wx/webview.h>
#include <wx/webview_chromium.h>

#include "include/cef_app.h"

class ExampleCefApp : public CefApp, public CefBrowserProcessHandler
{
public:
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
    {
        return this;
    }
    IMPLEMENT_REFCOUNTING(ExampleCefApp);
};

class ExampleWebViewHandler : public wxWebViewHandler
{
public:
    ExampleWebViewHandler(const wxString &directoryMapping) : wxWebViewHandler("wxapp") {}
    virtual ~ExampleWebViewHandler() {}
    void StartRequest(const wxWebViewHandlerRequest &request,
                      wxSharedPtr<wxWebViewHandlerResponse> response) override
    {
        wxString uri(request.GetURI());
        uri.Replace("wxapp:", "", true);
    }

private:
};

class ExampleApp : public wxApp
{
public:
    bool OnInit() override
    {
        m_frame = std::make_unique<wxFrame>(nullptr, wxID_ANY, "Chromium Example", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
        wxWebView *webView = wxWebView::New(m_frame.get(), wxID_ANY, "",
                                            wxDefaultPosition, wxDefaultSize, wxWebViewBackendChromium);
        webView->Bind(
            wxEVT_WEBVIEW_CREATED,
            [webView](wxWebViewEvent &event)
            {
                const wxString directoryMapping("");
                wxSharedPtr<wxWebViewHandler> handler(new ExampleWebViewHandler(directoryMapping));
                webView->RegisterHandler(handler);
                webView->LoadURL("wxapp:index.html");
            });
        return true;
    }
    int OnExit() override
    {
        m_frame.reset();
        return 0;
    }

private:
    std::unique_ptr<wxFrame> m_frame;
};

static void AppMain()
{
    std::unique_ptr<wxApp> app(new ExampleApp());
    wxApp::SetInstance(app.get());
    wxTheApp->CallOnInit();
    wxTheApp->OnRun();
    wxTheApp->OnExit();
    wxTheApp->CleanUp();
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    CefMainArgs main_args(hInstance);
    CefRefPtr<ExampleCefApp> app = new ExampleCefApp();
    int exit_code = CefExecuteProcess(main_args, app, nullptr);
    if (exit_code >= 0)
    {
        return exit_code;
    }
    wxInitialize();
    AppMain();
    wxUninitialize();
    return 0;
}