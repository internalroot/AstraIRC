# LuminaCore IRCD Development Roadmap

IRC server with built-in services and advanced role hierarchy.

---

## Project Overview

| Project | Language | Current Version | Description |
|---------|----------|-----------------|-------------|
| **LuminaCore IRCD** | Node.js | v0.1-dev | IRC server with built-in services |

---

## Current State

### LuminaCore IRCD (v0.1-dev) ✅

- [x] TCP server with line buffering
- [x] NICK/USER registration
- [x] PING/PONG keepalive
- [x] JOIN/PART/QUIT
- [x] PRIVMSG/NOTICE to channels and users
- [x] TOPIC (query/set, respects +t)
- [x] KICK command
- [x] Channel modes: +n, +t, +i, +m, +p, +s
- [x] MODE +o/-o (op/deop)
- [x] First joiner becomes channel op
- [x] MOTD and welcome numerics
- [x] ISUPPORT (005)

---

## Version Roadmap

---

### Phase 1: Core Stability

#### LuminaCore v0.2.0 — Basic IRC Completion

- [ ] **WHOIS command** — Return nick, user, host, realname, channels, idle, signon
- [ ] **WHO command** — List users in channel or matching mask
- [ ] **AWAY command** — Set/clear away message, include in WHOIS
- [ ] **LIST command** — Return channel list with user counts
- [ ] **MOTD command** — Allow re-requesting MOTD
- [ ] **LUSERS command** — Server statistics
- [ ] **User modes** — +i (invisible), +w (wallops), +o (oper)
- [ ] **OPER command** — Authenticate as IRC operator (config-based)
- [ ] **Server PASS validation** — Check password on connect if configured

---

### Phase 2: User Profiles & Accounts

#### LuminaCore v0.3.0 — Account System

- [ ] **Account storage** — JSON file or SQLite database
- [ ] **REGISTER command** — `REGISTER <password> [email]`
- [ ] **LOGIN command** — `LOGIN <account> <password>`
- [ ] **LOGOUT command** — End session
- [ ] **Password hashing** — bcrypt or argon2
- [ ] **Session tracking** — Track logged-in account per client
- [ ] **Account info in WHOIS** — Show account name if logged in
- [ ] **Nick linking** — Multiple nicks per account (optional)
- [ ] **Custom WHOIS numerics** — Account status, registration date

---

### Phase 3: Channel Management

#### LuminaCore v0.4.0 — Channel Services

- [ ] **Channel registration** — `CHANREG <#channel>`
- [ ] **Channel ownership** — Track founder per channel
- [ ] **Persistent storage** — Save to database
- [ ] **Access lists** — Founder, admin, op, halfop, voice levels
- [ ] **CHANADD command** — `CHANADD <#channel> <level> <account>`
- [ ] **CHANREMOVE command** — `CHANREMOVE <#channel> <account>`
- [ ] **CHANINFO command** — Query registration info
- [ ] **Auto-op on join** — Apply access list automatically
- [ ] **Persistence** — Reload channels on server restart
- [ ] **Channel modes**: +k (key), +l (limit), +b (bans)

---

### Phase 4: Messaging & Notifications

#### LuminaCore v0.5.0 — Permission Hierarchy & Moderation

- [ ] **Role hierarchy**: Owner → Admin → Oper → Helper → User

- [ ] **Role storage** — Persist roles in database

- [ ] **Server Owner**:
  - Designated in config file
  - Can manage all other roles
  - Multiple owners supported

- [ ] **Server Admin**:
  - MAKEADMIN / REMOVEADMIN (owner only)
  - Can manage opers and helpers
  - Can set max ban duration

- [ ] **Server Operator**:
  - MAKEOPER / REMOVEOPER (admin+ only)
  - Can manage helpers
  - Full moderation powers

- [ ] **Server Helper**:
  - MAKEHELPER / REMOVEHELPER (oper+ only)
  - Hidden from normal users
  - Kick any user, any channel
  - Temp ban only (with reason + duration)
  - Cannot exceed max ban duration
  - Cannot change channel settings

- [ ] **Undercover system**:
  - GOCOVER command — hide role
  - UNCOVER command — reveal role
  - Staff always see actual roles
  - Log status changes

- [ ] **STAFFLIST command** — List all staff (helpers+ only)

- [ ] **Max ban duration** — Configurable by owner/admin

- [ ] **TEMPBAN command** — `TEMPBAN <#chan> <nick> <duration> <reason>`

- [ ] **Ban expiry** — Auto-remove expired bans

- [ ] **Custom WHOIS numerics**:
  - Role info (respects visibility rules)
  - Undercover indicator (staff only)

---

### Phase 5: Configuration & Persistence

#### LuminaCore v0.6.0 — Server Configuration

