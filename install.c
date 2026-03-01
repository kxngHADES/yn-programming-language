#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <direct.h>
#include <sys/stat.h>

#define INSTALL_DIR "C:\\yn"
#define EXE_NAME "ync.exe"
#define TARGET_EXE "C:\\yn\\ync.exe"

// Helper to check if file exists
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// Function to append C:\yn to the user's PATH environment variable
void add_to_path() {
    HKEY hKey;
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, "Environment", 0, KEY_READ | KEY_WRITE, &hKey);
    
    if (result != ERROR_SUCCESS) {
        printf("Failed to open registry key for Environment Variables.\n");
        return;
    }

    char cur_path[4096];
    DWORD path_size = sizeof(cur_path);
    result = RegQueryValueExA(hKey, "Path", NULL, NULL, (LPBYTE)cur_path, &path_size);

    if (result != ERROR_SUCCESS) {
        // If "Path" doesn't exist, just create it
        strcpy(cur_path, "");
    }

    // Check if C:\yn is already in the PATH
    if (strstr(cur_path, INSTALL_DIR) != NULL) {
        printf("C:\\yn is already in your System PATH.\n");
    } else {
        printf("Adding C:\\yn to System PATH...\n");
        char new_path[4096];
        if (strlen(cur_path) > 0 && cur_path[strlen(cur_path) - 1] != ';') {
            snprintf(new_path, sizeof(new_path), "%s;%s", cur_path, INSTALL_DIR);
        } else {
            snprintf(new_path, sizeof(new_path), "%s%s", cur_path, INSTALL_DIR);
        }

        result = RegSetValueExA(hKey, "Path", 0, REG_EXPAND_SZ, (LPBYTE)new_path, strlen(new_path) + 1);
        
        if (result == ERROR_SUCCESS) {
            printf("Successfully updated PATH environment variable!\n");
            // Broadcast message so active windows update their environment block
            SendMessageTimeoutA(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)"Environment", SMTO_ABORTIFHUNG, 5000, NULL);
        } else {
            printf("Failed to update PATH environment variable.\n");
        }
    }

    RegCloseKey(hKey);
}

#define YN_VERSION "1.0.0"

int main() {
    printf("=========================================\n");
    printf("       YN Language Installer v%s        \n", YN_VERSION);
    printf("=========================================\n\n");

    // 1. Check if the compiler exists in the current directory before installing
    if (!file_exists(EXE_NAME)) {
        printf("Error: Could not find '%s' in the current directory.\n", EXE_NAME);
        printf("Please compile 'ync.c' into '%s' first before running the installer.\n", EXE_NAME);
        printf("Press Enter to exit...");
        getchar();
        return 1;
    }

    // 2. Create the target directory C:\yn
    printf("Setting up directory: %s\n", INSTALL_DIR);
    if (!file_exists(INSTALL_DIR)) {
        if (_mkdir(INSTALL_DIR) != 0) {
            printf("Error: Failed to create directory '%s'. Try running as Administrator.\n", INSTALL_DIR);
            printf("Press Enter to exit...");
            getchar();
            return 1;
        }
    }

    // 3. Copy the compiler into the directory
    printf("Installing compiler to %s...\n", TARGET_EXE);
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "copy /Y %s %s > NUL", EXE_NAME, TARGET_EXE);
    
    if (system(cmd) != 0) {
        printf("Error: Failed to copy compiler to C:\\yn.\n");
        printf("Press Enter to exit...");
        getchar();
        return 1;
    }

    // 4. Update the system PATH
    add_to_path();

    printf("\n=========================================\n");
    printf(" Installation Complete! \n");
    printf(" You can now run any `.yn` file simply by typing:\n");
    printf("     yn script.yn \n");
    printf(" (You may need to restart your terminal for the PATH changes to take effect)\n");
    printf("=========================================\n");

    printf("\nPress Enter to finish installation...");
    getchar();

    return 0;
}
