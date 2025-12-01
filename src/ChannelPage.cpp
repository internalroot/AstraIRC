#include "ChannelPage.h"
#include "AppSettings.h"
#include "ServerConnectionPanel.h"

#include <wx/datetime.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/utils.h>
#include <vector>

// -------- LogPanel --------

LogPanel::LogPanel(wxWindow* parent, const AppSettings* settings, ServerConnectionPanel* serverPanel)
    : wxPanel(parent, wxID_ANY),
      m_settings(settings),
      m_serverPanel(serverPanel),
      m_defaultForeground(wxColour(220, 220, 220)),
      m_defaultBackground(wxColour(30, 30, 30))
{
    m_log = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize,
        wxTE_READONLY | wxTE_MULTILINE | wxVSCROLL | wxTE_RICH2);

    // Set dark background and default colors
    m_log->SetBackgroundColour(m_defaultBackground);

    // Use a monospace font for better readability
    wxFont font(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    // Set default text style
    wxRichTextAttr defaultStyle;
    defaultStyle.SetTextColour(m_defaultForeground);
    defaultStyle.SetBackgroundColour(m_defaultBackground);
    defaultStyle.SetFont(font);
    m_log->SetBasicStyle(defaultStyle);

    // Hide caret by preventing focus
    m_log->SetWindowStyleFlag(m_log->GetWindowStyleFlag() | wxTE_PROCESS_TAB);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_log, 1, wxEXPAND | wxALL, 2);
    SetSizer(sizer);

    // Bind char event to redirect typing to input
    m_log->Bind(wxEVT_CHAR, &LogPanel::OnChar, this);

    // Block ALL key events to prevent any editing
    m_log->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent& evt) {
        // Consume all key events and redirect to input
        if (m_serverPanel) {
            wxTextCtrl* input = m_serverPanel->GetInputCtrl();
            if (input && !input->HasFocus()) {
                input->SetFocus();
            }
        }
        // Don't skip - consume the event to prevent any editing
    });

    // Handle mouse clicks for URL detection
    m_log->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& evt) {
        // Check if user was selecting text (don't treat as URL click)
        long selStart, selEnd;
        m_log->GetSelection(&selStart, &selEnd);
        if (selStart != selEnd) {
            evt.Skip();
            return;
        }

        // Get click position
        wxPoint pt = evt.GetPosition();
        long pos = m_log->GetInsertionPoint();

        // Get the text at click position
        wxRichTextLine* textLine = m_log->GetBuffer().GetLineAtPosition(pos);
        if (!textLine) {
            evt.Skip();
            return;
        }

        wxRichTextRange lineRange = textLine->GetAbsoluteRange();
        wxString lineText = m_log->GetRange(lineRange.GetStart(), lineRange.GetEnd());

        // Simple URL detection
        int httpPos = lineText.Find("http://");
        int httpsPos = lineText.Find("https://");
        int wwwPos = lineText.Find("www.");

        int urlStart = -1;
        if (httpPos != wxNOT_FOUND) urlStart = httpPos;
        else if (httpsPos != wxNOT_FOUND) urlStart = httpsPos;
        else if (wwwPos != wxNOT_FOUND) urlStart = wwwPos;

        if (urlStart != -1) {
            // Find end of URL
            int urlEnd = lineText.find_first_of(" \t\n\r", urlStart);
            wxString url = (urlEnd == wxNOT_FOUND) ? lineText.Mid(urlStart) : lineText.Mid(urlStart, urlEnd - urlStart);

            // Clean up trailing punctuation
            while (!url.IsEmpty() && (url.Last() == '.' || url.Last() == ',' ||
                   url.Last() == ')' || url.Last() == ']' || url.Last() == '}')) {
                url.RemoveLast();
            }

            // Add protocol if needed
            if (url.StartsWith("www.")) {
                url = "http://" + url;
            }

            if (!url.IsEmpty()) {
                wxLaunchDefaultBrowser(url);
                return;
            }
        }

        evt.Skip();
    });

    // Change cursor to hand over URLs (simple check)
    m_log->Bind(wxEVT_MOTION, [this](wxMouseEvent& evt) {
        wxPoint pt = evt.GetPosition();
        wxTextCoord col, row;
        wxTextCtrlHitTestResult result = m_log->HitTest(pt, &col, &row);

        if (result != wxTE_HT_UNKNOWN && result != wxTE_HT_BEYOND) {
            // Get text of current line
            wxString line = m_log->GetLineText(row);

            // Simple check if line contains URL
            if (line.Find("http://") != wxNOT_FOUND ||
                line.Find("https://") != wxNOT_FOUND ||
                line.Find("www.") != wxNOT_FOUND) {
                m_log->SetCursor(wxCursor(wxCURSOR_HAND));
            } else {
                m_log->SetCursor(wxCursor(wxCURSOR_IBEAM));
            }
        }

        evt.Skip();
    });

    // CRITICAL: Prevent log area from ever holding focus
    m_log->Bind(wxEVT_SET_FOCUS, [this](wxFocusEvent& evt) {
        if (m_serverPanel) {
            wxTextCtrl* input = m_serverPanel->GetInputCtrl();
            if (input) {
                input->SetFocus();
                return;
            }
        }
        evt.Skip();
    });
}

