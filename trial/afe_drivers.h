/**
 * @file afe_drivers.h
 * @brief Texas Instruments Analog Front End (AFE) Hardware Drivers
 * * Note: In a production environment, this file is provided by the 
 * hardware vendor and should act as the single source of truth.
 */

#ifndef AFE_DRIVERS_H
#define AFE_DRIVERS_H

#include <stdint.h>

// --- Return Type Enumeration ---
typedef enum RET_TYPE
{
    TI_AFE_RET_EXEC_PASS = 0,
    TI_AFE_RET_EXEC_FAIL = 1
} RetType_e;

#define NUM_SPI 4

// --- Standard Single-Instance SPI Commands ---

// Write a single byte to a specific AFE instance and address
uint32_t afeSpiRawWrite(uint8_t afeInst, uint16_t addr, uint8_t data);

// Read a single byte from a specific AFE instance and address
uint32_t afeSpiRawRead(uint8_t afeInst, uint16_t addr, uint8_t *readVal);

// Write an array of bytes to a specific AFE instance starting at an address
uint32_t afeSpiBurstWrite(uint8_t afeInst, uint16_t addr, uint16_t dataArraySize, uint8_t *data);

// Read an array of bytes from a specific AFE instance starting at an address
uint32_t afeSpiBurstRead(uint8_t afeInst, uint16_t addr, uint16_t dataArraySize, uint8_t *data);


// --- Multi-Instance Broadcast SPI Commands ---

// Broadcast a single byte write to multiple AFE instances
uint32_t afeSpiRawWriteMulti(uint8_t afeInstSel, uint16_t addr, uint8_t data);

// Read a single byte from multiple AFE instances (returns aggregated or specific result)
uint32_t afeSpiRawReadMulti(uint8_t afeInstSel, uint16_t addr, uint8_t *readVal);

// Broadcast an array of bytes to multiple AFE instances
uint32_t afeSpiBurstWriteMulti(uint8_t afeInstSel, uint16_t addr, uint16_t dataArraySize, uint8_t *data);


#endif // AFE_DRIVERS_H