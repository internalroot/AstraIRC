#include "UserProfileDialog.h"
#include "ServerConnectionPanel.h"
#include <wx/statline.h>
#include <sstream>
#include <ctime>
#include <iomanip>

UserProfileDialog::UserProfileDialog(wxWindow* parent,
                                     const UserInfo& userInfo,
                                     ServerConnectionPanel* serverPanel)
    : wxDialog(parent, wxID_ANY, wxString("User Profile: ") + userInfo.nick,
               wxDefaultPosition, wxSize(500, 600),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , m_userInfo(userInfo)
    , m_serverPanel(serverPanel)
{
    CreateControls(userInfo);
    Centre();
}

void UserProfileDialog::CreateControls(const UserInfo& userInfo)
{
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // === Basic Information Section ===
    wxStaticBoxSizer* basicBox = new wxStaticBoxSizer(wxVERTICAL, this, "Basic Information");
    wxFlexGridSizer* basicGrid = new wxFlexGridSizer(2, 5, 10);
    basicGrid->AddGrowableCol(1, 1);

    // Nickname
    basicGrid->Add(new wxStaticText(this, wxID_ANY, "Nickname:"),
                   0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    m_nickLabel = new wxStaticText(this, wxID_ANY, userInfo.nick);
    m_nickLabel->SetFont(m_nickLabel->GetFont().Bold());
    basicGrid->Add(m_nickLabel, 1, wxEXPAND);

    // Username
    basicGrid->Add(new wxStaticText(this, wxID_ANY, "Username:"),
                   0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    m_usernameLabel = new wxStaticText(this, wxID_ANY,
                                       userInfo.username.empty() ? "(unknown)" : userInfo.username);
    basicGrid->Add(m_usernameLabel, 1, wxEXPAND);

    // Hostname
    basicGrid->Add(new wxStaticText(this, wxID_ANY, "Hostname:"),
                   0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    m_hostnameLabel = new wxStaticText(this, wxID_ANY,
                                       userInfo.hostname.empty() ? "(unknown)" : userInfo.hostname);
    basicGrid->Add(m_hostnameLabel, 1, wxEXPAND);

    // Real name
    basicGrid->Add(new wxStaticText(this, wxID_ANY, "Real Name:"),
                   0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    m_realnameLabel = new wxStaticText(this, wxID_ANY,
                                       userInfo.realname.empty() ? "(unknown)" : userInfo.realname);
    basicGrid->Add(m_realnameLabel, 1, wxEXPAND);

    // Server
    basicGrid->Add(new wxStaticText(this, wxID_ANY, "Server:"),
                   0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
    std::string serverStr = userInfo.server;
    if (!userInfo.serverInfo.empty())
        serverStr += " (" + userInfo.serverInfo + ")";
    if (serverStr.empty())
        serverStr = "(unknown)";
    m_serverLabel = new wxStaticText(this, wxID_ANY, serverStr);
    basicGrid->Add(m_serverLabel, 1, wxEXPAND);

    // Account (if logged in)
    if (!userInfo.account.empty())
    {
        basicGrid->Add(new wxStaticText(this, wxID_ANY, "Account:"),
                       0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        m_accountLabel = new wxStaticText(this, wxID_ANY, userInfo.account);
        basicGrid->Add(m_accountLabel, 1, wxEXPAND);
    }

    // Idle time
    if (userInfo.idleSeconds > 0)
    {
        basicGrid->Add(new wxStaticText(this, wxID_ANY, "Idle Time:"),
                       0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        m_idleLabel = new wxStaticText(this, wxID_ANY, FormatIdleTime(userInfo.idleSeconds));
        basicGrid->Add(m_idleLabel, 1, wxEXPAND);
    }

    // Signon time
    if (userInfo.signonTime > 0)
    {
        basicGrid->Add(new wxStaticText(this, wxID_ANY, "Signon Time:"),
                       0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        m_signonLabel = new wxStaticText(this, wxID_ANY, FormatSignonTime(userInfo.signonTime));
        basicGrid->Add(m_signonLabel, 1, wxEXPAND);
    }

    // Away message
    if (!userInfo.awayMessage.empty())
    {
        basicGrid->Add(new wxStaticText(this, wxID_ANY, "Away:"),
                       0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
        m_awayLabel = new wxStaticText(this, wxID_ANY, userInfo.awayMessage);
        basicGrid->Add(m_awayLabel, 1, wxEXPAND);
    }

    basicBox->Add(basicGrid, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(basicBox, 0, wxEXPAND | wxALL, 10);

    // === Channels Section ===
    if (!userInfo.channels.empty())
    {
        wxStaticBoxSizer* channelsBox = new wxStaticBoxSizer(wxVERTICAL, this, "Channels in Common");

        std::string channelList;
        for (size_t i = 0; i < userInfo.channels.size(); ++i)
        {
            if (i > 0)
                channelList += ", ";
            channelList += userInfo.channels[i];
        }

        m_channelsLabel = new wxStaticText(this, wxID_ANY, channelList);
        m_channelsLabel->Wrap(450);
        channelsBox->Add(m_channelsLabel, 1, wxEXPAND | wxALL, 5);
        mainSizer->Add(channelsBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    }

    // === Server Roles Section ===
    wxStaticBoxSizer* rolesBox = new wxStaticBoxSizer(wxVERTICAL, this, "Server Roles");

    m_ownerCheckbox = new wxCheckBox(this, wxID_ANY, "Server Owner");
    m_ownerCheckbox->SetValue(userInfo.isServerOwner);
    m_ownerCheckbox->Enable(false);  // Read-only for now
    rolesBox->Add(m_ownerCheckbox, 0, wxALL, 3);

    m_adminCheckbox = new wxCheckBox(this, wxID_ANY, "Server Admin");
    m_adminCheckbox->SetValue(userInfo.isServerAdmin);
    m_adminCheckbox->Enable(false);
    rolesBox->Add(m_adminCheckbox, 0, wxALL, 3);

    m_operCheckbox = new wxCheckBox(this, wxID_ANY, "Server Operator");
    m_operCheckbox->SetValue(userInfo.isOperator);
    m_operCheckbox->Enable(false);
    rolesBox->Add(m_operCheckbox, 0, wxALL, 3);

    m_helperCheckbox = new wxCheckBox(this, wxID_ANY, "Server Helper");
    m_helperCheckbox->SetValue(userInfo.isServerHelper);
    m_helperCheckbox->Enable(false);
    rolesBox->Add(m_helperCheckbox, 0, wxALL, 3);

    // Show undercover status if applicable (future enhancement)
    if (userInfo.isUndercover && !userInfo.actualRole.empty())
    {
        wxStaticText* undercoverLabel = new wxStaticText(this, wxID_ANY,
            "⚠ UNDERCOVER - Actual role: " + userInfo.actualRole);
        undercoverLabel->SetForegroundColour(*wxRED);
        rolesBox->Add(undercoverLabel, 0, wxALL, 5);
    }

    mainSizer->Add(rolesBox, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    // === Separator ===
    mainSizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // === Action Buttons ===
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    m_sendPMButton = new wxButton(this, wxID_ANY, "Send Private Message");
    m_sendPMButton->Bind(wxEVT_BUTTON, &UserProfileDialog::OnSendPM, this);
    buttonSizer->Add(m_sendPMButton, 0, wxALL, 5);

    m_refreshButton = new wxButton(this, wxID_ANY, "Refresh");
    m_refreshButton->Bind(wxEVT_BUTTON, &UserProfileDialog::OnRefresh, this);
    buttonSizer->Add(m_refreshButton, 0, wxALL, 5);

    m_ignoreButton = new wxButton(this, wxID_ANY, "Ignore");
    m_ignoreButton->Bind(wxEVT_BUTTON, &UserProfileDialog::OnIgnore, this);
    buttonSizer->Add(m_ignoreButton, 0, wxALL, 5);

    buttonSizer->AddStretchSpacer();

    wxButton* closeButton = new wxButton(this, wxID_CLOSE, "Close");
    closeButton->Bind(wxEVT_BUTTON, &UserProfileDialog::OnClose, this);
    buttonSizer->Add(closeButton, 0, wxALL, 5);

    mainSizer->Add(buttonSizer, 0, wxEXPAND | wxALL, 10);

    SetSizer(mainSizer);
}

void UserProfileDialog::UpdateInfo(const UserInfo& userInfo)
{
    m_userInfo = userInfo;
    SetTitle(wxString("User Profile: ") + userInfo.nick);

    // Update all labels
    if (m_nickLabel)
        m_nickLabel->SetLabel(userInfo.nick);
    if (m_usernameLabel)
        m_usernameLabel->SetLabel(userInfo.username.empty() ? "(unknown)" : userInfo.username);
    if (m_hostnameLabel)
        m_hostnameLabel->SetLabel(userInfo.hostname.empty() ? "(unknown)" : userInfo.hostname);
    if (m_realnameLabel)
        m_realnameLabel->SetLabel(userInfo.realname.empty() ? "(unknown)" : userInfo.realname);

    std::string serverStr = userInfo.server;
    if (!userInfo.serverInfo.empty())
        serverStr += " (" + userInfo.serverInfo + ")";
    if (serverStr.empty())
        serverStr = "(unknown)";
    if (m_serverLabel)
        m_serverLabel->SetLabel(serverStr);

    if (m_accountLabel)
        m_accountLabel->SetLabel(userInfo.account.empty() ? "" : userInfo.account);
    if (m_idleLabel && userInfo.idleSeconds > 0)
        m_idleLabel->SetLabel(FormatIdleTime(userInfo.idleSeconds));
    if (m_signonLabel && userInfo.signonTime > 0)
        m_signonLabel->SetLabel(FormatSignonTime(userInfo.signonTime));
    if (m_awayLabel)
        m_awayLabel->SetLabel(userInfo.awayMessage);

    // Update checkboxes
    if (m_ownerCheckbox)
        m_ownerCheckbox->SetValue(userInfo.isServerOwner);
    if (m_adminCheckbox)
        m_adminCheckbox->SetValue(userInfo.isServerAdmin);
    if (m_operCheckbox)
        m_operCheckbox->SetValue(userInfo.isOperator);
    if (m_helperCheckbox)
        m_helperCheckbox->SetValue(userInfo.isServerHelper);

    Layout();
}

void UserProfileDialog::OnSendPM(wxCommandEvent& evt)
{
    // TODO: Implement when PM tab support is added
    wxMessageBox("Private message support will be implemented in a future update.",
                 "Not Yet Implemented", wxOK | wxICON_INFORMATION, this);
}

void UserProfileDialog::OnRefresh(wxCommandEvent& evt)
{
    // Request a fresh WHOIS
    if (m_serverPanel)
    {
        // The server panel will need to expose a method to request WHOIS
        // For now, just show a message
        wxMessageBox("Requesting fresh WHOIS for " + m_userInfo.nick,
                     "Refresh", wxOK | wxICON_INFORMATION, this);
        // TODO: Call m_serverPanel->RequestWhois(m_userInfo.nick);
    }
}

void UserProfileDialog::OnIgnore(wxCommandEvent& evt)
{
    // TODO: Implement ignore functionality
    wxMessageBox("Ignore functionality will be implemented in a future update.",
                 "Not Yet Implemented", wxOK | wxICON_INFORMATION, this);
}

void UserProfileDialog::OnClose(wxCommandEvent& evt)
{
    Close();
}

std::string UserProfileDialog::FormatIdleTime(int seconds) const
{
    std::ostringstream oss;

    if (seconds < 60)
    {
        oss << seconds << " seconds";
    }
    else if (seconds < 3600)
    {
        int minutes = seconds / 60;
        int secs = seconds % 60;
        oss << minutes << " minute" << (minutes != 1 ? "s" : "");
        if (secs > 0)
            oss << ", " << secs << " second" << (secs != 1 ? "s" : "");
    }
    else if (seconds < 86400)
    {
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        oss << hours << " hour" << (hours != 1 ? "s" : "");
        if (minutes > 0)
            oss << ", " << minutes << " minute" << (minutes != 1 ? "s" : "");
    }
    else
    {
        int days = seconds / 86400;
        int hours = (seconds % 86400) / 3600;
        oss << days << " day" << (days != 1 ? "s" : "");
        if (hours > 0)
            oss << ", " << hours << " hour" << (hours != 1 ? "s" : "");
    }

    return oss.str();
}

std::string UserProfileDialog::FormatSignonTime(long timestamp) const
{
    std::time_t t = static_cast<std::time_t>(timestamp);
    std::tm* tm = std::localtime(&t);

    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
