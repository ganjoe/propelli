/*
 * HappyHotWater.c
 *
 *  Created on: 03.12.2020
 *      Author: daniel
 */

#include "HappyHotWater.h"
#include "terminal.h"
#include "mcp23.h"

void hhw_lol_update			(TD_HappyHotwater* 	hhw,	Valuebuffer* db);
void hhw_lol_drain			(TD_HappyHotwater*	hhw, 	TD_MCP *mcp_io);
void hhw_lol_report			(TD_HappyHotwater*	hhw, 	Valuebuffer* db);
void hhw_lol_refill			(TD_HappyHotwater* 	hhw,	Valuebuffer* db, TD_MCP *mcp_io);
void hhw_lol_battery_soc 	(TD_HappyHotwater* 	hhw,	Valuebuffer* db);

int  hhw_set_instate		(TD_HappyHotwater* hhw, HHW_INWORD_NAMES ioitr, HHW_STATES stateitr, uint32_t iobuffer, int inv);
void hhw_set_state			(TD_HappyHotwater* hhw, HHW_STATES stateitr, int state);
int  hhw_get_state			(TD_HappyHotwater* hhw, HHW_STATES stateitr);
void hhw_set_outstates		(TD_HappyHotwater* hhw, HHW_OUTWORD_NAMES ioitr, HHW_STATES stateitr, TD_MCP *mcp_io);

void hhw_set_out			(TD_HappyHotwater* hhw, HHW_OUTWORD_NAMES ioitr, TD_MCP *mcp_io, int state);
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
	utils_set_bit_in_Word(&hhw->states, TANK_ALWAYS_MID, true);
}

void mftask_happyhotwater	(TD_HappyHotwater* hhw)
	{
	 if (hhw->mf_modflag.flag && hhw->mf_modflag.init_done)
		{
		hhw->mf_modflag.repeat = modflag_tickdiff(&hhw->mf_modflag);
		mcp_io.outlatch = 0;
		hhw_lol_update(hhw, &db);
		hhw_lol_drain(hhw,&mcp_io);
		hhw_lol_refill(hhw, &db, &mcp_io);
		hhw->mf_modflag.duration = modflag_tickdiff(&hhw->mf_modflag);
		hhw->mf_modflag.callcount++;
		hhw->mf_modflag.flag = false;
		}
	}

void mftick_happyhotwater	(TD_HappyHotwater* hhw)
	{
	modflag_upd_regular(&hhw->mf_modflag);
	}


int hhw_set_instate(TD_HappyHotwater* hhw, HHW_INWORD_NAMES ioitr, HHW_STATES stateitr, uint32_t iobuffer, int inv)
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

void hhw_set_state(TD_HappyHotwater* hhw, HHW_STATES stateitr, int state)
	{
	utils_set_bit_in_Word(&hhw->states, stateitr, state);
	}

int hhw_get_state(TD_HappyHotwater* hhw, HHW_STATES stateitr)
	{
	return utils_get_bit_in_Word(&hhw->states, stateitr);
	}

void hhw_set_outstates(TD_HappyHotwater* hhw, HHW_OUTWORD_NAMES ioitr, HHW_STATES stateitr, TD_MCP *mcp_io)
	{
	if (hhw_get_state(hhw, stateitr))
		utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[ioitr], 1);
	else
		utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[ioitr], 0);
	}

void hhw_set_out(TD_HappyHotwater* hhw, HHW_OUTWORD_NAMES ioitr, TD_MCP *mcp_io, int state)
	{
	utils_set_bit_in_Word(&mcp_io->outlatch, hhw->outwords[ioitr], state);
	}

void hhw_lol_update(TD_HappyHotwater* hhw,	Valuebuffer* db)
	{
	hhw_set_instate(hhw, BTN_KALT, COLDWTR_SPUELE, db->iostatus, 0);
	hhw_set_instate(hhw, BTN_KALT, COLDWTR_SHOWER, db->iostatus, 0);

	hhw_set_instate(hhw, BTN_WARM, HOTWTR_SHOWER, db->iostatus, 0);
	hhw_set_instate(hhw, BTN_WARM, HOTWTR_SPUELE, db->iostatus, 0);

	hhw_set_instate(hhw, LVLSW_MID, TANK_HOT_MID, db->iostatus, 0);
	hhw_set_instate(hhw, LVLSW_FULL, TANK_HOT_FULL, db->iostatus, 0);

	hhw_set_instate(hhw, BTN_BRAUSE, DUSCHEN, db->iostatus, 1);
	hhw_set_instate(hhw, BTN_SPUELE, SPUELEN, db->iostatus, 1);

	hhw_set_state(hhw, POWER_LV, (db->batthwvolt < hhw->VoltLevel_lowbatt));
	hhw_set_state(hhw, POWER_INV, (db->batthwcurr < 0));
	hhw_set_state(hhw, POWER_VB, (db->batthwvolt > hhw->VoltLevel_lowbatt));


	}

