// gui.cpp
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <shellapi.h> // for CommandLineToArgvW
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <atomic>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>
#include <algorithm>
#include <iomanip>


// Linker instructions for standard Win32 libraries
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "shell32.lib")

// Custom application messages
#define WM_APP_LOG      (WM_USER + 1)
#define WM_APP_PROGRESS (WM_USER + 2)
#define WM_APP_STATUS   (WM_USER + 3)
#define WM_APP_DONE     (WM_USER + 4)

// Constants for control IDs
#define IDC_PATH_EDIT       1001
#define IDC_BROWSE_BTN      1002
#define IDC_DRY_RUN_CHECK   1003
#define IDC_EDIT_CONFIG_BTN 1004
#define IDC_VIEW_LOG_BTN    1005
#define IDC_ACTION_BTN      1006
#define IDC_LOG_CONSOLE     1007
#define IDC_THEME_COMBO     1008
#define IDC_VIEW_REPORT_BTN 1009
#define IDC_REPORT_LIST     1010
#define IDC_SAVE_PDF_BTN    1011
#define IDC_CLOSE_REPORT_BTN 1012

// Theme Structure Definition
struct Theme {
    std::wstring name;
    COLORREF windowBg;
    COLORREF cardBg;
    COLORREF editBg;
    COLORREF logBg;
    COLORREF textTitle;
    COLORREF textSub;
    COLORREF textNormal;
    COLORREF accent;
    COLORREF accentHover;
    COLORREF accentPressed;
    COLORREF secondaryNormalBg;
    COLORREF secondaryNormalBorder;
    COLORREF secondaryHoverBg;
    COLORREF secondaryHoverBorder;
    COLORREF secondaryPressedBg;
    COLORREF secondaryPressedBorder;
    COLORREF editBorderNormal;
    COLORREF editBorderHover;
    COLORREF editBorderFocused;
};

// Global Themes Definition List
const std::vector<Theme> g_themes = {
    // 0: Midnight Dark
    {
        L"Midnight Dark",
        RGB(15, 15, 17),        // windowBg
        RGB(24, 24, 27),        // cardBg
        RGB(39, 39, 42),        // editBg
        RGB(24, 24, 27),        // logBg
        RGB(255, 255, 255),     // textTitle
        RGB(161, 161, 170),     // textSub
        RGB(228, 228, 231),     // textNormal
        RGB(99, 102, 241),      // accent
        RGB(79, 70, 229),       // accentHover
        RGB(67, 56, 202),       // accentPressed
        RGB(39, 39, 42),        // secondaryNormalBg
        RGB(39, 39, 42),        // secondaryNormalBorder
        RGB(39, 39, 42),        // secondaryHoverBg
        RGB(63, 63, 70),        // secondaryHoverBorder
        RGB(39, 39, 42),        // secondaryPressedBg
        RGB(82, 82, 91),        // secondaryPressedBorder
        RGB(39, 39, 42),        // editBorderNormal
        RGB(63, 63, 70),        // editBorderHover
        RGB(99, 102, 241)       // editBorderFocused
    },
    // 1: Minimalist Light
    {
        L"Minimalist Light",
        RGB(244, 244, 245),     // windowBg
        RGB(255, 255, 255),     // cardBg
        RGB(244, 244, 245),     // editBg
        RGB(250, 250, 250),     // logBg
        RGB(9, 9, 11),          // textTitle
        RGB(113, 113, 122),     // textSub
        RGB(39, 39, 42),        // textNormal
        RGB(79, 70, 229),       // accent
        RGB(67, 56, 202),       // accentHover
        RGB(55, 48, 163),       // accentPressed
        RGB(244, 244, 245),     // secondaryNormalBg
        RGB(228, 228, 231),     // secondaryNormalBorder
        RGB(228, 228, 231),     // secondaryHoverBg
        RGB(212, 212, 216),     // secondaryHoverBorder
        RGB(212, 212, 216),     // secondaryPressedBg
        RGB(161, 161, 170),     // secondaryPressedBorder
        RGB(228, 228, 231),     // editBorderNormal
        RGB(161, 161, 170),     // editBorderHover
        RGB(79, 70, 229)        // editBorderFocused
    },
    // 2: Nordic Frost
    {
        L"Nordic Frost",
        RGB(46, 52, 64),        // windowBg
        RGB(59, 66, 82),        // cardBg
        RGB(76, 86, 106),       // editBg
        RGB(59, 66, 82),        // logBg
        RGB(236, 239, 244),     // textTitle
        RGB(216, 222, 233),     // textSub
        RGB(229, 233, 240),     // textNormal
        RGB(136, 192, 208),     // accent
        RGB(143, 188, 187),     // accentHover
        RGB(129, 161, 193),     // accentPressed
        RGB(76, 86, 106),       // secondaryNormalBg
        RGB(76, 86, 106),       // secondaryNormalBorder
        RGB(76, 86, 106),       // secondaryHoverBg
        RGB(136, 192, 208),     // secondaryHoverBorder
        RGB(76, 86, 106),       // secondaryPressedBg
        RGB(129, 161, 193),     // secondaryPressedBorder
        RGB(76, 86, 106),       // editBorderNormal
        RGB(129, 161, 193),     // editBorderHover
        RGB(136, 192, 208)      // editBorderFocused
    },
    // 3: Forest Emerald
    {
        L"Forest Emerald",
        RGB(20, 28, 24),        // windowBg
        RGB(28, 40, 34),        // cardBg
        RGB(40, 54, 46),        // editBg
        RGB(28, 40, 34),        // logBg
        RGB(240, 247, 244),     // textTitle
        RGB(163, 186, 175),     // textSub
        RGB(218, 231, 224),     // textNormal
        RGB(16, 185, 129),      // accent
        RGB(5, 150, 105),       // accentHover
        RGB(4, 120, 87),        // accentPressed
        RGB(40, 54, 46),        // secondaryNormalBg
        RGB(40, 54, 46),        // secondaryNormalBorder
        RGB(40, 54, 46),        // secondaryHoverBg
        RGB(16, 185, 129),      // secondaryHoverBorder
        RGB(40, 54, 46),        // secondaryPressedBg
        RGB(5, 150, 105),       // secondaryPressedBorder
        RGB(40, 54, 46),        // editBorderNormal
        RGB(5, 150, 105),       // editBorderHover
        RGB(16, 185, 129)       // editBorderFocused
    },
    // 4: Neon Cyberpunk
    {
        L"Neon Cyberpunk",
        RGB(10, 10, 15),        // windowBg
        RGB(20, 15, 30),        // cardBg
        RGB(35, 25, 50),        // editBg
        RGB(20, 15, 30),        // logBg
        RGB(0, 255, 240),       // textTitle
        RGB(255, 0, 127),       // textSub
        RGB(255, 255, 255),     // textNormal
        RGB(255, 0, 127),       // accent
        RGB(255, 51, 153),      // accentHover
        RGB(204, 0, 102),       // accentPressed
        RGB(35, 25, 50),        // secondaryNormalBg
        RGB(0, 255, 240),       // secondaryNormalBorder
        RGB(35, 25, 50),        // secondaryHoverBg
        RGB(255, 0, 127),       // secondaryHoverBorder
        RGB(35, 25, 50),        // secondaryPressedBg
        RGB(255, 51, 153),      // secondaryPressedBorder
        RGB(35, 25, 50),        // editBorderNormal
        RGB(0, 255, 240),       // editBorderHover
        RGB(255, 0, 127)        // editBorderFocused
    }
};

// Thread structures
struct OrganizeParams {
    HWND hwnd;
    std::wstring sourceDirectory;
    bool dryRun;
    std::map<std::wstring, std::vector<std::wstring>> fileTypeMap;
};

// Moved File Information structure for PDF reporting
struct MovedFileInfo {
    std::wstring fileName;
    std::wstring category;
    uint64_t fileSize;
    std::wstring status;
};

// Global variables
HWND g_hwndMain = NULL;
HWND g_hwndPathEdit = NULL;
HWND g_hwndBrowseBtn = NULL;
HWND g_hwndDryRunCheck = NULL;
HWND g_hwndEditConfigBtn = NULL;
HWND g_hwndViewLogBtn = NULL;
HWND g_hwndActionButton = NULL;
HWND g_hwndLog = NULL;
HWND g_hwndThemeCombo = NULL;
HWND g_hwndViewReportBtn = NULL;

// Active Theme Index
int g_currentThemeIdx = 0;

// Global UI Fonts
HFONT g_hFontTitle = NULL;
HFONT g_hFontSub = NULL;
HFONT g_hFontNormal = NULL;
HFONT g_hFontBold = NULL;
HFONT g_hFontLog = NULL;

// Global Brushes for Theming
HBRUSH g_hWindowBgBrush = NULL;
HBRUSH g_hCardBgBrush = NULL;
HBRUSH g_hEditBgBrush = NULL;
HBRUSH g_hLogBgBrush = NULL;

// Thread states
std::atomic<bool> g_isRunning(false);
std::atomic<bool> g_cancelRequested(false);
std::map<std::wstring, std::vector<std::wstring>> g_fileTypeMap;

// Progress tracking variables
size_t g_processedFiles = 0;
size_t g_totalFiles = 0;
std::wstring g_statusText = L"Ready to organize.";
std::wstring g_lastReportPath;
std::vector<MovedFileInfo> g_lastMovedFiles;
std::wstring g_lastSourceDir;

// Dynamic DPI Scaling Factor (calculated at startup based on screen/monitor)
double g_scaleDPI = 1.0;
int ScaleX(int x) { return (int)(x * g_scaleDPI); }
int ScaleY(int y) { return (int)(y * g_scaleDPI); }

