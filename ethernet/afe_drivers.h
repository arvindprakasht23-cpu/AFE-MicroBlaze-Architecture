/*
 * afe_drivers.h
 * Author: PSG_TI_TEAM
 * Hardware abstraction layer prototypes for TI AFE SPI communication.
 *
 * Defines the standard API for interacting with the AFE chips. Supports 
 * single-instance targeting and multi-instance bitmask broadcasting.
 */

#ifndef AFE_DRIVERS_H
#define AFE_DRIVERS_H

#include <stdint.h>

/* Standard execution return codes for the hardware drivers. */
typedef enum RET_TYPE
{
    TI_AFE_RET_EXEC_PASS = 0, /* Hardware execution completed successfully */
    TI_AFE_RET_EXEC_FAIL = 1  /* Hardware execution failed or timed out */
} RetType_e;

/* The total number of physical SPI devices connected to the bus */
#define NUM_SPI 8

/* --- OFFICIAL DRIVER PROTOTYPES --- */

/* Writes a single byte to a specific register on a single AFE instance. */
uint32_t afeSpiRawWrite(uint8_t afeInst, uint16_t addr, uint8_t data);

/* Reads a single byte from a specific register on a single AFE instance. */
uint32_t afeSpiRawRead(uint8_t afeInst, uint16_t addr, uint8_t *readVal);

/* Writes a sequential block of memory to a single AFE instance. */
uint32_t afeSpiBurstWrite(uint8_t afeInst, uint16_t addr, uint8_t *data, uint16_t dataArraySize);

/* Reads a sequential block of memory from a single AFE instance. */
uint32_t afeSpiBurstRead(uint8_t afeInst, uint16_t addr, uint16_t dataArraySize, uint8_t *data);

/* Broadcasts a single byte to multiple AFE instances simultaneously. */
uint32_t afeSpiRawWriteMulti(uint8_t afeInstSel, uint16_t addr, uint8_t data);

/*
 * Reads a single byte from multiple AFE instances based on a selection bitmask.
 * Architecture uses Direct Index Mapping. The hardware returns an array of size NUM_SPI.
 * If bit 'i' is set in afeInstSel, readVal[i] contains the valid data.
 * If bit 'i' is 0, readVal[i] is zeroed out to prevent stale data ghosting.
 */
uint32_t afeSpiRawReadMulti(uint8_t afeInstSel, uint16_t addr, uint8_t *readVal);

/* Broadcasts a sequential memory block to multiple AFE instances. */
uint32_t afeSpiBurstWriteMulti(uint8_t afeInstSel, uint16_t addr, uint8_t *data, uint16_t dataArraySize);

#endif