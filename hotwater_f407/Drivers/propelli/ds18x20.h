/*
 * ds18x20.h
 *
 *  Created on: Oct 10, 2020
 *      Author: danie
 */

#ifndef PROPELLI_DS18X20_H_
#define PROPELLI_DS18X20_H_

#include "oneWire.h"
#include "datatypes_tasks.h"
#include "datatypes.h"

typedef enum
{
    FAHRENHEIT,
    CELSIUS,
    KELVIN,
}
TEMP_OFFSET;
typedef struct
{
    uint8_t 	presentFlag;
    uint8_t 	data[12];
    uint8_t 	addr[8];
    int32_t 	lastTempI;
    float 	lastTempF;
    modflag 	mf_tsensor;
    TEMP_OFFSET offset;
    ONEWIRE 	wire;
}
ds18x20;

void mfinit_tsensor(ds18x20 *tsensor);
void mftick_tsensor(ds18x20 *tsensor);
void mftask_tsensor(ds18x20 *tsensor);

int ts_read_single(ds18x20 *tsensor);
int ts_read_multi(ds18x20 *tsensor, char* romcode);

extern ds18x20 tsensor_cold, tsensor_hot;


#endif /* PROPELLI_DS18X20_H_ */
