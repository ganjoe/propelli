/*
 * potis.h
 *
 *  Created on: Sep 23, 2020
 *      Author: danie
 */

#ifndef POTIS_H_
#define POTIS_H_

#include "datatypes_tasks.h"//für modflag typedef

#include "stdbool.h"
#include "stdint.h"
#include "adc.h"

typedef enum
{
    MCUTEMP=0,
    VREFINT,
    VBATT,
}
potichannel;

typedef struct
{
    uint32_t chan_count;
    potichannel potichan;
    uint16_t adc_lsb;
    uint32_t *buffer, *tbuff;
    uint32_t bufflen,moditr,avg_samples;
    float inMin, inMax, outMin, outMax;
    float *result;
    ADC_HandleTypeDef adc_n;
    modflag mf_potiupd;

}POTIS;

void mfinit_poti(POTIS *poti);
void mftask_potis(POTIS *poti);
void mftick_potis(POTIS *poti);
float potis_get_channel(POTIS *poti,uint8_t channel_nr);

extern POTIS analogchan;



#endif /* POTIS_H_ */
