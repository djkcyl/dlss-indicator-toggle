#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <stdio.h>
#include "resource.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

static constexpr LPCWSTR REG_PATH            = L"SOFTWARE\\NVIDIA Corporation\\Global\\NGXCore";
static constexpr LPCWSTR REG_NAME            = L"ShowDlssIndicator";
static constexpr LPCWSTR REGEDIT_APPLET_PATH = L"Software\\Microsoft\\Windows\\CurrentVersion\\Applets\\Regedit";
static constexpr LPCWSTR REGEDIT_TARGET_PATH = L"HKEY_LOCAL_MACHINE\\SOFTWARE\\NVIDIA Corporation\\Global\\NGXCore";

static const DWORD RADIO_VALUES[] = { 0, 1, 1024 };

static BOOL ReadRegValue(DWORD* outValue) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, REG_PATH, 0, KEY_READ, &hKey) != ERROR_SUCCESS) return FALSE;
    DWORD value = 0, size = sizeof(value), type = 0;
    LONG r = RegQueryValueExW(hKey, REG_NAME, nullptr, &type, (LPBYTE)&value, &size);
    RegCloseKey(hKey);
    if (r == ERROR_SUCCESS && type == REG_DWORD) { *outValue = value; return TRUE; }
    return FALSE;
}

static BOOL WriteRegValue(DWORD value) {
    HKEY hKey;
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, REG_PATH, 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr) != ERROR_SUCCESS) return FALSE;
    LONG r = RegSetValueExW(hKey, REG_NAME, 0, REG_DWORD, (LPBYTE)&value, sizeof(value));
    RegCloseKey(hKey);
    return r == ERROR_SUCCESS;
}

static void SetMessage(HWND hDlg, LPCWSTR msg) {
    SetDlgItemTextW(hDlg, IDC_MESSAGE, msg);
}

static void UpdateRadiosEnabled(HWND hDlg) {
    BOOL on = (IsDlgButtonChecked(hDlg, IDC_ENABLE_CHECK) == BST_CHECKED);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIO_0),    on);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIO_1),    on);
    EnableWindow(GetDlgItem(hDlg, IDC_RADIO_1024), on);
}

static void RefreshUI(HWND hDlg) {
    DWORD value = 0;
    BOOL exists = ReadRegValue(&value);

    CheckDlgButton(hDlg, IDC_ENABLE_CHECK, exists ? BST_CHECKED : BST_UNCHECKED);

    int radioId = IDC_RADIO_0;
    if (exists) {
        if (value == 1)         radioId = IDC_RADIO_1;
        else if (value == 1024) radioId = IDC_RADIO_1024;
        else if (value != 0)    radioId = 0;
    }
    CheckRadioButton(hDlg, IDC_RADIO_0, IDC_RADIO_1024, radioId);

    WCHAR buf[128];
    if (!exists)            wcscpy_s(buf, L"当前: 未设置");
    else if (value == 0)    wcscpy_s(buf, L"当前: 0 (关闭)");
    else if (value == 1)    wcscpy_s(buf, L"当前: 1 (绿色方块)");
    else if (value == 1024) wcscpy_s(buf, L"当前: 1024 (文字详情)");
    else                    swprintf_s(buf, L"当前: %lu (未识别)", value);
    SetDlgItemTextW(hDlg, IDC_STATUS, buf);

    UpdateRadiosEnabled(hDlg);
}

static void ApplyChanges(HWND hDlg) {
    if (IsDlgButtonChecked(hDlg, IDC_ENABLE_CHECK) != BST_CHECKED) {
        SetMessage(hDlg, L"未勾选，未做更改。");
        return;
    }
    int idx = -1;
    if      (IsDlgButtonChecked(hDlg, IDC_RADIO_0)    == BST_CHECKED) idx = 0;
    else if (IsDlgButtonChecked(hDlg, IDC_RADIO_1)    == BST_CHECKED) idx = 1;
    else if (IsDlgButtonChecked(hDlg, IDC_RADIO_1024) == BST_CHECKED) idx = 2;

    if (idx < 0) {
        SetMessage(hDlg, L"当前值未识别，请先选一个选项。");
        return;
    }

    if (WriteRegValue(RADIO_VALUES[idx])) {
        SetMessage(hDlg, L"已写入。重启游戏后生效。");
        RefreshUI(hDlg);
    } else {
        SetMessage(hDlg, L"写入失败。请以管理员身份运行。");
    }
}

static void CopyToClipboard(HWND hDlg, LPCWSTR text) {
    if (!OpenClipboard(hDlg)) return;
    EmptyClipboard();
    SIZE_T bytes = (lstrlenW(text) + 1) * sizeof(WCHAR);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bytes);
    if (hMem) {
        void* p = GlobalLock(hMem);
        if (p) {
            memcpy(p, text, bytes);
            GlobalUnlock(hMem);
            SetClipboardData(CF_UNICODETEXT, hMem);
        }
    }
    CloseClipboard();
}

static void KillRegedit() {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32W pe = { sizeof(pe) };
    if (Process32FirstW(snap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, L"regedit.exe") == 0) {
                HANDLE h = OpenProcess(PROCESS_TERMINATE | SYNCHRONIZE, FALSE, pe.th32ProcessID);
                if (h) {
                    TerminateProcess(h, 0);
                    WaitForSingleObject(h, 3000);
                    CloseHandle(h);
                }
            }
        } while (Process32NextW(snap, &pe));
    }
    CloseHandle(snap);
}

static void OpenRegedit(HWND hDlg) {
    CopyToClipboard(hDlg, REGEDIT_TARGET_PATH);
    KillRegedit();
    Sleep(150);

    HKEY hKey;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, REGEDIT_APPLET_PATH, 0, nullptr,
                        REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS) {
        RegSetValueExW(hKey, L"LastKey", 0, REG_SZ,
                       (LPBYTE)REGEDIT_TARGET_PATH,
                       (lstrlenW(REGEDIT_TARGET_PATH) + 1) * sizeof(WCHAR));
        RegFlushKey(hKey);
        RegCloseKey(hKey);
    }

    ShellExecuteW(hDlg, L"open", L"regedit.exe", nullptr, nullptr, SW_SHOWNORMAL);
    SetMessage(hDlg, L"已启动 regedit。路径已复制到剪贴板，若未跳转可粘贴到地址栏。");
}

static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG:
            RefreshUI(hDlg);
            return TRUE;

        case WM_COMMAND: {
            WORD id   = LOWORD(wParam);
            WORD code = HIWORD(wParam);
            switch (id) {
                case IDC_ENABLE_CHECK:
                    if (code == BN_CLICKED) UpdateRadiosEnabled(hDlg);
                    return TRUE;
                case IDC_APPLY:    ApplyChanges(hDlg); return TRUE;
                case IDC_REFRESH:  RefreshUI(hDlg); SetMessage(hDlg, L""); return TRUE;
                case IDC_REGEDIT:  OpenRegedit(hDlg); return TRUE;
                case IDCANCEL:     EndDialog(hDlg, 0); return TRUE;
            }
            break;
        }
        case WM_CLOSE:
            EndDialog(hDlg, 0);
            return TRUE;
    }
    return FALSE;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int) {
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES };
    InitCommonControlsEx(&icc);
    DialogBoxW(hInstance, MAKEINTRESOURCEW(IDD_MAIN_DIALOG), nullptr, DialogProc);
    return 0;
}
