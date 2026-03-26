#ifndef COMMAND_DICT_H
#define COMMAND_DICT_H

#include <stdint.h>

#define MAX_ARGS 10

typedef enum {
    ARG_NONE,
    ARG_U8,
    ARG_U16,
    ARG_ARRAY_U8
} arg_type_t;

typedef struct {
    const char *cmd_string;
    uint8_t opcode;
    uint8_t min_args;
    arg_type_t arg_format[MAX_ARGS];
} command_meta_t;

extern const command_meta_t cmd_dict[];
extern const int DICT_SIZE;

#endif