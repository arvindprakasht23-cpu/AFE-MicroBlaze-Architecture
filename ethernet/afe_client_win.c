#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

/* Instruct MSVC to link the Winsock library. 
 * If using MinGW/GCC, you must compile with: gcc afe_client_win.c -o client.exe -lws2_32 
 */
#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "192.168.1.10" // Your FPGA's IP address
#define SERVER_PORT 7            // Your FPGA's TCP Port
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE];

    // 1. Initialize Winsock (Mandatory on Windows)
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return -1;
    }

    // 2. Create the TCP socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation error: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // 3. Convert IPv4 address from text to binary format
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("Invalid address / Address not supported\n");
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // 4. Connect to the FPGA
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection to FPGA Failed! Check Ethernet cable and IP subnet. Error: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    printf("Connected to FPGA at %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Type commands like: spiBurstWrite(1,4A22,4,1,2,3,4) or 'quit' to exit.\n\n");

    // 5. The Master Input Loop
    while (1) {
        printf(">>> ");
        fflush(stdout);

        // Get command string from the Windows command prompt
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // Remove the trailing newline character for a clean string
        input[strcspn(input, "\n")] = 0;

        // Check for quit command
        if (strcmp(input, "quit") == 0) {
            break;
        }
        
        // Ignore empty enters
        if (strlen(input) == 0) {
            continue; 
        }

        // Send the raw ASCII string to the FPGA
        if (send(sock, input, strlen(input), 0) == SOCKET_ERROR) {
            printf("Failed to send command. Error: %d\n", WSAGetLastError());
            break;
        }

        // Clear the buffer and wait for the FPGA to reply
        memset(buffer, 0, BUFFER_SIZE);
        
        // Use recv() instead of read() for Windows sockets
        int bytes_read = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_read > 0) {
            // Print the SUCCESS/FAIL message from the FPGA
            printf("FPGA: %s", buffer); 
        } 
        else if (bytes_read == 0) {
            printf("Connection closed by FPGA.\n");
            break;
        } 
        else {
            printf("Read error: %d\n", WSAGetLastError());
            break;
        }
    }

    // 6. Clean up
    closesocket(sock);
    WSACleanup();
    printf("Disconnected.\n");
    return 0;
}