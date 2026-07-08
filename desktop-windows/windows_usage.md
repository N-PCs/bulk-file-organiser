# urFileManager — Windows Usage Guide

Two programs share the same engine (`urfm_common.cpp`):

- **`ufmgr.exe`** — GUI launcher (double-click to use)
- **`ufmgr-cli.exe`** — command-line interface (run from terminal)

Both read `config.json` from the same folder.

---

## GUI (`ufmgr.exe`)

1. Double-click `ufmgr.exe` (or `run.bat`).
2. Click **Browse** and pick a folder.
3. **Dry Run** is checked by default — uncheck to actually move files.
4. Click **Start Organizing**.
5. A PDF report is created in the organized folder.
6. Click **Undo Last Organize** to revert.

You can switch themes and view the audit log from the GUI.

---

## CLI (`ufmgr-cli.exe`)

Open a terminal in the `desktop-windows` folder:

| Command | Description |
|---------|-------------|
| `ufmgr-cli.exe <folder>` | Preview organization (safe — no files moved) |
| `ufmgr-cli.exe <folder> --no-dry-run` | Actually move files |
| `ufmgr-cli.exe --revert <folder>` | Undo a previous organization |
| `ufmgr-cli.exe -h` | Show help |

Both `--flag` and `-flag` syntax are accepted.

### Examples

```bat
:: Preview (default)
ufmgr-cli.exe "C:\Users\YourName\Downloads"

:: Execute
ufmgr-cli.exe "C:\Users\YourName\Downloads" --no-dry-run

:: Revert
ufmgr-cli.exe --revert "C:\Users\YourName\Downloads"

:: Other folders
ufmgr-cli.exe "D:\Photos 2024" --no-dry-run
ufmgr-cli.exe --revert "D:\Photos 2024"

:: Help
ufmgr-cli.exe -h
```

---

## Build from Source

```bat
cd desktop-windows
build.bat
```

Requires MinGW-w64 with `windres`. Produces `ufmgr.exe` and `ufmgr-cli.exe`.

---

## Files

| File | Purpose |
|------|---------|
| `ufmgr.exe` | GUI launcher |
| `ufmgr-cli.exe` | CLI binary |
| `cli.cpp` | CLI source code |
| `gui_app.cpp` | GUI source code |
| `urfm_common.cpp` / `.h` | Shared engine (config parsing, PDF report, revert) |
| `config.json` | Sorting rules (edit to customize) |
| `organizer.log` | Audit log (created at runtime) |
| `organization_report.pdf` | Report (created on organize, deleted on revert) |
