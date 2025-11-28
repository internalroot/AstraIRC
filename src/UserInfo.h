#pragma once

#include <string>
#include <vector>

// Stores information about an IRC user, populated from WHOIS responses
struct UserInfo
{
    // Basic identity (RPL_WHOISUSER 311)
    std::string nick;
    std::string username;
    std::string hostname;
    std::string realname;

    // Server info (RPL_WHOISSERVER 312)
    std::string server;
    std::string serverInfo;

    // Channels (RPL_WHOISCHANNELS 319)
    std::vector<std::string> channels;

    // Idle/signon time (RPL_WHOISIDLE 317)
    int idleSeconds = 0;
    long signonTime = 0;

    // Operator status (RPL_WHOISOPERATOR 313)
    bool isOperator = false;
    std::string operatorInfo;

    // Account (RPL_WHOISACCOUNT 330 - some servers)
    std::string account;

    // Away status (RPL_AWAY 301)
    std::string awayMessage;

    // LuminaCore custom fields (future)
    std::string registrationDate;
    bool isServerOwner = false;
    bool isServerAdmin = false;
    bool isServerHelper = false;
    bool isUndercover = false;
    std::string actualRole;

    // Status flags
    bool whoisComplete = false;
    bool whoisInProgress = false;

    void clear()
    {
        nick.clear();
        username.clear();
        hostname.clear();
        realname.clear();
        server.clear();
        serverInfo.clear();
        channels.clear();
        idleSeconds = 0;
        signonTime = 0;
        isOperator = false;
        operatorInfo.clear();
        account.clear();
        awayMessage.clear();
        registrationDate.clear();
        isServerOwner = false;
        isServerAdmin = false;
        isServerHelper = false;
        isUndercover = false;
        actualRole.clear();
        whoisComplete = false;
        whoisInProgress = false;
    }
};
