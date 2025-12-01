#pragma once

#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>
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

    // Helper methods for common message types
    void AppendSystemMessage(const wxString& message);
    void AppendErrorMessage(const wxString& message);
    void AppendChatMessage(const wxString& nick, const wxString& message);
    void AppendNotice(const wxString& nick, const wxString& message);
    void AppendAction(const wxString& nick, const wxString& action);
    void AppendTopicMessage(const wxString& message);

    // IRC color code parsing
    void AppendIRCStyledText(const wxString& text);

private:
    // IRC color code helpers
    wxColour GetIRCColor(int colorCode);
    struct IRCTextSegment
    {
        wxString text;
        wxColour foreground;
        wxColour background;
        bool bold = false;
        bool italic = false;
        bool underline = false;
        bool useDefaultFg = true;
        bool useDefaultBg = true;
    };
    void ParseIRCColors(const wxString& input, std::vector<IRCTextSegment>& segments);

    wxRichTextCtrl* m_log = nullptr;
    const AppSettings* m_settings = nullptr;
    ServerConnectionPanel* m_serverPanel = nullptr;
    wxColour m_defaultForeground;
    wxColour m_defaultBackground;
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
