#include "MainFrame.h"
#include "ServerConnectionPanel.h"

#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/radiobut.h>
#include <wx/statbox.h>
#include <wx/spinctrl.h>

// ---------- PreferencesDialog (local to this file) ----------

class PreferencesDialog : public wxDialog
{
public:
    PreferencesDialog(wxWindow* parent, const AppSettings& settings)
        : wxDialog(parent, wxID_ANY, "Preferences",
                   wxDefaultPosition, wxDefaultSize,
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
          m_settings(settings)
    {
        auto* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Timestamps section
        auto* timestampBox = new wxStaticBoxSizer(wxVERTICAL, this, "Timestamps");

        m_showTimestamps = new wxCheckBox(this, wxID_ANY, "Show timestamps");
        m_showTimestamps->SetValue(m_settings.showTimestamps);
        timestampBox->Add(m_showTimestamps, 0, wxALL, 5);

        auto* formatLabel = new wxStaticText(this, wxID_ANY, "Time format:");
        timestampBox->Add(formatLabel, 0, wxLEFT | wxRIGHT | wxTOP, 5);

        m_format24Hour = new wxRadioButton(this, wxID_ANY, "24-hour (13:45:30)",
                                           wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
        m_format12Hour = new wxRadioButton(this, wxID_ANY, "12-hour (1:45:30 PM)");

        m_format24Hour->SetValue(m_settings.use24HourFormat);
        m_format12Hour->SetValue(!m_settings.use24HourFormat);

        timestampBox->Add(m_format24Hour, 0, wxLEFT, 20);
        timestampBox->Add(m_format12Hour, 0, wxLEFT, 20);

        mainSizer->Add(timestampBox, 0, wxEXPAND | wxALL, 10);

        // Connection section
        auto* connectionBox = new wxStaticBoxSizer(wxVERTICAL, this, "Connection");

        m_autoReconnect = new wxCheckBox(this, wxID_ANY, "Automatically reconnect on disconnect");
        m_autoReconnect->SetValue(m_settings.autoReconnect);
        connectionBox->Add(m_autoReconnect, 0, wxALL, 5);

        // Max reconnect attempts
        auto* attemptsRow = new wxBoxSizer(wxHORIZONTAL);
        auto* attemptsLabel = new wxStaticText(this, wxID_ANY, "Max reconnect attempts:");
        m_maxAttempts = new wxSpinCtrl(this, wxID_ANY);
        m_maxAttempts->SetRange(0, 99);
        m_maxAttempts->SetValue(m_settings.maxReconnectAttempts);

        attemptsRow->Add(attemptsLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        attemptsRow->Add(m_maxAttempts, 0);

        connectionBox->Add(attemptsRow, 0, wxALL, 5);

        auto* attemptsNote = new wxStaticText(this, wxID_ANY, "(0 = unlimited attempts)");
        wxFont noteFont = attemptsNote->GetFont();
        noteFont.SetPointSize(noteFont.GetPointSize() - 1);
        attemptsNote->SetFont(noteFont);
        connectionBox->Add(attemptsNote, 0, wxLEFT, 20);

        mainSizer->Add(connectionBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        // Buttons
        auto* btnOk = new wxButton(this, wxID_OK, "OK");
        auto* btnCancel = new wxButton(this, wxID_CANCEL, "Cancel");

        auto* btnSizer = new wxBoxSizer(wxHORIZONTAL);
        btnSizer->AddStretchSpacer(1);
        btnSizer->Add(btnOk, 0, wxRIGHT, 5);
        btnSizer->Add(btnCancel, 0);

        mainSizer->Add(btnSizer, 0, wxEXPAND | wxALL, 10);

        SetSizerAndFit(mainSizer);
        CentreOnParent();

        btnOk->SetDefault();
    }

    AppSettings GetSettings() const
    {
        AppSettings settings;
        settings.showTimestamps = m_showTimestamps->GetValue();
        settings.use24HourFormat = m_format24Hour->GetValue();
        settings.autoReconnect = m_autoReconnect->GetValue();
        settings.maxReconnectAttempts = m_maxAttempts->GetValue();
        return settings;
    }

private:
    AppSettings m_settings;
    wxCheckBox* m_showTimestamps = nullptr;
    wxRadioButton* m_format24Hour = nullptr;
    wxRadioButton* m_format12Hour = nullptr;
    wxCheckBox* m_autoReconnect = nullptr;
    wxSpinCtrl* m_maxAttempts = nullptr;
};

// ---------- QuickConnectDialog (local to this file) ----------

class QuickConnectDialog : public wxDialog
{
public:
    QuickConnectDialog(wxWindow* parent,
                       const wxString& defaultServer,
                       const wxString& defaultPort,
                       const wxString& defaultNick,
                       const wxString& defaultPassword = "")
        : wxDialog(parent, wxID_ANY, "Quick Connect",
                   wxDefaultPosition, wxDefaultSize,
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    {
        auto* serverLabel = new wxStaticText(this, wxID_ANY, "Server:");
        auto* portLabel = new wxStaticText(this, wxID_ANY, "Port:");
        auto* nickLabel = new wxStaticText(this, wxID_ANY, "Nickname:");
        auto* passwordLabel = new wxStaticText(this, wxID_ANY, "Password:");

        m_serverCtrl = new wxTextCtrl(this, wxID_ANY, defaultServer);
        m_portCtrl = new wxTextCtrl(this, wxID_ANY, defaultPort);
        m_nickCtrl = new wxTextCtrl(this, wxID_ANY, defaultNick);
        m_passwordCtrl = new wxTextCtrl(this, wxID_ANY, defaultPassword,
                                        wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);

        auto* formSizer = new wxFlexGridSizer(4, 2, 5, 5);
        formSizer->Add(serverLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        formSizer->Add(m_serverCtrl, 1, wxEXPAND);
        formSizer->Add(portLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        formSizer->Add(m_portCtrl, 0, wxEXPAND);
        formSizer->Add(nickLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        formSizer->Add(m_nickCtrl, 0, wxEXPAND);
        formSizer->Add(passwordLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        formSizer->Add(m_passwordCtrl, 0, wxEXPAND);
        formSizer->AddGrowableCol(1, 1);

        auto* passwordNote = new wxStaticText(this, wxID_ANY,
            "(Optional - leave blank if not required)");
        wxFont noteFont = passwordNote->GetFont();
        noteFont.SetPointSize(noteFont.GetPointSize() - 1);
        passwordNote->SetFont(noteFont);

        auto* btnOk = new wxButton(this, wxID_OK, "Connect");
        auto* btnCancel = new wxButton(this, wxID_CANCEL, "Cancel");

        auto* btnSizer = new wxBoxSizer(wxHORIZONTAL);
        btnSizer->AddStretchSpacer(1);
        btnSizer->Add(btnOk, 0, wxRIGHT, 5);
        btnSizer->Add(btnCancel, 0);

        auto* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(formSizer, 0, wxEXPAND | wxALL, 10);
        mainSizer->Add(passwordNote, 0, wxLEFT | wxRIGHT | wxBOTTOM, 10);
        mainSizer->Add(btnSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        SetSizerAndFit(mainSizer);
        CentreOnParent();

        btnOk->SetDefault();
    }

    wxString GetServer() const { return m_serverCtrl->GetValue(); }
    wxString GetPort() const { return m_portCtrl->GetValue(); }
    wxString GetNick() const { return m_nickCtrl->GetValue(); }
    wxString GetPassword() const { return m_passwordCtrl->GetValue(); }

private:
    wxTextCtrl* m_serverCtrl = nullptr;
    wxTextCtrl* m_portCtrl = nullptr;
    wxTextCtrl* m_nickCtrl = nullptr;
    wxTextCtrl* m_passwordCtrl = nullptr;
};

// ---------- Menu IDs ----------

enum
{
    ID_Menu_Connect = wxID_HIGHEST + 1,
    ID_Menu_Disconnect,
    ID_Menu_ChangeNick,
    ID_Menu_ServerList,
    ID_Menu_Preferences,
    ID_Menu_About
};

// ---------- MainFrame implementation ----------

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "AstraIRC", wxDefaultPosition, wxSize(1100, 700))
{
    // Menus
    auto* menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT, "E&xit\tAlt-F4");

    auto* menuConnection = new wxMenu;
    menuConnection->Append(ID_Menu_Connect, "&Quick Connect...\tCtrl+K");
    menuConnection->Append(ID_Menu_Disconnect, "&Disconnect Current\tCtrl+D");
    menuConnection->AppendSeparator();
    menuConnection->Append(ID_Menu_ChangeNick, "Change &Nick...\tCtrl+N");
    menuConnection->Append(ID_Menu_ServerList, "&Server List...");

    auto* menuTools = new wxMenu;
    menuTools->Append(ID_Menu_Preferences, "&Preferences...\tCtrl+,");

    auto* menuHelp = new wxMenu;
    menuHelp->Append(ID_Menu_About, "&About AstraIRC");

    auto* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuConnection, "&Connection");
    menuBar->Append(menuTools, "&Tools");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar(1);
    SetStatusText("Not connected");

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Create server notebook for multiple connections
    m_serverNotebook = new wxAuiNotebook(
        this, wxID_ANY,
        wxDefaultPosition, wxDefaultSize,
        wxAUI_NB_DEFAULT_STYLE |
        wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
        wxAUI_NB_CLOSE_ON_ALL_TABS
    );

    m_serverNotebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE,
                           &MainFrame::OnServerTabClosed,
                           this);

    mainSizer->Add(m_serverNotebook, 1, wxEXPAND | wxALL, 5);
    SetSizer(mainSizer);

    // Default connection settings
    m_defaultServer = "irc.libera.chat";
    m_defaultPort = "6667";
    m_defaultNick = "AstraUser";

    // Menu bindings
    Bind(wxEVT_MENU, &MainFrame::OnMenuExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &MainFrame::OnMenuConnect, this, ID_Menu_Connect);
    Bind(wxEVT_MENU, &MainFrame::OnMenuDisconnect, this, ID_Menu_Disconnect);
    Bind(wxEVT_MENU, &MainFrame::OnMenuChangeNick, this, ID_Menu_ChangeNick);
    Bind(wxEVT_MENU, &MainFrame::OnMenuServerList, this, ID_Menu_ServerList);
    Bind(wxEVT_MENU, &MainFrame::OnMenuPreferences, this, ID_Menu_Preferences);
    Bind(wxEVT_MENU, &MainFrame::OnMenuAbout, this, ID_Menu_About);

    // Window activation
    Bind(wxEVT_ACTIVATE, &MainFrame::OnActivate, this);
}

void MainFrame::OnMenuExit(wxCommandEvent&)
{
    Close(true);
}

void MainFrame::OnMenuConnect(wxCommandEvent&)
{
    QuickConnectDialog dlg(this, m_defaultServer, m_defaultPort, m_defaultNick, m_defaultPassword);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxString server = dlg.GetServer();
    wxString port = dlg.GetPort();
    wxString nick = dlg.GetNick();
    wxString password = dlg.GetPassword();

    if (server.IsEmpty() || port.IsEmpty() || nick.IsEmpty())
    {
        wxMessageBox("Server, port, and nickname must not be empty.",
                     "Quick Connect", wxOK | wxICON_WARNING, this);
        return;
    }

    // Save as new defaults
    m_defaultServer = server;
    m_defaultPort = port;
    m_defaultNick = nick;
    m_defaultPassword = password;

    // Create a new server connection panel
    auto* serverPanel = new ServerConnectionPanel(m_serverNotebook, server, port, nick, m_settings, password);
    wxString tabTitle = GenerateServerTabTitle(server, nick);

    int pageIdx = m_serverNotebook->AddPage(serverPanel, tabTitle, true);
    m_serverPanels[pageIdx] = serverPanel;

    SetStatusText("Connecting to " + server + ":" + port + "...");
}

void MainFrame::OnMenuDisconnect(wxCommandEvent&)
{
    ServerConnectionPanel* panel = GetCurrentServerPanel();
    if (!panel)
    {
        wxMessageBox("No active connection to disconnect.",
                     "Disconnect", wxOK | wxICON_INFORMATION, this);
        return;
    }

    panel->DisconnectCore();
    SetStatusText("Disconnected");
}

void MainFrame::OnMenuChangeNick(wxCommandEvent&)
{
    ServerConnectionPanel* panel = GetCurrentServerPanel();
    if (!panel)
    {
        wxMessageBox("No active server.",
                     "Change Nick", wxOK | wxICON_INFORMATION, this);
        return;
    }

    wxString currentNick = panel->GetNick();

    wxTextEntryDialog dlg(this,
                          "Enter new nickname:",
                          "Change Nick",
                          currentNick);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString newNick = dlg.GetValue();
        if (!newNick.IsEmpty())
            panel->RequestNickChange(newNick);
    }
}

void MainFrame::OnMenuServerList(wxCommandEvent&)
{
    wxMessageBox("Server List dialog is not implemented yet.",
                 "Server List",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MainFrame::OnMenuPreferences(wxCommandEvent&)
{
    PreferencesDialog dlg(this, m_settings);
    if (dlg.ShowModal() == wxID_OK)
    {
        m_settings = dlg.GetSettings();

        // Apply settings to all server panels
        for (auto& [idx, panel] : m_serverPanels)
        {
            if (panel)
                panel->ApplySettings(m_settings);
        }
    }
}

void MainFrame::OnMenuAbout(wxCommandEvent&)
{
    wxMessageBox(
        "AstraIRC v1.2.0\n\n"
        "A modern, cross-platform IRC client\n"
        "built with wxWidgets.\n\n"
        "Supports Windows, macOS, and Linux.\n\n"
        "New in v1.2.0:\n"
        "- User Profile Window (WHOIS)\n"
        "- Multiple server connections",
        "About AstraIRC",
        wxOK | wxICON_INFORMATION,
        this);
}

void MainFrame::OnActivate(wxActivateEvent& evt)
{
    // When window is activated, focus the input box
    if (evt.GetActive())
    {
        ServerConnectionPanel* panel = GetCurrentServerPanel();
        if (panel)
            panel->FocusInput();
    }
    evt.Skip();
}

ServerConnectionPanel* MainFrame::GetCurrentServerPanel()
{
    if (!m_serverNotebook)
        return nullptr;

    int selection = m_serverNotebook->GetSelection();
    if (selection == wxNOT_FOUND)
        return nullptr;

    auto it = m_serverPanels.find(selection);
    if (it != m_serverPanels.end())
        return it->second;

    return nullptr;
}

wxString MainFrame::GenerateServerTabTitle(const wxString& server, const wxString& nick)
{
    return server + " (" + nick + ")";
}

void MainFrame::OnServerTabClosed(wxAuiNotebookEvent& evt)
{
    int page = evt.GetSelection();

    // Remove from map
    auto it = m_serverPanels.find(page);
    if (it != m_serverPanels.end())
    {
        // Disconnect before closing
        if (it->second)
            it->second->DisconnectCore();

        m_serverPanels.erase(it);
    }

    // Update status
    if (m_serverPanels.empty())
        SetStatusText("Not connected");
}
