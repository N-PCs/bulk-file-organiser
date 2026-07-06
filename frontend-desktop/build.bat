@echo off
echo.
echo =======================================================
echo   urFileManager - Windows C++ Win32 GUI Builder
echo =======================================================
echo.

echo [1/3] Compiling resource script using windres...
windres -O coff ufmgr.rc -o ufmgr.o
if %errorlevel% neq 0 (
    echo [ERROR] Failed to compile resources. Ensure MinGW-w64 is installed.
    pause
    exit /b %errorlevel%
)

echo [2/3] Compiling C++ native GUI...
g++ -std=c++17 -O3 -mwindows -o ufmgr.exe gui_win32.cpp ufmgr.o -lshlwapi -lole32 -lcomctl32 -ldwmapi -luuid -lcomctl32
if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed.
    pause
    exit /b %errorlevel%
)

echo [3/3] Build successful! Created 'ufmgr.exe'.
echo.
echo Run 'ufmgr.exe' to launch the GUI.
echo Use 'ufmgr.bat ^<directory^> [--dry-run]' for CLI mode.
echo =======================================================
pause
