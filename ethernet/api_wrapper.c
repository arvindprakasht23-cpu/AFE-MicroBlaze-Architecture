/*
 * api_wrapper.c
 *
 * Created on: Apr 7, 2026
 * Author: PSG_TI_TEAM
 */

#include <string.h>
#include "api_wrapper.h"
#include "../axi_regs.h"
#include "afe_drivers.h"

#define MAX_BURST_SIZE 64

/* ------------------------------------------------------------------ */
/* Write wrappers — no result register written                        */
/* ------------------------------------------------------------------ */

u16 api_afeSpiRawWrite_wrapper(volatile u8 *operands) {
    Cmd_spiRawWrite_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(args));
    return (u16)afeSpiRawWrite(args.afeInst, args.addr, args.data);
}

u16 api_afeSpiBurstWrite_wrapper(volatile u8 *operands) {
    Cmd_spiBurstWrite_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(args));
    if (args.dataArraySize > MAX_BURST_SIZE) {
        args.dataArraySize = MAX_BURST_SIZE;
    }
    uint8_t data[MAX_BURST_SIZE];
    memcpy(data, (const void *)&operands[sizeof(args)], args.dataArraySize);
    return (u16)afeSpiBurstWrite(args.afeInst, args.addr, data, args.dataArraySize);
}

u16 api_afeSpiRawWriteMulti_wrapper(volatile u8 *operands) {
    Cmd_spiRawWriteMulti_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(args));
    return (u16)afeSpiRawWriteMulti(args.afeInstSel, args.addr, args.data);
}

u16 api_afeSpiBurstWriteMulti_wrapper(volatile u8 *operands) {
    Cmd_spiBurstWriteMulti_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(args));
    if (args.dataArraySize > MAX_BURST_SIZE) {
        args.dataArraySize = MAX_BURST_SIZE;
    }
    uint8_t data[MAX_BURST_SIZE];
    memcpy(data, (const void *)&operands[sizeof(args)], args.dataArraySize);
    return (u16)afeSpiBurstWriteMulti(args.afeInstSel, args.addr, data, args.dataArraySize);
}

/* ------------------------------------------------------------------ */
/* Read wrappers — result register layout:                            */
/* */
/* Raw  read: [uint8_t readVal]                                     */
/* Burst read: [uint16_t dataArraySize][uint8_t data[dataArraySize]]*/
/* ------------------------------------------------------------------ */

u16 api_afeSpiRawRead_wrapper(volatile u8 *operands) {
    Cmd_spiRawRead_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(args));

    /* Result: single byte at HW_RESULT_BASE */
    Result_spiRawRead_t *result = (Result_spiRawRead_t *)HW_RESULT_BASE;
    return (u16)afeSpiRawRead(args.afeInst, args.addr, &result->readVal);
}

u16 api_afeSpiBurstRead_wrapper(volatile u8 *operands) {
    Cmd_spiBurstRead_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(args));

    if (args.dataArraySize > MAX_BURST_SIZE) {
        args.dataArraySize = MAX_BURST_SIZE;
    }

    /*
     * Result: dataArraySize written first so the host knows how many
     * bytes to collect, followed immediately by the data bytes.
     */
    Result_spiBurstRead_t *result = (Result_spiBurstRead_t *)HW_RESULT_BASE;
    result->dataArraySize = args.dataArraySize;
    return (u16)afeSpiBurstRead(args.afeInst, args.addr, args.dataArraySize, result->data);
}

u16 api_afeSpiRawReadMulti_wrapper(volatile u8 *operands) {
    Cmd_spiRawReadMulti_Args_t args;
    memcpy(&args, (const void *)operands, sizeof(args));

    Result_spiRawReadMulti_t *result = (Result_spiRawReadMulti_t *)HW_RESULT_BASE;
        return (u16)afeSpiRawReadMulti(args.afeInstSel, args.addr, result->readVal);
}

/* ------------------------------------------------------------------ */
/* API dispatch table — order must match opcode_t enum                */
/* ------------------------------------------------------------------ */

api_func_ptr api_table[API_TABLE_SIZE] = {
    api_afeSpiRawWrite_wrapper,      /* 0: OPCODE_SPI_RAW_WRITE        */
    api_afeSpiBurstWrite_wrapper,    /* 2: OPCODE_SPI_BURST_WRITE      */
    api_afeSpiRawWriteMulti_wrapper, /* 4: OPCODE_SPI_RAW_WRITE_MULTI  */
    api_afeSpiBurstWriteMulti_wrapper, /* 6: OPCODE_SPI_BURST_WRITE_MULTI*/
    api_afeSpiRawRead_wrapper,       /* 1: OPCODE_SPI_RAW_READ         */
    api_afeSpiBurstRead_wrapper,     /* 3: OPCODE_SPI_BURST_READ       */
    api_afeSpiRawReadMulti_wrapper,  /* 5: OPCODE_SPI_RAW_READ_MULTI   */
};
