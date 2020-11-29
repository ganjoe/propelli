/*
 * oneWire.h
 *
 *  Created on: 09.10.2020
 *      Author: daniel
 */

#ifndef PROPELLI_ONEWIRE_H_
#define PROPELLI_ONEWIRE_H_
#include "gpio.h"
#include "datatypes_tasks.h"

#define OW_OUTPUT_LOW 0
#define OW_OUTPUT_HIGH 1
#define OW_INPUT_HIGHZ 2
#define OW_INPUT_PULLUP 3

typedef struct
{
    GPIO_InitTypeDef 	GPIO_InitStruct;
    GPIO_TypeDef*	onewire_port;
    uint8_t	 	pinstate, bufflen, powerflag;
    uint8_t* 		writebuffer;
    uint8_t 		ROM_NO[8];
    uint8_t 		LastDiscrepancy,
			LastFamilyDiscrepancy,
			LastDeviceFlag;
}
ONEWIRE;
void init_onewire	(ONEWIRE *wire);
int oneWire_drivePin	(ONEWIRE *wire, int state);
int oneWire_reset	(ONEWIRE *wire);

void oneWire_WriteBit	(ONEWIRE *wire, int bit);
void oneWire_WriteByte	(ONEWIRE *wire, uint8_t byte);
void oneWire_WriteBytes	(ONEWIRE *wire, const uint8_t *buf,uint16_t count);

int oneWire_ReadBit	(ONEWIRE *wire);
int oneWire_ReadByte	(ONEWIRE *wire);
void oneWire_ReadBytes	(ONEWIRE *wire,uint8_t *buf, uint16_t count);

void oneWire_ROMskip	(ONEWIRE *wire);
void oneWire_SelROM	(ONEWIRE *wire,const uint8_t rom[8]);
void oneWire_noPower	(ONEWIRE *wire);

void oneWire_rstSearch	(ONEWIRE *wire);
void oneWire_setSearch	(ONEWIRE *wire, uint8_t family_code);
int oneWire_search	(ONEWIRE *wire, uint8_t *newAddr, uint8_t search_mode);
extern ONEWIRE wire, wire1;

#endif /* PROPELLI_ONEWIRE_H_ */
