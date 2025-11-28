#pragma once

#include <string>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

// Platform-specific socket type
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    using SocketType = SOCKET;
    constexpr SocketType InvalidSocket = INVALID_SOCKET;
#else
    using SocketType = int;
    constexpr SocketType InvalidSocket = -1;
#endif

class IRCCore
{
public:
    using LogCallback = std::function<void(const std::string&)>;
    using MessageCallback = std::function<void(const std::string& source, const std::string& text)>;
    using RawLineCallback = std::function<void(const std::string&)>;
    using DisconnectCallback = std::function<void()>;

    IRCCore();
    ~IRCCore();

    // Non-copyable, non-movable (due to thread and socket ownership)
    IRCCore(const IRCCore&) = delete;
    IRCCore& operator=(const IRCCore&) = delete;
    IRCCore(IRCCore&&) = delete;
    IRCCore& operator=(IRCCore&&) = delete;

    // GUI hooks
    void setLogCallback(LogCallback cb);
    void setMessageCallback(MessageCallback cb);
    void setRawLineCallback(RawLineCallback cb);
    void setDisconnectCallback(DisconnectCallback cb);

    // Connection management
    void connectToServer(const std::string& host, int port, const std::string& nick, const std::string& password = "");
    void disconnect();
    bool isConnected() const;

    // From GUI: stuff the user typed into the console
    void handleUserInput(const std::string& line);

    // Raw IRC line (no CRLF needed; IRCCore adds it)
    void sendRaw(const std::string& line);

    // Accessors
    std::string getNick() const;

private:
    // Internal helpers
    void networkThreadFunc();
    void log(const std::string& msg);
    void handleServerLine(const std::string& line);
    void enqueueToSend(const std::string& lineWithCRLF);
    void closeSocket();

private:
    // Thread / run state
    std::thread networkThread;
    std::atomic<bool> running{ false };

    // Connection info
    std::string serverHost;
    int serverPort{ 6667 };
    SocketType sock{ InvalidSocket };
    std::string currentNick;
    std::string serverPassword;
    mutable std::mutex nickMutex;

    // Callbacks
    LogCallback onLog;
    MessageCallback onMessage;
    RawLineCallback onRawLine;
    DisconnectCallback onDisconnect;

    // Outgoing queue
    std::mutex sendMutex;
    std::vector<std::string> sendQueue;

    // Incoming line buffer
    std::string recvBuffer;

    // For one-time Winsock init on Windows
#ifdef _WIN32
    static bool wsaInitialized;
    static std::mutex wsaMutex;
#endif
};
