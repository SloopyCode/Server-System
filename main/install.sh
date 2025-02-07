#!/bin/bash

set -e  # Beendet das Skript bei Fehlern

REPO_URL="https://github.com/SloopyCode/Server-System/raw/main/src/english/server.c"
SRC_FILE="server.c"
OUTPUT_FILE="server"

echo "Detecting OS..."
OS=$(uname)

install_mac() {
    echo "Installing dependencies for macOS..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    brew install gcc
}

install_linux() {
    echo "Installing dependencies for Linux..."
    if command -v apt &> /dev/null; then
        sudo apt update
        sudo apt install -y gcc make
    else
        echo "Unsupported Linux distribution. Trying Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        brew install gcc
    fi
}

if [[ "$OS" == "Darwin" ]]; then
    install_mac
elif [[ "$OS" == "Linux" ]]; then
    install_linux
else
    echo "Unsupported OS"
    exit 1
fi

echo "Downloading server source code..."
curl -L -o "$SRC_FILE" "$REPO_URL"

echo "Compiling server..."
gcc "$SRC_FILE" -o "$OUTPUT_FILE" -lpthread

echo "Setting execution permissions..."
chmod +x "$OUTPUT_FILE"

echo "Installation completed! Start the server with ./$OUTPUT_FILE"
