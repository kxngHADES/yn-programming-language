#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <sys/stat.h>

// Automatically embed our compiler array!
#include "ync_payload.h"

#define INSTALL_DIR "C:\\yn"
#define MINGW_DIR "C:\\yn\\mingw"
#define TARGET_EXE "C:\\yn\\ync.exe"
#define YN_VERSION "1.0.0"

// GUI Component IDs
#define ID_BTN_INSTALL_YN   1001
#define ID_BTN_INSTALL_BOTH 1002
#define ID_LBL_STATUS       1003

HWND hStatusLabel;
HWND hBtnYN;
HWND hBtnBoth;

int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

int unpack_compiler() {
    FILE *out = fopen(TARGET_EXE, "wb");
    if (!out) return 0;
    
    size_t written = fwrite(ync_payload, 1, ync_payload_size, out);
    fclose(out);
    
    return written == ync_payload_size;
}

void add_to_path(const char* new_folder) {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey);
    
    if (result != ERROR_SUCCESS) return;

    char cur_path[8192];
    DWORD path_size = sizeof(cur_path);
    result = RegQueryValueExA(hKey, "Path", NULL, NULL, (LPBYTE)cur_path, &path_size);

    if (result != ERROR_SUCCESS) {
        strcpy(cur_path, "");
    }

    if (strstr(cur_path, new_folder) == NULL) {
        char new_path[8192];
        if (strlen(cur_path) > 0 && cur_path[strlen(cur_path) - 1] != ';') {
            snprintf(new_path, sizeof(new_path), "%s;%s", cur_path, new_folder);
        } else {
            snprintf(new_path, sizeof(new_path), "%s%s", cur_path, new_folder);
        }

        RegSetValueExA(hKey, "Path", 0, REG_EXPAND_SZ, (LPBYTE)new_path, strlen(new_path) + 1);
        SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
    }
    RegCloseKey(hKey);
}

void install_mingw_gcc() {
    SetWindowText(hStatusLabel, "Status: Downloading GCC (w64devkit)... Please wait.");
    // Force UI update
    UpdateWindow(hStatusLabel);

    char download_cmd[] = "powershell -WindowStyle Hidden -Command \"Invoke-WebRequest -Uri 'https://github.com/skeeto/w64devkit/releases/download/v2.0.0/w64devkit-2.0.0.zip' -OutFile 'C:\\yn\\mingw.zip'\"";
    system(download_cmd);

    SetWindowText(hStatusLabel, "Status: Extracting GCC files...");
    UpdateWindow(hStatusLabel);
    
    char extract_cmd[] = "powershell -WindowStyle Hidden -Command \"Expand-Archive -Force 'C:\\yn\\mingw.zip' 'C:\\yn\\mingw_temp'\"";
    system(extract_cmd);

    system("move /Y C:\\yn\\mingw_temp\\w64devkit C:\\yn\\mingw > NUL");
    remove("C:\\yn\\mingw.zip");
    system("rmdir /Q /S C:\\yn\\mingw_temp");

    add_to_path("C:\\yn\\mingw\\bin");
}

void do_installation(int include_gcc) {
    // Disable buttons so user cannot click twice
    EnableWindow(hBtnYN, FALSE);
    EnableWindow(hBtnBoth, FALSE);

    SetWindowText(hStatusLabel, "Status: Creating C:\\yn Directory...");
    if (!file_exists(INSTALL_DIR)) {
        if (_mkdir(INSTALL_DIR) != 0) {
            MessageBox(NULL, "Failed to create directory C:\\yn! Try running as Administrator.", "Error", MB_ICONERROR);
            SetWindowText(hStatusLabel, "Status: Error creating directory.");
            EnableWindow(hBtnYN, TRUE);
            EnableWindow(hBtnBoth, TRUE);
            return;
        }
    }

    SetWindowText(hStatusLabel, "Status: Extracting embedded YN Compiler...");
    if (!unpack_compiler()) {
        MessageBox(NULL, "Failed to write compiler to C:\\yn\\ync.exe", "Error", MB_ICONERROR);
        SetWindowText(hStatusLabel, "Status: Target executable write failed.");
        EnableWindow(hBtnYN, TRUE);
        EnableWindow(hBtnBoth, TRUE);
        return;
    }

    SetWindowText(hStatusLabel, "Status: Updating System PATH Variable...");
    add_to_path(INSTALL_DIR);

    if (include_gcc) {
        install_mingw_gcc();
    }

    SetWindowText(hStatusLabel, "Status: Installation Complete!");
    MessageBox(NULL, "YN Language installed successfully! \n\nPlease completely exit and restart any open Terminals before typing 'yn'.", "Success", MB_ICONINFORMATION);
    
    EnableWindow(hBtnYN, TRUE);
    EnableWindow(hBtnBoth, TRUE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            // Title Label
            CreateWindow("STATIC", "Welcome to the YN Language Installer v1.0.0",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                20, 20, 340, 25,
                hwnd, NULL, NULL, NULL);

            // Install Native compiler
            hBtnYN = CreateWindow("BUTTON", "Install YN Compiler Only",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                50, 60, 280, 40,
                hwnd, (HMENU)ID_BTN_INSTALL_YN, NULL, NULL);

            // Install Network MinGW + YN
            hBtnBoth = CreateWindow("BUTTON", "Install YN + MinGW GCC (Recommended)",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                50, 110, 280, 40,
                hwnd, (HMENU)ID_BTN_INSTALL_BOTH, NULL, NULL);

            // Status Label
            hStatusLabel = CreateWindow("STATIC", "Status: Ready",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                10, 170, 360, 20,
                hwnd, (HMENU)ID_LBL_STATUS, NULL, NULL);

            // Set default font to something cleaner
            HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
            SendMessage(hBtnYN, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage(hBtnBoth, WM_SETFONT, (WPARAM)hFont, TRUE);
            return 0;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == ID_BTN_INSTALL_YN) {
                do_installation(0);
            } else if (LOWORD(wParam) == ID_BTN_INSTALL_BOTH) {
                do_installation(1);
            }
            return 0;
        }

        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "YN Installer Class";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "YN Installer",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, // Fixed size
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 240,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
