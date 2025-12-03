# AstraIRC

A modern, cross-platform IRC client built with wxWidgets and C++17.

![Platforms](https://img.shields.io/badge/platforms-Windows%20%7C%20macOS%20%7C%20Linux-blue)
![C++](https://img.shields.io/badge/C%2B%2B-17-green)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

## Features

### Core IRC (v1.0.0)
- Multi-channel support with tabbed interface
- Real-time nick list updates
- Support for standard IRC commands (`/join`, `/part`, `/nick`, `/msg`, `/me`, `/quit`, `/raw`)
- Cross-platform (Windows, macOS, Linux)

### Stability & Usability (v1.1.0)
- Auto-reconnect with toggle (enable/disable in preferences)
- Server password support (PASS command)
- Input history (up/down arrows to recall commands)
- Clickable URLs (auto-detected, opens in browser)
- Configurable timestamps (toggle on/off, 12-hour or 24-hour format)
- Window title updates (shows activity/current channel)
- Tab completion (complete nicks and commands with Tab key)

### User Profiles (v1.2.0)
- Multiple server connections (each server in its own tab)
- User Profile Window (double-click nick to view WHOIS info)
- WHOIS integration with detailed user information
- Server role display (Owner, Admin, Operator, Helper - ready for LuminaCore)
- Channel role display (Owner ~, Op @, Half-Op %, Voice +)

## Installation (Prebuilt Binaries)

Download the latest release from the [Releases](https://github.com/internalroot/AstraIRC/releases) page.

### Windows

1. Download `AstraIRC-Windows.tar.gz`
2. Extract the archive
3. Run `AstraIRC.exe`

### macOS

1. Download `AstraIRC-macOS.tar.gz`
2. Extract the archive
3. Run `AstraIRC.app`

### Linux

**Installation:**
1. Download the `.tar.gz` file from [Releases](https://github.com/internalroot/AstraIRC/releases)
2. Extract the archive: `tar -xzf AstraIRC-v*.tar.gz`
3. Run the application: `cd AstraIRC-v*-linux-x64 && ./AstraIRC`

**Note:** The binary is statically linked with the C++ standard library for maximum compatibility across Linux distributions. wxWidgets libraries are still required at runtime.

**Prerequisites (if needed):**

Most modern distributions include the required wxWidgets libraries. If you get missing library errors, install:

```bash
# Debian/Ubuntu
sudo apt-get install libwxgtk3.2-1 libwxgtk3.0-gtk3-0v5

# Fedora
sudo dnf install wxGTK3

# Arch Linux
sudo pacman -S wxwidgets-gtk3
```

**Troubleshooting:**

If you get "unknown file type" when double-clicking:
```bash
chmod +x AstraIRC
./AstraIRC
```

## Building

> **💡 Using Visual Studio?** See [SETUP.md](SETUP.md) for detailed Visual Studio setup instructions and troubleshooting.

### Prerequisites

- CMake 3.20 or later
- C++17 compatible compiler
- wxWidgets 3.2 or later

### Windows (with vcpkg)

> **💡 Using Visual Studio?** See [SETUP.md](SETUP.md) for detailed Visual Studio setup instructions.
> **📦 Packaging for distribution?** See [WINDOWS_PACKAGING.md](WINDOWS_PACKAGING.md) for DLL dependency management.

```powershell
# Install vcpkg if you haven't
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
# Set VCPKG_ROOT environment variable
$env:VCPKG_ROOT = "C:\vcpkg"

# Build AstraIRC (static linking - no DLLs needed)
cmake --preset x64-release-static
cmake --build --preset x64-release-static

# Or build with dynamic linking (requires DLLs)
cmake --preset x64-release
cmake --build --preset x64-release
```

### macOS (with Homebrew)

```bash
# Install wxWidgets
brew install wxwidgets cmake

# Build AstraIRC
cmake -B build
cmake --build build
```

### Linux

**Simple Build (Recommended for most users):**

```bash
# Debian/Ubuntu
sudo apt update
sudo apt install build-essential cmake libwxgtk3.2-dev
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Fedora
sudo dnf install cmake gcc-c++ wxGTK-devel
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Arch Linux
sudo pacman -S cmake wxwidgets-gtk3
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

**Build with vcpkg (for reproducible builds):**

See [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md) for detailed vcpkg setup and CMake preset usage.

The Linux build automatically enables static linking of the C++ standard library (`-static-libgcc -static-libstdc++`) for better compatibility across distributions.

## Running

After building, the executable will be in the `build` directory:

- **Windows**: `build\Release\AstraIRC.exe`
- **macOS**: `build/AstraIRC.app` or `build/AstraIRC`
- **Linux**: `build/AstraIRC`

## Usage

### Connecting

1. Go to **Connection → Quick Connect** (or press `Ctrl+K`)
2. Enter the server address, port, and your desired nickname
3. Click **Connect**

### IRC Commands

| Command | Description |
|---------|-------------|
| `/join #channel` | Join a channel |
| `/part #channel [reason]` | Leave a channel |
| `/msg target message` | Send a private message |
| `/nick newnick` | Change your nickname |
| `/me action` | Send an action message |
| `/quit [reason]` | Disconnect from server |
| `/raw command` | Send raw IRC command |

### Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| `Ctrl+K` | Quick Connect |
| `Ctrl+D` | Disconnect |
| `Ctrl+N` | Change Nick |
| `Enter` | Send message |

## Project Structure

```
AstraIRC/
├── CMakeLists.txt          # Cross-platform build configuration
├── README.md               # This file
└── src/
    ├── main.cpp            # Application entry point
    ├── MainFrame.cpp/h     # Main window and menus
    ├── ServerConnectionPanel.cpp/h  # Server connection UI
    ├── ChannelPage.cpp/h   # Channel tab UI
    └── irc_core.cpp/h      # IRC protocol implementation
```

## Roadmap

See [AstraIRC_ROADMAP.md](AstraIRC_ROADMAP.md) for the complete development roadmap.

### Next Up: Phase 3 (v1.3.0) — Discord-Style UI & Settings
- Server sidebar (replace nested tabs with modern Discord-style layout)
- Server list manager (add/edit/remove servers, auto-join channels)
- Settings persistence (save preferences, server list, window state)
- Complete preferences dialog (notifications, appearance, logging)

### Coming Soon:
- Private message tabs with activity indicators (v1.4.0)
- Channel management window (v1.5.0)
- SSL/TLS support (v1.6.0)
- Themes and polish (v1.7.0)

## License

MIT License - see LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.
