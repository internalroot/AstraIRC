#pragma once

#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include "UserInfo.h"

class ServerConnectionPanel;  // Forward declaration

class UserProfileDialog : public wxDialog
{
public:
    UserProfileDialog(wxWindow* parent,
                      const UserInfo& userInfo,
                      ServerConnectionPanel* serverPanel = nullptr);

    void UpdateInfo(const UserInfo& userInfo);

private:
    void CreateControls(const UserInfo& userInfo);
    void OnSendPM(wxCommandEvent& evt);
    void OnRefresh(wxCommandEvent& evt);
    void OnIgnore(wxCommandEvent& evt);
    void OnClose(wxCommandEvent& evt);
    void OnCloseWindow(wxCloseEvent& evt);

    std::string FormatIdleTime(int seconds) const;
    std::string FormatSignonTime(long timestamp) const;

private:
    UserInfo m_userInfo;
    ServerConnectionPanel* m_serverPanel;

    // Basic info labels
    wxStaticText* m_nickLabel = nullptr;
    wxStaticText* m_usernameLabel = nullptr;
    wxStaticText* m_hostnameLabel = nullptr;
    wxStaticText* m_realnameLabel = nullptr;
    wxStaticText* m_serverLabel = nullptr;
    wxStaticText* m_idleLabel = nullptr;
    wxStaticText* m_signonLabel = nullptr;
    wxStaticText* m_channelsLabel = nullptr;
    wxStaticText* m_accountLabel = nullptr;
    wxStaticText* m_awayLabel = nullptr;

    // Server role checkboxes (read-only for now)
    wxCheckBox* m_ownerCheckbox = nullptr;
    wxCheckBox* m_adminCheckbox = nullptr;
    wxCheckBox* m_operCheckbox = nullptr;
    wxCheckBox* m_helperCheckbox = nullptr;

    // Buttons
    wxButton* m_sendPMButton = nullptr;
    wxButton* m_refreshButton = nullptr;
    wxButton* m_ignoreButton = nullptr;
};
