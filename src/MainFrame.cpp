#include "MainFrame.h"
#include "ServerConnectionPanel.h"

#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>

// ---------- QuickConnectDialog (local to this file) ----------

class QuickConnectDialog : public wxDialog
{
public:
    QuickConnectDialog(wxWindow* parent,
                       const wxString& defaultServer,
                       const wxString& defaultPort,
                       const wxString& defaultNick)
        : wxDialog(parent, wxID_ANY, "Quick Connect",
                   wxDefaultPosition, wxDefaultSize,
                   wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    {
        auto* serverLabel = new wxStaticText(this, wxID_ANY, "Server:");
        auto* portLabel = new wxStaticText(this, wxID_ANY, "Port:");
        auto* nickLabel = new wxStaticText(this, wxID_ANY, "Nickname:");

        m_serverCtrl = new wxTextCtrl(this, wxID_ANY, defaultServer);
        m_portCtrl = new wxTextCtrl(this, wxID_ANY, defaultPort);
        m_nickCtrl = new wxTextCtrl(this, wxID_ANY, defaultNick);

        auto* formSizer = new wxFlexGridSizer(3, 2, 5, 5);
        formSizer->Add(serverLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        formSizer->Add(m_serverCtrl, 1, wxEXPAND);
        formSizer->Add(portLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        formSizer->Add(m_portCtrl, 0, wxEXPAND);
        formSizer->Add(nickLabel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        formSizer->Add(m_nickCtrl, 0, wxEXPAND);
        formSizer->AddGrowableCol(1, 1);

        auto* btnOk = new wxButton(this, wxID_OK, "Connect");
        auto* btnCancel = new wxButton(this, wxID_CANCEL, "Cancel");

        auto* btnSizer = new wxBoxSizer(wxHORIZONTAL);
        btnSizer->AddStretchSpacer(1);
        btnSizer->Add(btnOk, 0, wxRIGHT, 5);
        btnSizer->Add(btnCancel, 0);

        auto* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(formSizer, 0, wxEXPAND | wxALL, 10);
        mainSizer->Add(btnSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

        SetSizerAndFit(mainSizer);
        CentreOnParent();

        btnOk->SetDefault();
    }

    wxString GetServer() const { return m_serverCtrl->GetValue(); }
    wxString GetPort() const { return m_portCtrl->GetValue(); }
    wxString GetNick() const { return m_nickCtrl->GetValue(); }

private:
    wxTextCtrl* m_serverCtrl = nullptr;
    wxTextCtrl* m_portCtrl = nullptr;
    wxTextCtrl* m_nickCtrl = nullptr;
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
}

void MainFrame::OnMenuExit(wxCommandEvent&)
{
    Close(true);
}

void MainFrame::OnMenuConnect(wxCommandEvent&)
{
    QuickConnectDialog dlg(this, m_defaultServer, m_defaultPort, m_defaultNick);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxString server = dlg.GetServer();
    wxString port = dlg.GetPort();
    wxString nick = dlg.GetNick();

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

    if (!m_serverPanel)
    {
        m_serverPanel = new ServerConnectionPanel(this, server, port, nick);
        GetSizer()->Add(m_serverPanel, 1, wxEXPAND);
    }
    else
    {
        m_serverPanel->ConnectWith(server, port, nick);
    }

    Layout();
    SetStatusText("Connecting to " + server + ":" + port + "...");
}

void MainFrame::OnMenuDisconnect(wxCommandEvent&)
{
    if (!m_serverPanel)
    {
        wxMessageBox("No active connection to disconnect.",
                     "Disconnect", wxOK | wxICON_INFORMATION, this);
        return;
    }

    m_serverPanel->DisconnectCore();
    SetStatusText("Disconnected");
}

void MainFrame::OnMenuChangeNick(wxCommandEvent&)
{
    if (!m_serverPanel)
    {
        wxMessageBox("No active server.",
                     "Change Nick", wxOK | wxICON_INFORMATION, this);
        return;
    }

    wxString currentNick = m_serverPanel->GetNick();

    wxTextEntryDialog dlg(this,
                          "Enter new nickname:",
                          "Change Nick",
                          currentNick);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxString newNick = dlg.GetValue();
        if (!newNick.IsEmpty())
            m_serverPanel->RequestNickChange(newNick);
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
    wxMessageBox("Preferences dialog is not implemented yet.",
                 "Preferences",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void MainFrame::OnMenuAbout(wxCommandEvent&)
{
    wxMessageBox(
        "AstraIRC v1.0.0\n\n"
        "A modern, cross-platform IRC client\n"
        "built with wxWidgets.\n\n"
        "Supports Windows, macOS, and Linux.",
        "About AstraIRC",
        wxOK | wxICON_INFORMATION,
        this);
}
