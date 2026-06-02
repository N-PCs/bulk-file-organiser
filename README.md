# Bulk File Organizer (urFiles)

A powerful, native C++ Windows utility that organizes cluttered folders into categorized subdirectories — Images, Documents, Audio, Video, Archives — in seconds. Comes with a GUI, CLI mode, dry-run preview, PDF reports, and 5 UI themes.

## Features

- **Smart Extension Sorting** — Moves loose files into category folders based on customizable rules
- **Dry-Run Preview** — Preview every move before committing (enabled by default)
- **PDF Reports** — Generate detailed organization reports with file names, sizes, and status
- **Full Audit Logging** — Every action recorded in `organizer.log` with timestamps
- **Conflict Resolution** — Duplicates renamed automatically (e.g. `report (1).pdf`)
- **Five UI Themes** — Midnight Dark, Minimalist Light, Nordic Frost, Forest Emerald, Neon Cyberpunk
- **Editable Config** — Add file types or categories via `config.json` — no recompile needed
- **GUI + CLI Modes** — Double-click for the GUI, or use `organizer.bat` for scripting

## Quick Start

1. Download the latest ZIP from the [website](https://urfiles.app) or grab it from `release/`
2. Extract anywhere
3. Double-click `run.bat` to launch the GUI

### CLI Usage

```sh
.\organizer.bat C:\Downloads --dry-run
.\organizer.bat C:\Downloads
```

## Building from Source

### Requirements

- Windows 10+
- Python 3.10+ (for the Python version)
- MinGW-w64 or MSVC (for C++ version)

### Python

```sh
python -m venv venv
venv\Scripts\activate
pip install tqdm
python organizer.py C:\Downloads
```

### C++ (GUI)

```sh
.\build.bat
```

## Website

The marketing website lives in `website/` and is built with React + Vite.

```sh
cd website
npm install
npm run dev
```

## Project Structure

```
├── organizer.py          # Python CLI version
├── organizer.bat         # CLI wrapper
├── run.bat               # GUI launcher
├── gui.cpp               # Native Windows GUI (C++)
├── config.json           # Sorting rules configuration
├── build.bat             # C++ build script
├── scripts/              # Release automation
├── website/              # React marketing site
└── release/              # Packaged binaries
```

## License

MIT
