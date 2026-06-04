#!/usr/bin/env bash
# Build script for macOS
# Requires: Xcode Command Line Tools, FLTK
#
# Install FLTK:   brew install fltk
set -euo pipefail

echo "=============================================="
echo "  urFileManager (urFM) - macOS Build"
echo "=============================================="
echo ""

if ! command -v g++ &> /dev/null; then
    echo "[ERROR] C++ compiler not found."
    echo "Install Xcode Command Line Tools: xcode-select --install"
    exit 1
fi

FLTK_FLAGS=""
if command -v fltk-config &> /dev/null; then
    FLTK_FLAGS=$(fltk-config --cxxflags --ldflags)
else
    if [ -f /opt/homebrew/include/FL/Fl.H ]; then
        FLTK_FLAGS="-I/opt/homebrew/include -L/opt/homebrew/lib -lfltk -lfltk_images -lpthread"
    elif [ -f /usr/local/include/FL/Fl.H ]; then
        FLTK_FLAGS="-I/usr/local/include -L/usr/local/lib -lfltk -lfltk_images -lpthread"
    else
        echo "[ERROR] FLTK not found. Install it with:"
        echo "  brew install fltk"
        exit 1
    fi
fi

echo "[1/2] Compiling core module..."
g++ -std=c++17 -O2 -c core.cpp -o core.o

echo "[2/2] Compiling GUI + linking..."
g++ -std=c++17 -O2 gui_fltk.cpp core.o -o urfm $FLTK_FLAGS

echo ""
echo "Build successful! Created 'urfm'."
echo "Run: ./urfm <directory> [--dry-run]"
echo "Or:  ./urfm  (launches GUI)"
echo "=============================================="
