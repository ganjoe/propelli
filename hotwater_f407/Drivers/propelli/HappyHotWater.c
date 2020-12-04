/*
 * HappyHotWater.c
 *
 *  Created on: 03.12.2020
 *      Author: daniel
 */

#include "HappyHotWater.h"


void mftask_happyhotwater	(TD_HappyHotwater* hhw,	Valuebuffer* db)
{
	/***********/

	if (db->iostatus == hhw->outwords[BTN_KALT])
		{
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SPUELE, true);
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SHOWER , true);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SPUELE, false);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SHOWER, false);
		}
	if (db->iostatus == hhw->outwords[BTN_WARM])
		{
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SPUELE, false);
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SHOWER , false);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SPUELE, true);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SHOWER, true);
		}
	if (db->batthwvolt > hhw->VoltLevel_pwrok)
		{
		utils_set_bit_in_Word(&hhw->states, STDBY_INV, true);
		utils_set_bit_in_Word(&hhw->states, STDBY_LV, false);
		}
	else
		{
		utils_set_bit_in_Word(&hhw->states, STDBY_INV, false);
		}
	if (db->batthwvolt < hhw->VoltLevel_lowbatt)
		{
		utils_set_bit_in_Word(&hhw->states, STDBY_LV, true);
		}
	//
		//hhw_bimetal(value, delta, setpoint)
	//ausgangswörter kopieren.
		//hhw_lol_setcoils




	/***********/
}
void mfinit_happyhotwater	(TD_HappyHotwater* hhw);

void mftick_happyhotwater	(TD_HappyHotwater* hhw);

TD_HappyHotwater Hhw;
