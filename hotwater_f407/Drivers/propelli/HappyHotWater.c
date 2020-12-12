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
void hhw_lol_report(TD_HappyHotwater *hhw, Valuebuffer* db);
void hhw_lol_refill			(TD_HappyHotwater* hhw,	Valuebuffer* db);
void hhw_lol_battery_soc (TD_HappyHotwater* hhw,	Valuebuffer* db);

void mfinit_happyhotwater	(TD_HappyHotwater* hhw)
{
	modflag_init(&hhw->mf_modflag, HALTICK, 10);
	/*
	 * portzustände bzw. bitmasken
	 * für ein und ausgänge werden im array
	 * gespeichert und über enums adressiert
	 */
	hhw->iowords[BTN_WARM] 		=	0x5;
	hhw->iowords[BTN_KALT] 		=	0x3;
	hhw->iowords[BTN_OFF] 		=	0x6;		//invertiert abfragen
	hhw->iowords[BTN_BRAUSE] 	=	0x20;
	hhw->iowords[BTN_SPUELE] 	=	0x40;
	hhw->iowords[LVLSW_EMPTY]	=	0x3;
	hhw->iowords[LVLSW_FULL] 	=	0x19;
	hhw->iowords[LVLSW_MID]     =	0x11;
	hhw->iowords[HOTROD_300]	=	0x400;
	hhw->iowords[VALVE_DRAIN]	=	0x10000;
	hhw->iowords[VALVE_SHOWR]	=	0x8000;
	hhw->iowords[VALVE_COLD]	=	0x4000;
	hhw->iowords[VALVE_HOT]		=	0x2000;
	hhw->iowords[PUMP_HOT]		=	0x1000;
	hhw->iowords[PUMP_COLD]		=	0x800;

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

void hhw_lol_update(TD_HappyHotwater* hhw,	Valuebuffer* db)
	{
	if (! utils_get_bits_in_Word(&db->iostatus, hhw->iowords[BTN_OFF]))
		{
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SPUELE, false);
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SHOWER , false);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SPUELE, false);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SHOWER, false);
		}
	if (utils_get_bits_in_Word(&db->iostatus, hhw->iowords[BTN_KALT]))
		{
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SPUELE, true);
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SHOWER, true);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SPUELE, false);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SHOWER, false);
		}
	if (utils_get_bits_in_Word(&db->iostatus, hhw->iowords[BTN_WARM]))
		{
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SPUELE, false);
		utils_set_bit_in_Word(&hhw->states, COLDWTR_SHOWER , false);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SPUELE, true);
		utils_set_bit_in_Word(&hhw->states, HOTWTR_SHOWER, true);
		}

	utils_set_bit_in_Word(&hhw->states, TANK_HOT_EMPTY, true);

	if (utils_get_bits_in_Word(&db->iostatus, hhw->iowords[LVLSW_MID]))
		{
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_MID, true);
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_EMPTY, false);
		}

	if (utils_get_bits_in_Word(&db->iostatus, hhw->iowords[LVLSW_FULL]))
		{
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_FULL, true);
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_EMPTY , false);
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_MID, false);
		}
	else
		{
		utils_set_bit_in_Word(&hhw->states, TANK_HOT_FULL, false);
		}









	if ((!utils_get_bits_in_Word(&db->iostatus, hhw->iowords[BTN_BRAUSE])) & (utils_get_bits_in_Word(&db->iostatus, hhw->iowords[BTN_SPUELE])))
		{
		utils_set_bit_in_Word(&hhw->states, DUSCHEN, true);
		}
	else
		{
		utils_set_bit_in_Word(&hhw->states, DUSCHEN, false);
		}








	if ((!utils_get_bits_in_Word(&db->iostatus, hhw->iowords[BTN_SPUELE])) &(utils_get_bits_in_Word(&db->iostatus, hhw->iowords[BTN_BRAUSE])))
		{
		utils_set_bit_in_Word(&hhw->states, SPUELEN, true);
		}
	else
		{
		utils_set_bit_in_Word(&hhw->states, SPUELEN, false);
		}





	if ((!utils_get_bits_in_Word(&db->iostatus, hhw->iowords[BTN_SPUELE])) &(!utils_get_bits_in_Word(&db->iostatus, hhw->iowords[BTN_BRAUSE])))
		{
		utils_set_bit_in_Word(&hhw->states, SPUELEN, true);
		utils_set_bit_in_Word(&hhw->states, DUSCHEN, true);
		}









	if (db->batthwvolt < hhw->VoltLevel_lowbatt)
		{
		utils_set_bit_in_Word(&hhw->states, POWER_LV, true);
		}
	else
		{
		utils_set_bit_in_Word(&hhw->states, POWER_LV, false);
		}









	if (db->batthwcurr < 0)
		{
		utils_set_bit_in_Word(&hhw->states, POWER_INV, true);
		}
	else
		{
		utils_set_bit_in_Word(&hhw->states, POWER_INV, false);
		}










	if (db->batthwcurr > 0	)
		{
		utils_set_bit_in_Word(&hhw->states, POWER_VB, true);
		}
	else
		{
		utils_set_bit_in_Word(&hhw->states, POWER_VB, false);
		}
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
				mcp_io->outlatch |= hhw->iowords[VALVE_COLD]; //bitweise oder verknüpfen
				mcp_io->outlatch |= hhw->iowords[VALVE_DRAIN];
				mcp_io->outlatch |= hhw->iowords[PUMP_COLD];
				}
			if(utils_get_bit_in_Word(&hhw->states, HOTWTR_SPUELE))
				{
				term_printf(&btTerm, "hhw_lol_drain:HOTWTR_SPUELE ok\r");
				mcp_io->outlatch |= hhw->iowords[VALVE_HOT]; //bitweise oder verknüpfen
				mcp_io->outlatch |= hhw->iowords[VALVE_DRAIN];
				mcp_io->outlatch |= hhw->iowords[PUMP_HOT];
				}
			}
		if(utils_get_bit_in_Word(&hhw->states, DUSCHEN))
			{
			if(utils_get_bit_in_Word(&hhw->states, COLDWTR_SHOWER))
				{
				mcp_io->outlatch |= hhw->iowords[VALVE_COLD]; //bitweise oder verknüpfen
				mcp_io->outlatch |= hhw->iowords[VALVE_SHOWR];
				mcp_io->outlatch |= hhw->iowords[PUMP_COLD];
				}
			if(utils_get_bit_in_Word(&hhw->states, HOTWTR_SHOWER))
				{
				mcp_io->outlatch |= hhw->iowords[VALVE_HOT]; //bitweise oder verknüpfen
				mcp_io->outlatch |= hhw->iowords[VALVE_SHOWR];
				mcp_io->outlatch |= hhw->iowords[PUMP_HOT];
				}
			}
		}
	}

void hhw_lol_refill	(TD_HappyHotwater* hhw,	Valuebuffer* db)
	{
	if(utils_get_bit_in_Word(&hhw->states, TANK_ALWAYS_FULL))
		{
		if(utils_get_bit_in_Word(&hhw->states, LVLSW_EMPTY))
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
