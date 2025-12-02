# Windows Packaging Guide

This guide explains how to package AstraIRC for Windows distribution and handle DLL dependencies.

## Understanding DLL Dependencies

When building AstraIRC on Windows with vcpkg, the application depends on several DLLs:

1. **wxWidgets DLLs** - GUI framework libraries (multiple DLLs like wxbase*.dll, wxmsw*.dll)
2. **Visual C++ Runtime** - Microsoft runtime libraries (msvcp140.dll, vcruntime140.dll)
3. **Windows Universal CRT** - Usually already present on Windows 10/11

## Two Build Approaches

### Option 1: Static Linking (RECOMMENDED for Distribution)

Static linking embeds all dependencies into the executable, eliminating DLL requirements.

**Advantages:**
- Single .exe file (no DLLs needed)
- No "missing DLL" errors on user machines
- Easier distribution
- Larger executable size (~10-15 MB vs ~1 MB)

**Build Steps:**

```powershell
# Clean previous builds
Remove-Item -Recurse -Force out, build, vcpkg_installed -ErrorAction SilentlyContinue

# Configure with static linking preset
cmake --preset x64-release-static

# Build
cmake --build --preset x64-release-static

# The executable is now standalone!
# Location: out\build\x64-release-static\Release\AstraIRC.exe
```

**Packaging:**

```powershell
# Just zip the executable - no DLLs needed!
$version = (Get-Content VERSION).Trim()
Compress-Archive -Path "out\build\x64-release-static\Release\AstraIRC.exe" `
                 -DestinationPath "AstraIRC-v$version-windows-x64-static.zip"
```

### Option 2: Dynamic Linking (Smaller executable, requires DLLs)

Dynamic linking creates a smaller executable but requires distributing DLLs.

**Advantages:**
- Smaller executable size (~1 MB)
- Shared libraries can be updated independently

**Disadvantages:**
- Must distribute multiple DLLs
- Users may encounter "missing DLL" errors if not packaged correctly

**Build Steps:**

```powershell
# Clean previous builds
Remove-Item -Recurse -Force out, build, vcpkg_installed -ErrorAction SilentlyContinue

# Configure with dynamic linking preset
cmake --preset x64-release

# Build
cmake --build --preset x64-release
```

**Packaging:**

Use the provided packaging script:

```powershell
.\package-windows-release.ps1
```

This script will:
1. Copy the executable
2. Copy all wxWidgets DLLs from vcpkg
3. Display instructions for Visual C++ runtime DLLs

## Identifying Missing DLLs

If users report missing DLL errors, use these tools to identify dependencies:

### Method 1: Dependencies Tool (Recommended)

Download [Dependencies](https://github.com/lucasg/Dependencies) - a modern dependency walker.

```powershell
# Download and run
Dependencies.exe -imports AstraIRC.exe
```

This shows all DLL dependencies and their locations.

### Method 2: dumpbin (Visual Studio)

```cmd
# From Visual Studio Developer Command Prompt
dumpbin /dependents AstraIRC.exe
```

### Method 3: Test on Clean System

The most reliable method:

1. Set up a fresh Windows 11 VM or clean install
2. Copy your packaged application
3. Run it and note any missing DLL errors
4. Add missing DLLs to your package

## Common Missing DLLs

### wxWidgets DLLs (from vcpkg)

Located in: `vcpkg_installed\x64-windows\bin\`

Typically includes:
- `wxbase32u_*.dll` - Base wxWidgets functionality
- `wxmsw32u_*.dll` - Windows-specific GUI components
- Various other wx*.dll files

**Solution:** The `package-windows-release.ps1` script automatically copies these.

### Visual C++ Runtime DLLs

**Option A: Include redistributable installer (RECOMMENDED)**

Download the official installer:
- x64: https://aka.ms/vs/17/release/vc_redist.x64.exe
- x86: https://aka.ms/vs/17/release/vc_redist.x86.exe

Include it in your package with instructions for users to install it first.

**Option B: Copy runtime DLLs directly**

Locate these files (usually in `C:\Windows\System32\`):
- `msvcp140.dll`
- `vcruntime140.dll`
- `vcruntime140_1.dll` (if it exists)

Copy them to your application directory.

> **Note:** Redistributing these DLLs may have licensing implications. The redistributable installer is the recommended approach.

### Universal CRT

Windows 10/11 includes this by default. For Windows 7/8:
- Include the Visual C++ Redistributable installer (includes UCRT)
- Or use static linking to avoid the dependency

## Recommended Distribution Strategy

### For GitHub Releases

**Approach 1: Static Build (Simplest)**

```powershell
# Build static
cmake --preset x64-release-static
cmake --build --preset x64-release-static

