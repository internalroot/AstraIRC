#pragma once

#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/listbox.h>
#include <wx/aui/aui.h>
#include <map>

#include "ChannelPage.h"
#include "irc_core.h"

// -------------------------------------------------------
// ServerConnectionPanel
// One server connection UI instance. Contains:
// - Console tab
// - Channel tabs
// - Nick change & /join handling
// - Input box + Send button
// - IRCCore callbacks (log, raw line, message)
// -------------------------------------------------------

class ServerConnectionPanel : public wxPanel
{
public:
    ServerConnectionPanel(wxWindow* parent,
                          const wxString& server,
                          const wxString& port,
                          const wxString& nick);
    ~ServerConnectionPanel() override;

    // Non-copyable
    ServerConnectionPanel(const ServerConnectionPanel&) = delete;
    ServerConnectionPanel& operator=(const ServerConnectionPanel&) = delete;

    // Reconnect using updated settings
    void ConnectWith(const wxString& server,
                     const wxString& port,
                     const wxString& nick);

    // Append to console
    void LogToConsole(const wxString& line);

    // Accessors
    wxString GetServer() const { return m_server; }
    wxString GetPort() const { return m_port; }
    wxString GetNick() const { return m_nick; }

    // Core controls
    void DisconnectCore();
    void RequestNickChange(const wxString& newNick);

private:
    // Helpers
    wxString BuildConsoleTabTitle() const;
    void ConnectCore();

    // Channel creation helper
    ChannelPage* GetOrCreateChannelPage(const wxString& channelName);

    // IRCCore callback handlers
    void HandleCoreLog(const wxString& msg);
    void HandleCoreMessage(const wxString& source, const wxString& text);
    void HandleRawLine(const wxString& line);

    // UI handlers
    void HandleJoinCommand(const wxString& text);
    void OnSend(wxCommandEvent& evt);
    void OnTabClosed(wxAuiNotebookEvent& evt);

private:
    // UI elements
    wxAuiNotebook* m_viewBook = nullptr;
    LogPanel* m_consoleView = nullptr;  // console tab at index 0
    wxTextCtrl* m_input = nullptr;
    wxButton* m_btnSend = nullptr;

    // Connection fields
    wxString m_server;
    wxString m_port;
    wxString m_nick;

    // Networking
    IRCCore m_core;

    // Open channels (channel name -> page pointer)
    std::map<wxString, ChannelPage*> m_channels;
};
