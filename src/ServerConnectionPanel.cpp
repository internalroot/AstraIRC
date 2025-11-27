#include "ServerConnectionPanel.h"

#include <wx/frame.h>
#include <wx/msgdlg.h>

// ---------- Helper: Check if string is a channel name ----------

static bool IsChannelName(const wxString& name)
{
    if (name.IsEmpty())
        return false;
    wxChar first = name[0];
    return (first == '#' || first == '&' || first == '!' || first == '+');
}

// ---------- Helper: Normalize channel name (remove leading colon if present) ----------

static wxString NormalizeChannelName(const wxString& name)
{
    wxString result = name;
    result.Trim(true).Trim(false);
    
    // Remove leading colon if present (some servers send :#channel)
    if (result.StartsWith(":"))
        result = result.Mid(1);
    
    return result;
}

// ---------- ctor / dtor ----------

ServerConnectionPanel::ServerConnectionPanel(wxWindow* parent,
                                             const wxString& server,
                                             const wxString& port,
                                             const wxString& nick)
    : wxPanel(parent, wxID_ANY),
      m_server(server),
      m_port(port),
      m_nick(nick)
{
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    m_viewBook = new wxAuiNotebook(
        this, wxID_ANY,
        wxDefaultPosition, wxDefaultSize,
        wxAUI_NB_DEFAULT_STYLE |
        wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
        wxAUI_NB_CLOSE_ON_ALL_TABS
    );

    m_viewBook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE,
                     &ServerConnectionPanel::OnTabClosed,
                     this);

    // Console tab
    m_consoleView = new LogPanel(m_viewBook);
    wxString consoleTabTitle = BuildConsoleTabTitle();
    m_viewBook->AddPage(m_consoleView, consoleTabTitle, true);

    // Shared input
    m_input = new wxTextCtrl(this, wxID_ANY, "",
                             wxDefaultPosition, wxDefaultSize,
                             wxTE_PROCESS_ENTER);
    m_btnSend = new wxButton(this, wxID_ANY, "Send");

    auto* bottomSizer = new wxBoxSizer(wxHORIZONTAL);
    bottomSizer->Add(m_input, 1, wxEXPAND | wxRIGHT, 5);
    bottomSizer->Add(m_btnSend, 0);

    mainSizer->Add(m_viewBook, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(bottomSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);
    SetSizer(mainSizer);

    m_input->SetFocus();

    // Button + Enter handlers
    m_input->Bind(wxEVT_TEXT_ENTER, &ServerConnectionPanel::OnSend, this);
    m_btnSend->Bind(wxEVT_BUTTON, &ServerConnectionPanel::OnSend, this);

    // IRCCore callbacks - use CallAfter to marshal to GUI thread
    m_core.setLogCallback([this](const std::string& msg) {
        wxString wxMsg = wxString::FromUTF8(msg.c_str());
        CallAfter([this, wxMsg]() { HandleCoreLog(wxMsg); });
    });

    m_core.setMessageCallback([this](const std::string& source,
                                     const std::string& text) {
        wxString wxSrc = wxString::FromUTF8(source.c_str());
        wxString wxText = wxString::FromUTF8(text.c_str());
        CallAfter([this, wxSrc, wxText]() { HandleCoreMessage(wxSrc, wxText); });
    });

    m_core.setRawLineCallback([this](const std::string& line) {
        wxString wxLine = wxString::FromUTF8(line.c_str());
        CallAfter([this, wxLine]() { HandleRawLine(wxLine); });
    });

    // Begin connection
    ConnectCore();
}

ServerConnectionPanel::~ServerConnectionPanel()
{
    m_core.disconnect();
}

// ---------- public helpers ----------

void ServerConnectionPanel::ConnectWith(const wxString& server,
                                        const wxString& port,
                                        const wxString& nick)
{
    m_server = server;
    m_port = port;
    m_nick = nick;

    if (m_viewBook)
        m_viewBook->SetPageText(0, BuildConsoleTabTitle());

    ConnectCore();
}

void ServerConnectionPanel::LogToConsole(const wxString& line)
{
    if (m_consoleView)
        m_consoleView->AppendLog(line);
}

void ServerConnectionPanel::DisconnectCore()
{
    m_core.disconnect();
}

void ServerConnectionPanel::RequestNickChange(const wxString& newNick)
{
    if (newNick.IsEmpty())
        return;

    // Just send the request - the server will echo back the change
    // and we'll update everything in HandleRawLine
    m_core.sendRaw("NICK " + std::string(newNick.ToUTF8()));
}

