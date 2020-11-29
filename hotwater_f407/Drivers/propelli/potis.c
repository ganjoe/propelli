/*
 * potis.c
 *
 *  Created on: Sep 23, 2020
 *      Author: danie
 */

#include "potis.h"
#include "stdlib.h"
#include "utils.h"


POTIS analogchan;
uint16_t tbuff[24];

void mfinit_poti(POTIS *poti)
    {
/* buffer für bufflen*komplettscan
bsp: [chan1, chan2, chan3, chan1, chan2, chan3,...] usw*/
    modflag_init(&poti->mf_potiupd, HALTICK, 10);
    poti->inMin = 0;
    poti->inMax = 0xFFF;	//deadzone
    poti->outMin = 0;
    poti->outMax = 1;	//normieren
    poti->avg_samples = 8;	//pro channel
    poti->chan_count =3;
    poti->adc_lsb = 0xFFF;
    poti->bufflen =  (poti->avg_samples * poti->chan_count);	//zweidim-array

    poti->tbuff = calloc(poti->chan_count, sizeof(uint32_t));
    poti->buffer =calloc(poti->bufflen, sizeof(uint32_t));
   // poti->result =calloc(poti->chan_count, sizeof(float));
    poti->adc_n = hadc1;
    poti->mf_potiupd.rampcounter =0;
/* in CubeMx (bzw. in adc.h konfigurierte conversion-channel lesen
 * DMA  auf circular stellen*/
    HAL_ADC_Start_DMA(&poti->adc_n, tbuff,  poti->chan_count);
    }

void mftask_potis(POTIS *poti)
    {
    if (poti->mf_potiupd.flag && poti->mf_potiupd.init_done)
	{
	poti->mf_potiupd.repeat = modflag_tickdiff(&poti->mf_potiupd);
	poti->moditr = (poti->mf_potiupd.rampcounter % poti->avg_samples);
	poti->mf_potiupd.rampcounter++;

	for (int var = 0; var < poti->chan_count; ++var)
	    {
	    poti->buffer[(poti->moditr * poti->chan_count) + var] =tbuff[var];
	    }
	    db.tempmcu = potis_get_channel(&analogchan, MCUTEMP);
	    db.battvolt = potis_get_channel(&analogchan, VREFINT);
	    db.mcuvolt = potis_get_channel(&analogchan, MCUVOLT);

	poti->mf_potiupd.flag = false;
	poti->mf_potiupd.duration = modflag_tickdiff(&poti->mf_potiupd);
	}
    }

void mftick_potis(POTIS *poti)
    {
    modflag_upd_regular(&poti->mf_potiupd);
    }


float potis_get_channel(POTIS *poti,uint8_t channel_nr)
    {
    float sum=0;

	for (int var = 0; var < poti->bufflen; var += poti->chan_count)
	    {
	    sum += poti->buffer[var+channel_nr];
	    }
	/* gleitenen mittelwert für jeden channel berechnen*/
	sum = sum / (float)poti->avg_samples;
	//utils_truncate_number	(&sum, poti->inMin, poti->inMax);
	//utils_map_ptr		(&sum, poti->inMin, poti->inMax, poti->outMin,  poti->outMax);

    return sum;
    }











