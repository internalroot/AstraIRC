#pragma once

#include <wx/frame.h>
#include <wx/string.h>
#include "AppSettings.h"

class ServerConnectionPanel;

class MainFrame : public wxFrame
{
public:
    MainFrame();

private:
    void OnMenuExit(wxCommandEvent& evt);
    void OnMenuConnect(wxCommandEvent& evt);
    void OnMenuDisconnect(wxCommandEvent& evt);
    void OnMenuChangeNick(wxCommandEvent& evt);
    void OnMenuServerList(wxCommandEvent& evt);
    void OnMenuPreferences(wxCommandEvent& evt);
    void OnMenuAbout(wxCommandEvent& evt);
    void OnActivate(wxActivateEvent& evt);

private:
    ServerConnectionPanel* m_serverPanel = nullptr;

    wxString m_defaultServer;
    wxString m_defaultPort;
    wxString m_defaultNick;
    wxString m_defaultPassword;

    AppSettings m_settings;
};
