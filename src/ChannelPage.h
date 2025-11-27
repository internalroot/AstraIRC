#pragma once

#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/string.h>

// Simple log panel used by console and channels
class LogPanel : public wxPanel
{
public:
    explicit LogPanel(wxWindow* parent);

    void AppendLog(const wxString& line);
    void Clear();

private:
    wxTextCtrl* m_log = nullptr;
};

// A single channel tab: log on left, nick list on right
class ChannelPage : public wxPanel
{
public:
    ChannelPage(wxWindow* parent, const wxString& channelName);

    void AppendLog(const wxString& line);
    void ClearLog();
    wxListBox* GetNickList();
    const wxString& GetChannelName() const;

private:
    wxString m_channelName;
    LogPanel* m_log = nullptr;
    wxListBox* m_nickList = nullptr;
};
