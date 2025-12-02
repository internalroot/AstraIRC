# Build Instructions for AstraIRC

## Architecture Mismatch Fix

If you're seeing linker errors about library machine type conflicts (x86 vs x64), follow these steps:

### For Windows (x64 build) - RECOMMENDED

1. **Clean the build output and vcpkg cache:**
   ```powershell
   # Remove the out directory (contains build files)
   Remove-Item -Recurse -Force out -ErrorAction SilentlyContinue

   # Remove the build directory if it exists
   Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue

   # Remove vcpkg installed packages
   Remove-Item -Recurse -Force vcpkg_installed -ErrorAction SilentlyContinue
   ```

2. **Configure with CMake using the x64 preset:**
   ```powershell
   cmake --preset x64-release
   # or for debug build:
   cmake --preset x64-debug
   ```

3. **Build the project:**
   ```powershell
   cmake --build --preset x64-release
   # or for debug build:
   cmake --build --preset x64-debug
   ```

### For Windows (x86 build)

If you specifically want to build a 32-bit version:

1. **Clean the build output and vcpkg cache:**
   ```powershell
   Remove-Item -Recurse -Force out -ErrorAction SilentlyContinue
   Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
   Remove-Item -Recurse -Force vcpkg_installed -ErrorAction SilentlyContinue
   ```

2. **Configure with CMake using the x86 preset:**
   ```powershell
   cmake --preset x86-release
   # or for debug build:
   cmake --preset x86-debug
   ```

3. **Build the project:**
   ```powershell
   cmake --build --preset x86-release
   # or for debug build:
   cmake --build --preset x86-debug
   ```

### For Linux

**Option 1: Simple Build (Recommended)**

Uses system-installed wxWidgets. No vcpkg required.

```bash
# Install dependencies (Debian/Ubuntu example)
sudo apt update
sudo apt install build-essential cmake libwxgtk3.2-dev

# Clean and build
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# Binary location: build/AstraIRC
```

**Option 2: Build with vcpkg (For reproducible builds)**

Requires vcpkg installation and VCPKG_ROOT environment variable.

```bash
# Clean build
rm -rf out build vcpkg_installed

# Configure
cmake --preset linux-release
# or for debug:
cmake --preset linux-debug

# Build
cmake --build --preset linux-release
# or for debug:
cmake --build --preset linux-debug

# Binary location: out/build/linux-release/AstraIRC
```

**Binary Compatibility:**

The Linux build uses static linking for the C++ standard library (`-static-libgcc -static-libstdc++`) to maximize compatibility across different Linux distributions. This avoids GLIBCXX version conflicts when running the binary on older distributions.

**Packaging for Distribution:**

After building, package the binary for GitHub releases to preserve execute permissions:

```bash
./package-linux-release.sh
```

This creates a `.tar.gz` archive with proper permissions. Upload this archive to GitHub releases instead of the raw binary.

**Note:** Raw Linux binaries uploaded to GitHub releases lose their execute permissions. Always package them in a `.tar.gz` archive to preserve permissions, or users will get "unknown file type" errors when trying to run the binary.

**For Users Who Downloaded a Raw Binary:**

If you downloaded a raw binary and get "unknown file type" errors:

```bash
# Make the binary executable
chmod +x AstraIRC

# Then run it
./AstraIRC
```

**Troubleshooting GLIBCXX Errors:**

If you see errors like `GLIBCXX_3.4.XX not found`, you're running a binary built on a newer system. Solutions:
1. Download a binary built with static linking (post-v1.2.0 releases)
2. Build from source on your system
3. Update your system's libstdc++ (may require OS upgrade)

### For macOS

```bash
# Clean build
rm -rf out build vcpkg_installed

# Configure
cmake --preset macos-release
# or for debug:
cmake --preset macos-debug

# Build
cmake --build --preset macos-release
# or for debug:
cmake --build --preset macos-debug
```

## Important Notes

- The CMakePresets.json now includes proper vcpkg triplet configuration
- Each architecture (x64/x86) uses the matching vcpkg triplet
- Architecture is now set using "set" strategy which works from any command prompt
- **Always use the CMake presets** instead of opening .sln files or manual CMake commands
- Make sure the VCPKG_ROOT environment variable is set to your vcpkg installation path
- Build files are placed in the `out/build/` directory (not `build/`)

## Troubleshooting

If you still see architecture mismatch errors:

1. **Delete ALL build artifacts:**
   ```powershell
   # Windows PowerShell
   Remove-Item -Recurse -Force out, build, vcpkg_installed -ErrorAction SilentlyContinue
   ```
   ```bash
   # Linux/macOS
   rm -rf out build vcpkg_installed
   ```

2. **Verify VCPKG_ROOT is set correctly:**
   ```powershell
   echo $env:VCPKG_ROOT
   ```

3. **After configuring, verify vcpkg installed the correct architecture:**
   ```powershell
   # For x64 build, you should see: vcpkg_installed\x64-windows
   # For x86 build, you should see: vcpkg_installed\x86-windows
   dir vcpkg_installed
   ```

4. **Always use CMake presets** - Do NOT:
   - Open the .sln file in Visual Studio and build from there
   - Use `cmake -B build` or other manual CMake commands
   - Build from the Visual Studio IDE unless configured through CMake presets

5. **If the linker still uses wrong architecture:**
   - Make sure you're running the latest version of the fix (git pull)
   - The preset should now use "set" strategy which doesn't require specific command prompts
