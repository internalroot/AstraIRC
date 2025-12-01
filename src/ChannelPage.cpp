#include "ChannelPage.h"
#include "AppSettings.h"
#include "ServerConnectionPanel.h"

#include <wx/datetime.h>
#include <wx/stc/stc.h>
#include <wx/utils.h>

// -------- LogPanel --------

LogPanel::LogPanel(wxWindow* parent, const AppSettings* settings, ServerConnectionPanel* serverPanel)
    : wxPanel(parent, wxID_ANY),
      m_settings(settings),
      m_serverPanel(serverPanel)
{
    m_log = new wxStyledTextCtrl(this, wxID_ANY);

    // Make it read-only
    m_log->SetReadOnly(true);

    // Hide the caret
    m_log->SetCaretWidth(0);

    // Use a monospace font for better readability
    wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    m_log->StyleSetFont(wxSTC_STYLE_DEFAULT, font);
    m_log->StyleClearAll();  // Apply default style to all styles

    // Disable margins (line numbers, folding, etc.)
    m_log->SetMarginWidth(0, 0);
    m_log->SetMarginWidth(1, 0);
    m_log->SetMarginWidth(2, 0);

    // Disable scroll bar on right (optional - keeps vertical scrollbar only)
    m_log->SetUseHorizontalScrollBar(false);

    // Set word wrap mode to wrap at word boundaries
    m_log->SetWrapMode(wxSTC_WRAP_WORD);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_log, 1, wxEXPAND | wxALL, 2);
    SetSizer(sizer);

    // Bind char event to redirect typing to input
    m_log->Bind(wxEVT_CHAR, &LogPanel::OnChar, this);

    // Bind left mouse click event for URL detection
    m_log->Bind(wxEVT_LEFT_DOWN, &LogPanel::OnLeftDown, this);
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

    // Temporarily make writable to append text
    m_log->SetReadOnly(false);
    m_log->AppendText(output);

    // Auto-scroll to bottom
    m_log->GotoPos(m_log->GetLength());

    // Make read-only again
    m_log->SetReadOnly(true);
}

void LogPanel::Clear()
{
    m_log->SetReadOnly(false);
    m_log->ClearAll();
    m_log->SetReadOnly(true);
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

void LogPanel::OnLeftDown(wxMouseEvent& evt)
{
    // Get position clicked
    int pos = m_log->PositionFromPoint(wxPoint(evt.GetX(), evt.GetY()));

    // Get the line at the clicked position
    int line = m_log->LineFromPosition(pos);
    wxString lineText = m_log->GetLine(line);

    // Simple URL detection - look for http:// or https://
    if (lineText.Contains("http://") || lineText.Contains("https://"))
    {
        // Find the URL in the line
        int httpPos = lineText.Find("http://");
        int httpsPos = lineText.Find("https://");
        int urlStart = wxNOT_FOUND;

        if (httpPos != wxNOT_FOUND && httpsPos != wxNOT_FOUND)
            urlStart = wxMin(httpPos, httpsPos);
        else if (httpPos != wxNOT_FOUND)
            urlStart = httpPos;
        else
            urlStart = httpsPos;

        if (urlStart != wxNOT_FOUND)
        {
            // Find the end of the URL (first whitespace or end of line)
            int urlEnd = lineText.find_first_of(" \t\n\r", urlStart);
            wxString url;
            if (urlEnd == wxNOT_FOUND)
                url = lineText.Mid(urlStart);
            else
                url = lineText.Mid(urlStart, urlEnd - urlStart);

            // Remove trailing punctuation that's probably not part of the URL
            while (!url.IsEmpty() && (url.Last() == '.' || url.Last() == ',' ||
                   url.Last() == ')' || url.Last() == ']' || url.Last() == '}'))
            {
                url.RemoveLast();
            }

            if (!url.IsEmpty())
            {
                wxLaunchDefaultBrowser(url);
                return;  // Don't skip the event - we handled it
            }
        }
    }

    // Allow default handling (text selection, etc.)
    evt.Skip();
}

// -------- ChannelPage --------

ChannelPage::ChannelPage(wxWindow* parent, const wxString& channelName, const AppSettings* settings, ServerConnectionPanel* serverPanel)
    : wxPanel(parent, wxID_ANY),
      m_channelName(channelName),
      m_serverPanel(serverPanel)
{
    m_log = new LogPanel(this, settings, serverPanel);
    m_nickList = new wxListBox(this, wxID_ANY);

    // Use monospace font for nick list too
    wxFont font = m_nickList->GetFont();
    font.SetFamily(wxFONTFAMILY_TELETYPE);
    m_nickList->SetFont(font);

    // Bind double-click event on nick list
    m_nickList->Bind(wxEVT_LISTBOX_DCLICK, &ChannelPage::OnNickDoubleClick, this);

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

void ChannelPage::OnNickDoubleClick(wxCommandEvent& evt)
{
    int selection = m_nickList->GetSelection();
    if (selection == wxNOT_FOUND)
        return;

    wxString nick = m_nickList->GetString(selection);

    // Remove channel prefix symbols (@, +, %, ~, etc.)
    while (!nick.IsEmpty() && !wxIsalnum(nick[0]) && nick[0] != '[' && nick[0] != '{' && nick[0] != '_')
        nick = nick.Mid(1);

    if (nick.IsEmpty())
        return;

    // Request WHOIS from the server panel
    if (m_serverPanel)
        m_serverPanel->RequestWhois(nick);
}