// IRC color palette (mIRC standard colors)
wxColour LogPanel::GetIRCColor(int colorCode)
{
    static const wxColour IRC_COLORS[] = {
        wxColour(255, 255, 255),  // 0  - White
        wxColour(0,   0,   0),    // 1  - Black
        wxColour(0,   0,   127),  // 2  - Blue
        wxColour(0,   147, 0),    // 3  - Green
        wxColour(255, 0,   0),    // 4  - Red
        wxColour(127, 0,   0),    // 5  - Brown/Maroon
        wxColour(156, 0,   156),  // 6  - Purple
        wxColour(252, 127, 0),    // 7  - Orange
        wxColour(255, 255, 0),    // 8  - Yellow
        wxColour(0,   252, 0),    // 9  - Light Green
        wxColour(0,   147, 147),  // 10 - Cyan
        wxColour(0,   255, 255),  // 11 - Light Cyan
        wxColour(0,   0,   252),  // 12 - Light Blue
        wxColour(255, 0,   255),  // 13 - Pink/Magenta
        wxColour(127, 127, 127),  // 14 - Gray
        wxColour(210, 210, 210)   // 15 - Light Gray
    };

    if (colorCode >= 0 && colorCode < 16)
        return IRC_COLORS[colorCode];

    return m_defaultForeground;  // Default if out of range
}

// Parse IRC color codes from text
void LogPanel::ParseIRCColors(const wxString& input, std::vector<IRCTextSegment>& segments)
{
    IRCTextSegment current;
    size_t i = 0;

    while (i < input.length())
    {
        wxChar ch = input[i];

        // Color code: \x03
        if (ch == 0x03)
        {
            // Save current segment if it has text
            if (!current.text.IsEmpty())
            {
                segments.push_back(current);
                current.text.Clear();
            }

            i++;

            // Check if followed by color number
            if (i < input.length() && wxIsdigit(input[i]))
            {
                // Parse foreground color (1-2 digits)
                wxString fgStr;
                while (i < input.length() && wxIsdigit(input[i]) && fgStr.length() < 2)
                {
                    fgStr += input[i++];
                }

                long fg;
                if (fgStr.ToLong(&fg))
                {
                    current.foreground = GetIRCColor(fg);
                    current.useDefaultFg = false;
                }

                // Check for background color (comma followed by 1-2 digits)
                if (i < input.length() && input[i] == ',')
                {
                    i++;
                    wxString bgStr;
                    while (i < input.length() && wxIsdigit(input[i]) && bgStr.length() < 2)
                    {
                        bgStr += input[i++];
                    }

                    long bg;
                    if (bgStr.ToLong(&bg))
                    {
                        current.background = GetIRCColor(bg);
                        current.useDefaultBg = false;
                    }
                }
            }
            else
            {
                // \x03 alone resets colors
                current.foreground = m_defaultForeground;
                current.background = m_defaultBackground;
                current.useDefaultFg = true;
                current.useDefaultBg = true;
            }
        }
        // Bold: \x02
        else if (ch == 0x02)
        {
            if (!current.text.IsEmpty())
            {
                segments.push_back(current);
                current.text.Clear();
            }
            current.bold = !current.bold;
            i++;
        }
        // Italic: \x1D
        else if (ch == 0x1D)
        {
            if (!current.text.IsEmpty())
            {
                segments.push_back(current);
                current.text.Clear();
            }
            current.italic = !current.italic;
            i++;
        }
        // Underline: \x1F
        else if (ch == 0x1F)
        {
            if (!current.text.IsEmpty())
            {
                segments.push_back(current);
                current.text.Clear();
            }
            current.underline = !current.underline;
            i++;
        }
        // Reset: \x0F
        else if (ch == 0x0F)
        {
            if (!current.text.IsEmpty())
            {
                segments.push_back(current);
                current.text.Clear();
            }
            current.foreground = m_defaultForeground;
            current.background = m_defaultBackground;
            current.bold = false;
            current.italic = false;
            current.underline = false;
            current.useDefaultFg = true;
            current.useDefaultBg = true;
            i++;
        }
        else
        {
            current.text += ch;
            i++;
        }
    }

    // Add final segment
    if (!current.text.IsEmpty())
        segments.push_back(current);
}

