# Bulk File Organizer (urFiles)

A cross-platform, native C++ utility that organizes cluttered folders into categorized subdirectories — Images, Documents, Audio, Video, Archives — in seconds. Comes with a polished GUI, CLI mode, dry-run preview, PDF reports, and 5 stunning themes.

**Platforms:** Windows (native Win32 GUI) · Linux (FLTK GUI) · macOS (FLTK GUI)

## Features

- **Smart Extension Sorting** — Moves loose files into category folders based on customizable rules in `config.json`
- **Dry-Run Preview** — Preview every move before committing (enabled by default for safety)
- **PDF Reports** — Generate detailed organization reports with file names, sizes, and status
- **Full Audit Logging** — Every action recorded in `organizer.log` with timestamps
- **Conflict Resolution** — Duplicates renamed automatically (e.g. `report (1).pdf`)
- **Five UI Themes** — Midnight Dark, Minimalist Light, Nordic Frost, Forest Emerald, Neon Cyberpunk
- **Editable Config** — Add file types or categories via `config.json` — no recompile needed
- **GUI + CLI Modes** — Double-click for the GUI, or pass a folder path for scripting

## Quick Start

### Windows

1. Download `urFiles-windows.zip` from the website
2. Extract anywhere
3. Double-click `run.bat` to launch the GUI, or use:

```powershell
.\organizer.exe C:\Downloads --dry-run
```

### Linux (Fedora / Ubuntu / Arch)

```bash
# Install FLTK dependency
sudo apt install libfltk1.3-dev   # Ubuntu
sudo dnf install fltk-devel       # Fedora

# Build and run
chmod +x build.sh
./build.sh
./urfiles ~/Downloads --dry-run
```

### macOS

```bash
# Install FLTK via Homebrew
brew install fltk

# Build and run
chmod +x build_mac.sh
./build_mac.sh
./urfiles ~/Downloads --dry-run
```

## Building from Source

### Windows (native Win32 GUI)

Requires MinGW-w64 with `windres`.

```cmd
build.bat
```

### Linux (FLTK GUI)

```bash
chmod +x build.sh
./build.sh
```

### macOS (FLTK GUI)

```bash
chmod +x build_mac.sh
./build_mac.sh
```

### Python (cross-platform CLI)

Works on all platforms without compilation.

```bash
python -m venv venv
source venv/bin/activate   # Linux/macOS: venv\Scripts\activate on Windows
pip install tqdm
python organizer.py ~/Downloads
```

## Project Structure

```
├── organizer.py              # Python CLI (cross-platform)
├── organizer.bat             # Windows CLI wrapper
├── run.bat                   # Windows GUI launcher
├── gui.cpp                   # GUI redirect (platform dispatch)
├── gui_win32.cpp             # Windows native Win32 GUI (C++)
├── gui_fltk.cpp              # Cross-platform FLTK GUI (C++, Linux/macOS)
├── core.h / core.cpp         # Shared cross-platform logic
├── config.json               # Sorting rules configuration
├── build.bat                 # Windows build script
├── build.sh                  # Linux build script
├── build_mac.sh              # macOS build script
├── scripts/                  # Release automation
├── website/                  # React + Vite marketing site
└── release/                  # Release binaries
```

## License

MIT
