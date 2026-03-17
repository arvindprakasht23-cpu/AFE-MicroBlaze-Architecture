#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "192.168.1.10" // Your FPGA's IP address
#define SERVER_PORT 7            // Your FPGA's TCP Port
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char input[BUFFER_SIZE];

    // 1. Create the TCP socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // 2. Convert IPv4 address from text to binary format
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("Invalid address / Address not supported\n");
        return -1;
    }

    // 3. Connect to the FPGA
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection to FPGA Failed! Check Ethernet cable and IP subnet.");
        return -1;
    }

    printf("Connected to FPGA at %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Type commands like: spiBurstWrite(1,4A22,4,1,2,3,4) or 'quit' to exit.\n\n");

    // 4. The Master Input Loop
    while (1) {
        printf(">>> ");
        fflush(stdout);

        // Get command string from the Red Hat terminal
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
        if (send(sock, input, strlen(input), 0) < 0) {
            perror("Failed to send command");
            break;
        }

        // Clear the buffer and wait for the FPGA to reply
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read = read(sock, buffer, BUFFER_SIZE - 1);
        
        if (bytes_read > 0) {
            // Print the SUCCESS/FAIL message from the FPGA
            printf("FPGA: %s", buffer); 
        } 
        else if (bytes_read == 0) {
            printf("Connection closed by FPGA.\n");
            break;
        } 
        else {
            perror("Read error");
            break;
        }
    }

    // Clean up
    close(sock);
    printf("Disconnected.\n");
    return 0;
}