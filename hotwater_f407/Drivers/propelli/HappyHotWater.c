/*
 * HappyHotWater.c
 *
 *  Created on: 03.12.2020
 *      Author: daniel
 */

#include "HappyHotWater.h"
#include "terminal.h"

void mfinit_happyhotwater	(TD_HappyHotwater* hhw)
{
	modflag_init(&hhw->mf_modflag, HALTICK, 10);

	/* die bitmasken haben physische entsprechung (anschlussreihenfolge)*/
	/* einzelne relais */
	hhw->inwords[BTN_KALT] 		=	BIT1;
	hhw->inwords[BTN_WARM] 		=	BIT2;
	hhw->inwords[LVLSW_FULL] 	=	BIT3;
	hhw->inwords[LVLSW_MID]     =	BIT4;
	hhw->inwords[BTN_SPUELE] 	=	BIT5;
	hhw->inwords[BTN_BRAUSE] 	=	BIT6;

	hhw->outwords[HOTROD_300]	=	BIT9;
	hhw->outwords[VALVE_DRAIN]	=	BIT15;
	hhw->outwords[VALVE_SHOWR]	=	BIT14;
	hhw->outwords[VALVE_COLD]	=	BIT13;
	hhw->outwords[VALVE_HOT]	=	BIT12;
	hhw->outwords[PUMP_HOT]		=	BIT11;
	hhw->outwords[PUMP_COLD]	=	BIT10;

	hhw->VoltLevel_lowbatt = 11.8;
	hhw->VoltLevel_highbatt = 12.72;
	hhw->VoltLevel_power_inv = 13.5;

	hhw->TempLevel_high = 26.0;
	hhw->TempLevel_low = 25.0;

	hhw_set_state(hhw, TANK_MODE_ECO, true);
}
	/* programmablaufmerker, registerbits   */



	/* kombinierte relaiszustände  */


void mftask_happyhotwater	(TD_HappyHotwater* hhw)
	{
	 if (hhw->mf_modflag.flag && hhw->mf_modflag.init_done)
		{
		hhw->mf_modflag.repeat = modflag_tickdiff(&hhw->mf_modflag);
		hhw->outword = 0;
		hhw_lol_update(hhw, &db);
		hhw_lol_drain(hhw,&mcp_io);
		hhw_lol_fillnheat(hhw, &db, &mcp_io);
		mcp_io.outlatch = hhw->outword;

		hhw->mf_modflag.duration = modflag_tickdiff(&hhw->mf_modflag);
		hhw->mf_modflag.callcount++;
		hhw->mf_modflag.flag = false;
		}
	}

void mftick_happyhotwater	(TD_HappyHotwater* hhw)
	{
	modflag_upd_regular(&hhw->mf_modflag);
	}

void hhw_set_state	(TD_HappyHotwater* hhw, int stateitr, int state)
	{
	utils_set_bits_in_dWord(&hhw->state, stateitr, state);
	}

int hhw_get_state	(TD_HappyHotwater* hhw, int stateitr)
	{
	return utils_get_bits_in_dWord(&hhw->state, stateitr );
	}

int hhw_get_invstate(TD_HappyHotwater* hhw, int stateitr)
	{
	return utils_get_inv_bits_in_dWord(&hhw->state, stateitr );
	}

int hhw_set_instate	(TD_HappyHotwater* hhw, HHW_INWORD_NAMES ioitr, uint32_t stateitr, uint32_t iobuffer, int inv)
	{
	if (inv)
		{
		if (utils_get_inv_bits_in_dWord(&iobuffer, hhw->inwords[ioitr]))
		{
			hhw_set_state(hhw, stateitr, true);
			return 1;
		}else{
			hhw_set_state(hhw, stateitr, false);
			return 0;
		}
		}
	else
		{
		if (utils_get_bits_in_dWord(&iobuffer, hhw->inwords[ioitr]))
			{
			hhw_set_state(hhw, stateitr, true);
			return 1;
		}else{
			hhw_set_state(hhw, stateitr, false);
			return 0;
		}
	}
	}

void hhw_set_outstates(TD_HappyHotwater* hhw, int ioitr, int stateitr, TD_MCP *mcp_io)
	{
	if (hhw_get_state(hhw, stateitr))
		hhw_set_out(hhw,  ioitr,  1);
	else
		hhw_set_out(hhw, ioitr,  0);
	}

