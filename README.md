# AstraIRC

A modern, cross-platform IRC client built with wxWidgets and C++17.

![Platforms](https://img.shields.io/badge/platforms-Windows%20%7C%20macOS%20%7C%20Linux-blue)
![C++](https://img.shields.io/badge/C%2B%2B-17-green)
![License](https://img.shields.io/badge/license-MIT-lightgrey)

## Features

- Multi-channel support with tabbed interface
- Real-time nick list updates
- Timestamps on all messages
- Support for standard IRC commands (`/join`, `/part`, `/nick`, `/msg`, `/me`, etc.)
- Cross-platform (Windows, macOS, Linux)

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

**Note:** The binary is statically linked with the C++ standard library to avoid GLIBCXX issues. However, it still requires a compatible GLIBC version. If you get GLIBC errors, your distribution is older than the build system - build from source instead.

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

If you get `GLIBC_2.XX not found` errors:
```bash
# Your system is older than the build system
# Build from source instead (see Building section below)
sudo apt install build-essential cmake libwxgtk3.2-dev
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/AstraIRC
```

## Building

> **💡 Using Visual Studio?** See [SETUP.md](SETUP.md) for detailed Visual Studio setup instructions and troubleshooting.

### Prerequisites

- CMake 3.20 or later
- C++17 compatible compiler
- wxWidgets 3.2 or later

### Windows (with vcpkg)

```powershell
# Install vcpkg if you haven't
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat

# Install wxWidgets
C:\vcpkg\vcpkg install wxwidgets:x64-windows

# Build AstraIRC
cmake -B build -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
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

## Future Improvements

- [ ] SSL/TLS support
- [ ] Multiple server connections
- [ ] Server list with favorites
- [ ] User preferences (save/load)
- [ ] Nick/channel auto-completion
- [ ] Input history (up/down arrows)
- [ ] Color and formatting support
- [ ] DCC file transfers
- [ ] Notification system

## License

MIT License - see LICENSE file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.
