#include "ChannelPage.h"

#include <wx/datetime.h>

// -------- LogPanel --------

LogPanel::LogPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    m_log = new wxTextCtrl(
        this, wxID_ANY, "",
        wxDefaultPosition, wxDefaultSize,
        wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2
    );

    // Use a monospace font for better readability
    wxFont font = m_log->GetFont();
    font.SetFamily(wxFONTFAMILY_TELETYPE);
    m_log->SetFont(font);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_log, 1, wxEXPAND | wxALL, 2);
    SetSizer(sizer);
}

void LogPanel::AppendLog(const wxString& line)
{
    // Add timestamp
    wxString timestamp = wxDateTime::Now().Format("[%H:%M:%S] ");
    m_log->AppendText(timestamp + line + "\n");
}

void LogPanel::Clear()
{
    m_log->Clear();
}

// -------- ChannelPage --------

ChannelPage::ChannelPage(wxWindow* parent, const wxString& channelName)
    : wxPanel(parent, wxID_ANY),
      m_channelName(channelName)
{
    m_log = new LogPanel(this);
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
