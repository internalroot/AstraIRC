# AstraIRC & LuminaCore Development Roadmap

A unified development plan for the AstraIRC client and LuminaCore IRCD server.

---

## Project Overview

| Project | Language | Current Version | Description |
|---------|----------|-----------------|-------------|
| **AstraIRC** | C++ / wxWidgets | v1.0.0 | Cross-platform IRC client |
| **LuminaCore IRCD** | Node.js | v0.1-dev | IRC server with built-in services |

---

## Complete Role Hierarchy

```
┌─────────────────────────────────────────────────────────────┐
│                    SERVER-LEVEL ROLES                        │
│              (Global across all channels)                    │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│   👑 Server Owner(s)     ← Top level, config designated      │
│          ↓                  Can: Manage everything           │
│                                                              │
│   🛡️  Server Admin        ← Can: Manage opers, helpers,      │
│          ↓                       server settings             │
│                                                              │
│   ⚔️  Server Operator     ← Can: Manage helpers, moderate    │
│          ↓                                                   │
│                                                              │
│   🤝 Server Helper        ← Hidden role, temp bans only      │
│                             Can: Kick, temp ban, mute        │
│                                                              │
│   ⭐ ALL STAFF can go UNDERCOVER (appear as normal user)     │
│                                                              │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                   CHANNEL-LEVEL ROLES                        │
│                   (Per-channel basis)                        │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│   🏠 Channel Owner (~)    ← Founder, full channel control    │
│          ↓                                                   │
│                                                              │
│   🔧 Channel Op (@)       ← Kick, ban, modes, topic          │
│          ↓                                                   │
│                                                              │
│   🔨 Half-Op (%)          ← Limited control (maybe?)         │
│          ↓                                                   │
│                                                              │
│   🎤 Voice (+)            ← Can speak in +m channels         │
│          ↓                                                   │
│                                                              │
│   👤 Normal User          ← Basic participation              │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## Server Role Details

### Visibility Rules

| Checkbox | Who Can SEE | Who Can CHANGE |
|----------|-------------|----------------|
| Server Owner | Everyone | Config file only |
| Server Admin | Everyone | Owners only |
| Server Operator | Everyone | Admins+ |
| Server Helper | **Helpers+ only** | Opers+ |

### Undercover System

Staff can hide their role while retaining full powers:

| Aspect | Details |
|--------|---------|
| Who can go undercover | Any staff (Admin, Oper, Helper) |
| Powers when undercover | Full powers retained |
| Visibility to users | Appears as normal user |
| Visibility to staff | Staff always see actual roles |
| Toggle | GOCOVER / UNCOVER commands |
| Default | Visible (must choose to hide) |
| Logging | Status changes logged for accountability |

### Permission Matrix

| Action | Owner | Admin | Oper | Helper |
|--------|-------|-------|------|--------|
| Manage Admins | ✅ | ❌ | ❌ | ❌ |
| Manage Opers | ✅ | ✅ | ❌ | ❌ |
| Manage Helpers | ✅ | ✅ | ✅ | ❌ |
| Server settings | ✅ | ✅ | ❌ | ❌ |
| Set max ban time | ✅ | ✅? | ❌ | ❌ |
| Kick any user | ✅ | ✅ | ✅ | ✅ |
| Permanent ban | ✅ | ✅ | ✅ | ❌ |
| Temp ban | ✅ | ✅ | ✅ | ✅ |
| Go undercover | ✅ | ✅ | ✅ | ✅ |
| See helpers | ✅ | ✅ | ✅ | ✅ |
| See undercover | ✅ | ✅ | ✅ | ✅ |

### Helper Restrictions

| Restriction | Details |
|-------------|---------|
| Ban type | Temporary only |
| Ban requirements | Must include reason AND duration |
| Max duration | Cannot exceed server's max ban time |
| Channel modes | Cannot change |
| Topic | Cannot change |
| Visibility | Hidden from normal users |

---

## Channel Role Details

### Standard IRC Prefixes

| Mode | Prefix | Role | Granted By |
|------|--------|------|------------|
| `q` | `~` | Channel Owner | Registration / Founder transfer |
| `o` | `@` | Channel Operator | Owner, or access list |
| `h` | `%` | Half-Operator | Owner/Op, or access list (maybe) |
| `v` | `+` | Voice | Owner/Op/HalfOp |

### Channel Role Permissions

| Action | Owner (~) | Op (@) | HalfOp (%) | Voice (+) | User |
|--------|-----------|--------|------------|-----------|------|
| Kick | ✅ | ✅ | ✅ | ❌ | ❌ |
| Perm ban | ✅ | ✅ | ⚠️ Temp? | ❌ | ❌ |
| Set topic | ✅ | ✅ | ❌ | ❌ | ❌ |
| Change modes | ✅ | ✅ | ❌ | ❌ | ❌ |
| Grant Op | ✅ | ❌ | ❌ | ❌ | ❌ |
| Grant HalfOp | ✅ | ✅ | ❌ | ❌ | ❌ |
| Grant Voice | ✅ | ✅ | ✅? | ❌ | ❌ |
| Speak in +m | ✅ | ✅ | ✅ | ✅ | ❌ |

---

## Current State

### AstraIRC (v1.0.0) ✅

- [x] Basic IRC protocol (NICK, USER, PRIVMSG, JOIN, PART, QUIT)
- [x] wxWidgets GUI with tabbed interface
- [x] Console tab + channel tabs with nick lists
- [x] Cross-platform sockets (Windows/macOS/Linux)
- [x] Threaded networking with GUI marshalling
- [x] Commands: /join, /part, /nick, /msg, /me, /quit, /raw

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

#### AstraIRC v1.1.0 — Stability & Usability

- [ ] **Auto-reconnect with toggle** — Reconnect on disconnect; preference to enable/disable
- [ ] **Server password support** — Send PASS before NICK/USER
- [ ] **Multiple server connections** — Tabs or tree view for multiple servers
- [ ] **Input history** — Up/down arrows recall previous commands
- [ ] **Tab completion** — Complete nicks and commands with Tab key
- [ ] **Clickable URLs** — Detect links, open in browser on click
- [ ] **Configurable timestamps** — Toggle on/off; choose 12-hour or 24-hour format
- [ ] **Window title updates** — Show activity/current channel in title

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

#### AstraIRC v1.2.0 — User Profiles

- [ ] **User Profile Window** — Double-click nick opens profile dialog:
  - Nickname, username, hostname
  - Real name (from WHOIS)
  - Server connected to
  - Idle time, signon time
  - Channels in common
  - Away message (if set)
  - Account name (LuminaCore)
  - Registration date (LuminaCore)

- [ ] **Server role checkboxes**:
  - Server Owner — visible to all, changeable by config/owners
  - Server Admin — visible to all, changeable by owners
  - Server Operator — visible to all, changeable by admins+
  - Server Helper — **visible to helpers+ only**, changeable by opers+
  
- [ ] **Undercover indicator** (staff view only):
  - Shows when user is undercover
  - Displays actual role vs visible role
  - "Go Visible" / "Go Undercover" button on own profile

- [ ] **Channel role display**:
  - Channel Owner (~)
  - Channel Op (@)
  - Half-Op (%)
  - Voice (+)
  - Checkboxes enabled based on your channel permissions

- [ ] **Profile actions**:
  - "Send Private Message" button → opens PM tab
  - "Refresh" button → re-sends WHOIS
  - "Ignore" button → adds to ignore list

- [ ] **WHOIS parsing** — Handle standard + LuminaCore custom numerics

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

#### AstraIRC v1.3.0 — Channel Management Window

- [ ] **Channel Options Window** — Double-click channel log to open:

  **Info Tab:**
  - Channel name, creation date
  - User count
  - Topic with "Set Topic" button (enabled if allowed)

  **Modes Tab:**
  - Checkboxes: +n, +t, +i, +m, +s, +p
  - Key (+k) text field
  - User limit (+l) number field
  - All greyed unless you're channel op+

  **Bans Tab:**
  - Current ban list (+b)
  - Add/Remove buttons (enabled if chanop)
  - Ban exceptions (+e) if supported

  **Access Tab (LuminaCore):**
  - Auto-op list
  - Auto-voice list  
  - Channel founder/owner display
  - Access level management

  **Settings Tab (LuminaCore):**
  - Channel registration status
  - Founder transfer
  - Channel passwords

- [ ] **Permission-aware UI** — Controls enable/disable based on status
- [ ] **Real-time updates** — Track mode changes live
- [ ] **Context menu alternative** — Right-click tab for quick access

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

#### AstraIRC v1.4.0 — Private Messaging & Notifications

- [ ] **Private message tabs** — Separate tabs for PMs
- [ ] **Tab highlighting**:
  - New messages (one color)
  - Mentions of your nick (different color)
  - Activity indicator
- [ ] **Nick highlighting** — Highlight messages containing your nick
- [ ] **Desktop notifications** — System notifications for mentions/PMs (toggleable)
- [ ] **Sound alerts** — Optional sounds (toggleable)
- [ ] **Away integration** — /away command, show in profiles
- [ ] **Ignore list** — /ignore command, manageable in preferences

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

#### AstraIRC v1.5.0 — Settings & Persistence

- [ ] **Settings file** — JSON format, auto-save

- [ ] **Preferences dialog**:
  - **General**: Timestamps (on/off, 12h/24h), auto-reconnect toggle
  - **Notifications**: Sounds, desktop notifications, highlight words
  - **Appearance**: Fonts, colors
  - **Logging**: Enable/disable, file location

- [ ] **Server list manager**:
  - Save favorite servers
  - Address, port, SSL toggle
  - Nickname + alternatives
  - Server password
  - Auto-join channels
  - Perform commands
  - Account credentials (LuminaCore)

- [ ] **Nick alternatives** — Try nick_, nick__ on 433

- [ ] **Window state** — Remember size, position, tabs

- [ ] **Chat logging** — Save to files

- [ ] **LuminaCore account UI**:
  - Registration dialog
  - Login dialog
  - Auto-login option
  - Password change

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

#### AstraIRC v1.6.0 — Advanced IRC

- [ ] **SSL/TLS support** — Secure connections (port 6697)
- [ ] **SASL authentication** — Modern auth
- [ ] **CAP negotiation** — Request capabilities
- [ ] **IRCv3 features** — multi-prefix, away-notify, account-notify
- [ ] **CTCP support** — VERSION, TIME, PING replies
- [ ] **DCC awareness** — Display DCC requests
- [ ] **Channel list browser** — /list with searchable dialog

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

#### AstraIRC v1.7.0 — Polish & UX

- [ ] **Themes** — Light and dark mode
- [ ] **Spell checking** — System integration
- [ ] **Search in channel** — Ctrl+F
- [ ] **Scrollback limit** — Configurable buffer
- [ ] **Keyboard shortcuts** — Ctrl+Tab, Ctrl+W, etc.
- [ ] **Drag-and-drop tabs** — Reorder tabs
- [ ] **System tray** — Minimize, badges
- [ ] **Split view** — Multiple channels side-by-side (optional)

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

#### AstraIRC v2.0.0 — Release Ready

- [ ] **Installers** — Windows, macOS, Linux packages
- [ ] **Auto-update checker**
- [ ] **Help system** — Built-in or docs link
- [ ] **About dialog** — Credits, license
- [ ] **First-run wizard**
- [ ] **Localization** — Multi-language
- [ ] **Accessibility** — Screen reader, keyboard nav
- [ ] **Cross-platform testing**

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

## Profile Window Quick Reference

### What Normal Users See

```
┌─────────────────────────────────────┐
│  User Profile: TargetUser           │
├─────────────────────────────────────┤
│  SERVER ROLES:                      │
│  ☐ Server Owner     (greyed)        │
│  ☐ Server Admin     (greyed)        │
│  ☐ Server Operator  (greyed)        │
│  [Helper checkbox NOT visible]      │
│                                     │
│  CHANNEL ROLES (#current):          │
│  ☐ Channel Owner    (greyed)        │
│  ☐ Channel Op       (greyed)        │
│  ☐ Half-Op          (greyed)        │
│  ☐ Voice            (greyed)        │
│                                     │
│  [Send PM]  [Whois]  [Ignore]       │
└─────────────────────────────────────┘
```

### What Staff See (viewing undercover admin)

```
┌─────────────────────────────────────┐
│  User Profile: UndercoverAdmin      │
├─────────────────────────────────────┤
│  SERVER ROLES:                      │
│  ☐ Server Owner                     │
│  ☐ Server Admin    ⚠️ UNDERCOVER    │
│  ☐ Server Operator                  │
│  ☐ Server Helper                    │
│                                     │
│  ℹ️ Actual role: Admin (hidden)     │
│                                     │
│  [Send PM]  [Whois]  [Ignore]       │
└─────────────────────────────────────┘
```

### Viewing Your Own Profile (as undercover admin)

```
┌─────────────────────────────────────┐
│  User Profile: YourNick (You)       │
├─────────────────────────────────────┤
│  SERVER ROLES:                      │
│  ☐ Server Owner                     │
│  ☑ Server Admin    🔒 UNDERCOVER    │
│  ☐ Server Operator                  │
│  ☐ Server Helper                    │
│                                     │
│  [Go Visible]  [Send PM]  [Ignore]  │
└─────────────────────────────────────┘
```

---

## Open Questions

- [ ] Half-Op: Include, skip, or implement but hide?
- [ ] Max ban duration: Owner only, or Admin too?
- [ ] Helper prefix: Hidden, visible, or none?
- [ ] Multiple owners: Config-based list?

---

## Notes

- LuminaCore name may change
- AstraIRC works with any IRC server; enhanced features on LuminaCore
- Built-in services eliminate external NickServ/ChanServ
- Undercover system allows staff to blend in
- Helpers are always hidden from normal users

---

*Last updated: November 2025*
