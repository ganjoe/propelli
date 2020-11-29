/*
 * oneWire.c
 *
 *  Created on: 09.10.2020
 *      Author: daniel
 *  fork aus http://www.pjrc.com/teensy/td_libs_OneWire.html
 */

#include "oneWire.h"
#include "delay.h"
#include "main.h"


// Perform the onewire reset function.  We will wait up to 250uS for
// the bus to come high, if it doesn't then it is broken or shorted
// and we return a 0;
//
// Returns 1 if a device asserted a presence pulse, 0 otherwise.
//

void init_onewire(ONEWIRE *wire)
    {

    }
int oneWire_drivePin(ONEWIRE *wire, int state)
    {
    int readpin;

    if ((state == OW_OUTPUT_LOW) | (state == OW_OUTPUT_HIGH))
	{
	if ((wire->pinstate == OW_INPUT_HIGHZ)
		| (wire->pinstate == OW_INPUT_PULLUP))
	    {
	    wire->GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	    wire->GPIO_InitStruct.Pull = GPIO_NOPULL;
	    wire->GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init	(wire->onewire_port, &wire->GPIO_InitStruct);
	    HAL_GPIO_WritePin	(wire->onewire_port, wire->GPIO_InitStruct.Pin , state);
	    wire->pinstate = state;
	    return 1;
	    }
	else
	    {
	    HAL_GPIO_WritePin	(wire->onewire_port, wire->GPIO_InitStruct.Pin , state);
	    wire->pinstate = state;
	    return 1;
	    }
	}

    if (state == OW_INPUT_PULLUP)
	{
	if ((wire->pinstate == OW_INPUT_PULLUP) == 0)
	    {
	    wire->GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    wire->GPIO_InitStruct.Pull = GPIO_PULLUP;
	    wire->GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init	(wire->onewire_port, &wire->GPIO_InitStruct);
	    readpin = HAL_GPIO_ReadPin(wire->onewire_port, wire->GPIO_InitStruct.Pin);
	    wire->pinstate = state;
	    return readpin;
	    }
	else
	    {
	    wire->pinstate = state;
	    readpin = HAL_GPIO_ReadPin(wire->onewire_port, wire->GPIO_InitStruct.Pin);
	    return readpin;
	    }

	}
    if (state == OW_INPUT_HIGHZ)
	{
	if ((wire->pinstate == OW_INPUT_HIGHZ) == 0)
	    {
	    wire->GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	    wire->GPIO_InitStruct.Pull = GPIO_NOPULL;
	    wire->GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	    HAL_GPIO_Init	(wire->onewire_port, &wire->GPIO_InitStruct);
	    readpin = HAL_GPIO_ReadPin(wire->onewire_port, wire->GPIO_InitStruct.Pin);
	    wire->pinstate = state;
	    return readpin;
	    }
	else
	    {
	    wire->pinstate = state;
	    readpin = HAL_GPIO_ReadPin(wire->onewire_port, wire->GPIO_InitStruct.Pin);
	    return readpin;
	    }

	}
    return 0;
    }
// Perform the onewire reset function.  We will wait up to 250uS for
// the bus to come high, if it doesn't then it is broken or shorted
// and we return a 0;
//
// Returns 1 if a device asserted a presence pulse, 0 otherwise.00000000000000000000000000000000000000000000000000
int oneWire_reset(ONEWIRE *wire)
    {
    uint8_t response;
    oneWire_drivePin(wire, OW_INPUT_HIGHZ);
    uint32_t prim = __get_PRIMASK();
    __disable_irq();
    oneWire_drivePin(wire, OW_OUTPUT_LOW);	// drive output low

    delay_us(&delay, 480);
    oneWire_drivePin(wire, OW_INPUT_HIGHZ);	// allow it to float
    delay_us(&delay, 70);
    response = !oneWire_drivePin(wire, OW_INPUT_HIGHZ);
    delay_us(&delay, 410);

    if (!prim)
	{
	__enable_irq();
	}

    return response;

    }
