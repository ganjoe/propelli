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
	modflag_init(&hhw->mf_modflag, HALTICK, 10);
	/*
	 * portzustände bzw. bitmasken
	 * für ein und ausgänge werden im array
	 * gespeichert und über enums adressiert
	 */
	hhw->iowords[BTN_BRAUSE] 	=	0x0;
	hhw->iowords[BTN_KALT] 		=	0x1;
	hhw->iowords[BTN_SPUELE] 	=	0x4;
	hhw->iowords[BTN_WARM] 		=	0x2;
	hhw->iowords[LVLSW_EMPTY] 		=	0x3;
	hhw->iowords[LVLSW_FULL] 		=	0x3;
	hhw->iowords[LVLSW_MID] 		=	0x20;
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
	 if (hhw->mf_modflag.flag && hhw->mf_modflag.init_done)
		{
		hhw->mf_modflag.repeat = modflag_tickdiff(&hhw->mf_modflag);
		hhw_lol_update(hhw, db);
		//hhw_lol_drain(hhw);
		//hhw_lol_shower(hhw);

		hhw->mf_modflag.duration = modflag_tickdiff(&hhw->mf_modflag);
		hhw->mf_modflag.callcount++;
		hhw->mf_modflag.flag = false;
		}
	}

void mftick_happyhotwater	(TD_HappyHotwater* hhw)
	{
	modflag_upd_regular(&hhw->mf_modflag);
	}


void hhw_lol_update(TD_HappyHotwater* hhw,	Valuebuffer* db)
	{

	if (hhw->iowords[BTN_KALT] && db->iostatus)
		{
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SPUELE, true);
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SHOWER, true);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SPUELE, false);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SHOWER, false);
		}
	if (hhw->iowords[BTN_WARM] && db->iostatus)
		{
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SPUELE, false);
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SHOWER , false);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SPUELE, true);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SHOWER, true);
		}
	if (hhw->iowords[LVLSW_FULL] && db->iostatus)
		{
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_FULL, true);
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_EMPTY , false);
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_MID, false);
		}
	if (hhw->iowords[LVLSW_MID] && db->iostatus)
		{
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_FULL, false);
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_EMPTY , false);
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_MID, true);
		}
	if (hhw->iowords[LVLSW_EMPTY] && db->iostatus)
		{
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_FULL, false);
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_EMPTY , true);
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_MID, false);
		}

	if (db->batthwvolt < hhw->VoltLevel_lowbatt)
		{
		utils_set_bit_in_Word(&hhw->states, STDBY_LV, true);
		utils_set_bit_in_Word(&hhw->states, STDBY_INV, false);
		}
	if (db->batthwvolt > hhw->VoltLevel_power_ok)
		{
		utils_set_bit_in_Word(&hhw->states, STDBY_LV, false);
		utils_set_bit_in_Word(&hhw->states, STDBY_INV, true);
		}


		}




TD_HappyHotwater Hhw;
