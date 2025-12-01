#pragma once

#include <wx/panel.h>
#include <wx/stc/stc.h>
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
    // Text style IDs for different message types
    enum TextStyle
    {
        STYLE_DEFAULT = 0,      // Default text (white/black depending on theme)
        STYLE_TIMESTAMP,        // Timestamps (gray)
        STYLE_NICK,            // Nicknames (cyan/blue)
        STYLE_SYSTEM,          // System messages (green)
        STYLE_ERROR,           // Error messages (red)
        STYLE_ACTION,          // Action messages (purple/magenta)
        STYLE_NOTICE,          // Notice messages (yellow/orange)
        STYLE_TOPIC,           // Topic text (bright cyan)
        STYLE_URL              // URLs (bright blue, underlined)
    };

    explicit LogPanel(wxWindow* parent, const AppSettings* settings = nullptr, ServerConnectionPanel* serverPanel = nullptr);

    void AppendLog(const wxString& line);
    void Clear();
    void SetSettings(const AppSettings* settings);

    // Append text with specific style
    void AppendStyledText(const wxString& text, TextStyle style);

    // Append multiple segments with different styles
    void AppendStyledLog(const wxString& line, TextStyle defaultStyle = STYLE_DEFAULT);

    // Helper methods for common message types
    void AppendSystemMessage(const wxString& message);
    void AppendErrorMessage(const wxString& message);
    void AppendChatMessage(const wxString& nick, const wxString& message);
    void AppendNotice(const wxString& nick, const wxString& message);
    void AppendAction(const wxString& nick, const wxString& action);
    void AppendTopicMessage(const wxString& message);

private:
    void InitializeStyles();
    void OnChar(wxKeyEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnMouseMove(wxMouseEvent& evt);
    wxString FindUrlAtPosition(int pos);

    wxStyledTextCtrl* m_log = nullptr;
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