void hhw_set_out(TD_HappyHotwater* hhw, uint16_t outnamesmask, int state)

	{

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
	hhw_set_state(hhw, TANK_TEMP_HL, db->temphot > hhw->TempLevel_high);

	if ((db->temphot < hhw->TempLevel_low) & (db->temphot != 0.0) )
		{
		hhw_set_state(hhw, FREIG_HEIZEN, 1);
		}
	if (db->temphot > hhw->TempLevel_high)
		{
		hhw_set_state(hhw, FREIG_HEIZEN, 0);
		}

	if ( db->batthwcurr < 0)
		{
		/* falls der shunt gebrückt ist (=0)erfolgt die erkennung fürs laden durch den spannungslevel */
		hhw_set_state(hhw, POWER_VB, db->batthwvolt > hhw->VoltLevel_power_inv);
		hhw_set_state(hhw, POWER_INV, 1);
		}
	else
		{
		hhw_set_state(hhw, POWER_INV, 0);
		hhw_set_state(hhw, POWER_VB, db->batthwvolt > hhw->VoltLevel_lowbatt);
		hhw_set_state(hhw, POWER_LV, db->batthwvolt < hhw->VoltLevel_lowbatt);
		}

	db->battsoc = utils_map(db->batthwvolt, hhw->VoltLevel_lowbatt, hhw->VoltLevel_highbatt, 0.0, 100.0);


	}

void hhw_lol_report(TD_HappyHotwater* hhw, Valuebuffer* db)
	{

	term_printf(&btTerm, "\r***hhw states***\r");

	if(hhw_get_state(hhw, TANK_HOT_FULL))	term_printf(&btTerm, "_TANK_HOT_FULL_");

	if(hhw_get_state(hhw, TANK_HOT_MID))	term_printf(&btTerm, "_TANK_HOT_MID_");

	if(hhw_get_state(hhw, COLDWTR_SPUELE))	term_printf(&btTerm, "_COLDWTR_SPUELE_");
	if(hhw_get_state(hhw, COLDWTR_SHOWER))	term_printf(&btTerm, "_COLDWTR_SHOWER_");
	if(hhw_get_state(hhw, HOTWTR_SHOWER))	term_printf(&btTerm, "_HOTWTR_SHOWER_");
	if(hhw_get_state(hhw, HOTWTR_SPUELE))	term_printf(&btTerm, "_HOTWTR_SPUELE_");

	if(hhw_get_state(hhw, SPUELEN))			term_printf(&btTerm, "_SPUELEN_");
	if(hhw_get_state(hhw, DUSCHEN))			term_printf(&btTerm, "_DUSCHEN_");
	if(hhw_get_state(hhw, REFILL))			term_printf(&btTerm, "_REFILL_");
	if(hhw_get_state(hhw, FREIG_HEIZEN))	term_printf(&btTerm, "_FREIG_HEIZEN_");
	if(hhw_get_state(hhw, TANK_TEMP_HL))	term_printf(&btTerm, "_TANK_TEMP_HL_");
	if(hhw_get_state(hhw, POWER_LV))		term_printf(&btTerm, "_POWER_LV_");
	if(hhw_get_state(hhw, POWER_INV))		term_printf(&btTerm, "_POWER_INV_");
	if(hhw_get_state(hhw, POWER_VB))		term_printf(&btTerm, "_POWER_VB_");
	if(hhw_get_state(hhw, TANK_MODE_ECO))	term_printf(&btTerm, "_TANK_MODE_ECO_");
	if(hhw_get_state(hhw, TANK_MODE_FULL))	term_printf(&btTerm, "_TANK_MODE_FULL_");

	term_printf(&btTerm, "\r***outputs***\r");
	if(utils_get_bits_in_dWord(&hhw->outword,  hhw->outwords[VALVE_COLD]))	term_printf(&btTerm, "_VALVE_COLD_");
	if(utils_get_bits_in_dWord(&hhw->outword,  hhw->outwords[VALVE_DRAIN]))	term_printf(&btTerm, "_VALVE_DRAIN_");
	if(utils_get_bits_in_dWord(&hhw->outword,  hhw->outwords[VALVE_HOT]))	term_printf(&btTerm, "_VALVE_HOT_");
	if(utils_get_bits_in_dWord(&hhw->outword,  hhw->outwords[VALVE_SHOWR]))	term_printf(&btTerm, "_VALVE_SHOWR_");
	if(utils_get_bits_in_dWord(&hhw->outword,  hhw->outwords[HOTROD_300]))	term_printf(&btTerm, "_HOTROD_300_");
	if(utils_get_bits_in_dWord(&hhw->outword,  hhw->outwords[PUMP_COLD]))	term_printf(&btTerm, "_PUMP_COLD_");
	if(utils_get_bits_in_dWord(&hhw->outword,  hhw->outwords[PUMP_HOT]))	term_printf(&btTerm, "_PUMP_HOT_");
	term_printf(&btTerm, "\r");

	}

