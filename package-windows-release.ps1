# PowerShell script to package AstraIRC Windows release with all dependencies
# This script copies the executable and all required DLLs to a distribution folder

param(
    [string]$BuildType = "Release",
    [string]$Architecture = "x64"
)

$ErrorActionPreference = "Stop"

# Determine build preset and paths
$PresetName = "$Architecture-release"
$BuildDir = "out\build\$PresetName"
$VcpkgInstalled = "vcpkg_installed\$Architecture-windows"

# Get version from VERSION file
$Version = (Get-Content "VERSION").Trim()
$PackageName = "AstraIRC-v$Version-windows-$Architecture"
$PackageDir = $PackageName

Write-Host "Packaging AstraIRC $Version for Windows ($Architecture)..." -ForegroundColor Cyan

# Verify build exists
if (-not (Test-Path "$BuildDir\$BuildType\AstraIRC.exe")) {
    Write-Host "ERROR: Build not found at $BuildDir\$BuildType\AstraIRC.exe" -ForegroundColor Red
    Write-Host "Please build the project first using:" -ForegroundColor Yellow
    Write-Host "  cmake --preset $PresetName" -ForegroundColor Yellow
    Write-Host "  cmake --build --preset $PresetName" -ForegroundColor Yellow
    exit 1
}

# Verify vcpkg dependencies exist
if (-not (Test-Path $VcpkgInstalled)) {
    Write-Host "ERROR: vcpkg dependencies not found at $VcpkgInstalled" -ForegroundColor Red
    Write-Host "Please configure and build the project first using CMake presets" -ForegroundColor Yellow
    exit 1
}

# Create package directory
Write-Host "Creating package directory: $PackageDir" -ForegroundColor Green
if (Test-Path $PackageDir) {
    Remove-Item -Recurse -Force $PackageDir
}
New-Item -ItemType Directory -Path $PackageDir | Out-Null

# Copy executable
Write-Host "Copying executable..." -ForegroundColor Green
Copy-Item "$BuildDir\$BuildType\AstraIRC.exe" $PackageDir

# Copy wxWidgets DLLs from vcpkg
Write-Host "Copying wxWidgets DLLs..." -ForegroundColor Green
$BinDir = "$VcpkgInstalled\bin"
if (Test-Path $BinDir) {
    $DllFiles = Get-ChildItem "$BinDir\*.dll"
    foreach ($dll in $DllFiles) {
        Copy-Item $dll.FullName $PackageDir
        Write-Host "  - $($dll.Name)" -ForegroundColor Gray
    }
} else {
    Write-Host "WARNING: No DLL directory found at $BinDir" -ForegroundColor Yellow
    Write-Host "This might be a static build or DLLs are in a different location" -ForegroundColor Yellow
}

# List what we've packaged
Write-Host "`nPackaged files:" -ForegroundColor Cyan
Get-ChildItem $PackageDir | ForEach-Object {
    $size = if ($_.PSIsContainer) { "DIR" } else { "{0:N0} KB" -f ($_.Length / 1KB) }
    Write-Host "  $($_.Name) - $size" -ForegroundColor Gray
}

# Check for Visual C++ Runtime dependencies
Write-Host "`n==================================================" -ForegroundColor Yellow
Write-Host "IMPORTANT: Visual C++ Runtime Requirements" -ForegroundColor Yellow
Write-Host "==================================================" -ForegroundColor Yellow
Write-Host "The packaged application requires the Microsoft Visual C++ Redistributable." -ForegroundColor White
Write-Host ""
Write-Host "You have two options:" -ForegroundColor White
Write-Host ""
Write-Host "Option 1: Include redistributable installer (RECOMMENDED)" -ForegroundColor Cyan
Write-Host "  Download from: https://aka.ms/vs/17/release/vc_redist.$Architecture.exe" -ForegroundColor Gray
Write-Host "  Include it in your package and instruct users to run it first" -ForegroundColor Gray
Write-Host ""
Write-Host "Option 2: Copy runtime DLLs directly" -ForegroundColor Cyan
Write-Host "  Locate these DLLs on your system and copy them to $PackageDir\":" -ForegroundColor Gray
Write-Host "    - msvcp140.dll" -ForegroundColor Gray
Write-Host "    - vcruntime140.dll" -ForegroundColor Gray
Write-Host "    - vcruntime140_1.dll (if exists)" -ForegroundColor Gray
Write-Host "  Typically found in: C:\Windows\System32\" -ForegroundColor Gray
Write-Host ""
Write-Host "To automatically find runtime DLLs, you can use the Dependency Walker tool:" -ForegroundColor Cyan
Write-Host "  https://github.com/lucasg/Dependencies" -ForegroundColor Gray
Write-Host "  Run: Dependencies.exe -imports $PackageDir\AstraIRC.exe" -ForegroundColor Gray
Write-Host "==================================================" -ForegroundColor Yellow

# Create archive
Write-Host "`nCreating archive..." -ForegroundColor Green
$ArchiveName = "$PackageName.zip"
if (Test-Path $ArchiveName) {
    Remove-Item $ArchiveName
}
Compress-Archive -Path $PackageDir -DestinationPath $ArchiveName

Write-Host "`nPackaging complete!" -ForegroundColor Green
Write-Host "Package location: $ArchiveName" -ForegroundColor Cyan
Write-Host "Package directory: $PackageDir\" -ForegroundColor Cyan
Write-Host "`nNext steps:" -ForegroundColor Yellow
Write-Host "1. Test the package on a clean Windows system" -ForegroundColor White
Write-Host "2. If DLLs are missing, add them to $PackageDir\ and re-run this script" -ForegroundColor White
Write-Host "3. Consider including the VC++ redistributable installer" -ForegroundColor White
Write-Host "4. Upload $ArchiveName to GitHub releases" -ForegroundColor White
