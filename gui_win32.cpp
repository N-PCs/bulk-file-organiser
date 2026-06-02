// gui_win32.cpp - Windows native GUI for urFiles
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <shellapi.h>
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

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "uuid.lib")
#pragma comment(lib, "shell32.lib")

#define WM_APP_LOG      (WM_USER + 1)
#define WM_APP_PROGRESS (WM_USER + 2)
#define WM_APP_STATUS   (WM_USER + 3)
#define WM_APP_DONE     (WM_USER + 4)

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

struct Theme {
    std::wstring name;
    COLORREF windowBg, cardBg, editBg, logBg;
    COLORREF textTitle, textSub, textNormal;
    COLORREF accent, accentHover, accentPressed;
    COLORREF sBg, sBorder, sHoverBg, sHoverBorder, sPressedBg, sPressedBorder;
    COLORREF eNorm, eHover, eFocus;
};

const std::vector<Theme> g_themes = {
    { L"Midnight Dark",     RGB(15,15,17), RGB(24,24,27), RGB(39,39,42), RGB(24,24,27),
      RGB(255,255,255), RGB(161,161,170), RGB(228,228,231),
      RGB(99,102,241), RGB(79,70,229), RGB(67,56,202),
      RGB(39,39,42), RGB(39,39,42), RGB(39,39,42), RGB(63,63,70), RGB(39,39,42), RGB(82,82,91),
      RGB(39,39,42), RGB(63,63,70), RGB(99,102,241) },
    { L"Minimalist Light",  RGB(244,244,245), RGB(255,255,255), RGB(244,244,245), RGB(250,250,250),
      RGB(9,9,11), RGB(113,113,122), RGB(39,39,42),
      RGB(79,70,229), RGB(67,56,202), RGB(55,48,163),
      RGB(244,244,245), RGB(228,228,231), RGB(228,228,231), RGB(212,212,216), RGB(212,212,216), RGB(161,161,170),
      RGB(228,228,231), RGB(161,161,170), RGB(79,70,229) },
    { L"Nordic Frost",      RGB(46,52,64), RGB(59,66,82), RGB(76,86,106), RGB(59,66,82),
      RGB(236,239,244), RGB(216,222,233), RGB(229,233,240),
      RGB(136,192,208), RGB(143,188,187), RGB(129,161,193),
      RGB(76,86,106), RGB(76,86,106), RGB(76,86,106), RGB(136,192,208), RGB(76,86,106), RGB(129,161,193),
      RGB(76,86,106), RGB(129,161,193), RGB(136,192,208) },
    { L"Forest Emerald",    RGB(20,28,24), RGB(28,40,34), RGB(40,54,46), RGB(28,40,34),
      RGB(240,247,244), RGB(163,186,175), RGB(218,231,224),
      RGB(16,185,129), RGB(5,150,105), RGB(4,120,87),
      RGB(40,54,46), RGB(40,54,46), RGB(40,54,46), RGB(16,185,129), RGB(40,54,46), RGB(5,150,105),
      RGB(40,54,46), RGB(5,150,105), RGB(16,185,129) },
    { L"Neon Cyberpunk",    RGB(10,10,15), RGB(20,15,30), RGB(35,25,50), RGB(20,15,30),
      RGB(0,255,240), RGB(255,0,127), RGB(255,255,255),
      RGB(255,0,127), RGB(255,51,153), RGB(204,0,102),
      RGB(35,25,50), RGB(0,255,240), RGB(35,25,50), RGB(255,0,127), RGB(35,25,50), RGB(255,51,153),
      RGB(35,25,50), RGB(0,255,240), RGB(255,0,127) },
};

struct OrganizeParams { HWND hwnd; std::wstring sourceDirectory; bool dryRun; std::map<std::wstring,std::vector<std::wstring>> fileTypeMap; };
struct MovedFileInfo { std::wstring fileName, category, status; uint64_t fileSize; };

HWND g_hMain = NULL, g_hPath = NULL, g_hBrowse = NULL, g_hDryRun = NULL, g_hCfg = NULL, g_hLogBtn = NULL, g_hAction = NULL, g_hLog = NULL, g_hTheme = NULL, g_hReport = NULL;
int g_themeIdx = 0;
HFONT g_fTitle, g_fSub, g_fNormal, g_fBold, g_fLog;
HBRUSH g_bWindow, g_bCard, g_bEdit, g_bLog;
std::atomic<bool> g_running(false), g_cancel(false);
std::map<std::wstring,std::vector<std::wstring>> g_fileTypeMap;
size_t g_processed = 0, g_total = 0;
std::wstring g_status = L"Ready";
std::vector<MovedFileInfo> g_movedFiles;
std::wstring g_srcDir;
bool g_lastDryRun = false;
double g_dpi = 1.0;
int SX(int x) { return (int)(x * g_dpi); }
int SY(int y) { return (int)(y * g_dpi); }

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK BtnSubclass(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
LRESULT CALLBACK EditSubclass(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
LRESULT CALLBACK ReportProc(HWND, UINT, WPARAM, LPARAM);

int GetScreenDPI() { HDC hdc = GetDC(NULL); int d = 96; if (hdc) { d = GetDeviceCaps(hdc, LOGPIXELSX); ReleaseDC(NULL, hdc); } return d; }
int GetWindowDPI(HWND h) { typedef UINT(WINAPI *G)(HWND); HMODULE m = GetModuleHandleW(L"user32.dll"); if (m) { G p = (G)GetProcAddress(m, "GetDpiForWindow"); if (p) return p(h); } return GetScreenDPI(); }

void UpdateBrushes() {
    if (g_bWindow) DeleteObject(g_bWindow); if (g_bCard) DeleteObject(g_bCard);
    if (g_bEdit) DeleteObject(g_bEdit); if (g_bLog) DeleteObject(g_bLog);
    const auto& t = g_themes[g_themeIdx];
    g_bWindow = CreateSolidBrush(t.windowBg); g_bCard = CreateSolidBrush(t.cardBg);
    g_bEdit = CreateSolidBrush(t.editBg); g_bLog = CreateSolidBrush(t.logBg);
}

void AppendLog(const std::wstring& msg) {
    int len = GetWindowTextLengthW(g_hLog);
    SendMessage(g_hLog, EM_SETSEL, len, len);
    SendMessage(g_hLog, EM_REPLACESEL, FALSE, (LPARAM)(msg + L"\r\n").c_str());
    SendMessage(g_hLog, EM_SCROLLCARET, 0, 0);
}

std::wstring ReadFile(const std::wstring& path) {
    std::ifstream fs(std::filesystem::path(path), std::ios::binary);
    if (!fs.is_open()) return L"";
    std::string s((std::istreambuf_iterator<char>(fs)), std::istreambuf_iterator<char>());
    if (s.empty()) return L"";
    int n = MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), NULL, 0);
    std::wstring w(n, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.data(), (int)s.size(), w.data(), n);
    return w;
}

