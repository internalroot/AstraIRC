# LuminaCore Role Hierarchy

Complete documentation of the server and channel role hierarchy system for LuminaCore IRCD.

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

## Client Integration Examples

These examples show how the AstraIRC client displays role information.

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

## Implementation Notes

### Server Owner

- **Designation**: Specified in server config file
- **Multiple owners**: Supported (config-based list)
- **Powers**: Full control over server and all roles
- **Visibility**: Always visible to all users
- **Changes**: Only via config file modification

### Server Admin

- **Promotion**: MAKEADMIN command (owner only)
- **Demotion**: REMOVEADMIN command (owner only)
- **Powers**: Manage operators and helpers, server settings
- **Visibility**: Always visible to all users

### Server Operator

- **Promotion**: MAKEOPER command (admin+ only)
- **Demotion**: REMOVEOPER command (admin+ only)
- **Powers**: Manage helpers, full moderation capabilities
- **Visibility**: Always visible to all users

### Server Helper

- **Promotion**: MAKEHELPER command (oper+ only)
- **Demotion**: REMOVEHELPER command (oper+ only)
- **Powers**: Kick, temporary bans with mandatory reason and duration
- **Visibility**: Hidden from normal users, visible only to helpers and above
- **Restrictions**: Cannot exceed max ban duration, cannot modify channel settings

### Undercover Mode

- **Activation**: GOCOVER command
- **Deactivation**: UNCOVER command
- **Powers**: All role powers retained
- **User view**: Appears as normal user
- **Staff view**: Actual role always visible
- **Logging**: All status changes logged
- **Default**: Visible (opt-in to hide)

---

## Open Questions

- [ ] Half-Op: Include, skip, or implement but hide?
- [ ] Max ban duration: Owner only, or Admin too?
- [ ] Helper prefix: Hidden, visible, or none?
- [ ] Multiple owners: Config-based list?

---

*Last updated: November 2025*
