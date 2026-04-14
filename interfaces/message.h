#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdint.h>

// Define all possible sources of commands
typedef enum {
    SRC_UART,
    SRC_ETHERNET
} source_id_t;

// The Universal Message Container
typedef struct {
    source_id_t source_id;
    char payload[256];
    uint16_t length;
} Message_t;

#endif // MESSAGE_H