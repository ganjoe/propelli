/*
 * mcp23.h
 *
 *  Created on: 01.10.2020
 *      Author: daniel
 */

#ifndef PROPELLI_MCP23_H_
#define PROPELLI_MCP23_H_
#include "datatypes_tasks.h"
#include "i2c.h"

typedef enum
    {
    INPUT,
    OUTPUT,
    PULLUP,
    HIGHZ,
    } MCP_PINMODE;

typedef struct
    {
	uint16_t addr;
	uint16_t pullups, direction, outlatch, inputstate;
	modflag mf_mcp23017;
	I2C_HandleTypeDef hi2c;
	int _dma_it_flag;
    } TD_MCP;

// interface
void mcp_PinMode	(TD_MCP *mcp_io, MCP_PINMODE pinmode, uint8_t pinnr);
int  mcp_ReadPin	(TD_MCP *mcp_io, uint8_t pinnr);
void mcp_WritePin	(TD_MCP *mcp_io, uint8_t pinnr, int state);
void mcp_ToggPin	(TD_MCP *mcp_io, uint8_t pinnr);
void mcp_set_all_input	(TD_MCP *mcp_io);

void mcp_WriteWord	(TD_MCP *mcp_io, uint16_t word);


// lowlevel
void mcp_lolWriteBuffer(TD_MCP *mcp_io, uint16_t *buffer, uint16_t addr);
void mcp_lolReadBuffer(TD_MCP *mcp_io, uint16_t *buffer, uint16_t addr);
// hardwarelevel
void mcp_lolWriteByte(TD_MCP *mcp_io, uint16_t regaddr, uint8_t data);
void mcp_lolReadByte(TD_MCP *mcp_io, uint16_t regaddr, uint8_t *data);
// modflag
void mfinit_mcp23017();
void mftick_mcp23017(modflag *thismf);
void mftask_mcp23017(TD_MCP *mcp_io);


#define REGISTER_IODIRA		0x00
#define REGISTER_GPPUA		0x0C
#define REGISTER_GPIOA		0x12
#define REGISTER_OLATA		0x14
#define MCP23017_PORTA		0x00

extern TD_MCP mcp_io;

#endif /* PROPELLI_MCP23_H_ */