void oneWire_WriteBit(ONEWIRE *wire, int bit)
    {
    uint32_t prim = __get_PRIMASK();
    __disable_irq();
    if (bit)
	{
	oneWire_drivePin(wire, OW_OUTPUT_LOW);
	delay_us(&delay, 10);
	oneWire_drivePin(wire, OW_INPUT_HIGHZ);	// drive output high
	delay_us(&delay, 55);
	}
    if (!bit)
	{
	oneWire_drivePin(wire, OW_OUTPUT_LOW);
	delay_us(&delay, 65);
	oneWire_drivePin(wire, OW_OUTPUT_HIGH);
	delay_us(&delay, 5);
	}
    if (!prim)
	{
	__enable_irq();
	}
    }
int oneWire_ReadBit(ONEWIRE *wire)
    {
    int response;
    uint32_t prim = __get_PRIMASK();
    __disable_irq();
    oneWire_drivePin(wire, OW_OUTPUT_LOW);
    delay_us(&delay, 3);
    oneWire_drivePin(wire, OW_INPUT_HIGHZ);// let pin float, pull up will raise
    delay_us(&delay, 10);
    response = oneWire_drivePin(wire, OW_INPUT_HIGHZ);
    delay_us(&delay, 53);
    if (!prim)
	{
	__enable_irq();
	}
    return response;
    }
// Write a byte. The writing code uses the active drivers to raise the
// pin high, if you need power after the write (e.g. DS18S20 in
// parasite power mode) then set 'power' to 1, otherwise the pin will
// go tri-state at the end of the write to avoid heating in a short or
// other mishap.
//
void oneWire_WriteByte(ONEWIRE *wire, uint8_t bit)
    {
    uint8_t bitMask;

    for (bitMask = 0x01; bitMask; bitMask <<= 1)
	{
	oneWire_WriteBit(wire, ((bitMask & bit) ? 1 : 0));
	}
    if (!wire->powerflag)
	{
	oneWire_drivePin(wire, OW_INPUT_HIGHZ);
	}
    }
void oneWire_WriteBytes(ONEWIRE *wire, const uint8_t *buf, uint16_t count)
    {
    for (uint16_t i = 0; i < count; i++)
	{
	oneWire_WriteByte(wire, buf[i]);
	}
    if (!wire->powerflag)
	{
	oneWire_drivePin(wire, OW_INPUT_HIGHZ);
	}
    }
int oneWire_ReadByte(ONEWIRE *wire)
    {
    uint8_t bitMask;
    uint8_t response = 0;
    for (bitMask = 0x01; bitMask; bitMask <<= 1)
	{
	if (oneWire_ReadBit(wire))
	    {
	    response |= bitMask;
	    }
	}
    return response;
    }
void oneWire_ReadBytes(ONEWIRE *wire, uint8_t *buf, uint16_t count)
    {
    for (uint16_t i = 0; i < count; i++)
	buf[i] = oneWire_ReadByte(wire);
    }
void oneWire_ROMskip(ONEWIRE *wire)
    {
    oneWire_WriteByte(wire, 0xCC);           // Skip ROM
    }
void oneWire_noPower(ONEWIRE *wire)
    {
    oneWire_drivePin(wire, OW_INPUT_HIGHZ);
    }
void oneWire_SelROM(ONEWIRE *wire, const uint8_t rom[8])
    {
    uint8_t i;
    oneWire_WriteByte(wire, 0x55);           // Choose ROM
    for (i = 0; i < 8; i++)
	{
	oneWire_WriteByte(wire, rom[i]);
	}
    }
void oneWire_rstSearch(ONEWIRE *wire)
    {
    // reset the search state
    wire->LastDiscrepancy = 0;
    wire->LastDeviceFlag = 0;
    wire->LastFamilyDiscrepancy = 0;
    for (int i = 7;; i--)
	{
	wire->ROM_NO[i] = 0;
	if (i == 0)
	    {
	    break;
	    }
	}
    }
// Setup the search to find the device type 'family_code' on the next call
// to search(*newAddr) if it is present.
//
void oneWire_setSearch(ONEWIRE *wire, uint8_t family_code)
    {
    // set the search state to find SearchFamily type devices
    wire->ROM_NO[0] = family_code;
    for (uint8_t i = 1; i < 8; i++)
	{
	wire->ROM_NO[i] = 0;
	}
    wire->LastDiscrepancy = 64;
    wire->LastFamilyDiscrepancy = 0;
    wire->LastDeviceFlag = 0;
    }