// ---------- private helpers ----------

wxString ServerConnectionPanel::BuildConsoleTabTitle() const
{
    return "Console - " + m_server + " (" + m_nick + ")";
}

void ServerConnectionPanel::ConnectCore()
{
    long portVal = 6667;
    m_port.ToLong(&portVal);

    m_core.connectToServer(
        std::string(m_server.ToUTF8()),
        static_cast<int>(portVal),
        std::string(m_nick.ToUTF8()));
}

ChannelPage* ServerConnectionPanel::GetOrCreateChannelPage(const wxString& channelName)
{
    auto it = m_channels.find(channelName);
    if (it != m_channels.end())
        return it->second;

    // Create new channel page
    auto* page = new ChannelPage(m_viewBook, channelName);
    m_viewBook->AddPage(page, channelName, true);

    m_channels[channelName] = page;
    return page;
}

// ---------- IRCCore callbacks ----------

void ServerConnectionPanel::HandleCoreLog(const wxString& msg)
{
    // Filter out noisy/internal messages
    if (msg.StartsWith("[Sent]"))
        return;
    if (msg.StartsWith("[Sending]"))
        return;
    if (msg.StartsWith("Network thread"))
        return;
    if (msg.StartsWith("[Auto]"))  // PONG replies
        return;

    // Update status bar for connection events
    if (msg.StartsWith("Connected to ") || msg.StartsWith("Connecting to "))
    {
        wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
        if (frame)
            frame->SetStatusText(msg);
    }

    LogToConsole(msg);
}

void ServerConnectionPanel::HandleCoreMessage(const wxString& /*source*/,
    const wxString& /*text*/)
{
    // Don't log here - messages are handled in HandleRawLine for channels
    // and this would cause duplicates
}


// ---------- RAW LINE HANDLER ----------