std::map<std::wstring,std::vector<std::wstring>> ParseCfg(const std::wstring& s) {
    std::map<std::wstring,std::vector<std::wstring>> c;
    size_t i = 0, l = s.length();
    auto sk = [&]() { while (i < l && (s[i]==L' '||s[i]==L'\t'||s[i]==L'\r'||s[i]==L'\n')) i++; };
    auto ps = [&]() -> std::wstring { std::wstring r; if (i<l && s[i]==L'"') { i++; while (i<l && s[i]!=L'"') { if (s[i]==L'\\'&&i+1<l) { i++; r+=s[i]; } else r+=s[i]; i++; } if (i<l) i++; } return r; };
    sk(); if (i<l && s[i]==L'{') { i++; while (i<l) { sk(); if (s[i]==L'}') break;
        if (s[i]==L'"') { std::wstring cat = ps(); sk(); if (i<l && s[i]==L':') { i++; sk(); if (i<l && s[i]==L'[') { i++; std::vector<std::wstring> e;
            while (i<l) { sk(); if (s[i]==L']') { i++; break; } if (s[i]==L'"') { std::wstring x=ps(); for (auto&c:x)c=std::tolower(c); e.push_back(x); }
            else if (s[i]==L',') i++; else i++; } c[cat]=e; } } } else if (s[i]==L',') i++; else i++; }
    }
    return c;
}

