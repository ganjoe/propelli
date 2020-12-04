/*
 * HappyHotWater.c
 *
 *  Created on: 03.12.2020
 *      Author: daniel
 */

#include "HappyHotWater.h"

void hhw_lol_update	(TD_HappyHotwater* hhw,	Valuebuffer* db);
void hhw_lol_drain	(TD_HappyHotwater* hhw);
void hhw_lol_shower	(TD_HappyHotwater* hhw);
void hhw_lol_refill	(TD_HappyHotwater* hhw);

void mfinit_happyhotwater	(TD_HappyHotwater* hhw)
{
	/*
	 * portzustände bzw. bitmasken
	 * für ein und ausgänge werden im array
	 * gespeichert und über enums adressiert
	 */
	hhw->iowords[BTN_BRAUSE] 	=	0x1;
	hhw->iowords[BTN_KALT] 		=	0x2;
	hhw->iowords[BTN_SPUELE] 	=	0x4;
	hhw->iowords[BTN_WARM] 		=	0x8;
	hhw->iowords[LVLSW_UL] 		=	0x10;
	hhw->iowords[LVLSW_LL] 		=	0x20;
	hhw->iowords[HOTROD_300]	=	0x40;
	hhw->iowords[VALVE_DRAIN]	=	0x80;
	hhw->iowords[VALVE_SHOWR]	=	0x100;
	hhw->iowords[VALVE_COLD]	=	0x200;
	hhw->iowords[VALVE_HOT]		=	0x400;
	hhw->iowords[PUMP_HOT]		=	0x800;
	hhw->iowords[PUMP_COLD]		=	0x1000;
}

void mftask_happyhotwater	(TD_HappyHotwater* hhw,	Valuebuffer* db)
	{
	hhw_lol_update(hhw, db);
	hhw_lol_drain(hhw);
	hhw_lol_shower(hhw);

	}

void mftick_happyhotwater	(TD_HappyHotwater* hhw);



void hhw_lol_update(TD_HappyHotwater* hhw,	Valuebuffer* db)
	{
	if (db->iostatus == hhw->iowords[BTN_KALT])
			{
			utils_set_bit_in_Word(&hhw->states, COLDWTR_SPUELE, true);
			utils_set_bit_in_Word(&hhw->states, COLDWTR_SHOWER , true);
			utils_set_bit_in_Word(&hhw->states, HOTWTR_SPUELE, false);
			utils_set_bit_in_Word(&hhw->states, HOTWTR_SHOWER, false);
			}
		if (db->iostatus == hhw->iowords[BTN_WARM])
			{
			utils_set_bit_in_Word(&hhw->states, COLDWTR_SPUELE, false);
			utils_set_bit_in_Word(&hhw->states, COLDWTR_SHOWER , false);
			utils_set_bit_in_Word(&hhw->states, HOTWTR_SPUELE, true);
			utils_set_bit_in_Word(&hhw->states, HOTWTR_SHOWER, true);
			}
		if (db->batthwvolt > hhw->VoltLevel_power_ok)
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
	}




TD_HappyHotwater Hhw;