void hhw_lol_report(TD_HappyHotwater* hhw, Valuebuffer* db)
	{

	term_printf(&btTerm, "\r***hhw states***\r");

	if(hhw_get_state(hhw, TANK_HOT_FULL))	term_printf(&btTerm, "_TANK_HOT_FULL_");
	if(hhw_get_state(hhw,TANK_HOT_MID))	term_printf(&btTerm, "_TANK_HOT_MID_");
	if(hhw_get_state(hhw, TANK_HOT_EMPTY))	term_printf(&btTerm, "_TANK_HOT_EMPTY_");
	if(hhw_get_state(hhw, COLDWTR_SPUELE))	term_printf(&btTerm, "_COLDWTR_SPUELE_");
	if(hhw_get_state(hhw, COLDWTR_SHOWER))	term_printf(&btTerm, "_COLDWTR_SHOWER_");
	if(hhw_get_state(hhw, HOTWTR_SHOWER))	term_printf(&btTerm, "_HOTWTR_SHOWER_");
	if(hhw_get_state(hhw, HOTWTR_SPUELE))	term_printf(&btTerm, "_HOTWTR_SPUELE_");
	if(hhw_get_state(hhw, SPUELEN))		term_printf(&btTerm, "_SPUELEN_");
	if(hhw_get_state(hhw, DUSCHEN))		term_printf(&btTerm, "_DUSCHEN_");
	if(hhw_get_state(hhw, REFILL))			term_printf(&btTerm, "_REFILL_");
	if(hhw_get_state(hhw, POWER_LV))		term_printf(&btTerm, "_POWER_LV_");
	if(hhw_get_state(hhw, POWER_INV))		term_printf(&btTerm, "_POWER_INV_");
	if(hhw_get_state(hhw, POWER_VB))		term_printf(&btTerm, "_POWER_VB_");
	term_printf(&btTerm, "\r***outputs***\r");
	if(utils_get_bit_in_Word(&mcp_io.outlatch, hhw->outwords[VALVE_COLD]))term_printf(&btTerm, "_VALVE_COLD_");
	if(utils_get_bit_in_Word(&mcp_io.outlatch, hhw->outwords[VALVE_DRAIN]))term_printf(&btTerm, "_VALVE_DRAIN_");
	if(utils_get_bit_in_Word(&mcp_io.outlatch, hhw->outwords[VALVE_HOT]))	term_printf(&btTerm, "_VALVE_HOT_");
	if(utils_get_bit_in_Word(&mcp_io.outlatch, hhw->outwords[VALVE_SHOWR]))term_printf(&btTerm, "_VALVE_SHOWR_");
	if(utils_get_bit_in_Word(&mcp_io.outlatch, hhw->outwords[HOTROD_300]))	term_printf(&btTerm, "_HOTROD_300_");
	if(utils_get_bit_in_Word(&mcp_io.outlatch, hhw->outwords[PUMP_COLD]))	term_printf(&btTerm, "_PUMP_COLD_");
	if(utils_get_bit_in_Word(&mcp_io.outlatch, hhw->outwords[PUMP_HOT]))	term_printf(&btTerm, "_PUMP_HOT_");
	term_printf(&btTerm, "\r");


	}

void hhw_lol_drain(TD_HappyHotwater* hhw, TD_MCP *mcp_io)
	{
	hhw_set_state(hhw, REFILL, false);
	if(hhw_get_state(hhw, POWER_VB))
		{
		hhw_set_state(hhw, REFILL, true);
		if(((hhw_get_state(hhw, COLDWTR_SPUELE)) & hhw_get_state(hhw, SPUELEN)))
		{
		hhw_set_out(hhw, VALVE_COLD,  mcp_io, true);
		hhw_set_out(hhw, PUMP_COLD,  mcp_io, true);
		hhw_set_out(hhw, VALVE_DRAIN,  mcp_io, true);
		hhw_set_state(hhw, REFILL, false);
		}
		if(((hhw_get_state(hhw, HOTWTR_SPUELE)) & hhw_get_state(hhw, SPUELEN)))
		{
		hhw_set_out(hhw, VALVE_HOT,  mcp_io, true);
		hhw_set_out(hhw, VALVE_DRAIN,  mcp_io, true);
		hhw_set_out(hhw, PUMP_HOT,  mcp_io, true);
		hhw_set_state(hhw, REFILL, false);
		}
		if(((hhw_get_state(hhw, COLDWTR_SHOWER)) & hhw_get_state(hhw, DUSCHEN)))
		{
		hhw_set_out(hhw, VALVE_COLD,  mcp_io, true);
		hhw_set_out(hhw, VALVE_SHOWR,  mcp_io, true);
		hhw_set_out(hhw, PUMP_COLD,  mcp_io, true);
		hhw_set_state(hhw, REFILL, false);
		}
		if(((hhw_get_state(hhw, HOTWTR_SHOWER)) & hhw_get_state(hhw, DUSCHEN)))
		{
		hhw_set_out(hhw, VALVE_HOT,  mcp_io, true);
		hhw_set_out(hhw, VALVE_SHOWR,  mcp_io, true);
		hhw_set_out(hhw, PUMP_HOT,  mcp_io, true);
		hhw_set_state(hhw, REFILL, false);
		}

		}

/*
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
*/

		}

void hhw_lol_refill	(TD_HappyHotwater* hhw,	Valuebuffer* db, TD_MCP *mcp_io)
	{
	if	(( hhw_get_state(hhw, REFILL)) &( hhw_get_state(hhw,POWER_VB )))
		{
		if	( hhw_get_state(hhw,TANK_HOT_MID ) == 0)
			{
			hhw_set_outstates(hhw, VALVE_COLD, TANK_ALWAYS_MID, mcp_io);
			hhw_set_outstates(hhw, VALVE_HOT, TANK_ALWAYS_MID, mcp_io);
			hhw_set_outstates(hhw, PUMP_COLD, TANK_ALWAYS_MID, mcp_io);
			}
		if	( hhw_get_state(hhw,TANK_HOT_FULL ))
			{
			hhw_set_outstates(hhw, VALVE_COLD, TANK_ALWAYS_FULL, mcp_io);
			hhw_set_outstates(hhw, VALVE_HOT, TANK_ALWAYS_FULL, mcp_io);
			hhw_set_outstates(hhw, PUMP_COLD, TANK_ALWAYS_FULL, mcp_io);

			}
		}
	}


void hhw_lol_battery_soc (TD_HappyHotwater* hhw,	Valuebuffer* db)
	{

	}
TD_HappyHotwater Hhw;