# Package
$version = (Get-Content VERSION).Trim()
$packageName = "AstraIRC-v$version-windows-x64"
New-Item -ItemType Directory -Path $packageName
Copy-Item "out\build\x64-release-static\Release\AstraIRC.exe" $packageName
Compress-Archive -Path $packageName -DestinationPath "$packageName.zip"
```

**Approach 2: Dynamic Build with Installer**

```powershell
# Build dynamic
cmake --preset x64-release
cmake --build --preset x64-release

# Package with script
.\package-windows-release.ps1

# Download VC++ redistributable
# Add it to the package with a README instructing users to install it
```

## Troubleshooting

### Error: "The code execution cannot proceed because MSVCP140.dll was not found"

**Cause:** Missing Visual C++ runtime

**Solutions:**
1. Install Visual C++ Redistributable
2. Or rebuild with static linking: `cmake --preset x64-release-static`

### Error: "The code execution cannot proceed because wxbase32u_*.dll was not found"

**Cause:** Missing wxWidgets DLLs

**Solutions:**
1. Run `.\package-windows-release.ps1` to package with DLLs
2. Or rebuild with static linking to eliminate DLL dependencies

### Error: Application won't start, no error message

**Possible causes:**
- Architecture mismatch (x64 exe on x86 system or vice versa)
- Missing DLLs that aren't being reported

**Solutions:**
1. Verify architecture matches user's system
2. Use Dependencies.exe to check for missing DLLs
3. Try static linking build

### Build fails with "triplet not found" error

**Cause:** vcpkg doesn't have the static triplet built

**Solution:**
vcpkg will automatically build it on first use. Just ensure you have enough disk space and wait for the build to complete (may take 10-15 minutes for first build).

## Quick Reference

| Build Type | Preset | Executable Size | DLLs Required | Best For |
|------------|--------|----------------|---------------|----------|
| Static | `x64-release-static` | ~10-15 MB | None | Distribution |
| Dynamic | `x64-release` | ~1 MB | Many | Development |

## Complete Example: Packaging for Release

```powershell
# 1. Clean workspace
Remove-Item -Recurse -Force out, build, vcpkg_installed -ErrorAction SilentlyContinue

# 2. Build static (recommended)
cmake --preset x64-release-static
cmake --build --preset x64-release-static

# 3. Test the executable
.\out\build\x64-release-static\Release\AstraIRC.exe

# 4. Package
$version = (Get-Content VERSION).Trim()
$packageName = "AstraIRC-v$version-windows-x64"
New-Item -ItemType Directory -Path $packageName
Copy-Item "out\build\x64-release-static\Release\AstraIRC.exe" $packageName
Copy-Item "README.md" $packageName
Compress-Archive -Path $packageName -DestinationPath "$packageName.zip"

# 5. Upload to GitHub releases
Write-Host "Upload $packageName.zip to GitHub releases"
```

## Summary

**For the easiest distribution:** Use static linking (`x64-release-static` preset). This creates a single executable with no DLL dependencies, eliminating all "missing DLL" errors for users.

**For dynamic builds:** Use the `package-windows-release.ps1` script and include the Visual C++ redistributable installer in your package.
