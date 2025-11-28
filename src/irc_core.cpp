#include "irc_core.h"

#include <iostream>
#include <cstring>

#ifdef _WIN32
    #define _WINSOCK_DEPRECATED_NO_WARNINGS
    #include <ws2tcpip.h>
    // Note: ws2_32.lib is linked via CMakeLists.txt for cross-compiler compatibility
    bool IRCCore::wsaInitialized = false;
    std::mutex IRCCore::wsaMutex;
    #define CLOSE_SOCKET(s) closesocket(s)
    #define SHUTDOWN_SOCKET(s) shutdown(s, SD_BOTH)
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <cerrno>
    #define CLOSE_SOCKET(s) close(s)
    #define SHUTDOWN_SOCKET(s) shutdown(s, SHUT_RDWR)
    #define SOCKET_ERROR (-1)
#endif

// ----------------------
// IRCCore implementation
// ----------------------

IRCCore::IRCCore()
{
#ifdef _WIN32
    std::lock_guard<std::mutex> lock(wsaMutex);
    if (!wsaInitialized)
    {
        WSADATA wsaData;
        int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (res != 0)
        {
            std::cerr << "WSAStartup failed: " << res << std::endl;
        }
        else
        {
            wsaInitialized = true;
        }
    }
#endif
}

IRCCore::~IRCCore()
{
    disconnect();
}

void IRCCore::setLogCallback(LogCallback cb)
{
    onLog = std::move(cb);
}

void IRCCore::setMessageCallback(MessageCallback cb)
{
    onMessage = std::move(cb);
}

void IRCCore::setRawLineCallback(RawLineCallback cb)
{
    onRawLine = std::move(cb);
}

void IRCCore::setDisconnectCallback(DisconnectCallback cb)
{
    onDisconnect = std::move(cb);
}

void IRCCore::log(const std::string& msg)
{
    if (onLog)
        onLog(msg);
    else
        std::cerr << "[IRCCore] " << msg << std::endl;
}

bool IRCCore::isConnected() const
{
    return running.load() && sock != InvalidSocket;
}

std::string IRCCore::getNick() const
{
    std::lock_guard<std::mutex> lock(nickMutex);
    return currentNick;
}

void IRCCore::closeSocket()
{
    if (sock != InvalidSocket)
    {
        SHUTDOWN_SOCKET(sock);
        CLOSE_SOCKET(sock);
        sock = InvalidSocket;
    }
}

void IRCCore::connectToServer(const std::string& host, int port, const std::string& nick, const std::string& password)
{
    // If already running, disconnect first
    if (running.load())
    {
        disconnect();
    }

    // Ensure previous thread is fully cleaned up
    if (networkThread.joinable())
    {
        networkThread.join();
    }

    serverHost = host;
    serverPort = port;
    serverPassword = password;

    {
        std::lock_guard<std::mutex> lock(nickMutex);
        currentNick = nick;
    }

    running = true;
    networkThread = std::thread(&IRCCore::networkThreadFunc, this);
}

void IRCCore::disconnect()
{
    if (!running.load() && sock == InvalidSocket)
        return;

    running = false;

    // Closing the socket will break recv() in the thread
    closeSocket();

    if (networkThread.joinable())
        networkThread.join();

    log("Disconnected.");
}

void IRCCore::enqueueToSend(const std::string& lineWithCRLF)
{
    std::lock_guard<std::mutex> lock(sendMutex);
    sendQueue.push_back(lineWithCRLF);
}

void IRCCore::sendRaw(const std::string& line)
{
    enqueueToSend(line + "\r\n");
}

