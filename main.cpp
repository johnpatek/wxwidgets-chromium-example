#include <wx/wx.h>
#include <wx/webview.h>
#include <wx/webview_chromium.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/uri.h>
#include <wx/wfstream.h>
#include <wx/msw/private.h>
#include "include/cef_app.h"
#include "include/cef_client.h"

#include <queue>

static bool InitProcess(const std::function<bool()> &initParent);

// a simple message queue
class EchoHandler : public wxWebViewHandler
{
public:
    EchoHandler(wxWindow *window)
        : wxWebViewHandler("https"), m_window(window)
    {
        SetVirtualHost("wxreact.ipc");
    }

    void StartRequest(const wxWebViewHandlerRequest &request,
                      wxSharedPtr<wxWebViewHandlerResponse> response) override
    {
        response->SetHeader("Access-Control-Allow-Origin", "*");
        response->SetHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        if (request.GetMethod() == "POST" && request.GetURI().EndsWith("/EchoMessage"))
        {
            wxMessageBox(wxString::Format("Posted Message: %s", request.GetDataString().c_str()), "Message Received", wxOK | wxICON_INFORMATION, m_window);
            response->SetStatus(200);
            response->Finish("");
        }
        else
        {
            response->SetStatus(404);
            response->FinishWithError();
        }
    }
private:
    wxWindow *m_window;
};

class ReactFrame : public wxFrame
{
public:
    ReactFrame(const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize) : wxFrame(nullptr, wxID_ANY, "wxWebView StartRequest Example", pos, size, wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL)
    {
        m_webView = new wxWebViewChromium(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    }

    ~ReactFrame() = default;

    void Init()
    {
        m_webView->Bind(
            wxEVT_WEBVIEW_CREATED, [&](wxWebViewEvent &event)
            {
                wxString directoryMapping = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
#if _WIN32
                directoryMapping.Replace("\\", "/");
#endif
                directoryMapping.Append("/dist");
                m_webView->RegisterHandler(wxSharedPtr<wxWebViewHandler>(new EchoHandler(this)));
                m_webView->SetRoot(directoryMapping);
                m_webView->LoadURL("file:///index.html"); 
            });
    }

private:
    wxWebViewChromium *m_webView;
};

class ReactApp : public wxApp
{
public:
    bool OnInit() override
    {
        return InitProcess([this]()
                           {
            m_reactFrame = std::make_unique<ReactFrame>(wxPoint(50, 50), wxSize(800, 600));
            m_reactFrame->Init();
            m_reactFrame->Show();
            return true; });
    }

    int OnExit() override
    {
        return 0;
    }

private:
    std::unique_ptr<ReactFrame> m_reactFrame;
};

wxIMPLEMENT_APP(ReactApp);

class ReactCefApp : public CefApp, public CefBrowserProcessHandler
{
public:
    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
    {
        return this;
    }
    IMPLEMENT_REFCOUNTING(ReactCefApp);
};

bool InitProcess(const std::function<bool()> &initParent)
{
    bool result = false;
    CefMainArgs main_args(wxGetInstance());
    CefRefPtr<ReactCefApp> cef_app = new ReactCefApp;
    if (CefExecuteProcess(main_args, cef_app, nullptr) < 0)
    {
        result = initParent();
    }
    return result;
}