#!/usr/bin/env bash
# Build script for Linux (Fedora, Ubuntu, etc.)
# Requires: g++, FLTK development library
#
# Fedora: sudo dnf install fltk-devel
# Ubuntu: sudo apt install libfltk1.3-dev
# Arch:   sudo pacman -S fltk
set -euo pipefail

echo "=============================================="
echo "  urFileManager (urFM) - Linux Build"
echo "=============================================="
echo ""

# Check for g++
if ! command -v g++ &> /dev/null; then
    echo "[ERROR] g++ not found. Install it with:"
    echo "  Fedora: sudo dnf install gcc-c++"
    echo "  Ubuntu: sudo apt install g++"
    exit 1
fi

# Check for fltk-config
FLTK_FLAGS=""
if command -v fltk-config &> /dev/null; then
    FLTK_FLAGS=$(fltk-config --cxxflags --ldflags)
else
    # Try common FLTK include paths
    if [ -f /usr/include/FL/Fl.H ] || [ -f /usr/local/include/FL/Fl.H ]; then
        FLTK_FLAGS="-lfltk -lfltk_images -lpthread"
    else
        echo "[ERROR] FLTK not found. Install it with:"
        echo "  Fedora: sudo dnf install fltk-devel"
        echo "  Ubuntu: sudo apt install libfltk1.3-dev"
        echo "  Arch:   sudo pacman -S fltk"
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
