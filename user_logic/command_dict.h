/*
 * command_dict.h
 * Author: PSG_TI_TEAM
 *
 * Command dictionary structures for the UART parser.
 */

#ifndef COMMAND_DICT_H
#define COMMAND_DICT_H

#include "xil_types.h"

/* Allowed data types for parsed command arguments */
typedef enum {
    ARG_NONE = 0,    /* No argument / End of argument list */
    ARG_U8,          /* 8-bit unsigned integer (1 byte) */
    ARG_U16,         /* 16-bit unsigned integer (2 bytes) */
    ARG_U32,         /* 32-bit unsigned integer (4 bytes) */
    ARG_ARRAY_U8     /* Variable-length array of 8-bit integers (Burst data) */
} arg_type_t;

#define MAX_ARGS 5

/* Dictionary entry mapping a string command to its physical execution profile */
typedef struct {
    const char *cmd_string;
    uint8_t opcode;
    uint8_t min_args;
    arg_type_t arg_format[MAX_ARGS];
} command_meta_t;

/* Externally Linked Auto-Generated Data */
extern const command_meta_t cmd_dict[];
extern const int DICT_SIZE;

#endif // COMMAND_DICT_H
