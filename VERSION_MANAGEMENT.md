# Version Management Guide

This document describes how to manage versions in the AstraIRC project.

## Version Files

The project version is maintained in multiple files for different purposes:

| File | Purpose | Format |
|------|---------|--------|
| **VERSION** | Single source of truth | Plain text: `1.1.0` |
| **CMakeLists.txt** | CMake build system | `project(AstraIRC VERSION 1.1.0 ...)` |
| **vcpkg.json** | Package manifest | `"version": "1.1.0"` |
| **AstraIRC_ROADMAP.md** | Documentation | `v1.1.0` in project table |

## Version Scheme

AstraIRC follows [Semantic Versioning 2.0.0](https://semver.org/):

- **MAJOR.MINOR.PATCH** (e.g., `1.2.0`)
  - **MAJOR**: Incompatible API changes or major rewrites
  - **MINOR**: New features, backward-compatible
  - **PATCH**: Bug fixes, backward-compatible

### Version Timeline (from Roadmap)

- **v1.0.0** - Initial release with basic IRC features ✅
- **v1.1.0** - Stability & usability improvements ✅
- **v1.2.0** - User profiles & accounts 🚧 *Next milestone*
- **v1.3.0** - Channel management
- **v1.4.0** - Messaging & notifications
- **v1.5.0** - Settings & persistence
- **v1.6.0** - Advanced IRC features
- **v1.7.0** - Polish & UX
- **v2.0.0** - Release ready

## How to Update Version

### Option 1: Using the Script (Recommended)

```bash
# Make script executable (first time only)
chmod +x update-version.sh

# Update to new version
./update-version.sh 1.2.0
```

This automatically updates all version files.

### Option 2: Manual Update

If you need to update manually, ensure ALL these files are updated:

1. **VERSION** - Update the version number
2. **CMakeLists.txt** - Update `project(AstraIRC VERSION X.Y.Z ...)`
3. **vcpkg.json** - Update `"version": "X.Y.Z"`
4. **AstraIRC_ROADMAP.md** - Update version in Project Overview table

## Version Update Checklist

When completing a milestone from the roadmap:

- [ ] **Complete all features** listed for the version in `AstraIRC_ROADMAP.md`
- [ ] **Mark features as complete** in roadmap (change `[ ]` to `[x]`)
- [ ] **Run update script**: `./update-version.sh X.Y.Z`
- [ ] **Review changes**: `git diff`
- [ ] **Update roadmap status**:
  - Mark version section as complete with ✅
  - Update "Current State" section if needed
- [ ] **Test the build**:
  ```bash
  cmake -B build
  cmake --build build
  ```
- [ ] **Commit changes**:
  ```bash
  git add -A
  git commit -m "Bump version to vX.Y.Z

  Completed features:
  - Feature 1
  - Feature 2
  - Feature 3"
  ```
- [ ] **Tag the release**:
  ```bash
  git tag -a vX.Y.Z -m "Release vX.Y.Z"
  ```
- [ ] **Push to repository**:
  ```bash
  git push origin <branch-name>
  git push --tags
  ```

## Example: Completing v1.2.0

When you finish all v1.2.0 features:

1. Mark all v1.2.0 tasks as `[x]` in `AstraIRC_ROADMAP.md`
2. Update section header: `#### AstraIRC v1.2.0 — User Profiles ✅`
3. Run: `./update-version.sh 1.2.0`
4. Test the build
5. Commit: `git commit -m "Bump version to v1.2.0"`
6. Tag: `git tag -a v1.2.0 -m "Release v1.2.0 - User Profiles"`
7. Push with tags

## Version Verification

To verify all version files are in sync:

```bash
echo "VERSION file:    $(cat VERSION)"
echo "CMakeLists.txt:  $(grep 'project(AstraIRC VERSION' CMakeLists.txt | grep -oP '\d+\.\d+\.\d+')"
echo "vcpkg.json:      $(grep '"version"' vcpkg.json | grep -oP '\d+\.\d+\.\d+')"
echo "Roadmap:         $(grep 'AstraIRC.*C++.*wxWidgets.*v' AstraIRC_ROADMAP.md | grep -oP 'v\d+\.\d+\.\d+' | head -1)"
```

All four should show the same version.

## Git Tags

Each version should be tagged in git:

```bash
# List all version tags
git tag -l "v*"

# View tag details
git show v1.1.0
```

## Best Practices

1. **Never skip versions** - Follow the roadmap sequence (v1.1.0 → v1.2.0 → v1.3.0)
2. **Update VERSION first** - It's the source of truth
3. **Complete all milestone features** before bumping version
4. **Always tag releases** - Makes it easy to track history
5. **Use descriptive commit messages** when bumping versions
6. **Test before committing** - Ensure the build works with new version

## Current Status

- **Current Version**: v1.1.0
- **Next Milestone**: v1.2.0 - User Profiles
- **Branch**: claude/version-tracking-setup-01NPGTi7Ms6K68VmUvumy4hi

---

*Last updated: November 2025*
