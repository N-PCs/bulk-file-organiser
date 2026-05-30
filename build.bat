@echo off
echo.
echo =======================================================
echo   Bulk File Organizer - C++ Win32 GUI Builder
echo =======================================================
echo.

echo [1/3] Compiling resource script using windres...
windres -O coff organizer.rc -o organizer.o
if %errorlevel% neq 0 (
    echo [ERROR] Failed to compile resources.
    pause
    exit /b %errorlevel%
)

echo [2/3] Compiling C++ code with g++ (release mode)...
g++ -std=c++17 -O3 -mwindows -o organizer.exe gui.cpp organizer.o -lshlwapi -lole32 -lcomctl32 -ldwmapi -luuid
if %errorlevel% neq 0 (
    echo [ERROR] Failed to compile C++ source code.
    pause
    exit /b %errorlevel%
)

echo [3/3] Build successful! Created 'organizer.exe'.
echo.
echo You can run the program using 'run.bat' or by double clicking 'organizer.exe'.
echo =======================================================
