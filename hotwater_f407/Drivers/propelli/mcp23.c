/*
 * mcp23.c
 *
 *  Created on: 01.10.2020
 *      Author: daniel
 */
#include"mcp23.h"
#include "main.h"
#include "utils.h"
#include "i2c.h"
#include "delay.h"
TD_MCP mcp_io = {0};

void mfinit_mcp23017()
    {
    modflag_init(&mcp_io.mf_mcp23017, SYSTICK, 10);
    mcp_io.hi2c = hi2c1;
    mcp_io.addr = 0x20 << 1;

    mcp_PinMode(&mcp_io, OUTPUT, 1);
    mcp_io.outlatch = 0xFFFF;	// alle lampen an
    //AddressIOCON.BANK = 0 - default
    }
void mftick_mcp23017(modflag *thismf)
    {
    modflag_upd_regular(thismf);
    }
void mftask_mcp23017(TD_MCP *io)
    {
    if (io->mf_mcp23017.flag && io->mf_mcp23017.init_done)
	{
	io->mf_mcp23017.repeat = modflag_tickdiff(&io->mf_mcp23017);
	io->mf_mcp23017.rampcounter = io->mf_mcp23017.callcount % 4;
	    //mcpToggPin(&mcp_io, 0);

	if (io->mf_mcp23017.rampcounter==0)
	    {
	    mcp_lolWriteBuffer(io, &io->direction, REGISTER_IODIRA);
	    }
	if (io->mf_mcp23017.rampcounter==1)
	    {
	    mcp_lolWriteBuffer(io, &io->outlatch, REGISTER_OLATA);
	    }
	if (io->mf_mcp23017.rampcounter==2)
	    {
	    mcp_lolWriteBuffer(io, &io->pullups, REGISTER_GPPUA);
	     }
	if (io->mf_mcp23017.rampcounter==3)
	    {
	    mcp_lolReadBuffer(io, &io->inputstate, REGISTER_GPIOA);
	    }

	io->mf_mcp23017.duration = modflag_tickdiff(&io->mf_mcp23017);
	io->mf_mcp23017.callcount++;
	io->mf_mcp23017.flag = false;
	}
    if (!io->mf_mcp23017.init_done)
    {
    	db.iostatus=666;
    }
    }
/*
 * die aufsteigenden registeradressen vom mcp
 * enthalten abwechselnd die jewailige konfig
 * für port A und B.
 * dieses default-verhalten ist AddressIOCON.BANK = 0
 */


void mcp_WritePin	(TD_MCP *mcp_io, uint8_t pinnr, int state)
    {
    utils_set_bit_in_Word(&mcp_io->outlatch, pinnr, (bool) state);
    }
void mcp_ToggPin	(TD_MCP *io, uint8_t pinnr)
    {
    utils_togg_bit_in_Word(&io->outlatch, pinnr);
    }
int  mcp_ReadPin	(TD_MCP *io, uint8_t pinnr)
    {
    return utils_get_bit_in_Word(&io->inputstate, pinnr);
    }
void mcp_PinMode	(TD_MCP *io, MCP_PINMODE pinmode, uint8_t pinnr)
    {
    switch (pinmode)
	{
    case PULLUP:
	{
	utils_set_bit_in_Word(&io->direction, pinnr, true);
	utils_set_bit_in_Word(&io->pullups, pinnr, true);
	}
	break;
    case INPUT:
	{
	utils_set_bit_in_Word(&io->direction, pinnr, true);
	utils_set_bit_in_Word(&io->pullups, pinnr, false);
	}
	break;
    case OUTPUT:
	{
	utils_set_bit_in_Word(&io->direction, pinnr, false);
	utils_set_bit_in_Word(&io->pullups, pinnr, false);
	}

	break;
	}
    //When a bit is set, the corresponding pin becomes aninput.
	//mcp_lolWriteBuffer(io, &io->direction, REGISTER_IODIRA);

    //A read from this register results in a read of theOLAT  and  not  the  port  itself.
    //A  write  to  this  registermodifies  the  output  latches  that  modifies  the  pinsconfigured as outputs
	//mcp_lolWriteBuffer(io, &io->outlatch, REGISTER_OLATA);

    //If  a  bit  is  set  and  the  corresponding  pin  is configured  as  an  input,
    //the  corresponding  port  pin  isinternally pulled up with a 100 k resistor.
	//mcp_lolWriteBuffer(io, &io->pullups, REGISTER_GPPUA);

    }

void mcp_set_all_input	(TD_MCP *io)
    {
    io->direction = 0xFFFF;
    io->pullups = 0x0;
    }

void mcp_WriteWord	(TD_MCP *io, uint16_t word)
    {
    io->pullups = 0;
    io->direction = !word;
    io->outlatch=word;
    }

void mcp_lolWriteBuffer	(TD_MCP *mcp_io, uint16_t *buffer, uint16_t addr)
    {
    uint16_t dword = *buffer;
    uint8_t highByte = dword >> 8;
    uint8_t lowByte = dword & 0x00FF;
    mcp_lolWriteByte(mcp_io, addr, lowByte);
    mcp_lolWriteByte(mcp_io, addr + 1, highByte);
    }
void mcp_lolReadBuffer	(TD_MCP *mcp_io, uint16_t *buffer, uint16_t addr)
    {
    uint16_t dword;
    uint16_t *ptr = &dword;
    uint8_t highByte, lowByte;
    mcp_lolReadByte(mcp_io, addr, 	&lowByte);
    mcp_lolReadByte(mcp_io, addr + 1, 	&highByte);

    dword = highByte << 8;
    dword |= lowByte;
    *buffer = *ptr;

    }
void mcp_lolWriteByte	(TD_MCP *io, uint16_t regaddr, uint8_t data)
    {
    HAL_StatusTypeDef complete = !HAL_OK;
    complete = HAL_I2C_Mem_Write_DMA(&hi2c1, io->addr, regaddr, 1, &data, 1);
    if (complete == !HAL_OK)
    	{
    	io->mf_mcp23017.init_done = false;
    	}
    }
void mcp_lolReadByte	(TD_MCP *io, uint16_t regaddr, uint8_t *data)
    {
    uint8_t localbuff;
    HAL_StatusTypeDef complete = !HAL_OK;
	//complete =  HAL_I2C_Mem_Read_DMA(&hi2c1, io->addr, &regaddr, 1, &localbuff, 1);
	complete =  HAL_I2C_Master_Transmit(&hi2c1, io->addr, &regaddr, 1, HAL_TIMEOUT);
	complete = HAL_I2C_Master_Receive(&hi2c1, io->addr, &localbuff, 1, HAL_TIMEOUT);
    if (complete == !HAL_OK)
    	{
    	io->mf_mcp23017.init_done = false;
    	}
    *data = localbuff;
    }