// Forward Declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ButtonSubclass(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
LRESULT CALLBACK EditSubclass(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
std::wstring ReadFileToString(const std::wstring& path);
std::map<std::wstring, std::vector<std::wstring>> ParseConfig(const std::wstring& jsonStr);
std::wstring BrowseForFolder(HWND hwndParent);
void RunOrganize(OrganizeParams* params);
void AppendLogText(const std::wstring& msg, int severity);
int RunCLI(const std::wstring& targetDirStr, bool dryRun);
void UpdateThemeBrushes();
std::string EscapePDFText(const std::wstring& wtext);
std::wstring TruncateText(const std::wstring& text, size_t maxLen);
std::wstring FormatSize(uint64_t bytes);
void GeneratePDFReport(const std::wstring& reportPath, const std::vector<MovedFileInfo>& movedFiles, bool dryRun);
void ShowReportDialog(HWND hParent, const std::vector<MovedFileInfo>& files, const std::wstring& sourceDir);
LRESULT CALLBACK ReportDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


// Helper to query scale DPI factor
int GetScreenDPI() {
    HDC hdc = GetDC(NULL);
    int dpi = 96;
    if (hdc) {
        dpi = GetDeviceCaps(hdc, LOGPIXELSX);
        ReleaseDC(NULL, hdc);
    }
    return dpi;
}

int GetWindowDPI(HWND hwnd) {
    typedef UINT (WINAPI *GetDpiForWindowProc)(HWND);
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        GetDpiForWindowProc pGetDpiForWindow = (GetDpiForWindowProc)GetProcAddress(hUser32, "GetDpiForWindow");
        if (pGetDpiForWindow) {
            return pGetDpiForWindow(hwnd);
        }
    }
    return GetScreenDPI();
}

// Main Entrance Point supporting Dual-Mode CLI/GUI
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Parse command line arguments in Unicode
    LPWSTR* szArglist;
    int nArgs;
    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

    bool runCLI = false;
    std::wstring targetDirectory = L"";
    bool dryRunCLI = false;

    if (szArglist != NULL && nArgs > 1) {
        runCLI = true;
        for (int i = 1; i < nArgs; i++) {
            std::wstring arg = szArglist[i];
            if (arg == L"--dry-run") {
                dryRunCLI = true;
            } else if (targetDirectory.empty() && arg[0] != L'-') {
                targetDirectory = arg;
            }
        }
        LocalFree(szArglist);
    }

    // CLI Mode Execution Path
    if (runCLI) {
        // Attach to parent process console prompt
        if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
            // Allocate new console window if run from a shortcut or non-console client
            AllocConsole();
        }
        // Redirect standard stream handles
        FILE* fDummy;
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONIN$", "r", stdin);
        
        // Prevent std::wcout sync slowing issues
        std::ios_base::sync_with_stdio(false);

        int exitCode = RunCLI(targetDirectory, dryRunCLI);
        return exitCode;
    }

    // GUI Mode Execution Path
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // Initialize Common Controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);

    // DPI awareness initialization dynamically
    int screenDPI = GetScreenDPI();
    g_scaleDPI = (double)screenDPI / 96.0;

    // Register Window Class
    const wchar_t CLASS_NAME[] = L"BulkFileOrganizerGUI";
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassW(&wc);

    // Create Main Window (640x540 window client area, dynamically DPI-scaled)
    RECT rc = { 0, 0, ScaleX(640), ScaleY(540) };
    AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

    g_hwndMain = CreateWindowExW(
        0,
        CLASS_NAME,
        L"Bulk File Organizer",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (g_hwndMain == NULL) {
        return 0;
    }

    // Apply native dark title bar for Windows 10/11
    #ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
    #define DWMWA_USE_IMMERSIVE_DARK_MODE 20
    #endif
    BOOL useDark = TRUE;
    DwmSetWindowAttribute(g_hwndMain, DWMWA_USE_IMMERSIVE_DARK_MODE, &useDark, sizeof(useDark));

    ShowWindow(g_hwndMain, nCmdShow);
    UpdateWindow(g_hwndMain);

    // Win32 Message Loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return 0;
}