void hhw_lol_drain(TD_HappyHotwater* hhw, TD_MCP *mcp_io)
	{

	hhw_set_state(hhw, REFILL, true);


	if(hhw_get_state(hhw, (POWER_VB | POWER_INV)))
		{

		if(hhw_get_state(hhw, (COLDWTR_SPUELE | SPUELEN)))
			{
			utils_set_bits_in_dWord(&hhw->outword, ((hhw->outwords[VALVE_COLD]) |
													(hhw->outwords[VALVE_DRAIN])|
													(hhw->outwords[PUMP_COLD])), 1);
													hhw_set_state(hhw, REFILL, false);
			}
		if(hhw_get_state(hhw, (HOTWTR_SPUELE | SPUELEN)))
			{
			utils_set_bits_in_dWord(&hhw->outword, ((hhw->outwords[VALVE_HOT]) |
													(hhw->outwords[VALVE_DRAIN])|
													(hhw->outwords[PUMP_HOT])), 1);
													hhw_set_state(hhw, REFILL, false);
			}
		if(hhw_get_state(hhw, (COLDWTR_SHOWER | DUSCHEN)))
			{
			utils_set_bits_in_dWord(&hhw->outword, ((hhw->outwords[VALVE_COLD]) |
													(hhw->outwords[VALVE_SHOWR])|
													(hhw->outwords[PUMP_COLD])), 1);
													hhw_set_state(hhw, REFILL, false);
			}
		if(hhw_get_state(hhw, (HOTWTR_SHOWER | DUSCHEN)))
			{
			utils_set_bits_in_dWord(&hhw->outword, ((hhw->outwords[VALVE_HOT]) |
													(hhw->outwords[VALVE_SHOWR])|
													(hhw->outwords[PUMP_HOT])), 1);
													hhw_set_state(hhw, REFILL, false);
			}

		}
	}

void hhw_lol_fillnheat	(TD_HappyHotwater* hhw,	Valuebuffer* db, TD_MCP *mcp_io)
	{
	/* jedes powerlevel ausser batt low */
	if (hhw_get_state(hhw,(POWER_VB)) |  hhw_get_state(hhw, ( POWER_INV )) )
		{

		if(hhw_get_state(hhw,(TANK_MODE_ECO)) )
			{
			if(hhw_get_state(hhw, (REFILL )) & (!hhw_get_state(hhw, ( TANK_HOT_MID)))& (!hhw_get_state(hhw, ( TANK_HOT_FULL))))
				{
				utils_set_bits_in_dWord(&hhw->outword, ((hhw->outwords[VALVE_COLD]) |
														(hhw->outwords[VALVE_HOT])	|
														(hhw->outwords[PUMP_COLD])), 1);
				}

			if(hhw_get_state(hhw, (FREIG_HEIZEN  | TANK_HOT_MID )))
				{
				utils_set_bits_in_dWord(&hhw->outword, (hhw->outwords[HOTROD_300]), 1);
				}
			}

		if(hhw_get_state(hhw, (  TANK_MODE_FULL )))
			{

			/* aktiv bis tank unteren anschlag erreicht hat, heizung aus, weil könnte auch leer sein */
			if(hhw_get_state(hhw, (REFILL ))
			& (!hhw_get_state(hhw, ( TANK_HOT_MID)))
			& (!hhw_get_state(hhw, ( TANK_HOT_FULL))))
				{
				utils_set_bits_in_dWord(&hhw->outword, ((hhw->outwords[VALVE_COLD]) |
														(hhw->outwords[VALVE_HOT])	|
														(hhw->outwords[PUMP_COLD])), 1);

				}
			/* wenn der untere aber nicht der obere anschlag belegt ist, wird nachgefüllt solange es zu heiß ist */
			if(hhw_get_state(hhw, (REFILL | TANK_HOT_MID))
			& (!hhw_get_state(hhw, ( FREIG_HEIZEN)))
			& (!hhw_get_state(hhw, ( TANK_HOT_FULL))))
				{
				utils_set_bits_in_dWord(&hhw->outword, ((hhw->outwords[VALVE_COLD]) |
														(hhw->outwords[VALVE_HOT])	|
														(hhw->outwords[PUMP_COLD])), 1);
				utils_set_bits_in_dWord(&hhw->outword,  (hhw->outwords[HOTROD_300]), 0);
				}
			/* wenn der untere aber nicht der obere anschlag belegt ist, dann wird erst nachgefüllt wenn es zu heiß geworden ist */
			if(hhw_get_state(hhw, (REFILL | TANK_HOT_MID | FREIG_HEIZEN))
			& (!hhw_get_state(hhw, ( TANK_HOT_FULL))))
				{
				utils_set_bits_in_dWord(&hhw->outword, ((hhw->outwords[VALVE_COLD]) |
														(hhw->outwords[VALVE_HOT])	|
														(hhw->outwords[HOTROD_300])	|
														(hhw->outwords[PUMP_COLD])), 1);
				}

		}
	}
}

void hhw_lol_battery_soc (TD_HappyHotwater* hhw,	Valuebuffer* db)
	{

	}
TD_HappyHotwater Hhw;
