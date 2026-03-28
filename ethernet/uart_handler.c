#include "uart_handler.h"
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xuartlite_l.h"
#include <string.h>

static char uart_rx_buffer[128];
static int  uart_buf_idx = 0;
static volatile int uart_flag = 0;

void print_help(void) {
    xil_printf("\r\n--- Command Input Formats (Hex) ---\r\n");
    xil_printf("1. Raw Write:   spiRawWrite(Inst, Addr, Data)\r\n");
    xil_printf("2. Raw Read:    spiRawRead(Inst, Addr)\r\n");
    xil_printf("3. Burst Write: spiBurstWrite(Inst, Addr, Size, D0, D1...)\r\n");
    xil_printf("4. Burst Read:  spiBurstRead(Inst, Addr, Size)\r\n");
    xil_printf("5. Multi Write: spiRawWriteMulti(Sel, Addr, Data)\r\n");
    xil_printf("6. Multi Read:  spiRawReadMulti(Sel, Addr)\r\n");
    xil_printf("7. Multi Burst: spiBurstWriteMulti(Sel, Addr, Size, D0, D1...)\r\n");
    xil_printf("-----------------------------------\r\nCMD> ");
}

void uart_init(void) {
    init_platform();
    xil_printf("\r\n--- System & UART Initialization Successful ---\r\n");
    print_help();
}

void uart_poll(void) {
    if (!XUartLite_IsReceiveEmpty(XPAR_UARTLITE_0_BASEADDR)) {
        char c = XUartLite_ReadReg(XPAR_UARTLITE_0_BASEADDR, XUL_RX_FIFO_OFFSET);
        
        if (c == '\r' || c == '\n') {
            uart_rx_buffer[uart_buf_idx] = '\0';
            uart_flag = 1; 
            uart_buf_idx = 0;
            xil_printf("\r\n"); 
        } 
        else if (c == '\b' || c == 0x7F) {
            if (uart_buf_idx > 0) {
                uart_buf_idx--;
                xil_printf("\b \b");
            }
        } 
        else if (uart_buf_idx < 127) {
            uart_rx_buffer[uart_buf_idx++] = c;
            xil_printf("%c", c);
        }
    }
}

int uart_get_message(Message_t *msg) {
    if (uart_flag) {
        msg->source_id = SRC_UART;
        msg->length = strlen(uart_rx_buffer);
        strcpy(msg->payload, uart_rx_buffer);
        
        uart_flag = 0;
        return 1; 
    }
    return 0;
}