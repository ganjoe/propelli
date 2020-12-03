/*
 * ina219_monitor.c
 *
 *  Created on: Nov 26, 2020
 *      Author: danie
 */

#ifndef PROPELLI_INA219_MONITOR_C_
#define PROPELLI_INA219_MONITOR_C_

#include "ina219_monitor.h"
#include "i2c.h"
#include "utils.h"

void mfinit_ina219	(TD_INA219 *batt_hw)
    {
    modflag_init(&batt_hw->mf_ina219, SYSTICK, 20);
    batt_hw->hi2c = hi2c1;
    batt_hw->addr = 0x40 << 1;
    //bits im configbuffer werden nur durch reset gelöscht

    batt_hw->shunt = 0.1;
    batt_hw->max_current = 2;

    ina_setup(batt_hw);
    batt_hw->mf_ina219.init_done=1;

    }
void mftick_ina219	(TD_INA219 *batt_hw)
    {
    modflag_upd_regular(&batt_hw->mf_ina219);
    }
void mftask_ina219	(TD_INA219 *batt_hw)
    {
    if (batt_hw->mf_ina219.flag && batt_hw->mf_ina219.init_done)
		{
		batt_hw->mf_ina219.repeat = modflag_tickdiff(&batt_hw->mf_ina219);
		batt_hw->mf_ina219.rampcounter = batt_hw->mf_ina219.callcount % 3;


		if (batt_hw->mf_ina219.rampcounter==0)
			{
			ina_getvoltage(batt_hw);
			}
		if (batt_hw->mf_ina219.rampcounter==1)
			{
			ina_getcurrent(batt_hw);
			}
		if (batt_hw->mf_ina219.rampcounter==2)
			{
			ina_getpower(batt_hw);
			 }

		batt_hw->mf_ina219.duration = modflag_tickdiff(&batt_hw->mf_ina219);
		batt_hw->mf_ina219.callcount++;
		batt_hw->mf_ina219.flag = false;
		}
    if(!batt_hw->mf_ina219.init_done)
    	{
    	 batt_hw->pwerbuff = -1;
    	 batt_hw->voltbuff = -1;
    	 batt_hw->currbuff = -1;
    	}
    }

void ina_setup		(TD_INA219 *batt_hw)
    {
    ina_writeconfig(batt_hw, INA219_RESET);
    ina_writeconfig(batt_hw, INA219_SAMPLES_128);
    ina_writeconfig(batt_hw, INA219_RANGE_320mV);
    ina_writeconfig(batt_hw, INA219_SBVOLT_CONT);

    ina_lol_calc_cal(batt_hw);

    }
void ina_lol_calc_cal	(TD_INA219 *batt_hw)
    {

    int16_t oldconfig, newconfig;
    batt_hw->busvolt_lsb = 0.004;
    batt_hw->current_lsb = batt_hw->max_current / 0x8000;
    newconfig = 0.04096 /(batt_hw->current_lsb   * batt_hw->shunt);
    batt_hw->calibration =newconfig;
    batt_hw->power_lsb = batt_hw->current_lsb * 20;

    //der ic speichert das LSByte vom calibration-register immer als 0

    ina_lolwriteword(batt_hw, newconfig, CALIBRATION);
    HAL_Delay(1);
    ina_lolreadword(batt_hw, &oldconfig, CALIBRATION);
	if (oldconfig != newconfig)	{ batt_hw->mf_ina219.flag_reset = true; }

    }

void ina_writeconfig	(TD_INA219 *batt_hw, uint16_t config)
    {
    uint16_t oldconfig, newconfig;
    int16_t  temp;
    oldconfig = batt_hw->configbuff;
    newconfig = oldconfig | config;

    if (config == INA219_RESET)
	{
	// ic-zustand ina219 nach schreiben von 0x8000 ist 0x399F
	ina_lolwriteword 	(batt_hw, config, CONFIG);
	ina_lolreadword		(batt_hw, &temp, CONFIG);

	if (temp != config)
	    {
	    batt_hw->mf_ina219.flag_reset = true;
	    }
	else
	    {

	    batt_hw->configbuff = temp;
	    batt_hw->mf_ina219.flag_reset = false;
	    }
	}
    else
	{
	ina_lolwriteword 	(batt_hw, newconfig, CONFIG);
	ina_lolreadword		(batt_hw, &temp, CONFIG);
	if (temp != newconfig)	{ batt_hw->mf_ina219.flag_reset = true; }
	else 			{ batt_hw->configbuff = newconfig; }
	}
    }

float ina_getpower	(TD_INA219 *batt_hw)
    {
	HAL_StatusTypeDef err;
    int16_t powerbuff;
    float power;
    err = ina_lolreadword(batt_hw, &powerbuff, POWER);
    power = (float)powerbuff;
    batt_hw->pwerbuff = power * batt_hw->power_lsb;
    return power;
    }
float ina_getcurrent	(TD_INA219 *batt_hw)
    {
	HAL_StatusTypeDef err;
    int16_t currbuff;
    float current;
    err = ina_lolreadword(batt_hw, &currbuff, CURRENT);
    current = (float)currbuff * batt_hw->current_lsb;
    batt_hw->currbuff = current;
    return current;
    }
float ina_getvoltage	(TD_INA219 *batt_hw)
    {
	HAL_StatusTypeDef err;
    int16_t voltage;
    err = ina_lolreadword(batt_hw, &voltage, BUS);
    voltage = voltage >>3;
    batt_hw->voltbuff = (float)voltage*(float)0.004;
   // batt_hw->voltbuff = voltage * batt_hw->calibration;
    return voltage;
    }

HAL_StatusTypeDef ina_lolwriteword	(TD_INA219 *batt_hw, uint16_t buffer, uint8_t reg)
    {
    HAL_StatusTypeDef err;
    uint8_t dword[2];
    uint8_t lowbyte,highbyte;
    utils_get_bytes_in_word(&buffer, &lowbyte, &highbyte);

    dword[0]=highbyte;
    dword[1]=lowbyte;

    err = HAL_I2C_Mem_Write(&batt_hw->hi2c, batt_hw->addr, reg, 1, dword, 2, HAL_TIMEOUT);
    if(err == !HAL_OK)
    	{
    	batt_hw->mf_ina219.init_done = 0;
    	}
    return err;
    }
HAL_StatusTypeDef ina_lolreadword 	(TD_INA219 *batt_hw, int16_t *buffer, uint8_t reg)
    {
    HAL_StatusTypeDef xfer;
    uint8_t dword[2];

    /* read_dma oder _it brauchen variablen die auch nach verlassen der funktion beschrieben werden können */
    xfer = HAL_I2C_Mem_Read(&batt_hw->hi2c, batt_hw->addr, reg, 1, dword, 2, HAL_TIMEOUT);
    *buffer = utils_get_word_from_bytes(dword[0], dword[1], 1);
    if(xfer != HAL_OK)
    	{
    	batt_hw->mf_ina219.init_done = 0;
    	}

    return xfer;
    }


TD_INA219 batt_hw;

#endif /* PROPELLI_INA219_MONITOR_C_ */
