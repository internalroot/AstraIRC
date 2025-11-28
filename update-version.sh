#!/bin/bash
# Version Update Script for AstraIRC
# Usage: ./update-version.sh <new-version>
# Example: ./update-version.sh 1.2.0

set -e

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <new-version>"
    echo "Example: $0 1.2.0"
    exit 1
fi

NEW_VERSION=$1

# Validate version format (basic semantic versioning check)
if ! [[ $NEW_VERSION =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "Error: Version must be in format X.Y.Z (e.g., 1.2.0)"
    exit 1
fi

echo "Updating version to $NEW_VERSION..."

# Update VERSION file
echo "$NEW_VERSION" > VERSION

# Update CMakeLists.txt
sed -i.bak "s/project(AstraIRC VERSION [0-9.]\+ LANGUAGES CXX)/project(AstraIRC VERSION $NEW_VERSION LANGUAGES CXX)/" CMakeLists.txt
rm -f CMakeLists.txt.bak

# Update vcpkg.json
sed -i.bak "s/\"version\": \"[0-9.]\+\"/\"version\": \"$NEW_VERSION\"/" vcpkg.json
rm -f vcpkg.json.bak

# Update roadmap
sed -i.bak "s/| \*\*AstraIRC\*\* | C++ \/ wxWidgets | v[0-9.]\+ |/| **AstraIRC** | C++ \/ wxWidgets | v$NEW_VERSION |/" AstraIRC_ROADMAP.md
rm -f AstraIRC_ROADMAP.md.bak

echo "✓ Version updated to $NEW_VERSION in:"
echo "  - VERSION"
echo "  - CMakeLists.txt"
echo "  - vcpkg.json"
echo "  - AstraIRC_ROADMAP.md"
echo ""
echo "Next steps:"
echo "  1. Review the changes: git diff"
echo "  2. Update roadmap to mark completed features for v$NEW_VERSION"
echo "  3. Commit: git add -A && git commit -m \"Bump version to v$NEW_VERSION\""
echo "  4. Tag: git tag -a v$NEW_VERSION -m \"Release v$NEW_VERSION\""
echo "  5. Push: git push && git push --tags"
