# Build Instructions for AstraIRC

## Architecture Mismatch Fix

If you're seeing linker errors about library machine type conflicts (x86 vs x64), follow these steps:

### For Windows (x64 build)

1. **Clean the build directory and vcpkg cache:**
   ```powershell
   # Remove the build directory
   Remove-Item -Recurse -Force build

   # Remove vcpkg installed packages
   Remove-Item -Recurse -Force vcpkg_installed
   ```

2. **Configure with CMake using the x64 preset:**
   ```powershell
   cmake --preset x64-debug
   # or for release build:
   cmake --preset x64-release
   ```

3. **Build the project:**
   ```powershell
   cmake --build --preset x64-debug
   # or for release build:
   cmake --build --preset x64-release
   ```

### For Windows (x86 build)

If you specifically want to build a 32-bit version:

1. **Clean the build directory and vcpkg cache:**
   ```powershell
   Remove-Item -Recurse -Force build
   Remove-Item -Recurse -Force vcpkg_installed
   ```

2. **Configure with CMake using the x86 preset:**
   ```powershell
   cmake --preset x86-debug
   # or for release build:
   cmake --preset x86-release
   ```

3. **Build the project:**
   ```powershell
   cmake --build --preset x86-debug
   # or for release build:
   cmake --build --preset x86-release
   ```

### For Linux

```bash
# Clean build
rm -rf build vcpkg_installed

# Configure
cmake --preset linux-debug
# or for release:
cmake --preset linux-release

# Build
cmake --build --preset linux-debug
# or for release:
cmake --build --preset linux-release
```

### For macOS

```bash
# Clean build
rm -rf build vcpkg_installed

# Configure
cmake --preset macos-debug
# or for release:
cmake --preset macos-release

# Build
cmake --build --preset macos-debug
# or for release:
cmake --build --preset macos-release
```

## Important Notes

- The CMakePresets.json now includes proper vcpkg triplet configuration
- Each architecture (x64/x86) uses the matching vcpkg triplet
- Always use the CMake presets instead of manual CMake commands
- Make sure the VCPKG_ROOT environment variable is set to your vcpkg installation path

## Troubleshooting

If you still see architecture mismatch errors:

1. Verify VCPKG_ROOT is set correctly:
   ```powershell
   echo $env:VCPKG_ROOT
   ```

2. Ensure you've deleted both `build` and `vcpkg_installed` directories

3. Check that vcpkg has installed the correct architecture:
   ```powershell
   # After configuring, check the vcpkg_installed directory
   # For x64 build, you should see: vcpkg_installed\x64-windows
   # For x86 build, you should see: vcpkg_installed\x86-windows
   dir vcpkg_installed
   ```

4. If using Visual Studio, ensure the platform in Visual Studio matches your preset (x64 vs x86)