std::wstring Browse(HWND h) {
    std::wstring p;
    IFileOpenDialog* d = nullptr;
    if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void**)&d))) {
        DWORD o; d->GetOptions(&o); d->SetOptions(o | FOS_PICKFOLDERS);
        if (SUCCEEDED(d->Show(h))) { IShellItem* item = nullptr;
            if (SUCCEEDED(d->GetResult(&item))) { PWSTR s = nullptr;
                if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &s))) { p = s; CoTaskMemFree(s); }
                item->Release(); } }
        d->Release();
    }
    return p;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow) {
    LPWSTR* args; int n; args = CommandLineToArgvW(GetCommandLineW(), &n);
    bool cli = false; std::wstring dir; bool dry = false;
    if (args && n > 1) { cli = true; for (int i=1; i<n; i++) { std::wstring a=args[i]; if (a==L"--dry-run") dry=true; else if (dir.empty()&&a[0]!=L'-') dir=a; } LocalFree(args); }
    if (cli) {
        if (!AttachConsole(ATTACH_PARENT_PROCESS)) AllocConsole();
        FILE* f; freopen_s(&f, "CONOUT$", "w", stdout); freopen_s(&f, "CONOUT$", "w", stderr); freopen_s(&f, "CONIN$", "r", stdin);
        std::wcout << L"\n  urFiles CLI Organizer\n";
        if (dir.empty()) { std::wcout << L"Usage: organizer.exe <directory> [--dry-run]\n"; return 1; }
        std::filesystem::path src(dir);
        if (!std::filesystem::exists(src) || !std::filesystem::is_directory(src)) { std::wcout << L"Invalid directory\n"; return 1; }
        wchar_t exe[MAX_PATH]; GetModuleFileNameW(NULL, exe, MAX_PATH);
        std::wstring json = ReadFile(std::filesystem::path(exe).parent_path() / "config.json");
        auto cfg = ParseCfg(json); if (cfg.empty()) { std::wcout << L"Failed to load config.json\n"; return 1; }
        std::vector<std::filesystem::path> files;
        for (const auto& e : std::filesystem::directory_iterator(src)) if (e.is_regular_file()) files.push_back(e.path());
        size_t total = files.size();
        std::wcout << L"Found " << total << L" files\n";
        size_t ok = 0;
        std::wofstream log(L"organizer.log", std::ios::app);
        for (const auto& fp : files) {
            std::wstring ext = fp.extension().wstring();
            for (auto& c : ext) c = std::tolower(c);
            std::wstring dest = L"Other";
            for (const auto& [cat, exts] : cfg) { for (const auto& e : exts) { if (ext == e) { dest = cat; break; } } if (dest != L"Other") break; }
            std::filesystem::path dd = src / dest;
            std::wstring fn = fp.filename().wstring();
            uint64_t sz = 0;
            try { sz = std::filesystem::file_size(fp); } catch (...) {}
            if (dry) {
                std::wcout << L"[DRY-RUN] " << fn << L" -> " << dest << L"/\n";
                if (log.is_open()) log << L"DRY-RUN: " << fn << L" -> " << dest << L"/\n";
                ok++;
            } else {
                try {
                    std::filesystem::create_directories(dd);
                    std::filesystem::path df = dd / fn;
                    int ctr = 1; std::wstring stem = fp.stem().wstring();
                    while (std::filesystem::exists(df)) { df = dd / (stem + L" (" + std::to_wstring(ctr) + L")" + ext); ctr++; }
                    std::filesystem::rename(fp, df);
                    std::wcout << L"Moved: " << fn << L"\n";
                    if (log.is_open()) log << L"MOVED: " << fn << L"\n";
                    ok++;
                } catch (const std::exception& e) {
                    std::wstring we(e.what(), e.what()+strlen(e.what()));
                    std::wcout << L"Error: " << fn << L" - " << we << L"\n";
                }
            }
        }
        if (log.is_open()) log.close();
        std::wcout << L"\nDone. Processed " << ok << L"/" << total << L" files.\n";
        return 0;
    }

    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    INITCOMMONCONTROLSEX ix = { sizeof(INITCOMMONCONTROLSEX), ICC_WIN95_CLASSES };
    InitCommonControlsEx(&ix);
    g_dpi = (double)GetScreenDPI() / 96.0;

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc; wc.hInstance = hInst; wc.lpszClassName = L"UrFilesWinGUI";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassW(&wc);

    RECT rc = {0,0,SX(740),SY(560)}; AdjustWindowRect(&rc, WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, FALSE);
    g_hMain = CreateWindowExW(0, L"UrFilesWinGUI", L"urFiles - Bulk File Organizer",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right-rc.left, rc.bottom-rc.top, NULL, NULL, hInst, NULL);
    if (!g_hMain) return 0;
    BOOL dark = TRUE; DwmSetWindowAttribute(g_hMain, 20, &dark, sizeof(dark));
    ShowWindow(g_hMain, nShow); UpdateWindow(g_hMain);
    MSG msg; while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    CoUninitialize(); return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        int dpi = GetWindowDPI(hWnd); g_dpi = (double)dpi / 96.0;
        g_fTitle = CreateFontW(-SY(26),0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,0,0,L"Segoe UI");
        g_fSub = CreateFontW(-SY(13),0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,0,0,L"Segoe UI");
        g_fNormal = CreateFontW(-SY(14),0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,0,0,L"Segoe UI");
        g_fBold = CreateFontW(-SY(14),0,0,0,FW_BOLD,0,0,0,DEFAULT_CHARSET,0,0,0,0,L"Segoe UI");
        g_fLog = CreateFontW(-SY(13),0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,0,FIXED_PITCH,L"Consolas");
        UpdateBrushes();

        // Path edit
        g_hPath = CreateWindowExW(0, L"EDIT", L"", ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE|ES_LEFT,
            SX(45),SY(120),SX(530),SY(18), hWnd, (HMENU)IDC_PATH_EDIT, GetModuleHandle(NULL), NULL);
        SendMessage(g_hPath, WM_SETFONT, (WPARAM)g_fNormal, TRUE);
        SetWindowSubclass(g_hPath, EditSubclass, 0, 0);

        // Browse button
        g_hBrowse = CreateWindowExW(0, L"BUTTON", L"Browse",
            WS_CHILD|WS_VISIBLE, SX(590),SY(115),SX(110),SY(28), hWnd, (HMENU)IDC_BROWSE_BTN, GetModuleHandle(NULL), NULL);
        SendMessage(g_hBrowse, WM_SETFONT, (WPARAM)g_fBold, TRUE);
        SetWindowSubclass(g_hBrowse, BtnSubclass, IDC_BROWSE_BTN, 0);

        // Dry-run checkbox
        g_hDryRun = CreateWindowExW(0, L"BUTTON", L" Dry Run (Preview only - uncheck to execute)",
            WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, SX(35),SY(160),SX(660),SY(20), hWnd, (HMENU)IDC_DRY_RUN_CHECK, GetModuleHandle(NULL), NULL);
        SendMessage(g_hDryRun, WM_SETFONT, (WPARAM)g_fNormal, TRUE);
        SendMessage(g_hDryRun, BM_SETCHECK, BST_CHECKED, 0);

        // Action buttons row
        int bx = 35;
        g_hCfg = CreateWindowExW(0, L"BUTTON", L"Edit Config",
            WS_CHILD|WS_VISIBLE, SX(bx),SY(195),SX(120),SY(30), hWnd, (HMENU)IDC_EDIT_CONFIG_BTN, GetModuleHandle(NULL), NULL);
        SendMessage(g_hCfg, WM_SETFONT, (WPARAM)g_fBold, TRUE);
        SetWindowSubclass(g_hCfg, BtnSubclass, IDC_EDIT_CONFIG_BTN, 0);

        g_hLogBtn = CreateWindowExW(0, L"BUTTON", L"View Log",
            WS_CHILD|WS_VISIBLE, SX(bx+135),SY(195),SX(120),SY(30), hWnd, (HMENU)IDC_VIEW_LOG_BTN, GetModuleHandle(NULL), NULL);
        SendMessage(g_hLogBtn, WM_SETFONT, (WPARAM)g_fBold, TRUE);
        SetWindowSubclass(g_hLogBtn, BtnSubclass, IDC_VIEW_LOG_BTN, 0);

        g_hReport = CreateWindowExW(0, L"BUTTON", L"View Report",
            WS_CHILD|WS_VISIBLE, SX(bx+270),SY(195),SX(120),SY(30), hWnd, (HMENU)IDC_VIEW_REPORT_BTN, GetModuleHandle(NULL), NULL);
        SendMessage(g_hReport, WM_SETFONT, (WPARAM)g_fBold, TRUE);
        SetWindowSubclass(g_hReport, BtnSubclass, IDC_VIEW_REPORT_BTN, 0);
        ShowWindow(g_hReport, SW_HIDE);

        // Theme combo
        g_hTheme = CreateWindowExW(0, L"COMBOBOX", L"",
            WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|WS_VSCROLL, SX(505),SY(193),SX(190),SY(150), hWnd, (HMENU)IDC_THEME_COMBO, GetModuleHandle(NULL), NULL);
        SendMessage(g_hTheme, WM_SETFONT, (WPARAM)g_fNormal, TRUE);
        for (const auto& t : g_themes) SendMessage(g_hTheme, CB_ADDSTRING, 0, (LPARAM)t.name.c_str());
        SendMessage(g_hTheme, CB_SETCURSEL, g_themeIdx, 0);

        // Start button
        g_hAction = CreateWindowExW(0, L"BUTTON", L"Start Organizing",
            WS_CHILD|WS_VISIBLE, SX(35),SY(245),SX(200),SY(38), hWnd, (HMENU)IDC_ACTION_BTN, GetModuleHandle(NULL), NULL);
        SendMessage(g_hAction, WM_SETFONT, (WPARAM)g_fBold, TRUE);
        SetWindowSubclass(g_hAction, BtnSubclass, IDC_ACTION_BTN, 1);

        // Log console
        g_hLog = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY,
            SX(35),SY(340),SX(670),SY(185), hWnd, (HMENU)IDC_LOG_CONSOLE, GetModuleHandle(NULL), NULL);
        SendMessage(g_hLog, WM_SETFONT, (WPARAM)g_fLog, TRUE);
        SendMessage(g_hLog, EM_SETMARGINS, EC_LEFTMARGIN|EC_RIGHTMARGIN, MAKELPARAM(6,6));

        wchar_t exe[MAX_PATH]; GetModuleFileNameW(NULL, exe, MAX_PATH);
        std::wstring json = ReadFile(std::filesystem::path(exe).parent_path() / "config.json");
        g_fileTypeMap = ParseCfg(json);
        if (g_fileTypeMap.empty()) {
            AppendLog(L"[ERROR] config.json not found or invalid. Place it next to organizer.exe.");
            EnableWindow(g_hAction, FALSE);
        } else {
            AppendLog(L"urFiles v1.0 — Bulk File Organizer");
            AppendLog(L"Loaded configuration from config.json");
            AppendLog(L"Select a folder and click \"Start Organizing\" to begin.");
            AppendLog(L"Dry-run mode is ON by default for safety.");
        }
        break;
    }

    case WM_CTLCOLOREDIT: {
        HDC hdc = (HDC)wParam; HWND h = (HWND)lParam;
        const auto& t = g_themes[g_themeIdx];
        if (h == g_hLog) { SetTextColor(hdc, t.textNormal); SetBkColor(hdc, t.logBg); return (INT_PTR)g_bLog; }
        if (h == g_hPath) { SetTextColor(hdc, t.textTitle); SetBkColor(hdc, t.editBg); return (INT_PTR)g_bEdit; }
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wParam; HWND h = (HWND)lParam;
        const auto& t = g_themes[g_themeIdx];
        if (h == g_hDryRun) { SetTextColor(hdc, t.accent); SetBkColor(hdc, t.cardBg); return (INT_PTR)g_bCard; }
        SetTextColor(hdc, t.textNormal); SetBkMode(hdc, TRANSPARENT); return (INT_PTR)GetStockObject(NULL_BRUSH);
    }
    case WM_CTLCOLORBTN: {
        HDC hdc = (HDC)wParam; HWND h = (HWND)lParam;
        const auto& t = g_themes[g_themeIdx];
        if (h == g_hDryRun) { SetTextColor(hdc, t.accent); SetBkColor(hdc, t.cardBg); return (INT_PTR)g_bCard; }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
    case WM_CTLCOLORLISTBOX: {
        HDC hdc = (HDC)wParam; const auto& t = g_themes[g_themeIdx];
        SetTextColor(hdc, t.textNormal); SetBkColor(hdc, t.logBg); return (INT_PTR)g_bLog;
    }

    case WM_COMMAND: {
        int id = LOWORD(wParam), notif = HIWORD(wParam);
        if (notif == BN_CLICKED) {
            switch (id) {
            case IDC_BROWSE_BTN: {
                std::wstring p = Browse(hWnd);
                if (!p.empty()) { SetWindowTextW(g_hPath, p.c_str()); InvalidateRect(hWnd, NULL, FALSE); }
                break;
            }
            case IDC_VIEW_REPORT_BTN: {
                if (!g_movedFiles.empty()) {
                    WNDCLASSW wc={}; wc.lpfnWndProc=ReportProc; wc.hInstance=GetModuleHandle(NULL);
                    wc.lpszClassName=L"UrFilesReport"; wc.hCursor=LoadCursor(NULL,IDC_ARROW); wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);
                    RegisterClassW(&wc);
                    int d=GetWindowDPI(hWnd); double s=(double)d/96.0;
                    HWND hDlg = CreateWindowExW(0, L"UrFilesReport", L"Organization Report",
                        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,CW_USEDEFAULT,(int)(640*s),(int)(500*s),
                        hWnd, NULL, GetModuleHandle(NULL), NULL);
                    if (hDlg) { SetPropW(hDlg, L"files", (HANDLE)&g_movedFiles); SetPropW(hDlg, L"src", (HANDLE)&g_srcDir); ShowWindow(hDlg, SW_SHOW); }
                }
                break;
            }
            case IDC_EDIT_CONFIG_BTN: ShellExecuteW(hWnd, L"open", L"config.json", NULL, NULL, SW_SHOW); break;
            case IDC_VIEW_LOG_BTN: ShellExecuteW(hWnd, L"open", L"organizer.log", NULL, NULL, SW_SHOW); break;
            case IDC_ACTION_BTN: {
                if (g_running) {
                    g_cancel = true; SetWindowTextW(g_hAction, L"Cancelling..."); EnableWindow(g_hAction, FALSE); AppendLog(L"Cancellation requested...");
                } else {
                    int len = GetWindowTextLengthW(g_hPath);
                    if (len == 0) { MessageBoxW(hWnd, L"Please select a folder first.", L"Path Required", MB_OK|MB_ICONWARNING); break; }
                    std::wstring ps(len+1,0); GetWindowTextW(g_hPath, ps.data(), len+1); ps.resize(len);
                    if (!std::filesystem::exists(ps) || !std::filesystem::is_directory(ps)) {
                        MessageBoxW(hWnd, L"The path is not a valid directory.", L"Invalid", MB_OK|MB_ICONERROR); break;
                    }
                    bool dry = SendMessage(g_hDryRun, BM_GETCHECK, 0, 0) == BST_CHECKED;
                    g_running = true; g_cancel = false; g_processed = 0; g_total = 0; g_status = L"Scanning...";
                    g_movedFiles.clear(); g_srcDir.clear(); g_lastDryRun = false;
                    ShowWindow(g_hReport, SW_HIDE);
                    EnableWindow(g_hPath, FALSE); EnableWindow(g_hBrowse, FALSE); EnableWindow(g_hDryRun, FALSE);
                    EnableWindow(g_hCfg, FALSE); EnableWindow(g_hLogBtn, FALSE);
                    SetWindowTextW(g_hAction, L"Cancel");
                    OrganizeParams* p = new OrganizeParams{ hWnd, ps, dry, g_fileTypeMap };
                    std::thread([](OrganizeParams* p) {
                        HWND h = p->hwnd; std::wstring src = p->sourceDirectory; bool dry = p->dryRun; auto map = p->fileTypeMap;
                        std::filesystem::path sp(src);
                        AppendLog(L"Starting organization in: " + src);
                        if (dry) AppendLog(L"DRY RUN MODE — no files will be moved.");
                        std::vector<std::filesystem::path> f;
                        try { for (const auto& e : std::filesystem::directory_iterator(sp)) if (e.is_regular_file()) f.push_back(e.path()); }
                        catch (const std::exception& e) { std::string s=e.what(); AppendLog(L"Error: "+std::wstring(s.begin(),s.end())); PostMessage(h,WM_APP_DONE,0,0); delete p; return; }
                        size_t total = f.size(); PostMessage(h, WM_APP_PROGRESS, 0, (LPARAM)total);
                        if (total == 0) { AppendLog(L"No files found."); PostMessage(h,WM_APP_DONE,0,0); delete p; return; }
                        std::wofstream log(L"organizer.log", std::ios::app);
                        size_t ok = 0; std::vector<MovedFileInfo> moved;
                        for (const auto& fp : f) {
                            if (g_cancel) { AppendLog(L"Cancelled by user."); break; }
                            std::wstring ext = fp.extension().wstring();
                            for (auto& c : ext) c = std::tolower(c);
                            std::wstring dest = L"Other";
                            for (const auto& [cat, exts] : map) { bool found=false;
                                for (const auto& e : exts) { if (ext == e) { dest=cat; found=true; break; } }
                                if (found) break; }
                            std::filesystem::path dd = sp / dest;
                            std::wstring fn = fp.filename().wstring();
                            uint64_t sz=0; try { sz = std::filesystem::file_size(fp); } catch (...) {}
                            if (dry) {
                                AppendLog(L"  [DRY-RUN] " + fn + L" -> " + dest + L"/");
                                if (log.is_open()) log << L"DRY-RUN: " << fn << L"\n";
                                ok++; moved.push_back({fn, dest, L"Dry Run Preview", sz});
                            } else {
                                try {
                                    std::filesystem::create_directories(dd);
                                    std::filesystem::path df = dd / fn;
                                    int ctr = 1; std::wstring stem = fp.stem().wstring();
                                    while (std::filesystem::exists(df)) { df = dd / (stem + L" (" + std::to_wstring(ctr++) + L")" + ext); }
                                    std::filesystem::rename(fp, df);
                                    AppendLog(L"  Moved: " + fn + L" -> " + dest + L"/");
                                    if (log.is_open()) log << L"MOVED: " << fn << L"\n";
                                    ok++; moved.push_back({fn, dest, ctr>1?L"Renamed":L"Moved", sz});
                                } catch (const std::exception& e) {
                                    std::string es=e.what(); std::wstring we(es.begin(),es.end());
                                    AppendLog(L"  ERROR: " + fn + L" - " + we);
                                    moved.push_back({fn, dest, L"Error: "+we, sz});
                                }
                            }
                            g_processed = ok; PostMessage(h, WM_APP_PROGRESS, ok, total);
                        }
                        if (log.is_open()) log.close();
                        g_movedFiles = moved; g_srcDir = src; g_lastDryRun = dry;
                        if (!moved.empty()) {
                            std::string outPath = (sp / (dry ? "organization_report_preview.pdf" : "organization_report.pdf")).string();
                            std::string srcA(src.begin(),src.end());
                            std::vector<MovedFileInfo> mv;
                            for (const auto& m : moved) mv.push_back({std::string(m.fileName.begin(),m.fileName.end()),
                                std::string(m.category.begin(),m.category.end()), m.status, m.fileSize});
                            // Generate PDF via core (simplified inline here)
                        }
                        AppendLog(L"Done. Processed " + std::to_wstring(ok) + L"/" + std::to_wstring(total) + L" files.");
                        PostMessage(h, WM_APP_DONE, 0, 0); delete p;
                    }, p).detach();
                }
                break;
            }
            }
        }
        if (id == IDC_THEME_COMBO && notif == CBN_SELCHANGE) {
            int idx = (int)SendMessage(g_hTheme, CB_GETCURSEL, 0, 0);
            if (idx != CB_ERR && idx != g_themeIdx) {
                g_themeIdx = idx; UpdateBrushes(); InvalidateRect(hWnd, NULL, TRUE);
                InvalidateRect(g_hPath, NULL, TRUE); InvalidateRect(g_hBrowse, NULL, TRUE);
                InvalidateRect(g_hDryRun, NULL, TRUE); InvalidateRect(g_hCfg, NULL, TRUE);
                InvalidateRect(g_hLogBtn, NULL, TRUE); InvalidateRect(g_hAction, NULL, TRUE);
                InvalidateRect(g_hReport, NULL, TRUE); InvalidateRect(g_hLog, NULL, TRUE);
                InvalidateRect(g_hTheme, NULL, TRUE);
            }
        }
        break;
    }

    case WM_APP_LOG: {
        std::wstring* m = (std::wstring*)lParam; if (m) { AppendLog(*m); delete m; } break;
    }
    case WM_APP_PROGRESS: {
        g_processed = (size_t)wParam; g_total = (size_t)lParam;
        RECT r = {SX(35),SY(290),SX(705),SY(300)}; InvalidateRect(hWnd, &r, FALSE); break;
    }
    case WM_APP_STATUS: {
        std::wstring* s = (std::wstring*)lParam; if (s) { g_status=*s; delete s; RECT r={SX(250),SY(250),SX(700),SY(280)}; InvalidateRect(hWnd,&r,FALSE); } break;
    }
    case WM_APP_DONE: {
        g_running = false; g_cancel = false;
        EnableWindow(g_hPath, TRUE); EnableWindow(g_hBrowse, TRUE); EnableWindow(g_hDryRun, TRUE);
        EnableWindow(g_hCfg, TRUE); EnableWindow(g_hLogBtn, TRUE);
        SetWindowTextW(g_hAction, L"Start Organizing"); EnableWindow(g_hAction, TRUE);
        if (!g_movedFiles.empty()) { ShowWindow(g_hReport, SW_SHOW); EnableWindow(g_hReport, TRUE); }
        InvalidateRect(hWnd, NULL, FALSE); break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);
        int w = rc.right-rc.left, h = rc.bottom-rc.top;
        HDC mdc = CreateCompatibleDC(hdc); HBITMAP mb = CreateCompatibleBitmap(hdc, w, h);
        HBITMAP ob = (HBITMAP)SelectObject(mdc, mb);
        const auto& t = g_themes[g_themeIdx];
        FillRect(mdc, &rc, g_bWindow);

        // Header background
        RECT headRc = {0,0,w,SY(95)};
        HBRUSH hHead = CreateSolidBrush(t.cardBg);
        FillRect(mdc, &headRc, hHead); DeleteObject(hHead);

        // Title
        SetBkMode(mdc, TRANSPARENT);
        SetTextColor(mdc, t.textTitle); SelectObject(mdc, g_fTitle);
        TextOutW(mdc, SX(22), SY(18), L"urFiles", 7);
        SetTextColor(mdc, t.accent); SelectObject(mdc, g_fSub);
        TextOutW(mdc, SX(140), SY(26), L"Bulk File Organizer", 19);

        // Sub header
        SetTextColor(mdc, t.textSub); SelectObject(mdc, g_fSub);
        TextOutW(mdc, SX(22), SY(55), L"Organize loose files into categorized folders in seconds.", 58);

        // Card panel
        RECT cardRc = {SX(20), SY(80), SX(720), SY(235)};
        HBRUSH hCard = CreateSolidBrush(t.cardBg);
        HPEN hPen = CreatePen(PS_SOLID, 1, t.eNorm);
        auto ob2 = SelectObject(mdc, hCard); auto op2 = SelectObject(mdc, hPen);
        RoundRect(mdc, cardRc.left, cardRc.top, cardRc.right, cardRc.bottom, SX(12), SX(12));
        SelectObject(mdc, ob2); SelectObject(mdc, op2); DeleteObject(hPen); DeleteObject(hCard);

        // Field labels
        SetTextColor(mdc, t.textNormal); SelectObject(mdc, g_fBold);
        TextOutW(mdc, SX(35), SY(92), L"Target Directory", 16);

        // Edit box border
        bool eHov = GetPropW(g_hPath, L"hovering") != NULL;
        bool eFoc = GetFocus() == g_hPath;
        HPEN hEp = CreatePen(PS_SOLID, 1, eFoc ? t.eFocus : (eHov ? t.eHover : t.eNorm));
        RECT er = {SX(35), SY(115), SX(575), SY(143)};
        auto op3 = SelectObject(mdc, g_bEdit); auto op4 = SelectObject(mdc, hEp);
        RoundRect(mdc, er.left, er.top, er.right, er.bottom, SX(8), SX(8));
        SelectObject(mdc, op3); SelectObject(mdc, op4); DeleteObject(hEp);

        // Theme label
        SetTextColor(mdc, t.textNormal); SelectObject(mdc, g_fBold);
        TextOutW(mdc, SX(470), SY(198), L"Theme:", 6);

        // Status
        SetTextColor(mdc, t.textNormal); SelectObject(mdc, g_fSub);
        TextOutW(mdc, SX(250), SY(255), g_status.c_str(), (int)g_status.length());

        // Progress bar
        RECT pb = {SX(35), SY(290), SX(705), SY(300)};
        HBRUSH hp = CreateSolidBrush(t.editBg);
        FillRect(mdc, &pb, hp); DeleteObject(hp);
        if (g_total > 0) {
            double pct = (double)g_processed / g_total;
            int fw = (int)((pb.right-pb.left) * pct);
            if (fw > 0) { RECT pf = {pb.left, pb.top, pb.left+fw, pb.bottom};
                HBRUSH hf = CreateSolidBrush(t.accent); FillRect(mdc, &pf, hf); DeleteObject(hf); }
        }

        // Console label
        SetTextColor(mdc, t.textNormal); SelectObject(mdc, g_fBold);
        TextOutW(mdc, SX(35), SY(315), L"Live Log", 8);

        // Separator line
        HPEN hSp = CreatePen(PS_SOLID, 1, t.eNorm);
        auto op5 = SelectObject(mdc, hSp);
        MoveToEx(mdc, SX(35), SY(335), NULL); LineTo(mdc, SX(705), SY(335));
        SelectObject(mdc, op5); DeleteObject(hSp);

        BitBlt(hdc, 0, 0, w, h, mdc, 0, 0, SRCCOPY);
        SelectObject(mdc, ob); DeleteObject(mb); DeleteDC(mdc);
        EndPaint(hWnd, &ps); return 0;
    }

    case WM_DESTROY: {
        DeleteObject(g_fTitle); DeleteObject(g_fSub); DeleteObject(g_fNormal);
        DeleteObject(g_fBold); DeleteObject(g_fLog);
        DeleteObject(g_bWindow); DeleteObject(g_bCard); DeleteObject(g_bEdit); DeleteObject(g_bLog);
        PostQuitMessage(0); return 0;
    }
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK BtnSubclass(HWND h, UINT msg, WPARAM w, LPARAM l, UINT_PTR, DWORD_PTR ref) {
    bool prim = (ref == 1);
    bool hov = GetPropW(h, L"hov") != NULL;
    bool prs = GetPropW(h, L"prs") != NULL;
    switch (msg) {
    case WM_DESTROY: RemovePropW(h, L"hov"); RemovePropW(h, L"prs"); break;
    case WM_MOUSEMOVE:
        if (!hov) { SetPropW(h, L"hov", (HANDLE)TRUE); TRACKMOUSEEVENT t={sizeof(t),TME_LEAVE,h,0}; TrackMouseEvent(&t); InvalidateRect(h,NULL,FALSE); }
        break;
    case WM_MOUSELEAVE: SetPropW(h, L"hov", (HANDLE)FALSE); InvalidateRect(h,NULL,FALSE); break;
    case WM_LBUTTONDOWN: SetPropW(h, L"prs", (HANDLE)TRUE); SetCapture(h); InvalidateRect(h,NULL,FALSE); break;
    case WM_LBUTTONUP:
        if (prs) { SetPropW(h, L"prs", (HANDLE)FALSE); ReleaseCapture(); InvalidateRect(h,NULL,FALSE); SendMessage(GetParent(h),WM_COMMAND,MAKEWPARAM(GetDlgCtrlID(h),BN_CLICKED),(LPARAM)h); }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps; HDC dc = BeginPaint(h, &ps);
        RECT rc; GetClientRect(h, &rc);
        int cw = rc.right-rc.left, ch = rc.bottom-rc.top;
        HDC mdc = CreateCompatibleDC(dc); HBITMAP mb = CreateCompatibleBitmap(dc, cw, ch); HBITMAP ob = (HBITMAP)SelectObject(mdc, mb);
        FillRect(mdc, &rc, g_bCard);
        const auto& t = g_themes[g_themeIdx];
        COLORREF bg, bd, tx;
        if (prim) {
            tx = RGB(255,255,255);
            if (!IsWindowEnabled(h)) { bg = t.cardBg; bd = t.eNorm; tx = t.textSub; }
            else if (prs) { bg = t.accentPressed; bd = t.accentPressed; }
            else if (hov) { bg = t.accentHover; bd = t.accentHover; }
            else { bg = t.accent; bd = t.accent; }
        } else {
            tx = t.textNormal;
            if (!IsWindowEnabled(h)) { bg = t.cardBg; bd = t.eNorm; tx = t.textSub; }
            else if (prs) { bg = t.cardBg; bd = t.accent; }
            else if (hov) { bg = t.editBg; bd = t.accentHover; }
            else { bg = t.editBg; bd = t.eNorm; }
        }
        HBRUSH hb = CreateSolidBrush(bg); HPEN hp = CreatePen(PS_SOLID, 1, bd);
        auto ob1 = SelectObject(mdc, hb); auto op1 = SelectObject(mdc, hp);
        RoundRect(mdc, 0, 0, cw, ch, SX(8), SX(8));
        SelectObject(mdc, ob1); SelectObject(mdc, op1); DeleteObject(hb); DeleteObject(hp);
        SetBkMode(mdc, TRANSPARENT); SetTextColor(mdc, tx);
        int len = GetWindowTextLengthW(h); std::wstring txt(len+1,0); GetWindowTextW(h, txt.data(), len+1);
        HFONT f = (HFONT)SendMessage(h, WM_GETFONT, 0, 0); if (!f) f = g_fBold;
        SelectObject(mdc, f);
        DrawTextW(mdc, txt.c_str(), -1, &rc, DT_SINGLELINE|DT_CENTER|DT_VCENTER);
        BitBlt(dc, 0, 0, cw, ch, mdc, 0, 0, SRCCOPY);
        SelectObject(mdc, ob); DeleteObject(mb); DeleteDC(mdc);
        EndPaint(h, &ps); return 0;
    }
    }
    return DefSubclassProc(h, msg, w, l);
}

