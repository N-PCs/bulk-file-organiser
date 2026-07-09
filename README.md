# urFileManager (urFM)

![Windows](https://img.shields.io/badge/Windows-0078D6?logo=windows&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)
![Fedora](https://img.shields.io/badge/Fedora-294172?logo=fedora&logoColor=white)
![Ubuntu](https://img.shields.io/badge/Ubuntu-E95420?logo=ubuntu&logoColor=white)
![Debian](https://img.shields.io/badge/Debian-A81D33?logo=debian&logoColor=white)
![MIT](https://img.shields.io/badge/License-MIT-green)

A cross-platform file organizer that sorts cluttered folders into categorized subdirectories — Images, Documents, Audio, Video, Archives — in seconds. Native C++ on Windows, Java on Linux.

## Features

- **Smart sorting** — moves files into category folders by extension (rules in `config.json`)
- **Dry-run preview** — see every move before committing, safe by default
- **PDF reports** — detailed report with file names, sizes, and status
- **Audit log** — every action recorded in `organizer.log`
- **Conflict resolution** — duplicates renamed automatically (e.g. `report (1).pdf`)
- **Six GUI themes** — Midnight Dark, Minimalist Light, Red Sakura, Forest Emerald, Neon Cyberpunk, Obsidian Volt
- **Editable config** — add or remove file types in `config.json`, no recompile
- **GUI + CLI** — double-click for GUI or pass a folder path from the terminal

## Quick Start

### Windows

```bat
cd desktop-windows
build.bat                              # compile (requires MinGW-w64)
ufmgr.exe                              # launch GUI
ufmgr-cli.exe "C:\Downloads"           # preview (safe)
ufmgr-cli.exe "C:\Downloads" --no-dry-run  # execute
ufmgr-cli.exe --revert "C:\Downloads"  # undo
```

### Linux (Java)

```bash
cd desktop-linux
./build.sh                             # compile (requires JDK 17+)
./urfm                                 # launch GUI
./urfm ~/Downloads --dry-run           # preview
./urfm ~/Downloads                     # execute
./urfm ~/Downloads --revert            # undo
```

Install via RPM or DEB for system-wide access.

### Python (any OS)

```bash
pip install -r requirements.txt
python organizer.py ~/Downloads --dry-run
python organizer.py ~/Downloads
python organizer.py ~/Downloads --revert
```

## Project Structure

```
├── desktop-windows/           # Windows C++ app (Win32 GUI + CLI)
│   ├── cli.cpp                # CLI entry point
│   ├── gui_app.cpp            # Win32 GUI entry point
│   ├── urfm_common.h/.cpp     # Shared engine (config, PDF, revert)
│   ├── config.json            # Category-to-extension mapping
│   ├── build.bat              # Build script (MinGW-w64)
│   ├── ufmgr.exe / ufmgr-cli.exe  # Compiled binaries
│   └── windows_usage.md       # Full usage guide
├── desktop-linux/             # Linux Java Swing GUI
│   ├── src/urfm/              # Java sources
│   ├── build.sh               # Build script
│   └── RELEASE_README.md      # Quick start
├── frontend-web/              # React + Vite marketing site
├── organizer.py               # Python CLI (cross-platform)
├── config.json                # Sorting rules (shared)
└── scripts/                   # Release automation
```

## Building from Source

| Platform | Command | Requirements |
|----------|---------|-------------|
| Windows | `cd desktop-windows && build.bat` | MinGW-w64 with windres |
| Linux (Java) | `cd desktop-linux && ./build.sh` | JDK 17+ |
| Python | `pip install -r requirements.txt` | Python 3.8+ |

## Configuration

Edit `config.json` to customize rules:

```json
{
  "Images": [".jpeg", ".jpg", ".png", ".gif", ".bmp"],
  "Documents": [".pdf", ".docx", ".txt", ".pptx", ".xlsx"],
  "Audio": [".mp3", ".wav", ".aac", ".flac"],
  "Video": [".mp4", ".mov", ".avi", ".mkv", ".webm"],
  "Archives": [".zip", ".rar", ".tar", ".gz", ".7z"]
}
```

Unrecognized extensions go into `Other/`.

## Releases

Release archives and the manifest (`downloads.json`) live in `frontend-web/public/`.

```powershell
.\scripts\package-release.ps1   # builds Windows zip + Linux tarball
```

## License

MIT
