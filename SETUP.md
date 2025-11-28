# AstraIRC Setup Guide

This guide will help you set up AstraIRC for development with Visual Studio and other IDEs.

## Prerequisites

### Windows (Visual Studio)

1. **Visual Studio 2019 or later** with:
   - Desktop development with C++
   - C++ CMake tools for Windows
   - vcpkg package manager (optional but recommended)

2. **vcpkg** (recommended for dependency management):
   ```powershell
   # Clone vcpkg
   git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
   cd C:\vcpkg
   .\bootstrap-vcpkg.bat

   # Set environment variable (add to System Environment Variables for persistence)
   $env:VCPKG_ROOT = "C:\vcpkg"
   setx VCPKG_ROOT "C:\vcpkg"

   # Install wxWidgets
   .\vcpkg install wxwidgets[aui]:x64-windows
   # For 32-bit:
   # .\vcpkg install wxwidgets[aui]:x86-windows
   ```

### Linux

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake ninja-build libwxgtk3.0-gtk3-dev

# Fedora
sudo dnf install gcc-c++ cmake ninja-build wxGTK-devel

# Arch Linux
sudo pacman -S base-devel cmake ninja wxwidgets-gtk3
```

### macOS

```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake ninja wxwidgets
```

## Opening in Visual Studio

### Method 1: Using CMakePresets.json (Recommended)

1. Set the `VCPKG_ROOT` environment variable (if not already set):
   - Open System Properties → Environment Variables
   - Add new System variable: `VCPKG_ROOT` = `C:\vcpkg` (or your vcpkg path)
   - **Restart Visual Studio** for the change to take effect

2. Open Visual Studio

3. File → Open → CMake → Select `CMakeLists.txt` from the project root

4. Visual Studio will automatically detect the CMake presets

5. Select your desired preset from the dropdown:
   - `x64-debug` for 64-bit debug builds
   - `x64-release` for 64-bit release builds
   - `x86-debug` for 32-bit debug builds (if needed)

6. Build → Build All (or press F7)

### Method 2: Manual Configuration

If you encounter issues with presets or VCPKG_ROOT:

1. Open Visual Studio

2. File → Open → CMake → Select `CMakeLists.txt`

3. CMake → CMake Settings for AstraIRC

4. Add the vcpkg toolchain file manually:
   - Scroll to "CMake toolchain file"
   - Set to: `C:\vcpkg\scripts\buildsystems\vcpkg.cmake`

5. Save and let CMake reconfigure

## Building from Command Line

### Windows (PowerShell)

```powershell
# Configure
cmake --preset x64-debug

# Build
cmake --build out/build/x64-debug

# Or use the preset
cmake --build --preset x64-debug
```

### Linux/macOS

```bash
# Configure
cmake --preset linux-debug  # or macos-debug

# Build
cmake --build out/build/linux-debug  # or macos-debug

# Or use the preset
cmake --build --preset linux-debug
```

## Troubleshooting

### "wxWidgets not found" in Visual Studio

**Solution 1: Set VCPKG_ROOT environment variable**
1. Verify vcpkg is installed at `C:\vcpkg` (or note your actual path)
2. Add System Environment Variable: `VCPKG_ROOT` = `C:\vcpkg`
3. **Important**: Restart Visual Studio completely (close all instances)
4. Reopen the project

**Solution 2: Check vcpkg integration**
```powershell
# Run vcpkg integration
C:\vcpkg\vcpkg integrate install

# Verify wxWidgets is installed
C:\vcpkg\vcpkg list | Select-String wxwidgets
```

**Solution 3: Manual toolchain file**
1. CMake → CMake Settings for AstraIRC
2. Set "CMake toolchain file" to: `C:\vcpkg\scripts\buildsystems\vcpkg.cmake`
3. Delete CMake cache: CMake → Delete Cache and Reconfigure

### CMake Error about FindPackageHandleStandardArgs

This usually means wxWidgets isn't found. Follow the "wxWidgets not found" solutions above.

### Build works in PowerShell but not Visual Studio

This indicates Visual Studio's CMake environment differs from your PowerShell environment:
- Ensure `VCPKG_ROOT` is set as a **System** environment variable (not just User)
- Restart Visual Studio after setting environment variables
- Check that Visual Studio is using the correct CMake preset

### Different versions of wxWidgets

If you have multiple wxWidgets installations, vcpkg's version will be preferred when using the toolchain file. To use a system installation, either:
- Don't set CMAKE_TOOLCHAIN_FILE
- Remove vcpkg's wxWidgets: `vcpkg remove wxwidgets`

## vcpkg.json Manifest Mode

This project uses vcpkg's manifest mode (`vcpkg.json`), which automatically manages dependencies. When you build with the vcpkg toolchain file, it will:
1. Automatically detect `vcpkg.json`
2. Install required dependencies (wxWidgets) if not present
3. Use the installed dependencies for building

This makes the build process more reproducible and easier for other developers.