void IRCCore::handleUserInput(const std::string& line)
{
    if (line.empty())
        return;

    if (line[0] == '/')
    {
        std::string cmdLine = line.substr(1);
        std::string cmd;
        std::string rest;

        auto pos = cmdLine.find(' ');
        if (pos == std::string::npos)
        {
            cmd = cmdLine;
        }
        else
        {
            cmd = cmdLine.substr(0, pos);
            rest = cmdLine.substr(pos + 1);
        }

        // Convert command to lowercase for case-insensitive matching
        std::string cmdLower = cmd;
        for (auto& c : cmdLower)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        if (cmdLower == "quit" || cmdLower == "exit")
        {
            sendRaw("QUIT :Client exiting");
            log("Sent QUIT; disconnecting.");
            disconnect();
        }
        else if (cmdLower == "raw")
        {
            if (!rest.empty())
            {
                log("[Client] RAW: " + rest);
                sendRaw(rest);
            }
            else
            {
                log("[Client] Usage: /raw <line>");
            }
        }
        else if (cmdLower == "msg" || cmdLower == "privmsg")
        {
            auto p = rest.find(' ');
            if (p == std::string::npos)
            {
                log("[Client] Usage: /msg <target> <message>");
            }
            else
            {
                std::string target = rest.substr(0, p);
                std::string text = rest.substr(p + 1);
                sendRaw("PRIVMSG " + target + " :" + text);

                if (onMessage)
                {
                    std::lock_guard<std::mutex> lock(nickMutex);
                    onMessage(currentNick, text);
                }
            }
        }
        else if (cmdLower == "join" || cmdLower == "j")
        {
            if (!rest.empty())
            {
                sendRaw("JOIN " + rest);
            }
            else
            {
                log("[Client] Usage: /join <#channel>");
            }
        }
        else if (cmdLower == "part" || cmdLower == "leave")
        {
            if (!rest.empty())
            {
                sendRaw("PART " + rest);
            }
            else
            {
                log("[Client] Usage: /part <#channel> [reason]");
            }
        }
        else if (cmdLower == "nick")
        {
            if (!rest.empty())
            {
                sendRaw("NICK " + rest);
            }
            else
            {
                log("[Client] Usage: /nick <newnick>");
            }
        }
        else if (cmdLower == "me")
        {
            // CTCP ACTION - needs a target, handled differently
            log("[Client] Usage: /me is used in channel tabs");
        }
        else
        {
            // Unknown command - send as raw IRC command
            log("[Client] Sending: " + cmdLine);
            sendRaw(cmdLine);
        }
    }
    else
    {
        // Not a command - send as raw (for console)
        sendRaw(line);
    }
}

void IRCCore::networkThreadFunc()
{
    log("Network thread starting for " + serverHost + ":" + std::to_string(serverPort));

    // Resolve host
    SocketType s = InvalidSocket;

    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;      // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo* result = nullptr;
    std::string portStr = std::to_string(serverPort);

    int res = getaddrinfo(serverHost.c_str(), portStr.c_str(), &hints, &result);
    if (res != 0 || !result)
    {
        log("getaddrinfo failed: " + std::to_string(res));
        running = false;

        // Notify GUI of connection failure
        if (onDisconnect)
            onDisconnect();
        return;
    }

    // Try each address until we successfully connect
    for (addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next)
    {
        s = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (s == InvalidSocket)
            continue;

        if (connect(s, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen)) == SOCKET_ERROR)
        {
            CLOSE_SOCKET(s);
            s = InvalidSocket;
            continue;
        }

        // Successfully connected
        break;
    }

    freeaddrinfo(result);

    if (s == InvalidSocket)
    {
        log("Unable to connect to server.");
        running = false;

        // Notify GUI of connection failure
        if (onDisconnect)
            onDisconnect();
        return;
    }

    sock = s;
    log("Connected to " + serverHost + ":" + std::to_string(serverPort));

    // Send initial IRC registration
    {
        std::lock_guard<std::mutex> lock(nickMutex);
        if (!currentNick.empty())
        {
            // Send PASS command first if password is provided
            if (!serverPassword.empty())
            {
                sendRaw("PASS " + serverPassword);
            }

            sendRaw("NICK " + currentNick);
            sendRaw("USER " + currentNick + " 0 * :AstraIRC user");
        }
    }

    char buf[4096];

    while (running.load())
    {
        // Process send queue
        {
            std::vector<std::string> toSend;
            {
                std::lock_guard<std::mutex> lock(sendMutex);
                toSend.swap(sendQueue);
            }

            for (const auto& line : toSend)
            {
                int sent = send(sock, line.c_str(), static_cast<int>(line.size()), 0);
                if (sent == SOCKET_ERROR)
                {
                    log("send() failed, disconnecting.");
                    running = false;
                    break;
                }
                // Log without the trailing CRLF
                if (line.size() >= 2)
                    log("[Sent] " + line.substr(0, line.size() - 2));
            }
        }

        if (!running.load())
            break;

        // Use select() to wait for data with a timeout
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);

        timeval tv{};
        tv.tv_sec = 0;
        tv.tv_usec = 200000;  // 200ms timeout

