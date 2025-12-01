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

    // Make absolutely sure it's not editable
    m_log->SetEditable(false);

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

    // Block ALL key events to prevent any editing
    m_log->Bind(wxEVT_KEY_DOWN, [this](wxKeyEvent&) {
        // Consume all key events and redirect to input
        if (m_serverPanel) {
            wxTextCtrl* input = m_serverPanel->GetInputCtrl();
            if (input && !input->HasFocus()) {
                input->SetFocus();
            }
        }
        // Don't skip - consume the event to prevent any editing
    });

    // ALSO block CHAR events - critical to prevent text insertion!
    m_log->Bind(wxEVT_CHAR, [this](wxKeyEvent&) {
        // Consume char events too - this prevents text from being inserted
        if (m_serverPanel) {
            wxTextCtrl* input = m_serverPanel->GetInputCtrl();
            if (input && !input->HasFocus()) {
                input->SetFocus();
            }
        }
        // Don't skip - consume to prevent editing
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

    // Change cursor to hand over URLs - check character position precisely
    m_log->Bind(wxEVT_MOTION, [this](wxMouseEvent& evt) {
        wxPoint pt = evt.GetPosition();
        wxTextCoord col, row;
        wxTextCtrlHitTestResult result = m_log->HitTest(pt, &col, &row);

        wxStockCursor desiredCursor = wxCURSOR_ARROW;

        if (result != wxTE_HT_UNKNOWN && result != wxTE_HT_BEYOND) {
            // Convert row to position
            long lineStart = m_log->XYToPosition(0, row);
            if (lineStart >= 0) {
                // Get the line at this row
                wxString lineText = m_log->GetLineText(row);

                // Find URLs in the line and check if cursor is over one
                bool overURL = false;

                // Find all URLs in line
                size_t searchPos = 0;
                while (searchPos < lineText.length()) {
                    int httpPos = lineText.find("http://", searchPos);
                    int httpsPos = lineText.find("https://", searchPos);
                    int wwwPos = lineText.find("www.", searchPos);

                    int foundPos = -1;
                    if (httpPos != wxNOT_FOUND) foundPos = httpPos;
                    if (httpsPos != wxNOT_FOUND && (foundPos == -1 || httpsPos < foundPos)) foundPos = httpsPos;
                    if (wwwPos != wxNOT_FOUND && (foundPos == -1 || wwwPos < foundPos)) foundPos = wwwPos;

                    if (foundPos == -1) break;

                    // Find end of URL
                    int urlEnd = lineText.find_first_of(" \t\n\r", foundPos);
                    if (urlEnd == wxNOT_FOUND) urlEnd = lineText.length();

                    // Check if cursor is within this URL range (use col for position in line)
                    if (col >= foundPos && col < urlEnd) {
                        overURL = true;
                        break;
                    }

                    searchPos = urlEnd;
                }

                if (overURL) {
                    desiredCursor = wxCURSOR_HAND;
                }
            }
        }

        // Only change cursor if it's different from current
        if (desiredCursor != m_currentCursor) {
            m_currentCursor = desiredCursor;
            m_log->SetCursor(wxCursor(m_currentCursor));
        }

        evt.Skip();
    });

    // CRITICAL: Prevent log area from ever holding focus
    m_log->Bind(wxEVT_SET_FOCUS, [this](wxFocusEvent&) {
        if (m_serverPanel) {
            wxTextCtrl* input = m_serverPanel->GetInputCtrl();
            if (input) {
                input->SetFocus();
                return;
            }
        }
        // Don't skip - we handled it by redirecting focus
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

// Helper to detect if text contains a URL
bool ContainsURL(const wxString& text) {
    return text.Find("http://") != wxNOT_FOUND ||
           text.Find("https://") != wxNOT_FOUND ||
           text.Find("www.") != wxNOT_FOUND ||
           text.Find("ftp://") != wxNOT_FOUND;
}

// Append text with IRC color codes parsed and URLs styled
void LogPanel::AppendIRCStyledText(const wxString& text)
{
    std::vector<IRCTextSegment> segments;
    ParseIRCColors(text, segments);

    for (const auto& seg : segments)
    {
        // Check if this segment contains a URL
        if (ContainsURL(seg.text))
        {
            // Split and style URLs within this segment
            wxString remaining = seg.text;
            while (!remaining.IsEmpty())
            {
                int urlPos = -1;
                wxString protocol;

                // Find URL start
                int httpPos = remaining.Find("http://");
                int httpsPos = remaining.Find("https://");
                int wwwPos = remaining.Find("www.");
                int ftpPos = remaining.Find("ftp://");

                if (httpPos != wxNOT_FOUND) { urlPos = httpPos; protocol = "http://"; }
                if (httpsPos != wxNOT_FOUND && (urlPos == -1 || httpsPos < urlPos)) { urlPos = httpsPos; protocol = "https://"; }
                if (wwwPos != wxNOT_FOUND && (urlPos == -1 || wwwPos < urlPos)) { urlPos = wwwPos; protocol = "www."; }
                if (ftpPos != wxNOT_FOUND && (urlPos == -1 || ftpPos < urlPos)) { urlPos = ftpPos; protocol = "ftp://"; }

                if (urlPos == -1)
                {
                    // No URL found, write remaining text with normal style
                    wxRichTextAttr attr;
                    if (!seg.useDefaultFg)
                        attr.SetTextColour(seg.foreground);
                    else
                        attr.SetTextColour(m_defaultForeground);
                    if (!seg.useDefaultBg)
                        attr.SetBackgroundColour(seg.background);
                    wxFont font(10, wxFONTFAMILY_TELETYPE,
                               seg.italic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
                               seg.bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL,
                               seg.underline);
                    attr.SetFont(font);
                    m_log->BeginStyle(attr);
                    m_log->WriteText(remaining);
                    m_log->EndStyle();
                    break;
                }

                // Write text before URL
                if (urlPos > 0)
                {
                    wxRichTextAttr attr;
                    if (!seg.useDefaultFg)
                        attr.SetTextColour(seg.foreground);
                    else
                        attr.SetTextColour(m_defaultForeground);
                    if (!seg.useDefaultBg)
                        attr.SetBackgroundColour(seg.background);
                    wxFont font(10, wxFONTFAMILY_TELETYPE,
                               seg.italic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
                               seg.bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL,
                               seg.underline);
                    attr.SetFont(font);
                    m_log->BeginStyle(attr);
                    m_log->WriteText(remaining.Mid(0, urlPos));
                    m_log->EndStyle();
                }

                // Find URL end
                int urlEnd = remaining.find_first_of(" \t\n\r", urlPos);
                wxString url = (urlEnd == wxNOT_FOUND) ? remaining.Mid(urlPos) : remaining.Mid(urlPos, urlEnd - urlPos);

                // Clean trailing punctuation
                while (!url.IsEmpty() && (url.Last() == '.' || url.Last() == ',' ||
                       url.Last() == ')' || url.Last() == ']' || url.Last() == '}')) {
                    url.RemoveLast();
                }

                // Style URL with bright blue color and underline
                wxRichTextAttr urlAttr;
                urlAttr.SetTextColour(wxColour(80, 160, 255));  // Bright blue
                wxFont urlFont(10, wxFONTFAMILY_TELETYPE,
                              wxFONTSTYLE_NORMAL,
                              seg.bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL,
                              true);  // underlined = true
                urlAttr.SetFont(urlFont);
                m_log->BeginStyle(urlAttr);
                m_log->WriteText(url);
                m_log->EndStyle();

                // Update remaining
                remaining = (urlEnd == wxNOT_FOUND) ? "" : remaining.Mid(urlEnd);
            }
        }
        else
        {
            // Normal segment without URL
            wxRichTextAttr attr;
            if (!seg.useDefaultFg)
                attr.SetTextColour(seg.foreground);
            else
                attr.SetTextColour(m_defaultForeground);
            if (!seg.useDefaultBg)
                attr.SetBackgroundColour(seg.background);
            wxFont font(10, wxFONTFAMILY_TELETYPE,
                       seg.italic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL,
                       seg.bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL,
                       seg.underline);
            attr.SetFont(font);
            m_log->BeginStyle(attr);
            m_log->WriteText(seg.text);
            m_log->EndStyle();
        }
    }
}

void LogPanel::AppendSystemMessage(const wxString& message)
{
    // CRITICAL: Always append at the end, not at click position
    m_log->SetInsertionPointEnd();

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
    // CRITICAL: Always append at the end, not at click position
    m_log->SetInsertionPointEnd();

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
    // CRITICAL: Always append at the end, not at click position
    m_log->SetInsertionPointEnd();

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
    // CRITICAL: Always append at the end, not at click position
    m_log->SetInsertionPointEnd();

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
    // CRITICAL: Always append at the end, not at click position
    m_log->SetInsertionPointEnd();

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
    // CRITICAL: Always append at the end, not at click position
    m_log->SetInsertionPointEnd();

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
    // CRITICAL: Always append at the end, not at click position
    m_log->SetInsertionPointEnd();

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

void ChannelPage::AppendChatMessage(const wxString& nick, const wxString& message)
{
    if (m_log)
        m_log->AppendChatMessage(nick, message);
}

void ChannelPage::AppendNotice(const wxString& nick, const wxString& message)
{
    if (m_log)
        m_log->AppendNotice(nick, message);
}

void ChannelPage::AppendAction(const wxString& nick, const wxString& action)
{
    if (m_log)
        m_log->AppendAction(nick, action);
}

void ChannelPage::AppendSystemMessage(const wxString& message)
{
    if (m_log)
        m_log->AppendSystemMessage(message);
}

void ChannelPage::AppendErrorMessage(const wxString& message)
{
    if (m_log)
        m_log->AppendErrorMessage(message);
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
