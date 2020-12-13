/*
 * HappyHotWater.c
 *
 *  Created on: 03.12.2020
 *      Author: daniel
 */

#include "HappyHotWater.h"
#include "terminal.h"
#include "mcp23.h"

void hhw_lol_update			(TD_HappyHotwater* hhw,	Valuebuffer* db);
void hhw_lol_drain			(TD_HappyHotwater *hhw, TD_MCP *mcp_io);
void hhw_lol_report			(TD_HappyHotwater *hhw, Valuebuffer* db);
void hhw_lol_refill			(TD_HappyHotwater* hhw,	Valuebuffer* db);
void hhw_lol_battery_soc 	(TD_HappyHotwater* hhw,	Valuebuffer* db);
int hhw_lol_chk_input(TD_HappyHotwater* hhw, HHW_INWORD_NAMES ioitr, HHW_STATES stateitr, uint32_t iobuffer, int inv);

void mfinit_happyhotwater	(TD_HappyHotwater* hhw)
{
	modflag_init(&hhw->mf_modflag, HALTICK, 10);
	/*
	 * portzustände bzw. bitmasken
	 * für ein und ausgänge werden im array
	 * gespeichert und über enums adressiert
	 */
	hhw->inwords[BTN_WARM] 		=	2;
	hhw->inwords[BTN_KALT] 		=	1;
	hhw->inwords[BTN_BRAUSE] 	=	6;
	hhw->inwords[BTN_SPUELE] 	=	5;
	hhw->inwords[LVLSW_FULL] 	=	3;
	hhw->inwords[LVLSW_MID]     =	4;

	hhw->outwords[HOTROD_300]	=	16;
	hhw->outwords[VALVE_DRAIN]	=	15;
	hhw->outwords[VALVE_SHOWR]	=	14;
	hhw->outwords[VALVE_COLD]	=	13;
	hhw->outwords[VALVE_HOT]	=	12;
	hhw->outwords[PUMP_HOT]		=	11;
	hhw->outwords[PUMP_COLD]	=	10;

	hhw->VoltLevel_lowbatt = 11.8;
	hhw->VoltLevel_power_inv = 13.5;
	hhw->VoltLevel_highbatt = 12.72;
}

void mftask_happyhotwater	(TD_HappyHotwater* hhw,	Valuebuffer* db)
	{
	 if (hhw->mf_modflag.flag && hhw->mf_modflag.init_done)
		{
		hhw->mf_modflag.repeat = modflag_tickdiff(&hhw->mf_modflag);
		hhw_lol_update(hhw, db);
		//hhw_lol_report(hhw);
		hhw_lol_drain(&Hhw,&mcp_io);


		hhw->mf_modflag.duration = modflag_tickdiff(&hhw->mf_modflag);
		hhw->mf_modflag.callcount++;
		hhw->mf_modflag.flag = false;
		}
	}

void mftick_happyhotwater	(TD_HappyHotwater* hhw)
	{
	modflag_upd_regular(&hhw->mf_modflag);
	}

int hhw_lol_chk_input(TD_HappyHotwater* hhw, HHW_INWORD_NAMES ioitr, HHW_STATES stateitr, uint32_t iobuffer, int inv)
	{
	if (inv)
		{
		if (utils_get_inv_bit_in_Word(&iobuffer, hhw->inwords[ioitr]))
		{
			utils_set_bit_in_Word(&hhw->states, stateitr, true);
			return 1;
		}else{
			utils_set_bit_in_Word(&hhw->states, stateitr, false);
			return 0;
		}
		}
	else
		{
		if (utils_get_bit_in_Word(&iobuffer, hhw->inwords[ioitr])){
			utils_set_bit_in_Word(&hhw->states, stateitr, true);
			return 1;
		}else{
			utils_set_bit_in_Word(&hhw->states, stateitr, false);
			return 0;
		}
	}
	}

void hhw_lol_update(TD_HappyHotwater* hhw,	Valuebuffer* db)
	{

	hhw_lol_chk_input(hhw, BTN_KALT, COLDWTR_SPUELE, db->iostatus, 0);
	hhw_lol_chk_input(hhw, BTN_KALT, COLDWTR_SHOWER, db->iostatus, 0);

	hhw_lol_chk_input(hhw, BTN_WARM, HOTWTR_SHOWER, db->iostatus, 0);
	hhw_lol_chk_input(hhw, BTN_WARM, HOTWTR_SPUELE, db->iostatus, 0);

	hhw_lol_chk_input(hhw, LVLSW_MID, TANK_HOT_MID, db->iostatus, 1);
	hhw_lol_chk_input(hhw, LVLSW_FULL, TANK_HOT_FULL, db->iostatus, 1);

	hhw_lol_chk_input(hhw, BTN_BRAUSE, DUSCHEN, db->iostatus, 1);
	hhw_lol_chk_input(hhw, BTN_SPUELE, SPUELEN, db->iostatus, 1);



	if (db->batthwvolt < hhw->VoltLevel_lowbatt)
				utils_set_bit_in_Word((uint16_t*)hhw->states, POWER_LV, true);
		else	utils_set_bit_in_Word((uint16_t*)hhw->states, POWER_LV, false);


	if (db->batthwcurr < 0)
				utils_set_bit_in_Word((uint16_t*)hhw->states, POWER_INV, true);
		else	utils_set_bit_in_Word((uint16_t*)hhw->states, POWER_INV, false);

	if (db->batthwcurr > 0	)
				utils_set_bit_in_Word((uint16_t*)hhw->states, POWER_VB, true);
		else	utils_set_bit_in_Word((uint16_t*)hhw->states, POWER_VB, false);
	}