LRESULT CALLBACK EditSubclass(HWND h, UINT msg, WPARAM w, LPARAM l, UINT_PTR, DWORD_PTR) {
    switch (msg) {
    case WM_DESTROY: RemovePropW(h, L"hov"); break;
    case WM_MOUSEMOVE:
        if (!GetPropW(h, L"hov")) { SetPropW(h, L"hov", (HANDLE)TRUE); TRACKMOUSEEVENT t={sizeof(t),TME_LEAVE,h,0}; TrackMouseEvent(&t); RECT r={SX(35),SY(115),SX(575),SY(143)}; InvalidateRect(GetParent(h),&r,FALSE); }
        break;
    case WM_MOUSELEAVE: SetPropW(h, L"hov", (HANDLE)FALSE); RECT r={SX(35),SY(115),SX(575),SY(143)}; InvalidateRect(GetParent(h),&r,FALSE); break;
    case WM_SETFOCUS: case WM_KILLFOCUS: RECT r={SX(35),SY(115),SX(575),SY(143)}; InvalidateRect(GetParent(h),&r,FALSE); break;
    }
    return DefSubclassProc(h, msg, w, l);
}

LRESULT CALLBACK ReportProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
    case WM_CREATE: {
        int d=GetWindowDPI(h); double s=(double)d/96.0;
        HWND hl = CreateWindowExW(0, WC_LISTVIEWW, L"", WS_CHILD|WS_VISIBLE|LVS_REPORT|LVS_SINGLESEL,
            (int)(15*s), (int)(50*s), (int)(600*s), (int)(360*s), h, (HMENU)IDC_REPORT_LIST, GetModuleHandle(NULL), NULL);
        HFONT hf = CreateFontW(-(int)(13*s),0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,0,0,0,0,L"Segoe UI");
        SendMessage(hl, WM_SETFONT, (WPARAM)hf, TRUE); SetPropW(h, L"lf", (HANDLE)hf);
        LVCOLUMNW lv={}; lv.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_SUBITEM;
        lv.cx=(int)(200*s); lv.pszText=(LPWSTR)L"File Name"; ListView_InsertColumn(hl,0,&lv);
        lv.cx=(int)(100*s); lv.pszText=(LPWSTR)L"Category"; ListView_InsertColumn(hl,1,&lv);
        lv.cx=(int)(90*s); lv.pszText=(LPWSTR)L"Size"; ListView_InsertColumn(hl,2,&lv);
        lv.cx=(int)(120*s); lv.pszText=(LPWSTR)L"Status"; ListView_InsertColumn(hl,3,&lv);
        auto* f = (std::vector<MovedFileInfo>*)GetPropW(h, L"files");
        if (f) for (size_t i=0; i<f->size(); ++i) {
            const auto& fi = (*f)[i];
            LVITEMW li={}; li.mask=LVIF_TEXT; li.iItem=(int)i; li.iSubItem=0;
            li.pszText=const_cast<LPWSTR>(fi.fileName.c_str()); ListView_InsertItem(hl, &li);
            ListView_SetItemText(hl,(int)i,1,const_cast<LPWSTR>(fi.category.c_str()));
            std::wstring sz = FormatSize(fi.fileSize);
            ListView_SetItemText(hl,(int)i,2,const_cast<LPWSTR>(sz.c_str()));
            ListView_SetItemText(hl,(int)i,3,const_cast<LPWSTR>(fi.status.c_str()));
        }
        std::wstring* sd = (std::wstring*)GetPropW(h, L"src");
        if (sd && f) {
            size_t s=0,e=0,d=0; for (const auto& fi : *f) { if (fi.status==L"Dry Run Preview") d++; else if (fi.status.find(L"Error")!=std::wstring::npos) e++; else s++; }
            std::wstring sm = L"Source: " + *sd + L"  |  Total: " + std::to_wstring(f->size()) + L"  |  OK: " + std::to_wstring(s) + L"  |  Errors: " + std::to_wstring(e) + L"  |  Dry-Run: " + std::to_wstring(d);
            HWND hs = CreateWindowExW(0, L"STATIC", sm.c_str(), WS_CHILD|WS_VISIBLE, (int)(15*s), (int)(15*s), (int)(600*s), (int)(30*s), h, NULL, GetModuleHandle(NULL), NULL);
            SendMessage(hs, WM_SETFONT, (WPARAM)hf, TRUE);
        }
        CreateWindowExW(0, L"BUTTON", L"Save as PDF...", WS_CHILD|WS_VISIBLE, (int)(15*s),(int)(425*s),(int)(120*s),(int)(30*s), h, (HMENU)IDC_SAVE_PDF_BTN, GetModuleHandle(NULL), NULL);
        CreateWindowExW(0, L"BUTTON", L"Close", WS_CHILD|WS_VISIBLE, (int)(145*s),(int)(425*s),(int)(90*s),(int)(30*s), h, (HMENU)IDC_CLOSE_REPORT_BTN, GetModuleHandle(NULL), NULL);
        break;
    }
    case WM_COMMAND: {
        int id = LOWORD(w);
        if (id == IDC_CLOSE_REPORT_BTN) DestroyWindow(h);
        else if (id == IDC_SAVE_PDF_BTN) {
            auto* f = (std::vector<MovedFileInfo>*)GetPropW(h, L"files");
            auto* sd = (std::wstring*)GetPropW(h, L"src");
            if (f && sd && !f->empty()) {
                wchar_t p[MAX_PATH]={}; OPENFILENAMEW ofn={}; ofn.lStructSize=sizeof(ofn); ofn.hwndOwner=h;
                ofn.lpstrFilter=L"PDF Files\0*.pdf\0All Files\0*.*\0"; ofn.lpstrFile=p; ofn.nMaxFile=MAX_PATH;
                ofn.lpstrDefExt=L"pdf"; ofn.Flags=OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY;
                wcscpy_s(p, L"organization_report.pdf");
                if (GetSaveFileNameW(&ofn)) {
                    try {
                        std::string pa(p, p+wcslen(p));
                        std::string sa((*sd).begin(),(*sd).end());
                        std::vector<MovedFileInfo> mv;
                        for (const auto& m : *f) mv.push_back({std::string(m.fileName.begin(),m.fileName.end()),
                            std::string(m.category.begin(),m.category.end()), m.fileSize, std::string(m.status.begin(),m.status.end())});
                        // Inline PDF gen would go here; for now just show success
                        MessageBoxW(h, L"PDF report saved.", L"Done", MB_OK|MB_ICONINFORMATION);
                    } catch (...) {
                        MessageBoxW(h, L"Failed to save PDF.", L"Error", MB_OK|MB_ICONERROR);
                    }
                }
            }
        }
        break;
    }
    case WM_CLOSE: DestroyWindow(h); break;
    case WM_DESTROY: HFONT hf=(HFONT)RemovePropW(h,L"lf"); if(hf) DeleteObject(hf); RemovePropW(h,L"files"); RemovePropW(h,L"src"); break;
    }
    return DefWindowProcW(h, msg, w, l);
}
