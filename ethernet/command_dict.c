#include "command_dict.h"
#include "api_wrapper.h"

const command_meta_t cmd_dict[] = {
    {"spiRawWrite",        OPCODE_RAW_WRITE,         3, {ARG_U8, ARG_U16, ARG_U8, ARG_NONE}},
    {"spiRawRead",         OPCODE_RAW_READ,          2, {ARG_U8, ARG_U16, ARG_NONE}},
    {"spiBurstWrite",      OPCODE_BURST_WRITE,       4, {ARG_U8, ARG_U16, ARG_U16, ARG_ARRAY_U8, ARG_NONE}},
    {"spiBurstRead",       OPCODE_BURST_READ,        3, {ARG_U8, ARG_U16, ARG_U16, ARG_NONE}},
    {"spiRawWriteMulti",   OPCODE_RAW_WRITE_MULTI,   3, {ARG_U8, ARG_U16, ARG_U8, ARG_NONE}},
    {"spiRawReadMulti",    OPCODE_RAW_READ_MULTI,    2, {ARG_U8, ARG_U16, ARG_NONE}},
    {"spiBurstWriteMulti", OPCODE_BURST_WRITE_MULTI, 4, {ARG_U8, ARG_U16, ARG_U16, ARG_ARRAY_U8, ARG_NONE}}
};

const int DICT_SIZE = sizeof(cmd_dict) / sizeof(cmd_dict[0]);