// GUI Window Event Procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Retrieve actual monitor DPI (Per-Monitor V2)
            int dpi = GetWindowDPI(hWnd);
            g_scaleDPI = (double)dpi / 96.0;

            // Recreate UI Fonts using exact scaled dimensions
            int fontSizeTitle = ScaleY(24);
            int fontSizeSub = ScaleY(13);
            int fontSizeNormal = ScaleY(14);
            int fontSizeBold = ScaleY(14);
            int fontSizeLog = ScaleY(13);

            g_hFontTitle = CreateFontW(-fontSizeTitle, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            g_hFontSub = CreateFontW(-fontSizeSub, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            g_hFontNormal = CreateFontW(-fontSizeNormal, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            g_hFontBold = CreateFontW(-fontSizeBold, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            g_hFontLog = CreateFontW(-fontSizeLog, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, L"Consolas");

            // Setup custom brushes using initial theme
            UpdateThemeBrushes();

            // 1. Target Directory Path Edit Box (Borderless flat input)
            g_hwndPathEdit = CreateWindowExW(
                0, L"EDIT", L"",
                ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | ES_LEFT,
                ScaleX(43), ScaleY(115), ScaleX(434), ScaleY(18),
                hWnd, (HMENU)IDC_PATH_EDIT, GetModuleHandle(NULL), NULL
            );
            SendMessage(g_hwndPathEdit, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            SetWindowSubclass(g_hwndPathEdit, EditSubclass, 0, 0);

            // 2. Browse Folder Button
            g_hwndBrowseBtn = CreateWindowExW(
                0, L"BUTTON", L"Browse...",
                WS_CHILD | WS_VISIBLE,
                ScaleX(495), ScaleY(110), ScaleX(110), ScaleY(28),
                hWnd, (HMENU)IDC_BROWSE_BTN, GetModuleHandle(NULL), NULL
            );
            SendMessage(g_hwndBrowseBtn, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);
            SetWindowSubclass(g_hwndBrowseBtn, ButtonSubclass, IDC_BROWSE_BTN, 0); // 0 = secondary style

            // 3. Dry Run Checkbox (Standard modern styled checkbox, updated with clear instructional warnings)
            g_hwndDryRunCheck = CreateWindowExW(
                0, L"BUTTON", L"Dry Run (Preview changes - Turn off to execute live file organization)",
                WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                ScaleX(35), ScaleY(155), ScaleX(550), ScaleY(20),
                hWnd, (HMENU)IDC_DRY_RUN_CHECK, GetModuleHandle(NULL), NULL
            );
            SendMessage(g_hwndDryRunCheck, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            SendMessage(g_hwndDryRunCheck, BM_SETCHECK, BST_CHECKED, 0); // Default safely checked

            // 4. Quick Action Button: Edit Config
            g_hwndEditConfigBtn = CreateWindowExW(
                0, L"BUTTON", L"Edit Config",
                WS_CHILD | WS_VISIBLE,
                ScaleX(35), ScaleY(185), ScaleX(110), ScaleY(28),
                hWnd, (HMENU)IDC_EDIT_CONFIG_BTN, GetModuleHandle(NULL), NULL
            );
            SendMessage(g_hwndEditConfigBtn, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);
            SetWindowSubclass(g_hwndEditConfigBtn, ButtonSubclass, IDC_EDIT_CONFIG_BTN, 0);

            // 5. Quick Action Button: View Log
            g_hwndViewLogBtn = CreateWindowExW(
                0, L"BUTTON", L"View Log",
                WS_CHILD | WS_VISIBLE,
                ScaleX(155), ScaleY(185), ScaleX(110), ScaleY(28),
                hWnd, (HMENU)IDC_VIEW_LOG_BTN, GetModuleHandle(NULL), NULL
            );
            SendMessage(g_hwndViewLogBtn, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);
            SetWindowSubclass(g_hwndViewLogBtn, ButtonSubclass, IDC_VIEW_LOG_BTN, 0);

            // 5b. View Report Button (hidden until report generated)
            g_hwndViewReportBtn = CreateWindowExW(
                0, L"BUTTON", L"View Report",
                WS_CHILD | WS_VISIBLE,
                ScaleX(275), ScaleY(185), ScaleX(110), ScaleY(28),
                hWnd, (HMENU)IDC_VIEW_REPORT_BTN, GetModuleHandle(NULL), NULL
            );
            SendMessage(g_hwndViewReportBtn, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);
            SetWindowSubclass(g_hwndViewReportBtn, ButtonSubclass, IDC_VIEW_REPORT_BTN, 0);
            ShowWindow(g_hwndViewReportBtn, SW_HIDE);

            // Theme Selection ComboBox dropdown control
            g_hwndThemeCombo = CreateWindowExW(
                0, L"COMBOBOX", L"",
                WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
                ScaleX(430), ScaleY(183), ScaleX(170), ScaleY(150),
                hWnd, (HMENU)IDC_THEME_COMBO, GetModuleHandle(NULL), NULL
            );
            SendMessage(g_hwndThemeCombo, WM_SETFONT, (WPARAM)g_hFontNormal, TRUE);
            for (const auto& t : g_themes) {
                SendMessage(g_hwndThemeCombo, CB_ADDSTRING, 0, (LPARAM)t.name.c_str());
            }
            SendMessage(g_hwndThemeCombo, CB_SETCURSEL, g_currentThemeIdx, 0);

            // 6. Action Button: Start Organizing
            g_hwndActionButton = CreateWindowExW(
                0, L"BUTTON", L"Start Organizing",
                WS_CHILD | WS_VISIBLE,
                ScaleX(35), ScaleY(235), ScaleX(180), ScaleY(36),
                hWnd, (HMENU)IDC_ACTION_BTN, GetModuleHandle(NULL), NULL
            );
            SendMessage(g_hwndActionButton, WM_SETFONT, (WPARAM)g_hFontBold, TRUE);
            SetWindowSubclass(g_hwndActionButton, ButtonSubclass, IDC_ACTION_BTN, 1); // 1 = primary accent style


            // 7. Live Log Console Box (Read-only multiline terminal)
            g_hwndLog = CreateWindowExW(
                WS_EX_CLIENTEDGE, L"EDIT", L"",
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
                ScaleX(35), ScaleY(325), ScaleX(570), ScaleY(180),
                hWnd, (HMENU)IDC_LOG_CONSOLE, GetModuleHandle(NULL), NULL
            );
            SendMessage(g_hwndLog, WM_SETFONT, (WPARAM)g_hFontLog, TRUE);
            SendMessage(g_hwndLog, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(6, 6));

            // Initialize configurations relative to Executable
            wchar_t exePath[MAX_PATH];
            GetModuleFileNameW(NULL, exePath, MAX_PATH);
            std::filesystem::path configPath(exePath);
            configPath = configPath.parent_path() / "config.json";

            std::wstring jsonStr = ReadFileToString(configPath.wstring());
            g_fileTypeMap = ParseConfig(jsonStr);

            if (g_fileTypeMap.empty()) {
                AppendLogText(L"[ERROR] Failed to load config.json! Please verify that 'config.json' exists in the application directory.", 2);
                EnableWindow(g_hwndActionButton, FALSE);
            } else {
                AppendLogText(L"[INFO] Successfully loaded rules configuration from config.json.", 0);
                AppendLogText(L"[INFO] NOTE: Dry Run mode is ACTIVE by default for safety. Uncheck 'Dry Run' to permanently organize files.", 1);
                AppendLogText(L"[INFO] Select a target folder and click 'Start Organizing' to begin.", 0);
            }
            break;
        }

        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            HWND hCtrl = (HWND)lParam;
            const Theme& t = g_themes[g_currentThemeIdx];
            if (hCtrl == g_hwndLog) {
                SetTextColor(hdc, t.textNormal);
                SetBkColor(hdc, t.logBg);
                return (INT_PTR)g_hLogBgBrush;
            } else if (hCtrl == g_hwndPathEdit) {
                SetTextColor(hdc, t.textTitle);
                SetBkColor(hdc, t.editBg);
                return (INT_PTR)g_hEditBgBrush;
            }
            break;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wParam;
            HWND hCtrl = (HWND)lParam;
            const Theme& t = g_themes[g_currentThemeIdx];
            if (hCtrl == g_hwndDryRunCheck) {
                SetTextColor(hdc, RGB(196, 155, 255));
                SetBkColor(hdc, t.cardBg);
                return (INT_PTR)g_hCardBgBrush;
            } else if (hCtrl == g_hwndLog) {
                SetTextColor(hdc, t.textNormal);
                SetBkColor(hdc, t.logBg);
                return (INT_PTR)g_hLogBgBrush;
            }
            SetTextColor(hdc, t.textNormal);
            SetBkMode(hdc, TRANSPARENT);
            return (INT_PTR)GetStockObject(NULL_BRUSH);
        }

        case WM_CTLCOLORBTN: {
            HDC hdc = (HDC)wParam;
            HWND hCtrl = (HWND)lParam;
            const Theme& t = g_themes[g_currentThemeIdx];
            if (hCtrl == g_hwndDryRunCheck) {
                SetTextColor(hdc, RGB(196, 155, 255));
                SetBkColor(hdc, t.cardBg);
                return (INT_PTR)g_hCardBgBrush;
            }
            return DefWindowProcW(hWnd, uMsg, wParam, lParam);
        }

        case WM_CTLCOLORLISTBOX: {
            HDC hdc = (HDC)wParam;
            const Theme& t = g_themes[g_currentThemeIdx];
            SetTextColor(hdc, t.textNormal);
            SetBkColor(hdc, t.logBg);
            return (INT_PTR)g_hLogBgBrush;
        }


        case WM_COMMAND: {
            int controlId = LOWORD(wParam);
            int notification = HIWORD(wParam);

            if (notification == BN_CLICKED) {
                switch (controlId) {
                    case IDC_BROWSE_BTN: {
                        std::wstring path = BrowseForFolder(hWnd);
                        if (!path.empty()) {
                            SetWindowTextW(g_hwndPathEdit, path.c_str());
                            InvalidateRect(hWnd, NULL, FALSE);
                        }
                        break;
                    }
                    case IDC_VIEW_REPORT_BTN: {
                        if (!g_lastMovedFiles.empty()) {
                            ShowReportDialog(hWnd, g_lastMovedFiles, g_lastSourceDir);
                        }
                        break;
                    }

                    case IDC_EDIT_CONFIG_BTN: {
                        ShellExecuteW(hWnd, L"open", L"config.json", NULL, NULL, SW_SHOW);
                        break;
                    }

                    case IDC_VIEW_LOG_BTN: {
                        ShellExecuteW(hWnd, L"open", L"organizer.log", NULL, NULL, SW_SHOW);
                        break;
                    }

                    case IDC_ACTION_BTN: {
                        if (g_isRunning) {
                            g_cancelRequested = true;
                            SetWindowTextW(g_hwndActionButton, L"Cancelling...");
                            EnableWindow(g_hwndActionButton, FALSE);
                            AppendLogText(L"[INFO] Cancellation requested... wrapping up current file move.", 1);
                        } else {
                            int length = GetWindowTextLengthW(g_hwndPathEdit);
                            if (length == 0) {
                                MessageBoxW(hWnd, L"Please select or input a valid target directory first.", L"Path Required", MB_OK | MB_ICONWARNING);
                                break;
                            }

                            std::wstring pathStr(length + 1, 0);
                            GetWindowTextW(g_hwndPathEdit, &pathStr[0], length + 1);
                            pathStr.resize(length);

                            std::filesystem::path targetDir(pathStr);
                            if (!std::filesystem::is_directory(targetDir)) {
                                MessageBoxW(hWnd, L"The specified path is not a valid directory. Please browse or input a correct folder path.", L"Invalid Directory", MB_OK | MB_ICONERROR);
                                break;
                            }

                            bool dryRun = (SendMessage(g_hwndDryRunCheck, BM_GETCHECK, 0, 0) == BST_CHECKED);

                            g_isRunning = true;
                            g_cancelRequested = false;
                            g_processedFiles = 0;
                            g_totalFiles = 0;
                            g_statusText = L"Scanning directory files...";
                            g_lastReportPath.clear();
                            g_lastMovedFiles.clear();
                            g_lastSourceDir.clear();
                            ShowWindow(g_hwndViewReportBtn, SW_HIDE);
                            InvalidateRect(hWnd, NULL, FALSE);

                            // Lock GUI inputs during processing
                            EnableWindow(g_hwndPathEdit, FALSE);
                            EnableWindow(g_hwndBrowseBtn, FALSE);
                            EnableWindow(g_hwndDryRunCheck, FALSE);
                            EnableWindow(g_hwndEditConfigBtn, FALSE);
                            EnableWindow(g_hwndViewLogBtn, FALSE);

                            SetWindowTextW(g_hwndActionButton, L"Cancel");

                            // Fire background thread
                            OrganizeParams* params = new OrganizeParams();
                            params->hwnd = hWnd;
                            params->sourceDirectory = pathStr;
                            params->dryRun = dryRun;
                            params->fileTypeMap = g_fileTypeMap;

                            std::thread worker(RunOrganize, params);
                            worker.detach();
                        }
                        break;
                    }
                }
            }

            if (controlId == IDC_THEME_COMBO && notification == CBN_SELCHANGE) {
                int idx = (int)SendMessage(g_hwndThemeCombo, CB_GETCURSEL, 0, 0);
                if (idx != CB_ERR && idx != g_currentThemeIdx) {
                    g_currentThemeIdx = idx;
                    UpdateThemeBrushes();
                    InvalidateRect(hWnd, NULL, TRUE);
                    InvalidateRect(g_hwndPathEdit, NULL, TRUE);
                    InvalidateRect(g_hwndBrowseBtn, NULL, TRUE);
                    InvalidateRect(g_hwndDryRunCheck, NULL, TRUE);
                    InvalidateRect(g_hwndEditConfigBtn, NULL, TRUE);
                    InvalidateRect(g_hwndViewLogBtn, NULL, TRUE);
                    InvalidateRect(g_hwndActionButton, NULL, TRUE);
                    InvalidateRect(g_hwndViewReportBtn, NULL, TRUE);
                    InvalidateRect(g_hwndLog, NULL, TRUE);
                    InvalidateRect(g_hwndThemeCombo, NULL, TRUE);
                }
            }
            break;
        }

        case WM_APP_LOG: {
            std::wstring* logMsg = (std::wstring*)lParam;
            int severity = (int)wParam;
            if (logMsg) {
                AppendLogText(*logMsg, severity);
                delete logMsg;
            }
            break;
        }

        case WM_APP_PROGRESS: {
            g_processedFiles = (size_t)wParam;
            g_totalFiles = (size_t)lParam;
            // Force redraw of dynamic Progress bar rect
            RECT rcProg = { ScaleX(35), ScaleY(280), ScaleX(605), ScaleY(290) };
            InvalidateRect(hWnd, &rcProg, FALSE);
            break;
        }

        case WM_APP_STATUS: {
            std::wstring* statusText = (std::wstring*)lParam;
            if (statusText) {
                g_statusText = *statusText;
                delete statusText;
                // Force redraw of Status text bounding rect
                RECT rcStatus = { ScaleX(230), ScaleY(240), ScaleX(600), ScaleY(270) };
                InvalidateRect(hWnd, &rcStatus, FALSE);
            }
            break;
        }

        case WM_APP_DONE: {
            g_isRunning = false;
            g_cancelRequested = false;

            // Re-enable GUI inputs
            EnableWindow(g_hwndPathEdit, TRUE);
            EnableWindow(g_hwndBrowseBtn, TRUE);
            EnableWindow(g_hwndDryRunCheck, TRUE);
            EnableWindow(g_hwndEditConfigBtn, TRUE);
            EnableWindow(g_hwndViewLogBtn, TRUE);

            SetWindowTextW(g_hwndActionButton, L"Start Organizing");
            EnableWindow(g_hwndActionButton, TRUE);

            // Show the View Report button and open the in-app report dialog
            if (!g_lastMovedFiles.empty()) {
                ShowWindow(g_hwndViewReportBtn, SW_SHOW);
                EnableWindow(g_hwndViewReportBtn, TRUE);
                ShowReportDialog(hWnd, g_lastMovedFiles, g_lastSourceDir);
            }

            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rc;
            GetClientRect(hWnd, &rc);
            int width = rc.right - rc.left;
            int height = rc.bottom - rc.top;

            const Theme& t = g_themes[g_currentThemeIdx];

            // Double Buffering
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBM = CreateCompatibleBitmap(hdc, width, height);
            HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);

            // Fill canvas with theme window bg
            FillRect(memDC, &rc, g_hWindowBgBrush);

            // Draw Card Panel frame
            RECT cardRc = { ScaleX(20), ScaleY(75), ScaleX(620), ScaleY(225) };
            HPEN hCardPen = CreatePen(PS_SOLID, 1, t.editBorderNormal);
            HGDIOBJ oldBrush = SelectObject(memDC, g_hCardBgBrush);
            HGDIOBJ oldPen = SelectObject(memDC, hCardPen);

            RoundRect(memDC, cardRc.left, cardRc.top, cardRc.right, cardRc.bottom, ScaleX(12), ScaleY(12));

            SelectObject(memDC, oldBrush);
            SelectObject(memDC, oldPen);
            DeleteObject(hCardPen);

            // Text Labels drawing
            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, t.textTitle);
            SelectObject(memDC, g_hFontTitle);
            TextOutW(memDC, ScaleX(20), ScaleY(15), L"Bulk File Organizer", 19);

            SetTextColor(memDC, t.textSub);
            SelectObject(memDC, g_hFontSub);
            TextOutW(memDC, ScaleX(20), ScaleY(48), L"Blazing fast local directory organization in C++", 48);

            SetTextColor(memDC, t.textNormal);
            SelectObject(memDC, g_hFontBold);
            TextOutW(memDC, ScaleX(35), ScaleY(87), L"Target Directory", 16);

            // Draw Theme label inside the card
            SetTextColor(memDC, t.textNormal);
            SelectObject(memDC, g_hFontBold);
            TextOutW(memDC, ScaleX(395), ScaleY(188), L"Theme:", 6);

            // Draw Custom Dynamic Border for borderless Edit field
            bool isEditHovered = GetPropW(g_hwndPathEdit, L"hovering") ? true : false;
            bool isEditFocused = (GetFocus() == g_hwndPathEdit);

            RECT editBoxRc = { ScaleX(35), ScaleY(110), ScaleX(485), ScaleY(138) };
            HPEN hEditPen;
            if (isEditFocused) {
                hEditPen = CreatePen(PS_SOLID, 1, t.editBorderFocused);
            } else if (isEditHovered) {
                hEditPen = CreatePen(PS_SOLID, 1, t.editBorderHover);
            } else {
                hEditPen = CreatePen(PS_SOLID, 1, t.editBorderNormal);
            }
            
            oldBrush = SelectObject(memDC, g_hEditBgBrush);
            oldPen = SelectObject(memDC, hEditPen);
            
            RoundRect(memDC, editBoxRc.left, editBoxRc.top, editBoxRc.right, editBoxRc.bottom, ScaleX(8), ScaleY(8));
            
            SelectObject(memDC, oldBrush);
            SelectObject(memDC, oldPen);
            DeleteObject(hEditPen);

            // Status label repainting
            SetTextColor(memDC, t.textNormal);
            SelectObject(memDC, g_hFontSub);
            TextOutW(memDC, ScaleX(230), ScaleY(245), g_statusText.c_str(), (int)g_statusText.length());

            // Progress Bar render
            RECT progBg = { ScaleX(35), ScaleY(280), ScaleX(605), ScaleY(290) };
            HBRUSH hProgTrack = CreateSolidBrush(t.editBg);
            FillRect(memDC, &progBg, hProgTrack);
            DeleteObject(hProgTrack);

            if (g_totalFiles > 0) {
                double pct = (double)g_processedFiles / g_totalFiles;
                int fillWidth = (int)((progBg.right - progBg.left) * pct);
                if (fillWidth > 0) {
                    RECT progFill = { progBg.left, progBg.top, progBg.left + fillWidth, progBg.bottom };
                    HBRUSH hProgFill = CreateSolidBrush(t.accent);
                    FillRect(memDC, &progFill, hProgFill);
                    DeleteObject(hProgFill);
                }
            }

            // Console label
            SetTextColor(memDC, t.textNormal);
            SelectObject(memDC, g_hFontBold);
            TextOutW(memDC, ScaleX(35), ScaleY(305), L"Live Log Console", 16);

            // Paint memory frame on screen device context
            BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldBM);
            DeleteObject(memBM);
            DeleteDC(memDC);

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_DESTROY: {
            DeleteObject(g_hFontTitle);
            DeleteObject(g_hFontSub);
            DeleteObject(g_hFontNormal);
            DeleteObject(g_hFontBold);
            DeleteObject(g_hFontLog);

            DeleteObject(g_hWindowBgBrush);
            DeleteObject(g_hCardBgBrush);
            DeleteObject(g_hEditBgBrush);
            DeleteObject(g_hLogBgBrush);

            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// Custom Styled Subclassed Buttons Paint Procedure
LRESULT CALLBACK ButtonSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    bool isPrimary = (dwRefData == 1);
    bool hovering = GetPropW(hWnd, L"hovering") ? true : false;
    bool pressed = GetPropW(hWnd, L"pressed") ? true : false;

    switch (uMsg) {
        case WM_DESTROY:
            RemovePropW(hWnd, L"hovering");
            RemovePropW(hWnd, L"pressed");
            break;

        case WM_MOUSEMOVE: {
            if (!hovering) {
                SetPropW(hWnd, L"hovering", (HANDLE)TRUE);
                TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
                InvalidateRect(hWnd, NULL, FALSE);
            }
            break;
        }
        case WM_MOUSELEAVE: {
            SetPropW(hWnd, L"hovering", (HANDLE)FALSE);
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        case WM_LBUTTONDOWN: {
            SetPropW(hWnd, L"pressed", (HANDLE)TRUE);
            SetCapture(hWnd);
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        case WM_LBUTTONUP: {
            if (pressed) {
                SetPropW(hWnd, L"pressed", (HANDLE)FALSE);
                ReleaseCapture();
                InvalidateRect(hWnd, NULL, FALSE);
                SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(hWnd), BN_CLICKED), (LPARAM)hWnd);
            }
            break;
        }
        case WM_SETFOCUS:
        case WM_KILLFOCUS: {
            InvalidateRect(hWnd, NULL, FALSE);
            break;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rc;
            GetClientRect(hWnd, &rc);
            int w = rc.right - rc.left;
            int h = rc.bottom - rc.top;

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBM = CreateCompatibleBitmap(hdc, w, h);
            HBITMAP oldBM = (HBITMAP)SelectObject(memDC, memBM);

            COLORREF bgCol, borderCol, textCol;
            const Theme& t = g_themes[g_currentThemeIdx];
            if (isPrimary) {
                textCol = RGB(255, 255, 255);
                if (IsWindowEnabled(hWnd) == FALSE) {
                    bgCol = RGB((GetRValue(t.cardBg) + GetRValue(t.accent)) / 2,
                                (GetGValue(t.cardBg) + GetGValue(t.accent)) / 2,
                                (GetBValue(t.cardBg) + GetBValue(t.accent)) / 2);
                    borderCol = bgCol;
                    textCol = RGB((GetRValue(t.textNormal) + GetRValue(t.textSub)) / 2,
                                  (GetGValue(t.textNormal) + GetGValue(t.textSub)) / 2,
                                  (GetBValue(t.textNormal) + GetBValue(t.textSub)) / 2);
                } else if (pressed) {
                    bgCol = t.accentPressed;
                    borderCol = t.accentPressed;
                } else if (hovering) {
                    bgCol = t.accentHover;
                    borderCol = t.accentHover;
                } else {
                    bgCol = t.accent;
                    borderCol = t.accent;
                }
            } else {
                textCol = t.textNormal;
                if (IsWindowEnabled(hWnd) == FALSE) {
                    bgCol = t.cardBg;
                    borderCol = t.editBorderNormal;
                    textCol = t.textSub;
                } else if (pressed) {
                    bgCol = t.secondaryPressedBg;
                    borderCol = t.secondaryPressedBorder;
                } else if (hovering) {
                    bgCol = t.secondaryHoverBg;
                    borderCol = t.secondaryHoverBorder;
                } else {
                    bgCol = t.secondaryNormalBg;
                    borderCol = t.secondaryNormalBorder;
                }
            }

            FillRect(memDC, &rc, g_hCardBgBrush);

            HBRUSH hBg = CreateSolidBrush(bgCol);
            HPEN hPen = CreatePen(PS_SOLID, 1, borderCol);
            HGDIOBJ oldBrush = SelectObject(memDC, hBg);
            HGDIOBJ oldPen = SelectObject(memDC, hPen);

            RoundRect(memDC, 0, 0, w, h, ScaleX(8), ScaleY(8));

            SelectObject(memDC, oldBrush);
            SelectObject(memDC, oldPen);
            DeleteObject(hBg);
            DeleteObject(hPen);

            SetBkMode(memDC, TRANSPARENT);
            SetTextColor(memDC, textCol);

            int len = GetWindowTextLengthW(hWnd);
            std::wstring txt(len + 1, 0);
            GetWindowTextW(hWnd, &txt[0], len + 1);

            HFONT hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
            if (!hFont) hFont = g_hFontBold;
            HGDIOBJ oldFont = SelectObject(memDC, hFont);

            DrawTextW(memDC, txt.c_str(), -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

            SelectObject(memDC, oldFont);

            BitBlt(hdc, 0, 0, w, h, memDC, 0, 0, SRCCOPY);
            SelectObject(memDC, oldBM);
            DeleteObject(memBM);
            DeleteDC(memDC);

            EndPaint(hWnd, &ps);
            return 0;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Edit Subclass Proc to refresh dynamic border scaling on hover/focus
LRESULT CALLBACK EditSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_DESTROY:
            RemovePropW(hWnd, L"hovering");
            break;

        case WM_MOUSEMOVE: {
            BOOL hovering = (BOOL)(GetPropW(hWnd, L"hovering") != NULL);
            if (!hovering) {
                SetPropW(hWnd, L"hovering", (HANDLE)TRUE);
                TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, hWnd, 0 };
                TrackMouseEvent(&tme);
                RECT rc = { ScaleX(35), ScaleY(110), ScaleX(485), ScaleY(138) };
                InvalidateRect(GetParent(hWnd), &rc, FALSE);
            }
            break;
        }
        case WM_MOUSELEAVE: {
            SetPropW(hWnd, L"hovering", (HANDLE)FALSE);
            RECT rc = { ScaleX(35), ScaleY(110), ScaleX(485), ScaleY(138) };
            InvalidateRect(GetParent(hWnd), &rc, FALSE);
            break;
        }
        case WM_SETFOCUS:
        case WM_KILLFOCUS: {
            RECT rc = { ScaleX(35), ScaleY(110), ScaleX(485), ScaleY(138) };
            InvalidateRect(GetParent(hWnd), &rc, FALSE);
            break;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// Update Theme GDI brushes helper
void UpdateThemeBrushes() {
    if (g_hWindowBgBrush) DeleteObject(g_hWindowBgBrush);
    if (g_hCardBgBrush) DeleteObject(g_hCardBgBrush);
    if (g_hEditBgBrush) DeleteObject(g_hEditBgBrush);
    if (g_hLogBgBrush) DeleteObject(g_hLogBgBrush);

    const Theme& t = g_themes[g_currentThemeIdx];
    g_hWindowBgBrush = CreateSolidBrush(t.windowBg);
    g_hCardBgBrush = CreateSolidBrush(t.cardBg);
    g_hEditBgBrush = CreateSolidBrush(t.editBg);
    g_hLogBgBrush = CreateSolidBrush(t.logBg);
}

// Helper to escape special characters for PDF parentheses strings
std::string EscapePDFText(const std::wstring& wtext) {
    std::string s;
    for (wchar_t wc : wtext) {
        if (wc < 128) {
            char c = (char)wc;
            if (c == '(' || c == ')' || c == '\\') {
                s += '\\';
            }
            s += c;
        } else {
            // Replace unicode with basic safe printable char for Helvetica Standard 14 Fonts
            s += '?';
        }
    }
    return s;
}

// Truncate long strings to fit tabular columns
std::wstring TruncateText(const std::wstring& text, size_t maxLen) {
    if (text.length() <= maxLen) return text;
    return text.substr(0, maxLen - 3) + L"...";
}

// Format bytes size into human readable string
std::wstring FormatSize(uint64_t bytes) {
    double size = (double)bytes;
    std::wstring unit = L"B";
    if (size >= 1024) {
        size /= 1024;
        unit = L"KB";
    }
    if (size >= 1024) {
        size /= 1024;
        unit = L"MB";
    }
    if (size >= 1024) {
        size /= 1024;
        unit = L"GB";
    }
    std::wstringstream wss;
    wss << std::fixed << std::setprecision(2) << size << L" " << unit;
    return wss.str();
}

// Generates a fully compliant standard PDF report listing file operations
void GeneratePDFReport(const std::wstring& reportPath, const std::vector<MovedFileInfo>& movedFiles, bool dryRun) {
    if (movedFiles.empty()) return;

    // Calculate total space
    uint64_t totalSize = 0;
    for (const auto& f : movedFiles) {
        if (f.status == L"Moved" || f.status == L"Renamed" || f.status == L"Dry Run Preview") {
            totalSize += f.fileSize;
        }
    }
    std::wstring totalSizeStr = FormatSize(totalSize);

    // Calculate dynamic pagination rules
    const size_t rowsPerPageFirst = 25;
    const size_t rowsPerPageSubsequent = 32;
    size_t totalFiles = movedFiles.size();
    size_t pageCount = 0;
    {
        size_t temp = totalFiles;
        if (temp <= rowsPerPageFirst) {
            pageCount = 1;
        } else {
            pageCount = 1;
            temp -= rowsPerPageFirst;
            pageCount += (temp + rowsPerPageSubsequent - 1) / rowsPerPageSubsequent;
        }
    }

    // Capture standard timestamp
    auto getTimestamp = []() -> std::wstring {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &in_time_t);
        wchar_t buf[100];
        wcsftime(buf, sizeof(buf)/sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", &tm);
        return buf;
    };
    std::wstring timeStr = getTimestamp();

    // Prepare Page Contents streams
    std::vector<std::string> contents;
    size_t fileIdx = 0;

    for (size_t p = 0; p < pageCount; ++p) {
        std::stringstream ss;

        // Faint border on page margins
        ss << "0.9 0.9 0.9 RG 0.5 w 30 30 535 782 re S\n";

        size_t startY = 802;
        if (p == 0) {
            // Document Header Card Box
            ss << "0.95 0.95 0.98 rg 40 700 515 100 re f\n";
            ss << "0.85 0.85 0.9 RG 1 w 40 700 515 100 re S\n";

            // Title Text
            ss << "BT /F2 18 Tf 0.1 0.1 0.25 rg 55 765 Td (" 
               << (dryRun ? "Bulk File Organizer - DRY RUN REPORT" : "Bulk File Organizer - File Transfer Report") 
               << ") Tj ET\n";

            // Metadata Lines
            ss << "BT /F1 9 Tf 0.35 0.35 0.45 rg 55 745 Td (Report generated on: " << EscapePDFText(timeStr) << ") Tj ET\n";
            ss << "BT /F1 9 Tf 0.35 0.35 0.45 rg 55 730 Td (Target Folder: " << EscapePDFText(reportPath) << ") Tj ET\n";

            // Summary Info Strip
            ss << "BT /F2 10 Tf 0.15 0.5 0.15 rg 55 712 Td (Status: " 
               << (dryRun ? "PREVIEW ONLY" : "COMPLETED") 
               << "   |   Total Files: " << totalFiles 
               << "   |   Total Space Organised: " << EscapePDFText(totalSizeStr) << ") Tj ET\n";

            // Table Header Background and Text
            ss << "0.9 0.9 0.92 rg 40 655 515 22 re f\n";
            ss << "0.7 0.7 0.75 RG 1 w 40 655 515 22 re S\n";

            ss << "BT /F2 9 Tf 0.15 0.15 0.2 rg 48 662 Td (File Name) Tj ET\n";
            ss << "BT /F2 9 Tf 0.15 0.15 0.2 rg 268 662 Td (Category) Tj ET\n";
            ss << "BT /F2 9 Tf 0.15 0.15 0.2 rg 368 662 Td (Size) Tj ET\n";
            ss << "BT /F2 9 Tf 0.15 0.15 0.2 rg 458 662 Td (Status) Tj ET\n";

            startY = 655;
        } else {
            // Subsequent Page Table Header
            ss << "0.9 0.9 0.92 rg 40 770 515 22 re f\n";
            ss << "0.7 0.7 0.75 RG 1 w 40 770 515 22 re S\n";

            ss << "BT /F2 9 Tf 0.15 0.15 0.2 rg 48 777 Td (File Name) Tj ET\n";
            ss << "BT /F2 9 Tf 0.15 0.15 0.2 rg 268 777 Td (Category) Tj ET\n";
            ss << "BT /F2 9 Tf 0.15 0.15 0.2 rg 368 777 Td (Size) Tj ET\n";
            ss << "BT /F2 9 Tf 0.15 0.15 0.2 rg 458 777 Td (Status) Tj ET\n";

            startY = 770;
        }

        size_t itemsOnThisPage = (p == 0) ? rowsPerPageFirst : rowsPerPageSubsequent;
        size_t rowsDrawn = 0;
        for (size_t rowIdx = 0; rowIdx < itemsOnThisPage && fileIdx < totalFiles; ++rowIdx, ++fileIdx) {
            const auto& file = movedFiles[fileIdx];
            size_t rowY = startY - 20 - (rowIdx * 20);
            rowsDrawn++;

            // Alternate Row Zebra Background
            if (rowIdx % 2 == 1) {
                ss << "0.97 0.97 0.99 rg 40 " << rowY << " 515 20 re f\n";
            }
            
            // Horizontal row line divider
            ss << "0.9 0.9 0.9 RG 0.5 w 40 " << rowY << " m 555 " << rowY << " l S\n";

            size_t textY = rowY + 6;

            // Render columns
            ss << "BT /F1 9 Tf 0.15 0.15 0.15 rg 48 " << textY << " Td (" << EscapePDFText(TruncateText(file.fileName, 38)) << ") Tj ET\n";
            ss << "BT /F1 9 Tf 0.15 0.15 0.15 rg 268 " << textY << " Td (" << EscapePDFText(TruncateText(file.category, 15)) << ") Tj ET\n";
            ss << "BT /F1 9 Tf 0.15 0.15 0.15 rg 368 " << textY << " Td (" << EscapePDFText(FormatSize(file.fileSize)) << ") Tj ET\n";
            
            // Status coloring (green = moved, orange = renamed, red = error, blue = dry run)
            if (file.status == L"Moved") {
                ss << "BT /F2 9 Tf 0.1 0.5 0.1 rg 458 " << textY << " Td (" << EscapePDFText(file.status) << ") Tj ET\n";
            } else if (file.status == L"Renamed" || file.status.find(L"Conflict") != std::wstring::npos) {
                ss << "BT /F2 9 Tf 0.8 0.4 0.0 rg 458 " << textY << " Td (Renamed) Tj ET\n";
            } else if (file.status.find(L"Error") != std::wstring::npos || file.status.find(L"Could not") != std::wstring::npos || file.status.find(L"Failed") != std::wstring::npos) {
                ss << "BT /F2 9 Tf 0.8 0.1 0.1 rg 458 " << textY << " Td (Error) Tj ET\n";
            } else {
                ss << "BT /F1 9 Tf 0.2 0.2 0.7 rg 458 " << textY << " Td (" << EscapePDFText(file.status) << ") Tj ET\n";
            }
        }

        // Draw Vertical Grid Columns
        size_t finalY = startY - 20 - (rowsDrawn * 20);
        ss << "0.85 0.85 0.85 RG 0.5 w 40 " << finalY << " m 40 " << startY << " l S\n";
        ss << "0.85 0.85 0.85 RG 0.5 w 260 " << finalY << " m 260 " << startY << " l S\n";
        ss << "0.85 0.85 0.85 RG 0.5 w 360 " << finalY << " m 360 " << startY << " l S\n";
        ss << "0.85 0.85 0.85 RG 0.5 w 450 " << finalY << " m 450 " << startY << " l S\n";
        ss << "0.85 0.85 0.85 RG 0.5 w 555 " << finalY << " m 555 " << startY << " l S\n";

        // Table Bottom border
        ss << "0.7 0.7 0.75 RG 1 w 40 " << finalY << " m 555 " << finalY << " l S\n";

        // Footer Text (Page X of Y)
        ss << "BT /F1 8 Tf 0.5 0.5 0.5 rg 270 45 Td (Page " << (p + 1) << " of " << pageCount << ") Tj ET\n";

        contents.push_back(ss.str());
    }

    // Construct the final PDF catalog mapping structure
    std::vector<std::string> pdfObjects;

    // Obj 1: Document Catalog
    pdfObjects.push_back("<< /Type /Catalog /Pages 2 0 R >>");

    // Obj 2: Pages Catalog
    std::stringstream pagesSs;
    pagesSs << "<< /Type /Pages /Kids [";
    for (size_t p = 0; p < pageCount; ++p) {
        pagesSs << (5 + p) << " 0 R ";
    }
    pagesSs << "] /Count " << pageCount << " >>";
    pdfObjects.push_back(pagesSs.str());

    // Obj 3: F1 Font (Helvetica)
    pdfObjects.push_back("<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>");

    // Obj 4: F2 Font (Helvetica-Bold)
    pdfObjects.push_back("<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica-Bold >>");

    // Obj 5 to 5+pageCount-1: Individual Page Metadata Definitions
    for (size_t p = 0; p < pageCount; ++p) {
        std::stringstream pageSs;
        pageSs << "<< /Type /Page /Parent 2 0 R /MediaBox [0 0 595 842] /Resources << /Font << /F1 3 0 R /F2 4 0 R >> >> /Contents "
               << (5 + pageCount + p) << " 0 R >>";
        pdfObjects.push_back(pageSs.str());
    }

    // Obj 5+pageCount to 5+2*pageCount-1: Content stream objects
    for (size_t p = 0; p < pageCount; ++p) {
        const std::string& content_str = contents[p];
        std::stringstream contentSs;
        contentSs << "<< /Length " << content_str.length() << " >>\n"
                  << "stream\n"
                  << content_str
                  << "endstream";
        pdfObjects.push_back(contentSs.str());
    }

    // Write binary stream
    std::ofstream out(std::filesystem::path(reportPath), std::ios::binary);
    if (!out.is_open()) return;

    out << "%PDF-1.4\n";

    std::vector<size_t> obj_offsets;
    size_t current_offset = 9; // Length of "%PDF-1.4\n"

    for (size_t i = 0; i < pdfObjects.size(); ++i) {
        obj_offsets.push_back(current_offset);
        std::stringstream ss;
        ss << (i + 1) << " 0 obj\n" << pdfObjects[i] << "\nendobj\n";
        std::string obj_str = ss.str();
        out.write(obj_str.c_str(), obj_str.length());
        current_offset += obj_str.length();
    }

    size_t xref_offset = current_offset;
    out << "xref\n";
    out << "0 " << (pdfObjects.size() + 1) << "\n";

    char buf[64];
    snprintf(buf, sizeof(buf), "%010d 65535 f\r\n", 0);
    out.write(buf, strlen(buf));

    for (size_t i = 0; i < pdfObjects.size(); ++i) {
        snprintf(buf, sizeof(buf), "%010zu 00000 n\r\n", obj_offsets[i]);
        out.write(buf, strlen(buf));
    }

    out << "trailer\n";
    out << "<< /Size " << (pdfObjects.size() + 1) << " /Root 1 0 R >>\n";
    out << "startxref\n";
    out << xref_offset << "\n";
    out << "%%EOF\n";
    out.close();
}

// In-app Report Dialog Window
void ShowReportDialog(HWND hParent, const std::vector<MovedFileInfo>& files, const std::wstring& sourceDir) {
    const wchar_t CLASS_NAME[] = L"ReportDialogClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = ReportDialogProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassW(&wc);

    int dpi = GetWindowDPI(hParent);
    double scale = (double)dpi / 96.0;
    int w = (int)(640 * scale);
    int h = (int)(480 * scale);

    HWND hDlg = CreateWindowExW(
        0, CLASS_NAME, L"Organization Report",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, w, h,
        hParent, NULL, GetModuleHandle(NULL),
        NULL
    );

    if (hDlg) {
        // Store data in window properties for access in WndProc
        SetPropW(hDlg, L"files", (HANDLE)&g_lastMovedFiles);
        SetPropW(hDlg, L"sourceDir", (HANDLE)&g_lastSourceDir);
        ShowWindow(hDlg, SW_SHOW);
    }
}

LRESULT CALLBACK ReportDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            int dpi = GetWindowDPI(hWnd);
            double scale = (double)dpi / 96.0;

            // Create summary label
            HWND hSummary = CreateWindowExW(0, L"STATIC", L"",
                WS_CHILD | WS_VISIBLE,
                (int)(15 * scale), (int)(10 * scale),
                (int)(600 * scale), (int)(80 * scale),
                hWnd, NULL, GetModuleHandle(NULL), NULL);

            // Create ListView for file details
            HWND hList = CreateWindowExW(
                WS_EX_CLIENTEDGE, WC_LISTVIEWW, L"",
                WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
                (int)(15 * scale), (int)(100 * scale),
                (int)(600 * scale), (int)(300 * scale),
                hWnd, (HMENU)IDC_REPORT_LIST, GetModuleHandle(NULL), NULL
            );

            // Set list view font
            HFONT hFont = CreateFontW(-(int)(13 * scale), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
            SendMessage(hList, WM_SETFONT, (WPARAM)hFont, TRUE);
            SetPropW(hWnd, L"listFont", (HANDLE)hFont);

            // Add columns
            LVCOLUMNW lvc = {0};
            lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

            lvc.cx = (int)(200 * scale);
            lvc.pszText = const_cast<LPWSTR>(L"File Name");
            ListView_InsertColumn(hList, 0, &lvc);

            lvc.cx = (int)(100 * scale);
            lvc.pszText = const_cast<LPWSTR>(L"Category");
            ListView_InsertColumn(hList, 1, &lvc);

            lvc.cx = (int)(90 * scale);
            lvc.pszText = const_cast<LPWSTR>(L"Size");
            ListView_InsertColumn(hList, 2, &lvc);

            lvc.cx = (int)(120 * scale);
            lvc.pszText = const_cast<LPWSTR>(L"Status");
            ListView_InsertColumn(hList, 3, &lvc);

            // Add file data
            std::vector<MovedFileInfo>* files = (std::vector<MovedFileInfo>*)GetPropW(hWnd, L"files");
            if (files) {
                for (size_t i = 0; i < files->size(); ++i) {
                    const auto& f = (*files)[i];

                    LVITEMW lvi = {0};
                    lvi.mask = LVIF_TEXT;
                    lvi.iItem = (int)i;
                    lvi.iSubItem = 0;
                    lvi.pszText = const_cast<LPWSTR>(f.fileName.c_str());
                    ListView_InsertItem(hList, &lvi);

                    ListView_SetItemText(hList, (int)i, 1, const_cast<LPWSTR>(f.category.c_str()));

                    std::wstring sizeStr = FormatSize(f.fileSize);
                    ListView_SetItemText(hList, (int)i, 2, const_cast<LPWSTR>(sizeStr.c_str()));

                    ListView_SetItemText(hList, (int)i, 3, const_cast<LPWSTR>(f.status.c_str()));
                }
            }

            // Summary at top
            std::wstring* sourceDir = (std::wstring*)GetPropW(hWnd, L"sourceDir");
            if (sourceDir) {
                size_t total = files ? files->size() : 0;
                size_t success = 0, errors = 0, dryRun = 0;
                if (files) {
                    for (const auto& f : *files) {
                        if (f.status == L"Dry Run Preview") dryRun++;
                        else if (f.status.find(L"Error") != std::wstring::npos) errors++;
                        else success++;
                    }
                }
                std::wstring summary = L"Directory: " + *sourceDir + L"\r\n"
                    + L"Total: " + std::to_wstring(total)
                    + L" | Success: " + std::to_wstring(success)
                    + L" | Errors: " + std::to_wstring(errors)
                    + L" | Dry Run: " + std::to_wstring(dryRun);
                SetWindowTextW(hSummary, summary.c_str());
            }

            // "Save as PDF" button
            CreateWindowExW(0, L"BUTTON", L"Save as PDF...",
                WS_CHILD | WS_VISIBLE,
                (int)(15 * scale), (int)(415 * scale),
                (int)(120 * scale), (int)(30 * scale),
                hWnd, (HMENU)IDC_SAVE_PDF_BTN, GetModuleHandle(NULL), NULL);

            // Close button
            CreateWindowExW(0, L"BUTTON", L"Close",
                WS_CHILD | WS_VISIBLE,
                (int)(145 * scale), (int)(415 * scale),
                (int)(90 * scale), (int)(30 * scale),
                hWnd, (HMENU)IDC_CLOSE_REPORT_BTN, GetModuleHandle(NULL), NULL);

            break;
        }

        case WM_COMMAND: {
            int id = LOWORD(wParam);
            if (id == IDC_CLOSE_REPORT_BTN) {
                DestroyWindow(hWnd);
            } else if (id == IDC_SAVE_PDF_BTN) {
                std::wstring* sourceDir = (std::wstring*)GetPropW(hWnd, L"sourceDir");
                std::vector<MovedFileInfo>* files = (std::vector<MovedFileInfo>*)GetPropW(hWnd, L"files");
                if (sourceDir && files && !files->empty()) {
                    wchar_t savePath[MAX_PATH] = {0};
                    OPENFILENAMEW ofn = {0};
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hWnd;
                    ofn.lpstrFilter = L"PDF Files\0*.pdf\0All Files\0*.*\0";
                    ofn.lpstrFile = savePath;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrDefExt = L"pdf";
                    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
                    wcscpy_s(savePath, L"organization_report.pdf");

                    if (GetSaveFileNameW(&ofn)) {
                        try {
                            GeneratePDFReport(savePath, *files, false);
                            MessageBoxW(hWnd, L"PDF report saved successfully.", L"Export Complete", MB_OK | MB_ICONINFORMATION);
                        } catch (const std::exception& e) {
                            std::string err = e.what();
                            std::wstring werr(err.begin(), err.end());
                            MessageBoxW(hWnd, (L"Failed to save PDF: " + werr).c_str(), L"Error", MB_OK | MB_ICONERROR);
                        }
                    }
                }
            }
            break;
        }

        case WM_CLOSE: {
            DestroyWindow(hWnd);
            break;
        }

        case WM_DESTROY: {
            HFONT hFont = (HFONT)RemovePropW(hWnd, L"listFont");
            if (hFont) DeleteObject(hFont);
            RemovePropW(hWnd, L"files");
            RemovePropW(hWnd, L"sourceDir");
            break;
        }
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

// Read wide-char UTF-8 files helper
std::wstring ReadFileToString(const std::wstring& path) {
    std::ifstream fs(std::filesystem::path(path), std::ios::binary);
    if (!fs.is_open()) return L"";
    std::string bytes((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
    
    if (bytes.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &bytes[0], (int)bytes.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &bytes[0], (int)bytes.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

// Custom Config JSON scanner
std::map<std::wstring, std::vector<std::wstring>> ParseConfig(const std::wstring& jsonStr) {
    std::map<std::wstring, std::vector<std::wstring>> config;
    size_t i = 0;
    size_t len = jsonStr.length();
    
    auto skipWhitespace = [&]() {
        while (i < len && (jsonStr[i] == L' ' || jsonStr[i] == L'\t' || jsonStr[i] == L'\r' || jsonStr[i] == L'\n')) {
            i++;
        }
    };
    
    auto parseString = [&]() -> std::wstring {
        std::wstring s = L"";
        if (i < len && jsonStr[i] == L'"') {
            i++;
            while (i < len && jsonStr[i] != L'"') {
                if (jsonStr[i] == L'\\') {
                    i++;
                    if (i < len) s += jsonStr[i];
                } else {
                    s += jsonStr[i];
                }
                i++;
            }
            if (i < len) i++;
        }
        return s;
    };
    
    skipWhitespace();
    if (i < len && jsonStr[i] == L'{') {
        i++;
        while (i < len) {
            skipWhitespace();
            if (jsonStr[i] == L'}') {
                i++;
                break;
            }
            if (jsonStr[i] == L'"') {
                std::wstring category = parseString();
                skipWhitespace();
                if (i < len && jsonStr[i] == L':') {
                    i++;
                    skipWhitespace();
                    if (i < len && jsonStr[i] == L'[') {
                        i++;
                        std::vector<std::wstring> extensions;
                        while (i < len) {
                            skipWhitespace();
                            if (jsonStr[i] == L']') {
                                i++;
                                break;
                            }
                            if (jsonStr[i] == L'"') {
                                std::wstring ext = parseString();
                                for (auto& c : ext) c = std::tolower(c);
                                extensions.push_back(ext);
                            } else if (jsonStr[i] == L',') {
                                i++;
                            } else {
                                i++;
                            }
                        }
                        config[category] = extensions;
                    }
                }
            } else if (jsonStr[i] == L',') {
                i++;
            } else {
                i++;
            }
        }
    }
    return config;
}

// Modern Native Browse Folder Dialog
std::wstring BrowseForFolder(HWND hwndParent) {
    std::wstring folderPath = L"";
    IFileOpenDialog* pFileOpen = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (SUCCEEDED(hr)) {
        DWORD dwOptions;
        if (SUCCEEDED(pFileOpen->GetOptions(&dwOptions))) {
            pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
        }
        if (SUCCEEDED(pFileOpen->Show(hwndParent))) {
            IShellItem* pItem = nullptr;
            if (SUCCEEDED(pFileOpen->GetResult(&pItem))) {
                PWSTR pszFilePath = nullptr;
                if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath))) {
                    folderPath = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileOpen->Release();
    }
    return folderPath;
}

// Background Thread: GUI Organizing Worker
void RunOrganize(OrganizeParams* params) {
    HWND hwnd = params->hwnd;
    std::wstring sourcePathStr = params->sourceDirectory;
    bool dryRun = params->dryRun;
    auto fileTypeMap = params->fileTypeMap;

    std::filesystem::path sourcePath(sourcePathStr);
    
    // Log startup states with instructions
    std::wstring* logStart = new std::wstring(L"[INFO] Starting file organization in: " + sourcePathStr);
    PostMessage(hwnd, WM_APP_LOG, 0, (LPARAM)logStart);

    if (dryRun) {
        std::wstring* logDry = new std::wstring(L"[INFO] --- DRY RUN MODE ENABLED: No files will be moved. ---");
        PostMessage(hwnd, WM_APP_LOG, 1, (LPARAM)logDry);
        std::wstring* logInstruction = new std::wstring(L"[INFO] NOTE: Turn off the 'Dry Run' checkbox to permanently execute actual file moves.");
        PostMessage(hwnd, WM_APP_LOG, 0, (LPARAM)logInstruction);
    } else {
        std::wstring* logLive = new std::wstring(L"[WARNING] --- LIVE RUN MODE ACTIVE: File system changes will be written! ---");
        PostMessage(hwnd, WM_APP_LOG, 1, (LPARAM)logLive);
    }

    std::vector<std::filesystem::path> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(sourcePath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path());
            }
        }
    } catch (const std::exception& e) {
        std::string errStr = e.what();
        std::wstring wErr(errStr.begin(), errStr.end());
        std::wstring* logErr = new std::wstring(L"[ERROR] Failed to scan target directory: " + wErr);
        PostMessage(hwnd, WM_APP_LOG, 2, (LPARAM)logErr);
        PostMessage(hwnd, WM_APP_DONE, 0, 0);
        delete params;
        return;
    }

    size_t total = files.size();
    PostMessage(hwnd, WM_APP_PROGRESS, 0, (LPARAM)total);

    if (total == 0) {
        std::wstring* logNoFiles = new std::wstring(L"[INFO] No files found to organize in the target folder.");
        PostMessage(hwnd, WM_APP_LOG, 0, (LPARAM)logNoFiles);
        std::wstring* statusMsg = new std::wstring(L"Done. No files found.");
        PostMessage(hwnd, WM_APP_STATUS, 0, (LPARAM)statusMsg);
        PostMessage(hwnd, WM_APP_DONE, 0, 0);
        delete params;
        return;
    }

    std::wofstream logFile(L"organizer.log", std::ios::app);
    auto getTimestamp = []() -> std::wstring {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &in_time_t);
        wchar_t buf[100];
        wcsftime(buf, sizeof(buf)/sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", &tm);
        return buf;
    };

    size_t processed = 0;
    size_t successCount = 0;
    std::vector<MovedFileInfo> movedFiles;

    for (const auto& filePath : files) {
        if (g_cancelRequested) {
            std::wstring* logCancel = new std::wstring(L"[INFO] File organization cancelled by user.");
            PostMessage(hwnd, WM_APP_LOG, 1, (LPARAM)logCancel);
            break;
        }

        std::wstring ext = filePath.extension().wstring();
        std::wstring extLower = ext;
        for (auto& c : extLower) c = std::tolower(c);

        std::wstring destFolder = L"Other";
        for (const auto& [category, extensions] : fileTypeMap) {
            bool found = false;
            for (const auto& targetExt : extensions) {
                if (extLower == targetExt) {
                    destFolder = category;
                    found = true;
                    break;
                }
            }
            if (found) break;
        }

        std::filesystem::path destDir = sourcePath / destFolder;
        std::wstring fileName = filePath.filename().wstring();

        // Get file size safely
        uint64_t fileSize = 0;
        try {
            if (std::filesystem::exists(filePath)) {
                fileSize = std::filesystem::file_size(filePath);
            }
        } catch (...) {}

        if (dryRun) {
            std::wstring destFileStr = (destDir / fileName).wstring();
            std::wstring* logMsg = new std::wstring(L"[DRY RUN] Would move '" + fileName + L"' -> '" + destFolder + L"/" + fileName + L"'");
            PostMessage(hwnd, WM_APP_LOG, 0, (LPARAM)logMsg);

            if (logFile.is_open()) {
                logFile << getTimestamp() << L" - INFO - [C++ DRY RUN] Would move '" << fileName << L"' -> '" << destFileStr << L"'\n";
            }
            successCount++;

            MovedFileInfo info;
            info.fileName = fileName;
            info.category = destFolder;
            info.fileSize = fileSize;
            info.status = L"Dry Run Preview";
            movedFiles.push_back(info);
        } else {
            try {
                std::filesystem::create_directories(destDir);
                std::filesystem::path destFile = destDir / fileName;
                int counter = 1;
                std::wstring stem = filePath.stem().wstring();
                
                while (std::filesystem::exists(destFile)) {
                    std::wstring newName = stem + L" (" + std::to_wstring(counter) + L")" + ext;
                    destFile = destDir / newName;
                    counter++;
                    std::wstring* logWarn = new std::wstring(L"[WARNING] Conflict: '" + fileName + L"' already exists. Renaming to: " + newName);
                    PostMessage(hwnd, WM_APP_LOG, 1, (LPARAM)logWarn);
                }

                std::filesystem::rename(filePath, destFile);
                std::wstring destFileStr = destFile.wstring();
                std::wstring* logMsg = new std::wstring(L"[INFO] Moved: '" + fileName + L"' -> '" + destFolder + L"/" + destFile.filename().wstring() + L"'");
                PostMessage(hwnd, WM_APP_LOG, 0, (LPARAM)logMsg);

                if (logFile.is_open()) {
                    logFile << getTimestamp() << L" - INFO - [C++ LIVE] Moved: '" << fileName << L"' -> '" << destFileStr << L"'\n";
                }
                successCount++;

                MovedFileInfo info;
                info.fileName = fileName;
                info.category = destFolder;
                info.fileSize = fileSize;
                info.status = (counter > 1) ? L"Renamed" : L"Moved";
                movedFiles.push_back(info);
            } catch (const std::exception& e) {
                std::string errStr = e.what();
                std::wstring wErr(errStr.begin(), errStr.end());
                std::wstring* logErr = new std::wstring(L"[ERROR] Could not move file '" + fileName + L"'. Error: " + wErr);
                PostMessage(hwnd, WM_APP_LOG, 2, (LPARAM)logErr);

                if (logFile.is_open()) {
                    logFile << getTimestamp() << L" - ERROR - [C++] Failed to move '" << fileName << L"'. Error: " << wErr << L"\n";
                }

                MovedFileInfo info;
                info.fileName = fileName;
                info.category = destFolder;
                info.fileSize = fileSize;
                info.status = L"Error: " + wErr;
                movedFiles.push_back(info);
            }
        }

        processed++;
        PostMessage(hwnd, WM_APP_PROGRESS, (WPARAM)processed, (LPARAM)total);
        
        std::wstring statusText = L"Organized " + std::to_wstring(processed) + L" / " + std::to_wstring(total) + L" files...";
        std::wstring* statusMsg = new std::wstring(statusText);
        PostMessage(hwnd, WM_APP_STATUS, 0, (LPARAM)statusMsg);

        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }

    if (logFile.is_open()) {
        logFile.close();
    }

    // Store report data for in-app preview
    g_lastMovedFiles = movedFiles;
    g_lastSourceDir = sourcePathStr;
    g_lastReportPath = L"";

    std::wstring summary = L"[SUCCESS] Successfully processed " + std::to_wstring(successCount) + L" of " + std::to_wstring(total) + L" files!";
    std::wstring* logSummary = new std::wstring(summary);
    PostMessage(hwnd, WM_APP_LOG, 0, (LPARAM)logSummary);

    std::wstring statusDone = L"Successfully completed organization.";
    std::wstring* statusDoneMsg = new std::wstring(statusDone);
    PostMessage(hwnd, WM_APP_STATUS, 1, (LPARAM)statusDoneMsg);

    PostMessage(hwnd, WM_APP_DONE, 0, 0);
    delete params;
}

// Append texts to Log window subclass helper
void AppendLogText(const std::wstring& msg, int severity) {
    int len = GetWindowTextLengthW(g_hwndLog);
    SendMessage(g_hwndLog, EM_SETSEL, (WPARAM)len, (LPARAM)len);
    
    std::wstring formatted = msg + L"\r\n";
    SendMessage(g_hwndLog, EM_REPLACESEL, FALSE, (LPARAM)formatted.c_str());
    
    SendMessage(g_hwndLog, EM_SCROLLCARET, 0, 0);
}

// CLI Mode Synchronous Entry Point with Text-Based Progress Visuals
int RunCLI(const std::wstring& targetDirStr, bool dryRun) {
    std::wcout << L"\n=======================================================" << std::endl;
    std::wcout << L"  Bulk File Organizer - C++ Native CLI Engine  " << std::endl;
    std::wcout << L"=======================================================" << std::endl;

    if (targetDirStr.empty()) {
        std::wcout << L"[ERROR] Target directory required." << std::endl;
        std::wcout << L"Usage: organizer.exe <directory_path> [--dry-run]" << std::endl;
        return 1;
    }

    std::filesystem::path sourcePath(targetDirStr);
    if (!std::filesystem::exists(sourcePath) || !std::filesystem::is_directory(sourcePath)) {
        std::wcout << L"[ERROR] The provided path is not a valid directory: " << targetDirStr << std::endl;
        return 1;
    }

    // Load configs relative to Executable
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    std::filesystem::path configPath(exePath);
    configPath = configPath.parent_path() / "config.json";

    std::wstring jsonStr = ReadFileToString(configPath.wstring());
    auto fileTypeMap = ParseConfig(jsonStr);

    if (fileTypeMap.empty()) {
        std::wcout << L"[ERROR] Failed to load config.json in application directory!" << std::endl;
        return 1;
    }

    std::wcout << L"[INFO] Target Directory: " << targetDirStr << std::endl;
    if (dryRun) {
        std::wcout << L"[INFO] --- DRY RUN MODE ACTIVE: No files will be moved. ---" << std::endl;
        std::wcout << L"[INFO] NOTE: Turn off the '--dry-run' flag to execute the actual file organization." << std::endl;
    } else {
        std::wcout << L"[WARNING] --- LIVE RUN MODE ACTIVE: File system changes will be written! ---" << std::endl;
    }

    std::vector<std::filesystem::path> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(sourcePath)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path());
            }
        }
    } catch (const std::exception& e) {
        std::string errStr = e.what();
        std::wstring wErr(errStr.begin(), errStr.end());
        std::wcout << L"[ERROR] Failed to read directory contents: " << wErr << std::endl;
        return 1;
    }

    size_t total = files.size();
    if (total == 0) {
        std::wcout << L"[INFO] No files found to organize." << std::endl;
        return 0;
    }

    std::wcout << L"[INFO] Found " << total << L" files to process. Commencing organization...\n" << std::endl;

    std::wofstream logFile(L"organizer.log", std::ios::app);
    auto getTimestamp = []() -> std::wstring {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &in_time_t);
        wchar_t buf[100];
        wcsftime(buf, sizeof(buf)/sizeof(wchar_t), L"%Y-%m-%d %H:%M:%S", &tm);
        return buf;
    };

    size_t processed = 0;
    size_t successCount = 0;
    std::vector<MovedFileInfo> movedFiles;

    for (const auto& filePath : files) {
        std::wstring ext = filePath.extension().wstring();
        std::wstring extLower = ext;
        for (auto& c : extLower) c = std::tolower(c);

        std::wstring destFolder = L"Other";
        for (const auto& [category, extensions] : fileTypeMap) {
            bool found = false;
            for (const auto& targetExt : extensions) {
                if (extLower == targetExt) {
                    destFolder = category;
                    found = true;
                    break;
                }
            }
            if (found) break;
        }

        std::filesystem::path destDir = sourcePath / destFolder;
        std::wstring fileName = filePath.filename().wstring();

        // Get file size safely
        uint64_t fileSize = 0;
        try {
            if (std::filesystem::exists(filePath)) {
                fileSize = std::filesystem::file_size(filePath);
            }
        } catch (...) {}

        // Print active logs above the progress bar by clearing carriage returns
        if (dryRun) {
            std::wstring destFileStr = (destDir / fileName).wstring();
            std::wcout << L"\r\033[K[DRY RUN] Would move '" << fileName << L"' -> '" << destFolder << L"/" << fileName << L"'" << std::endl;

            if (logFile.is_open()) {
                logFile << getTimestamp() << L" - INFO - [C++ CLI DRY RUN] Would move '" << fileName << L"' -> '" << destFileStr << L"'\n";
            }
            successCount++;

            MovedFileInfo info;
            info.fileName = fileName;
            info.category = destFolder;
            info.fileSize = fileSize;
            info.status = L"Dry Run Preview";
            movedFiles.push_back(info);
        } else {
            try {
                std::filesystem::create_directories(destDir);
                std::filesystem::path destFile = destDir / fileName;
                int counter = 1;
                std::wstring stem = filePath.stem().wstring();
                
                while (std::filesystem::exists(destFile)) {
                    std::wstring newName = stem + L" (" + std::to_wstring(counter) + L")" + ext;
                    destFile = destDir / newName;
                    counter++;
                    std::wcout << L"\r\033[K[WARNING] Conflict: '" << fileName << L"' already exists. Renaming -> '" << newName << L"'" << std::endl;
                }

                std::filesystem::rename(filePath, destFile);
                std::wstring destFileStr = destFile.wstring();
                std::wcout << L"\r\033[K[INFO] Moved: '" << fileName << L"' -> '" << destFolder << L"/" << destFile.filename().wstring() << L"'" << std::endl;

                if (logFile.is_open()) {
                    logFile << getTimestamp() << L" - INFO - [C++ CLI LIVE] Moved: '" << fileName << L"' -> '" << destFileStr << L"'\n";
                }
                successCount++;

                MovedFileInfo info;
                info.fileName = fileName;
                info.category = destFolder;
                info.fileSize = fileSize;
                info.status = (counter > 1) ? L"Renamed" : L"Moved";
                movedFiles.push_back(info);
            } catch (const std::exception& e) {
                std::string errStr = e.what();
                std::wstring wErr(errStr.begin(), errStr.end());
                std::wcout << L"\r\033[K[ERROR] Failed to move '" << fileName << L"'. Error: " << wErr << std::endl;

                if (logFile.is_open()) {
                    logFile << getTimestamp() << L" - ERROR - [C++ CLI] Failed to move '" << fileName << L"'. Error: " << wErr << L"\n";
                }

                MovedFileInfo info;
                info.fileName = fileName;
                info.category = destFolder;
                info.fileSize = fileSize;
                info.status = L"Error: " + wErr;
                movedFiles.push_back(info);
            }
        }

        processed++;
        
        // Print interactive visual terminal progress bar
        int barWidth = 40;
        double pct = (double)processed / total;
        int pos = (int)(barWidth * pct);
        
        std::wcout << L"\r[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::wcout << L"=";
            else if (i == pos) std::wcout << L">";
            else std::wcout << L" ";
        }
        std::wcout << L"] " << int(pct * 100.0) << L"% (" << processed << L"/" << total << L" files)" << std::flush;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }

    if (logFile.is_open()) {
        logFile.close();
    }

    // Generate the PDF report
    std::filesystem::path reportPath = sourcePath / (dryRun ? L"organization_report_preview.pdf" : L"organization_report.pdf");
    try {
        GeneratePDFReport(reportPath.wstring(), movedFiles, dryRun);
        std::wcout << L"\n[INFO] Generated PDF Report: " << reportPath.filename().wstring() << std::endl;
    } catch (const std::exception& e) {
        std::string errStr = e.what();
        std::wstring wErr(errStr.begin(), errStr.end());
        std::wcout << L"\n[ERROR] Failed to generate PDF Report: " << wErr << std::endl;
    }

    std::wcout << L"\n=======================================================" << std::endl;
    std::wcout << L"[SUCCESS] Successfully processed " << successCount << L" of " << total << L" files!" << std::endl;
    std::wcout << L"=======================================================\n" << std::endl;

    return 0;
}