void ServerConnectionPanel::HandleRawLine(const wxString& line)
{
    // Parse IRC message: [:prefix] command [params] [:trailing]
    wxString prefix, command, params, trailing;
    wxString rest = line;

    // Extract prefix (starts with :)
    if (rest.StartsWith(":"))
    {
        rest = rest.Mid(1);
        prefix = rest.BeforeFirst(' ');
        rest = rest.AfterFirst(' ');
    }

    // Extract command
    command = rest.BeforeFirst(' ');
    rest = rest.AfterFirst(' ');

    // Extract trailing (after " :")
    if (rest.Contains(" :"))
    {
        params = rest.BeforeFirst(':');
        trailing = rest.AfterFirst(':');
    }
    else
    {
        params = rest;
    }

    params = params.Trim(true).Trim(false);

    // Split params by space
    auto words = wxSplit(params, ' ');

    // Helper to extract nick from prefix (nick!user@host)
    auto getNick = [](const wxString& pfx) -> wxString {
        if (pfx.Contains("!"))
            return pfx.BeforeFirst('!');
        return pfx;
    };

    // Helper to append to a channel's log
    auto appendToChan = [this](const wxString& chan, const wxString& text) {
        ChannelPage* page = GetOrCreateChannelPage(chan);
        page->AppendLog(text);
    };

    // ---------- PRIVMSG ----------
    if (command == "PRIVMSG" && !words.empty())
    {
        wxString target = NormalizeChannelName(words[0]);
        wxString nick = getNick(prefix);

        if (IsChannelName(target))
        {
            appendToChan(target, "<" + nick + "> " + trailing);
        }
        else
        {
            // Private message to us
            LogToConsole("[PM from " + nick + "] " + trailing);
        }
        return;
    }

    // ---------- NOTICE ----------
    if (command == "NOTICE" && !words.empty())
    {
        wxString target = NormalizeChannelName(words[0]);
        wxString nick = getNick(prefix);

        if (IsChannelName(target))
        {
            appendToChan(target, "-" + nick + "- " + trailing);
        }
        else
        {
            LogToConsole("-" + nick + "- " + trailing);
        }
        return;
    }

    // ---------- JOIN ----------
    if (command == "JOIN")
    {
        wxString nick = getNick(prefix);
        wxString chan = trailing.IsEmpty() ? params : trailing;
        chan = NormalizeChannelName(chan);

        ChannelPage* page = GetOrCreateChannelPage(chan);

        if (nick == m_nick)
        {
            // We joined - switch to the new tab
            int idx = m_viewBook->FindPage(page);
            if (idx != wxNOT_FOUND)
                m_viewBook->SetSelection(idx);
        }

        page->AppendLog(nick + " has joined " + chan);
        
        // Add nick to the list if not already present
        wxListBox* nickList = page->GetNickList();
        if (nickList->FindString(nick) == wxNOT_FOUND)
            nickList->Append(nick);

        return;
    }

    // ---------- PART ----------
    if (command == "PART" && !words.empty())
    {
        wxString chan = NormalizeChannelName(words[0]);
        wxString nick = getNick(prefix);

        auto it = m_channels.find(chan);
        if (it != m_channels.end())
        {
            ChannelPage* page = it->second;
            wxString reason = trailing.IsEmpty() ? "" : " (" + trailing + ")";
            page->AppendLog(nick + " has left " + chan + reason);

            int idx = page->GetNickList()->FindString(nick);
            if (idx != wxNOT_FOUND)
                page->GetNickList()->Delete(idx);
        }
        return;
    }

    // ---------- QUIT ----------
    if (command == "QUIT")
    {
        wxString nick = getNick(prefix);
        wxString reason = trailing.IsEmpty() ? "Quit" : trailing;

        for (auto& [chanName, page] : m_channels)
        {
            int idx = page->GetNickList()->FindString(nick);
            if (idx != wxNOT_FOUND)
            {
                page->AppendLog(nick + " has quit (" + reason + ")");
                page->GetNickList()->Delete(idx);
            }
        }
        return;
    }

    // ---------- KICK ----------
    if (command == "KICK" && words.size() >= 2)
    {
        wxString chan = NormalizeChannelName(words[0]);
        wxString kicked = words[1];
        wxString kicker = getNick(prefix);
        wxString reason = trailing.IsEmpty() ? kicked : trailing;

        auto it = m_channels.find(chan);
        if (it != m_channels.end())
        {
            ChannelPage* page = it->second;
            page->AppendLog(kicked + " was kicked by " + kicker + " (" + reason + ")");

            int idx = page->GetNickList()->FindString(kicked);
            if (idx != wxNOT_FOUND)
                page->GetNickList()->Delete(idx);
        }
        return;
    }

    // ---------- NICK CHANGE ----------
    if (command == "NICK")
    {
        wxString oldNick = getNick(prefix);
        
        // New nick can be in trailing (after :) or in params
        wxString newNick = trailing;
        if (newNick.IsEmpty())
            newNick = params;
        newNick.Trim(true).Trim(false);
        
        // Remove leading colon if present
        if (newNick.StartsWith(":"))
            newNick = newNick.Mid(1);

        if (newNick.IsEmpty())
        {
            LogToConsole("*** Error parsing NICK change");
            return;
        }

        // Update nick in all channels
        for (auto& [chanName, page] : m_channels)
        {
            int idx = page->GetNickList()->FindString(oldNick);
            if (idx != wxNOT_FOUND)
            {
                page->GetNickList()->Delete(idx);
                page->GetNickList()->Append(newNick);
                page->AppendLog(oldNick + " is now known as " + newNick);
            }
        }

        // Update our own nick if it changed
        if (oldNick == m_nick)
        {
            m_nick = newNick;
            if (m_viewBook)
                m_viewBook->SetPageText(0, BuildConsoleTabTitle());
            
            LogToConsole("You are now known as " + newNick);
        }
        return;
    }

    // ---------- TOPIC ----------
    if (command == "TOPIC" && !words.empty())
    {
        wxString chan = NormalizeChannelName(words[0]);
        wxString nick = getNick(prefix);

        auto it = m_channels.find(chan);
        if (it != m_channels.end())
        {
            it->second->AppendLog(nick + " changed topic to: " + trailing);
        }
        return;
    }

    // ---------- 332 (RPL_TOPIC) ----------
    if (command == "332" && words.size() >= 2)
    {
        wxString chan = NormalizeChannelName(words[1]);
        auto it = m_channels.find(chan);
        if (it != m_channels.end())
        {
            it->second->AppendLog("Topic: " + trailing);
        }
        return;
    }

    // ---------- 353 (RPL_NAMREPLY) ----------
    if (command == "353" && words.size() >= 3)
    {
        wxString chan = NormalizeChannelName(words[2]);
        wxArrayString nicks = wxSplit(trailing, ' ');

        ChannelPage* page = GetOrCreateChannelPage(chan);
        page->GetNickList()->Clear();

        for (auto& n : nicks)
        {
            // Strip mode prefixes (@, +, %, etc.)
            while (!n.IsEmpty() && (n[0] == '@' || n[0] == '+' || 
                                    n[0] == '%' || n[0] == '~' || n[0] == '&'))
            {
                n = n.Mid(1);
            }
            if (!n.IsEmpty())
                page->GetNickList()->Append(n);
        }
        return;
    }

    // ---------- 366 (RPL_ENDOFNAMES) ----------
    if (command == "366" && words.size() >= 2)
    {
        wxString chan = NormalizeChannelName(words[1]);
        auto it = m_channels.find(chan);
        if (it != m_channels.end())
        {
            it->second->AppendLog("--- End of NAMES list ---");
        }
        return;
    }

    // ---------- 433 (ERR_NICKNAMEINUSE) ----------
    if (command == "433")
    {
        LogToConsole("*** Nickname is already in use. Try /nick <newnick>");
        return;
    }

    // ---------- 005 (RPL_ISUPPORT) - Server features, hide from user ----------
    if (command == "005")
    {
        return;
    }

    // ---------- 331 (RPL_NOTOPIC) - No topic set, hide from user ----------
    if (command == "331")
    {
        return;
    }

    // ---------- Welcome messages (001-004) ----------
    if (command == "001" || command == "002" || command == "003" || command == "004")
    {
        LogToConsole(trailing);
        
        // Update status bar on successful connect (001)
        if (command == "001")
        {
            wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
            if (frame)
                frame->SetStatusText("Connected to " + m_server + " as " + m_nick);
        }
        return;
    }

    // ---------- MOTD lines ----------
    if (command == "372" || command == "375" || command == "376")
    {
        LogToConsole(trailing);
        return;
    }

    // ---------- Default: show in console ----------
    LogToConsole("<< " + line);
}

