# Creating Pre-Release Builds

This document explains how to create pre-release builds of AstraIRC for testing purposes.

## Automated Builds

The GitHub Actions workflow automatically builds AstraIRC for Windows, macOS, and Linux platforms.

### Method 1: Tag-Based Release (Recommended)

Create and push a version tag to trigger an automatic build:

```bash
# For a beta release
git tag v1.2.0-beta.1
git push origin v1.2.0-beta.1

# For an alpha release
git tag v1.2.0-alpha.1
git push origin v1.2.0-alpha.1

# For a release candidate
git tag v1.2.0-rc.1
git push origin v1.2.0-rc.1
```

The workflow will:
1. Build binaries for Windows, macOS, and Linux
2. Create a GitHub Release marked as "pre-release"
3. Upload the binaries to the release

### Method 2: Manual Workflow Dispatch

You can also trigger a build manually from the GitHub Actions tab:

1. Go to the **Actions** tab in your GitHub repository
2. Click on **Build Pre-Release** workflow
3. Click **Run workflow**
4. Enter the version tag (e.g., `v1.2.0-beta.1`)
5. Click **Run workflow**

## Accessing Pre-Release Builds

Once the workflow completes:

1. Go to the **Releases** section of your GitHub repository
2. Find the pre-release version (marked with a "Pre-release" badge)
3. Download the appropriate file for your platform:
   - **Windows**: `AstraIRC-Windows.tar.gz`
   - **macOS**: `AstraIRC-macOS.tar.gz`
   - **Linux**: `AstraIRC-Linux.tar.gz`
4. Extract and run

## Distribution

Share the GitHub release URL with testers. They can download the pre-built binaries without needing to compile from source.

Example: `https://github.com/YOUR_USERNAME/AstraIRC/releases/tag/v1.2.0-beta.1`

## Version Naming Convention

Follow semantic versioning with pre-release identifiers:

- `v1.2.0-alpha.1` - Early testing, may be unstable
- `v1.2.0-beta.1` - Feature complete, needs testing
- `v1.2.0-rc.1` - Release candidate, near final
- `v1.2.0` - Stable release

## Troubleshooting

### Build Failures

If a build fails:
1. Check the Actions tab for error logs
2. The workflow builds all three platforms independently, so one failure won't stop the others
3. Fix the issue and create a new tag (e.g., `v1.2.0-beta.2`)

### Platform-Specific Issues

Each platform has its own build configuration:
- **Windows**: Uses vcpkg for wxWidgets
- **macOS**: Uses Homebrew for wxWidgets
- **Linux**: Uses apt for wxWidgets (Ubuntu-based runner)

Check the workflow logs for platform-specific errors.
