#include "ChannelPage.h"
#include "AppSettings.h"
#include "ServerConnectionPanel.h"

#include <wx/datetime.h>
#include <wx/textctrl.h>
#include <wx/utils.h>

// -------- LogPanel --------

LogPanel::LogPanel(wxWindow* parent, const AppSettings* settings, ServerConnectionPanel* serverPanel)
    : wxPanel(parent, wxID_ANY),
      m_settings(settings),
      m_serverPanel(serverPanel)
{
    m_log = new wxTextCtrl(
        this, wxID_ANY, "",
        wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxTE_AUTO_URL
    );

    // Use a monospace font for better readability
    wxFont font = m_log->GetFont();
    font.SetFamily(wxFONTFAMILY_TELETYPE);
    m_log->SetFont(font);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_log, 1, wxEXPAND | wxALL, 2);
    SetSizer(sizer);

    // Bind char event to redirect typing to input
    m_log->Bind(wxEVT_CHAR, &LogPanel::OnChar, this);

    // Bind URL click event
    m_log->Bind(wxEVT_TEXT_URL, &LogPanel::OnURL, this);
}

void LogPanel::AppendLog(const wxString& line)
{
    wxString output;

    // Add timestamp if enabled
    if (m_settings && m_settings->showTimestamps)
    {
        wxString format = m_settings->use24HourFormat ? "[%H:%M:%S] " : "[%I:%M:%S %p] ";
        output = wxDateTime::Now().Format(format);
    }

    output += line + "\n";
    m_log->AppendText(output);
}

void LogPanel::Clear()
{
    m_log->Clear();
}

void LogPanel::SetSettings(const AppSettings* settings)
{
    m_settings = settings;
}

void LogPanel::OnChar(wxKeyEvent& evt)
{
    // When user types in the log, redirect focus to input and send the character there
    if (m_serverPanel)
    {
        wxTextCtrl* input = m_serverPanel->GetInputCtrl();
        if (input)
        {
            // Focus the input first
            input->SetFocus();

            // Append the character to the input
            wxChar ch = evt.GetUnicodeKey();
            if (ch != WXK_NONE)
            {
                input->WriteText(wxString(ch));
            }
        }
    }
}

void LogPanel::OnURL(wxTextUrlEvent& evt)
{
    // Only handle mouse clicks on URLs
    if (evt.GetMouseEvent().LeftDown())
    {
        wxString url = m_log->GetRange(evt.GetURLStart(), evt.GetURLEnd());
        wxLaunchDefaultBrowser(url);
    }
}

// -------- ChannelPage --------

ChannelPage::ChannelPage(wxWindow* parent, const wxString& channelName, const AppSettings* settings, ServerConnectionPanel* serverPanel)
    : wxPanel(parent, wxID_ANY),
      m_channelName(channelName)
{
    m_log = new LogPanel(this, settings, serverPanel);
    m_nickList = new wxListBox(this, wxID_ANY);

    // Use monospace font for nick list too
    wxFont font = m_nickList->GetFont();
    font.SetFamily(wxFONTFAMILY_TELETYPE);
    m_nickList->SetFont(font);

    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_log, 3, wxEXPAND | wxALL, 2);
    sizer->Add(m_nickList, 1, wxEXPAND | wxTOP | wxBOTTOM | wxRIGHT, 2);

    SetSizer(sizer);
}

void ChannelPage::AppendLog(const wxString& line)
{
    m_log->AppendLog(line);
}

void ChannelPage::ClearLog()
{
    m_log->Clear();
}

wxListBox* ChannelPage::GetNickList()
{
    return m_nickList;
}

const wxString& ChannelPage::GetChannelName() const
{
    return m_channelName;
}

void ChannelPage::SetSettings(const AppSettings* settings)
{
    if (m_log)
        m_log->SetSettings(settings);
}
