#!/bin/sh
# install.sh - Build and install runc
set -e

echo "==> Installing runc..."

# Detect compiler
CC=""
if command -v gcc >/dev/null 2>&1; then
    CC="gcc"
elif command -v clang >/dev/null 2>&1; then
    CC="clang"
else
    echo "Error: No C compiler found."
    echo "  macOS:  brew install gcc"
    echo "  Linux:  sudo apt install gcc"
    exit 1
fi

echo "    Using compiler: $CC"

# Compile
SRC="src/main.c src/cli.c src/config.c src/compiler.c src/utils.c"
$CC -Wall -Wextra -O2 -o runc $SRC

echo "    Compiled successfully."

# Install binary
INSTALL_DIR="/usr/local/bin"
echo "    Installing to $INSTALL_DIR (may require sudo)..."

if [ -w "$INSTALL_DIR" ]; then
    cp runc "$INSTALL_DIR/runc"
    chmod 755 "$INSTALL_DIR/runc"
    ln -sf "$INSTALL_DIR/runc" "$INSTALL_DIR/rc"
else
    sudo cp runc "$INSTALL_DIR/runc"
    sudo chmod 755 "$INSTALL_DIR/runc"
    sudo ln -sf "$INSTALL_DIR/runc" "$INSTALL_DIR/rc"
fi

# Create default config
CONFIG_DIR="$HOME/.runc"
mkdir -p "$CONFIG_DIR/bin"

if [ ! -f "$CONFIG_DIR/config" ]; then
    cat > "$CONFIG_DIR/config" <<'EOF'
# runc configuration
hidden=true
auto_clean=true
default_compiler=
default_flags=
timer=false
silent=false
EOF
    echo "    Created default config at $CONFIG_DIR/config"
fi

# Cleanup build artifact
rm -f runc

echo ""
echo "==> runc v1.0.0 installed successfully!"
echo ""
echo "    Usage:  runc main.c"
echo "            rc main.c"
echo ""
echo "    Config: runc config show"
echo "    Help:   runc --help"
