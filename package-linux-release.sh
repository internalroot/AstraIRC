#!/bin/bash
# Package Linux release with proper permissions for GitHub releases

set -e  # Exit on error

# Read version from VERSION file
VERSION=$(cat VERSION | tr -d '\n')
BINARY_PATH="out/build/linux-release/AstraIRC"
PACKAGE_NAME="AstraIRC-v${VERSION}-linux-x64"
PACKAGE_DIR="${PACKAGE_NAME}"

echo "Packaging AstraIRC v${VERSION} for Linux..."

# Check if binary exists
if [ ! -f "$BINARY_PATH" ]; then
    echo "Error: Binary not found at $BINARY_PATH"
    echo "Please build the project first using: cmake --build --preset linux-release"
    exit 1
fi

# Create package directory
rm -rf "$PACKAGE_DIR"
mkdir -p "$PACKAGE_DIR"

# Copy binary and set execute permissions
cp "$BINARY_PATH" "$PACKAGE_DIR/"
chmod +x "$PACKAGE_DIR/AstraIRC"

# Copy documentation
cp README.md "$PACKAGE_DIR/" 2>/dev/null || true
cp VERSION "$PACKAGE_DIR/" 2>/dev/null || true

# Create tarball (preserves permissions)
echo "Creating tarball..."
tar -czf "${PACKAGE_NAME}.tar.gz" "$PACKAGE_DIR"

# Cleanup
rm -rf "$PACKAGE_DIR"

echo "✓ Package created: ${PACKAGE_NAME}.tar.gz"
echo ""
echo "To upload to GitHub releases:"
echo "  1. Go to https://github.com/internalroot/AstraIRC/releases/new"
echo "  2. Upload the file: ${PACKAGE_NAME}.tar.gz"
echo ""
echo "Users can extract with: tar -xzf ${PACKAGE_NAME}.tar.gz"
