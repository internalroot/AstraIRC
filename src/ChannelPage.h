#pragma once

#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/string.h>

// Forward declare - only need pointer
struct AppSettings;
class ServerConnectionPanel;

// Simple log panel used by console and channels
class LogPanel : public wxPanel
{
public:
    explicit LogPanel(wxWindow* parent, const AppSettings* settings = nullptr, ServerConnectionPanel* serverPanel = nullptr);

    void AppendLog(const wxString& line);
    void Clear();
    void SetSettings(const AppSettings* settings);

private:
    void OnChar(wxKeyEvent& evt);
    void OnURL(wxTextUrlEvent& evt);

    wxTextCtrl* m_log = nullptr;
    const AppSettings* m_settings = nullptr;
    ServerConnectionPanel* m_serverPanel = nullptr;
};

// A single channel tab: log on left, nick list on right
class ChannelPage : public wxPanel
{
public:
    ChannelPage(wxWindow* parent, const wxString& channelName, const AppSettings* settings = nullptr, ServerConnectionPanel* serverPanel = nullptr);

    void AppendLog(const wxString& line);
    void ClearLog();
    wxListBox* GetNickList();
    const wxString& GetChannelName() const;
    void SetSettings(const AppSettings* settings);

private:
    void OnNickDoubleClick(wxCommandEvent& evt);

    wxString m_channelName;
    LogPanel* m_log = nullptr;
    wxListBox* m_nickList = nullptr;
    ServerConnectionPanel* m_serverPanel = nullptr;
};