#ifdef _WIN32
        int sel = select(0, &readfds, nullptr, nullptr, &tv);  // First param ignored on Windows
#else
        int sel = select(sock + 1, &readfds, nullptr, nullptr, &tv);
#endif

        if (sel == SOCKET_ERROR)
        {
            log("select() failed, disconnecting.");
            running = false;
            break;
        }
        else if (sel == 0)
        {
            // Timeout - no data, continue loop
            continue;
        }

        if (FD_ISSET(sock, &readfds))
        {
            int received = recv(sock, buf, sizeof(buf) - 1, 0);
            if (received <= 0)
            {
                if (received == 0)
                    log("Server closed connection.");
                else
                    log("recv() error, disconnecting.");
                running = false;
                break;
            }

            buf[received] = '\0';
            recvBuffer.append(buf);

            // Process complete lines
            std::size_t pos;
            while ((pos = recvBuffer.find("\r\n")) != std::string::npos)
            {
                std::string line = recvBuffer.substr(0, pos);
                recvBuffer.erase(0, pos + 2);
                handleServerLine(line);
            }
        }
    }

    closeSocket();
    running = false;

    log("Network thread stopped.");

    // Notify GUI of disconnection
    if (onDisconnect)
        onDisconnect();
}

void IRCCore::handleServerLine(const std::string& line)
{
    // Forward raw line to GUI for processing
    if (onRawLine)
        onRawLine(line);

    // Auto-reply to PING to keep connection alive
    if (line.rfind("PING ", 0) == 0)
    {
        std::string payload = line.substr(5);
        sendRaw("PONG " + payload);
        log("[Auto] Replied with PONG " + payload);
        return;
    }

    // Parse IRC message format: [:prefix] command [params] [:trailing]
    if (!line.empty() && line[0] == ':')
    {
        auto sp1 = line.find(' ');
        if (sp1 != std::string::npos)
        {
            std::string prefix = line.substr(1, sp1 - 1);
            auto sp2 = line.find(' ', sp1 + 1);
            if (sp2 != std::string::npos)
            {
                std::string command = line.substr(sp1 + 1, sp2 - (sp1 + 1));
                
                // Handle PRIVMSG for the message callback
                if (command == "PRIVMSG")
                {
                    auto sp3 = line.find(' ', sp2 + 1);
                    if (sp3 != std::string::npos)
                    {
                        // std::string target = line.substr(sp2 + 1, sp3 - (sp2 + 1));
                        std::string text;
                        auto colon = line.find(" :", sp3);
                        if (colon != std::string::npos)
                            text = line.substr(colon + 2);

                        if (onMessage)
                            onMessage(prefix, text);
                    }
                }
                // Handle NICK changes to update our nick if it's us
                else if (command == "NICK")
                {
                    auto bang = prefix.find('!');
                    std::string oldNick = (bang != std::string::npos) ? prefix.substr(0, bang) : prefix;
                    
                    std::string newNick;
                    auto colon = line.find(" :", sp2);
                    if (colon != std::string::npos)
                        newNick = line.substr(colon + 2);
                    else
                        newNick = line.substr(sp2 + 1);
                    
                    std::lock_guard<std::mutex> lock(nickMutex);
                    if (oldNick == currentNick)
                        currentNick = newNick;
                }
            }
        }
    }
}