void hhw_lol_report(TD_HappyHotwater *hhw, Valuebuffer* db)
	{
	term_printf(&btTerm, "\r***hhw_lol_report_states***\r");
	if(utils_get_bit_in_Word(&hhw->states, POWER_LV))
		term_printf(&btTerm, "POWER_LV (%2.2f < %2.1f)V\r", db->batthwvolt, hhw->VoltLevel_lowbatt);

	if(utils_get_bit_in_Word(&hhw->states, POWER_INV))
		term_printf(&btTerm, "POWER_INV (%2.2f > %2.1f)V\r", db->batthwvolt, hhw->VoltLevel_power_inv);

	if(utils_get_bit_in_Word(&hhw->states, POWER_VB))
		term_printf(&btTerm, "POWER_VB (%2.1f < %2.2f < %2.1f)V\r", hhw->VoltLevel_lowbatt, db->batthwvolt, hhw->VoltLevel_power_inv);

	if(utils_get_bit_in_Word(&hhw->states, TANK_HOT_FULL))	term_printf(&btTerm, "TANK_HOT_FULL\r");
	if(utils_get_bit_in_Word(&hhw->states, TANK_HOT_MID))	term_printf(&btTerm, "TANK_HOT_MID\r");
	if(utils_get_bit_in_Word(&hhw->states, TANK_HOT_EMPTY))	term_printf(&btTerm, "TANK_HOT_EMPTY\r");
	if(utils_get_bit_in_Word(&hhw->states, COLDWTR_SPUELE))	term_printf(&btTerm, "COLDWTR_SPUELE\r");
	if(utils_get_bit_in_Word(&hhw->states, COLDWTR_SHOWER))	term_printf(&btTerm, "COLDWTR_SHOWER\r");
	if(utils_get_bit_in_Word(&hhw->states, HOTWTR_SHOWER))	term_printf(&btTerm, "HOTWTR_SHOWER\r");
	if(utils_get_bit_in_Word(&hhw->states, HOTWTR_SPUELE))	term_printf(&btTerm, "HOTWTR_SPUELE\r");
	if(utils_get_bit_in_Word(&hhw->states, SPUELEN))		term_printf(&btTerm, "SPUELEN\r");
	if(utils_get_bit_in_Word(&hhw->states, DUSCHEN))		term_printf(&btTerm, "DUSCHEN\r");
	}

void hhw_lol_drain(TD_HappyHotwater *hhw, TD_MCP *mcp_io)
	{
	if(utils_get_bit_in_Word(&hhw->states, POWER_LV))
		{
		term_printf(&btTerm, "Fehler: HhW Batterie leer: Nachladen durch Einschalten vom Inverter");
		}
	else
		{
		mcp_io->outlatch = 0;
		if(utils_get_bit_in_Word(&hhw->states, SPUELEN))
			{
			if(utils_get_bit_in_Word(&hhw->states, COLDWTR_SPUELE))
				{
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[VALVE_COLD], 1);
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[VALVE_DRAIN], 1);
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[PUMP_COLD], 1);
				}
			if(utils_get_bit_in_Word(&hhw->states, HOTWTR_SPUELE))
				{
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[VALVE_HOT], 1);
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[VALVE_DRAIN], 1);
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[PUMP_HOT], 1);
				}
			}
		if(utils_get_bit_in_Word(&hhw->states, DUSCHEN))
			{
			if(utils_get_bit_in_Word(&hhw->states, COLDWTR_SHOWER))
				{
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[VALVE_COLD], 1);
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[VALVE_SHOWR], 1);
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[PUMP_COLD], 1);
				}
			if(utils_get_bit_in_Word(&hhw->states, HOTWTR_SHOWER))
				{
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[VALVE_HOT], 1);
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[VALVE_SHOWR], 1);
				utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[PUMP_HOT], 1);
				}
			}
		}
	}

void hhw_lol_refill	(TD_HappyHotwater* hhw,	Valuebuffer* db)
	{
	if(utils_get_bit_in_Word(&hhw->states, TANK_ALWAYS_FULL))
		{
		if(utils_get_bit_in_Word(&hhw->states, 1))
			term_printf(&btTerm, "hhw_lol_refill: filling from empty to mid..");

		if(utils_get_bit_in_Word(&hhw->states, LVLSW_MID))
			term_printf(&btTerm, "hhw_lol_refill: filling to mid is done..");

		if(utils_get_bit_in_Word(&hhw->states, LVLSW_FULL))
			term_printf(&btTerm, "hhw_lol_refill:draining from LVLSW_FULL to LVLSW_MID");
		}
	}

void hhw_lol_battery_soc (TD_HappyHotwater* hhw,	Valuebuffer* db)
	{

	}
TD_HappyHotwater Hhw;
