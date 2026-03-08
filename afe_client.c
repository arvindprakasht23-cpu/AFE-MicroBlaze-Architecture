#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define FPGA_IP "192.168.1.10"
#define FPGA_PORT 5001
#define BUFFER_SIZE 256

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char tx_buffer[BUFFER_SIZE] = {0};
    char rx_buffer[BUFFER_SIZE] = {0};

    printf("========================================\n");
    printf("  AFE MicroBlaze TCP Client Tester\n");
    printf("========================================\n");
    printf("Attempting to connect to %s:%d...\n", FPGA_IP, FPGA_PORT);

    // 1. Create the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nERROR: Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(FPGA_PORT);

    // 2. Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, FPGA_IP, &serv_addr.sin_addr) <= 0) {
        printf("\nERROR: Invalid address or Address not supported \n");
        return -1;
    }

    // 3. Connect to the FPGA
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nERROR: Connection Failed. Is the FPGA powered on and pingable?\n");
        return -1;
    }

    printf("SUCCESS: Connected to FPGA!\n");
    printf("Type your commands (e.g., spiRawWrite(00,00A0,FF)).\n");
    printf("Type 'exit' to close the connection.\n\n");

    // 4. The Interactive Super-Loop
    while (1) {
        printf("FPGA_CMD > ");
        
        // Get input from the user keyboard
        if (fgets(tx_buffer, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // Strip the invisible newline character (\n) that fgets adds when you hit Enter.
        // This ensures the FPGA parser strictly receives "spiRawWrite(00,00A0,FF)"
        tx_buffer[strcspn(tx_buffer, "\n")] = 0;

        // Check if user wants to quit
        if (strcmp(tx_buffer, "exit") == 0) {
            printf("Closing connection...\n");
            break;
        }

        // Send the string to the FPGA
        send(sock, tx_buffer, strlen(tx_buffer), 0);

        // Clear the receive buffer and wait for the FPGA's LwIP ACK
        memset(rx_buffer, 0, BUFFER_SIZE);
        int bytes_read = recv(sock, rx_buffer, BUFFER_SIZE, 0);
        
        if (bytes_read > 0) {
            printf("FPGA_REPLY < %s\n", rx_buffer);
        } else if (bytes_read == 0) {
            printf("\nERROR: FPGA dropped the connection.\n");
            break;
        }
    }

    // 5. Clean up
    close(sock);
    return 0;
}