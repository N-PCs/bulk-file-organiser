# urFileManager (urFM)

![Windows](https://img.shields.io/badge/Windows-0078D6?logo=windows&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)
![Fedora](https://img.shields.io/badge/Fedora-294172?logo=fedora&logoColor=white)
![Ubuntu](https://img.shields.io/badge/Ubuntu-E95420?logo=ubuntu&logoColor=white)
![Debian](https://img.shields.io/badge/Debian-A81D33?logo=debian&logoColor=white)
![MIT](https://img.shields.io/badge/License-MIT-green)

A cross-platform file organizer that sorts cluttered folders into categorized subdirectories — Images, Documents, Audio, Video, Archives — in seconds. Native C++ on Windows, Java on Linux.

**Platforms:** Windows (Native C++ Win32 GUI) · Linux (Java Swing GUI with Fedora RPM & Ubuntu DEB support)

## Download

### Via Command Line

Download the release zip / tarball from any terminal:

| Platform | Command |
|----------|---------|
| Windows (PowerShell) | `Invoke-WebRequest -Uri "https://urfilemanager.vercel.app/urfm-windows.zip" -OutFile "urfm-windows.zip"` |
| Windows (CMD) | `curl -L -o urfm-windows.zip "https://urfilemanager.vercel.app/urfm-windows.zip"` |
| Linux (tarball) | `curl -LO "https://urfilemanager.vercel.app/urfm-linux.tar.gz"` |
| Linux (wget) | `wget -O urfm-linux.tar.gz "https://urfilemanager.vercel.app/urfm-linux.tar.gz"` |

### Package Manager Install (Linux)

**Fedora / RHEL:**
```bash
sudo dnf install ./urfm-1.0.0-1.noarch.rpm
```

**Ubuntu / Debian:**
```bash
sudo apt install ./urfm_1.0.0_all.deb
```

## Usage

### Windows

Extract the ZIP anywhere, then use the GUI or CLI from that folder.

**GUI:** Double-click `run.bat` or `ufmgr.exe`, pick a folder, and click **Start Organizing**. Dry-run is checked by default — uncheck to actually move files.

**CLI (`ufmgr-cli.exe`):**

| Command | Description |
|---------|-------------|
| `ufmgr-cli.exe "C:\Downloads"` | Preview organization (safe — dry-run is the default) |
| `ufmgr-cli.exe "C:\Downloads" --no-dry-run` | Actually move files into category folders |
| `ufmgr-cli.exe --revert "C:\Downloads"` | Undo a previous organization of that folder |
| `ufmgr-cli.exe -h` | Show help |

Output is color-coded (`[DRY-RUN]`, `[MOVED]`, `[ERROR]`) and writes `organizer.log` plus a PDF report into the target folder. Both `--flag` and `-flag` styles are accepted.

### Linux

**Tarball:** `tar -xzf urfm-linux.tar.gz && chmod +x urfm && ./urfm`

**RPM/DEB:** Launch **urFileManager** from the app menu, or run `urfm` in a terminal. Requires Java 17+.

| Command | Description |
|---------|-------------|
| `urfm` | Launch the Java Swing GUI |
| `urfm ~/Downloads --dry-run` | Preview moves & generate a preview PDF (no filesystem changes) |
| `urfm ~/Downloads` | Organize the target directory |
| `urfm ~/Downloads --revert` | Undo the last organization on that folder |
| `urfm --version` | Print the app version |
| `urfm --gui` | Force open the GUI even when a directory argument is given |

## Building from Source

### Windows (MinGW-w64)

Requires MinGW-w64 with `windres`.

```cmd
cd desktop-windows
build.bat
```

### Linux — Java Swing

```bash
cd desktop-linux
chmod +x build.sh
./build.sh
# Produces urfm.jar + urfm launcher
```

### Linux — RPM / DEB packaging

```bash
cd desktop-linux
./build-rpm.sh      # Fedora/RHEL (.rpm)
./build-deb.sh      # Ubuntu/Debian (.deb)
```

## Configuration

Sorting rules are defined in `config.json` (placed next to the executable on Windows, or next to `urfm.jar` / its parent / the current directory on Linux). Map category names to lowercase extension lists:

```json
{
  "Images": [".jpg", ".jpeg", ".png", ".gif", ".webp", ".svg"],
  "Documents": [".pdf", ".docx", ".doc", ".txt", ".xlsx", ".pptx"],
  "Audio": [".mp3", ".wav", ".aac", ".flac", ".m4a"],
  "Video": [".mp4", ".mkv", ".mov", ".avi", ".webm"],
  "Archives": [".zip", ".tar.gz", ".rar", ".7z", ".tar"]
}
```

Files with unrecognized extensions land in an `Other/` folder. After editing `config.json`, restart the app or re-run the CLI to reload the rules.

## License

MIT