- [ ] **Config file** — JSON or YAML
- [ ] **Configurable MOTD** — Load from file
- [ ] **Server password** — Optional connection password
- [ ] **Port configuration** — Multiple ports, SSL
- [ ] **Operator passwords** — Config-based OPER auth
- [ ] **Connection limits** — Max clients, per-IP limits
- [ ] **Max ban duration setting** — For helper temp bans
- [ ] **Logging** — Server logs, moderation logs

---

### Phase 6: Advanced Features

#### LuminaCore v0.7.0 — Advanced Server

- [ ] **SSL/TLS support** — Secure port
- [ ] **CAP negotiation** — Advertise capabilities
- [ ] **IRCv3**: multi-prefix, away-notify, account-notify, extended-join
- [ ] **SASL authentication** — PLAIN mechanism
- [ ] **Ban list (+b)** — Store and enforce
- [ ] **Ban exceptions (+e)**
- [ ] **Invite list (+I)**
- [ ] **Channel key (+k)**
- [ ] **User limit (+l)**
- [ ] **Half-op mode (+h)** — If implementing

---

### Phase 7: Polish

#### LuminaCore v0.8.0 — Server Polish

- [ ] **Flood protection** — Rate limiting
- [ ] **K-line / G-line** — Server bans
- [ ] **REHASH** — Reload config live
- [ ] **Server statistics** — Detailed stats
- [ ] **KILL command** — Disconnect users (oper)
- [ ] **WALLOPS** — Broadcast to opers
- [ ] **Error handling** — Graceful edge cases

---

### Phase 8: Release

#### LuminaCore v1.0.0 — Release Ready

- [ ] **Documentation** — Setup, admin, protocol docs
- [ ] **Docker support**
- [ ] **Systemd service file**
- [ ] **Database migrations**
- [ ] **Backup/restore**
- [ ] **Performance testing**
- [ ] **Security audit**

---

## Custom Protocol Extensions

### Account Commands (Client → Server)

```
REGISTER <password> [email]      # Create account
LOGIN <account> <password>       # Authenticate
LOGOUT                           # End session
SETPASS <oldpass> <newpass>      # Change password
```

### Role Management (Client → Server)

```
MAKEADMIN <nick>                 # Owner only
REMOVEADMIN <nick>               # Owner only
MAKEOPER <nick>                  # Admin+ only
REMOVEOPER <nick>                # Admin+ only
MAKEHELPER <nick>                # Oper+ only
REMOVEHELPER <nick>              # Oper+ only
GOCOVER                          # Hide your role
UNCOVER                          # Reveal your role
STAFFLIST                        # List staff (helpers+)
```

### Moderation (Client → Server)

```
TEMPBAN <#chan> <nick> <duration> <reason>   # Helpers+, duration required
BAN <#chan> <nick> [reason]                  # Chanop+, can be permanent
```

### Channel Registration (Client → Server)

```
CHANREG <#channel>                           # Register (logged in)
CHANADD <#channel> <level> <account>         # Add to access list
CHANREMOVE <#channel> <account>              # Remove from list
CHANINFO <#channel>                          # Query info
```

### Custom Numerics (Server → Client)

```
# Account
900 <nick> <account> :You are now logged in as <account>
901 <nick> :You are not logged in
902 <nick> :Account created successfully
903 <nick> :Invalid credentials
904 <nick> :Account already exists
905 <nick> :Permission denied

# Roles (visibility rules apply)
770 <nick> <target> :is a Server Owner
771 <nick> <target> :is a Server Administrator
772 <nick> <target> :is a Server Operator
773 <nick> <target> :is a Server Helper           # Helpers+ only

# Undercover (staff only)
774 <nick> <target> <actual> :is undercover

# Standard (account in WHOIS)
330 <nick> <target> <account> :is logged in as
```

### Capability Detection

```
CAP LS → luminacore.com/accounts luminacore.com/roles luminacore.com/undercover
```

---

## ISUPPORT (005) Update

```
:server 005 nick CHANTYPES=# PREFIX=(qohv)~@%+ CHANMODES=b,k,l,imnpst :are supported
```

| Mode | Prefix | Role |
|------|--------|------|
| q | ~ | Channel Owner |
| o | @ | Channel Operator |
| h | % | Half-Operator |
| v | + | Voice |

---

## Open Questions

- [ ] Half-Op: Include, skip, or implement but hide?
- [ ] Max ban duration: Owner only, or Admin too?
- [ ] Helper prefix: Hidden, visible, or none?
- [ ] Multiple owners: Config-based list?

---

## Notes

- LuminaCore name may change
- Built-in services eliminate external NickServ/ChanServ
- Undercover system allows staff to blend in
- Helpers are always hidden from normal users
- See LuminaCore_HIERARCHY.md for complete role hierarchy details

---

*Last updated: November 2025*
