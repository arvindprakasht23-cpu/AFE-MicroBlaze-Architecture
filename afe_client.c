#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define FPGA_IP "192.168.1.10"
#define FPGA_PORT 5001
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char command_string[BUFFER_SIZE];

    printf("--- TI AFE String Terminal (Red Hat) ---\n");
    printf("Connecting to %s:%d...\n", FPGA_IP, FPGA_PORT);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("ERR: Socket failed"); return 1; }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(FPGA_PORT);
    inet_pton(AF_INET, FPGA_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("ERR: Connection failed"); return 1;
    }
    
    printf("Connected! Type e.g., 'spiRawWrite(00,00A0,FF)'\n");

    while (1) {
        printf("CMD> ");
        if (fgets(command_string, BUFFER_SIZE, stdin) == NULL) break;
        command_string[strcspn(command_string, "\n")] = 0;
        if (strcmp(command_string, "exit") == 0) break;
        if (strlen(command_string) == 0) continue;

        send(sock, command_string, strlen(command_string) + 1, 0);

        char response_buffer[128] = {0};
        int bytes = recv(sock, response_buffer, sizeof(response_buffer)-1, 0);
        if (bytes > 0) printf("FPGA: %s\n", response_buffer);
    }

    close(sock);
    return 0;
}