// ---------- UI HANDLERS ----------

void ServerConnectionPanel::HandleJoinCommand(const wxString& text)
{
    // Parse the channel name from command, but don't create tab yet
    // Tab creation happens when server echoes JOIN back
    wxString rest = text.AfterFirst(' ');
    rest.Trim(true).Trim(false);
    // Just validate that there's a channel name
    if (rest.IsEmpty())
    {
        LogToConsole("Usage: /join <#channel>");
    }
}

void ServerConnectionPanel::OnSend(wxCommandEvent&)
{
    wxString text = m_input->GetValue();
    if (text.IsEmpty())
        return;

    wxString lower = text.Lower();

    // Handle /join command
    if (lower.StartsWith("/join ") || lower.StartsWith("/j "))
    {
        HandleJoinCommand(text);
        m_core.handleUserInput(std::string(text.ToUTF8()));
        m_input->Clear();
        return;
    }

    int sel = m_viewBook->GetSelection();

    if (sel <= 0)
    {
        // Console tab - send raw
        m_core.handleUserInput(std::string(text.ToUTF8()));
    }
    else
    {
        // Channel tab
        wxString chanName = m_viewBook->GetPageText(sel);

        if (!chanName.IsEmpty())
        {
            if (text[0] == '/')
            {
                // Handle /me action
                wxString lowerText = text.Lower();
                if (lowerText.StartsWith("/me "))
                {
                    wxString action = text.Mid(4);
                    wxString ctcp = "PRIVMSG " + chanName + " :\001ACTION " + action + "\001";
                    m_core.sendRaw(std::string(ctcp.ToUTF8()));
                    
                    // Echo locally
                    auto it = m_channels.find(chanName);
                    if (it != m_channels.end())
                        it->second->AppendLog("* " + m_nick + " " + action);
                }
                else
                {
                    // Other commands
                    m_core.handleUserInput(std::string(text.ToUTF8()));
                }
            }
            else
            {
                // Regular message to channel
                wxString out = "/msg " + chanName + " " + text;
                m_core.handleUserInput(std::string(out.ToUTF8()));

                // Echo locally
                auto it = m_channels.find(chanName);
                if (it != m_channels.end())
                    it->second->AppendLog("<" + m_nick + "> " + text);
            }
        }
    }

    m_input->Clear();
}

void ServerConnectionPanel::OnTabClosed(wxAuiNotebookEvent& evt)
{
    int page = evt.GetSelection();

    if (page == 0)
    {
        // Closing console = disconnect
        m_core.disconnect();
        m_channels.clear();
        
        // Update status bar
        wxFrame* frame = wxDynamicCast(wxGetTopLevelParent(this), wxFrame);
        if (frame)
            frame->SetStatusText("Disconnected");
        return;
    }

    // Get channel name before it's removed
    wxString chan = m_viewBook->GetPageText(page);

    // Remove from our map
    m_channels.erase(chan);

    // Send PART to server
    m_core.sendRaw("PART " + std::string(chan.ToUTF8()));
}