// Append text with IRC color codes parsed
void LogPanel::AppendIRCStyledText(const wxString& text)
{
    std::vector<IRCTextSegment> segments;
    ParseIRCColors(text, segments);

    for (const auto& seg : segments)
    {
        wxRichTextAttr attr;

        // Set colors
        if (!seg.useDefaultFg)
            attr.SetTextColour(seg.foreground);
        else
            attr.SetTextColour(m_defaultForeground);

        if (!seg.useDefaultBg)
            attr.SetBackgroundColour(seg.background);

        // Set font styling
        wxFont font(10, wxFONTFAMILY_TELETYPE,
                   seg.italic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
                   seg.bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL,
                   seg.underline);
        attr.SetFont(font);

        // Append with style
        m_log->BeginStyle(attr);
        m_log->WriteText(seg.text);
        m_log->EndStyle();
    }
}

void LogPanel::AppendSystemMessage(const wxString& message)
{
    // Add timestamp if enabled
    if (m_settings && m_settings->showTimestamps)
    {
        wxString format = m_settings->use24HourFormat ? "[%H:%M:%S] " : "[%I:%M:%S %p] ";
        wxString timestamp = wxDateTime::Now().Format(format);
        m_log->BeginTextColour(wxColour(128, 128, 128));  // Gray
        m_log->WriteText(timestamp);
        m_log->EndTextColour();
    }

    m_log->BeginTextColour(wxColour(100, 200, 100));  // Light green
    m_log->WriteText(message + "\n");
    m_log->EndTextColour();
    m_log->ShowPosition(m_log->GetLastPosition());
}

void LogPanel::AppendErrorMessage(const wxString& message)
{
    // Add timestamp if enabled
    if (m_settings && m_settings->showTimestamps)
    {
        wxString format = m_settings->use24HourFormat ? "[%H:%M:%S] " : "[%I:%M:%S %p] ";
        wxString timestamp = wxDateTime::Now().Format(format);
        m_log->BeginTextColour(wxColour(128, 128, 128));  // Gray
        m_log->WriteText(timestamp);
        m_log->EndTextColour();
    }

    m_log->BeginBold();
    m_log->BeginTextColour(wxColour(255, 100, 100));  // Light red
    m_log->WriteText(message + "\n");
    m_log->EndTextColour();
    m_log->EndBold();
    m_log->ShowPosition(m_log->GetLastPosition());
}

void LogPanel::AppendChatMessage(const wxString& nick, const wxString& message)
{
    // Add timestamp if enabled
    if (m_settings && m_settings->showTimestamps)
    {
        wxString format = m_settings->use24HourFormat ? "[%H:%M:%S] " : "[%I:%M:%S %p] ";
        wxString timestamp = wxDateTime::Now().Format(format);
        m_log->BeginTextColour(wxColour(128, 128, 128));  // Gray
        m_log->WriteText(timestamp);
        m_log->EndTextColour();
    }

    // Format: <nick> message
    m_log->WriteText("<");
    m_log->BeginBold();
    m_log->BeginTextColour(wxColour(100, 200, 255));  // Light cyan/blue
    m_log->WriteText(nick);
    m_log->EndTextColour();
    m_log->EndBold();
    m_log->WriteText("> ");

    // Message might contain IRC color codes
    AppendIRCStyledText(message);
    m_log->WriteText("\n");
    m_log->ShowPosition(m_log->GetLastPosition());
}

