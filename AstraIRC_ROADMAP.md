# AstraIRC Development Roadmap

Cross-platform IRC client built with C++ and wxWidgets.

---

## Project Overview

| Project | Language | Current Version | Description |
|---------|----------|-----------------|-------------|
| **AstraIRC** | C++ / wxWidgets | v1.0.0 | Cross-platform IRC client |

---

## Current State

### AstraIRC (v1.0.0) ✅

- [x] Basic IRC protocol (NICK, USER, PRIVMSG, JOIN, PART, QUIT)
- [x] wxWidgets GUI with tabbed interface
- [x] Console tab + channel tabs with nick lists
- [x] Cross-platform sockets (Windows/macOS/Linux)
- [x] Threaded networking with GUI marshalling
- [x] Commands: /join, /part, /nick, /msg, /me, /quit, /raw

### AstraIRC (v1.1.0) ✅

- [x] Auto-reconnect with toggle — Reconnect on disconnect; preference to enable/disable
- [x] Server password support — Send PASS before NICK/USER
- [x] Input history — Up/down arrows recall previous commands
- [x] Clickable URLs — Detect links, open in browser on click
- [x] Configurable timestamps — Toggle on/off; choose 12-hour or 24-hour format
- [x] Window title updates — Show activity/current channel in title
- [x] Tab completion — Complete nicks and commands with Tab key

---

## Version Roadmap

---

### Phase 1: Core Stability

#### AstraIRC v1.1.0 — Stability & Usability ✅

- [x] **Auto-reconnect with toggle** — Reconnect on disconnect; preference to enable/disable
- [x] **Server password support** — Send PASS before NICK/USER
- [x] **Input history** — Up/down arrows recall previous commands
- [x] **Clickable URLs** — Detect links, open in browser on click
- [x] **Configurable timestamps** — Toggle on/off; choose 12-hour or 24-hour format
- [x] **Window title updates** — Show activity/current channel in title
- [x] **Tab completion** — Complete nicks and commands with Tab key

---

### Phase 2: User Profiles & Accounts

#### AstraIRC v1.2.0 — User Profiles

- [ ] **Multiple server connections** — Tabs or tree view for multiple servers

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

---

### Phase 5: Configuration & Persistence

#### AstraIRC v1.5.0 — Settings & Persistence

- [ ] **Settings file** — JSON format, auto-save

- [ ] **Preferences dialog** — Partially implemented:
  - [x] **General**: Timestamps (on/off, 12h/24h), auto-reconnect toggle
  - [ ] **Notifications**: Sounds, desktop notifications, highlight words
  - [ ] **Appearance**: Fonts, colors
  - [ ] **Logging**: Enable/disable, file location

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

## Notes

- AstraIRC works with any IRC server; enhanced features on LuminaCore
- Profile window features require LuminaCore server with custom protocol support
- See LuminaCore documentation for server-side role hierarchy details

---

*Last updated: November 2025*