// Perform a search. If this function returns a '1' then it has
// enumerated the next device and you may retrieve the ROM from the
// OneWire::address variable. If there are no devices, no further
// devices, or something horrible happens in the middle of the
// enumeration then a 0 is returned.  If a new device is found then
// its address is copied to newAddr.  Use OneWire::reset_search() to
// start over.
//
// --- Replaced by the one from the Dallas Semiconductor web site ---
//--------------------------------------------------------------------------
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
int oneWire_search(ONEWIRE *wire, uint8_t *newAddr, uint8_t search_mode)
    {
    uint8_t id_bit_number;
    uint8_t last_zero, rom_byte_number;
    uint8_t search_result;
    uint8_t id_bit, cmp_id_bit;

    unsigned char rom_byte_mask, search_direction;

    // initialize for search
    id_bit_number = 1;
    last_zero = 0;
    rom_byte_number = 0;
    rom_byte_mask = 1;
    search_result = false;

    // if the last call was not the last one
    if (!wire->LastDeviceFlag)
	{
	// 1-Wire reset
	if (!oneWire_reset(wire))
	    {
	    // reset the search
	    wire->LastDiscrepancy = 0;
	    wire->LastDeviceFlag = 0;
	    wire->LastFamilyDiscrepancy = 0;
	    return 0;
	    }

	// issue the search command
	if (search_mode == 1)
	    {
	    oneWire_WriteByte(wire, 0xF0);   // NORMAL SEARCH
	    }
	else
	    {
	    oneWire_WriteByte(wire, 0xEC);   // CONDITIONAL SEARCH
	    }

	// loop to do the search
	do
	    {
	    // read a bit and its complement
	    id_bit = oneWire_ReadBit(wire);
	    cmp_id_bit = oneWire_ReadBit(wire);

	    // check for no devices on 1-wire
	    if ((id_bit == 1) && (cmp_id_bit == 1))
		{
		break;
		}
	    else
		{
		// all devices coupled have 0 or 1
		if (id_bit != cmp_id_bit)
		    {
		    search_direction = id_bit;  // bit write value for search
		    }
		else
		    {
		    // if this discrepancy if before the Last Discrepancy
		    // on a previous next then pick the same as last time
		    if (id_bit_number < wire->LastDiscrepancy)
			{
			search_direction = ((wire->ROM_NO[rom_byte_number]
				& rom_byte_mask) > 0);
			}
		    else
			{
			// if equal to last pick 1, if not then pick 0
			search_direction = (id_bit_number
				== wire->LastDiscrepancy);
			}
		    // if 0 was picked then record its position in LastZero
		    if (search_direction == 0)
			{
			last_zero = id_bit_number;

			// check for Last discrepancy in family
			if (last_zero < 9)
			    wire->LastFamilyDiscrepancy = last_zero;
			}
		    }

		// set or clear the bit in the ROM byte rom_byte_number
		// with mask rom_byte_mask
		if (search_direction == 1)
		    {
		    wire->ROM_NO[rom_byte_number] |= rom_byte_mask;
		    }
		else
		    {
		    wire->ROM_NO[rom_byte_number] &= ~rom_byte_mask;
		    }

		// serial number search direction write bit
		oneWire_WriteBit(wire, search_direction);

		// increment the byte counter id_bit_number
		// and shift the mask rom_byte_mask
		id_bit_number++;
		rom_byte_mask <<= 1;

		// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
		if (rom_byte_mask == 0)
		    {
		    rom_byte_number++;
		    rom_byte_mask = 1;
		    }
		}
	    }
	while (rom_byte_number < 8);  // loop until through all ROM bytes 0-7

	// if the search was successful then
	if (!(id_bit_number < 65))
	    {
	    // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
	    wire->LastDiscrepancy = last_zero;

	    // check for last device
	    if (wire->LastDiscrepancy == 0)
		{
		wire->LastDeviceFlag = 1;
		}
	    search_result = 1;
	    }
	}

    // if no device found then reset counters so next 'search' will be like a first
    if (!search_result || !wire->ROM_NO[0])
	{
	wire->LastDiscrepancy = 0;
	wire->LastDeviceFlag = 0;
	wire->LastFamilyDiscrepancy = 0;
	search_result = 0;
	}
    else
	{
	for (int i = 0; i < 8; i++)
	    {
	    newAddr[i] = wire->ROM_NO[i];
	    }
	}
    return search_result;
    }

ONEWIRE wire, wire1;