void LogPanel::AppendNotice(const wxString& nick, const wxString& message)
{
    // Add timestamp if enabled
    if (m_settings && m_settings->showTimestamps)
    {
        wxString format = m_settings->use24HourFormat ? "[%H:%M:%S] " : "[%I:%M:%S %p] ";
        wxString timestamp = wxDateTime::Now().Format(format);
        m_log->BeginTextColour(wxColour(128, 128, 128));  // Gray
        m_log->WriteText(timestamp);
        m_log->EndTextColour();
    }

    // Format: -nick- message
    m_log->BeginTextColour(wxColour(255, 180, 80));  // Orange
    m_log->WriteText("-");
    m_log->EndTextColour();

    m_log->BeginBold();
    m_log->BeginTextColour(wxColour(100, 200, 255));  // Light cyan/blue
    m_log->WriteText(nick);
    m_log->EndTextColour();
    m_log->EndBold();

    m_log->BeginTextColour(wxColour(255, 180, 80));  // Orange
    m_log->WriteText("- ");
    m_log->WriteText(message + "\n");
    m_log->EndTextColour();
    m_log->ShowPosition(m_log->GetLastPosition());
}

void LogPanel::AppendAction(const wxString& nick, const wxString& action)
{
    // Add timestamp if enabled
    if (m_settings && m_settings->showTimestamps)
    {
        wxString format = m_settings->use24HourFormat ? "[%H:%M:%S] " : "[%I:%M:%S %p] ";
        wxString timestamp = wxDateTime::Now().Format(format);
        m_log->BeginTextColour(wxColour(128, 128, 128));  // Gray
        m_log->WriteText(timestamp);
        m_log->EndTextColour();
    }

    // Format: * nick action
    m_log->BeginItalic();
    m_log->BeginTextColour(wxColour(200, 120, 255));  // Purple
    m_log->WriteText("* ");
    m_log->EndTextColour();
    m_log->EndItalic();

    m_log->BeginBold();
    m_log->BeginTextColour(wxColour(100, 200, 255));  // Light cyan/blue
    m_log->WriteText(nick + " ");
    m_log->EndTextColour();
    m_log->EndBold();

    m_log->BeginItalic();
    m_log->BeginTextColour(wxColour(200, 120, 255));  // Purple
    m_log->WriteText(action + "\n");
    m_log->EndTextColour();
    m_log->EndItalic();
    m_log->ShowPosition(m_log->GetLastPosition());
}

void LogPanel::AppendTopicMessage(const wxString& message)
{
    // Add timestamp if enabled
    if (m_settings && m_settings->showTimestamps)
    {
        wxString format = m_settings->use24HourFormat ? "[%H:%M:%S] " : "[%I:%M:%S %p] ";
        wxString timestamp = wxDateTime::Now().Format(format);
        m_log->BeginTextColour(wxColour(128, 128, 128));  // Gray
        m_log->WriteText(timestamp);
        m_log->EndTextColour();
    }

    m_log->BeginTextColour(wxColour(80, 220, 220));  // Bright cyan
    m_log->WriteText(message + "\n");
    m_log->EndTextColour();
    m_log->ShowPosition(m_log->GetLastPosition());
}

void LogPanel::AppendLog(const wxString& line)
{
    // Add timestamp if enabled
    if (m_settings && m_settings->showTimestamps)
    {
        wxString format = m_settings->use24HourFormat ? "[%H:%M:%S] " : "[%I:%M:%S %p] ";
        wxString timestamp = wxDateTime::Now().Format(format);
        m_log->BeginTextColour(wxColour(128, 128, 128));  // Gray
        m_log->WriteText(timestamp);
        m_log->EndTextColour();
    }

    m_log->WriteText(line + "\n");
    m_log->ShowPosition(m_log->GetLastPosition());
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
