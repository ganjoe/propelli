/*
 * HappyHotWater.h
 *
 *  Created on: 03.12.2020
 *      Author: daniel
 */

#ifndef PROPELLI_HAPPYHOTWATER_H_
#define PROPELLI_HAPPYHOTWATER_H_

#include "datatypes.h"
#include "datatypes_tasks.h"
#include "utils.h"
#include "mcp23.h"

typedef enum
{

	/*Betriebsmodi für kalt und warm*/
	HOTWTR_SPUELE,
	COLDWTR_SPUELE,
	HOTWTR_SHOWER,
	COLDWTR_SHOWER,
	/*Betriebsmodi für Tank nachfüllen*/
	HOTROD300,
	TANK_MODE_ECO,
	TANK_MODE_FULL,
	//HOTROD_HALF_POWER, //taktender Betrieb oder pwm

	/*Betriebszustand*/
	POWER_INV,
	POWER_LV,
	POWER_VB,
	/*Tank*/
	TANK_HOT_FULL,
	TANK_HOT_MID,
	/*Wasser Marsch*/
	SPUELEN,
	DUSCHEN,
	FREIG_REFILL,
	TANK_MODE_NOHEAT,
}
HHW_STATES_NAMES;
typedef enum
{
	BTN_KALT,
	BTN_WARM,
	BTN_SPUELE,
	BTN_BRAUSE,
	LVLSW_FULL,
	LVLSW_MID,
}
HHW_INWORD_NAMES;
typedef enum
{
	VALVE_DRAIN,
	VALVE_SHOWR,
	VALVE_COLD,
	VALVE_HOT,
	PUMP_HOT,
	PUMP_COLD,
	HOTROD_300,	//heizung 300W
}
HHW_OUTWORD_NAMES;
typedef struct
{

	//schaltzustände und deren zahl an aufrufen
	uint64_t iocount[16];
	//bitmaske für Betriebszustände
	uint16_t inwords[16];
	uint16_t outwords[16];
	uint32_t states[32];
	//aktuelle kombination von betriebszuständen
	uint32_t state, outword;
	//TODO: inwords ??
	//zielwert regelung
	float hw_temperature;

	//hysterese zweipunktregler in grad
	float hw_tempdelta;

	//Spannungsschwellen für Betrieb
	float VoltLevel_power_inv, VoltLevel_lowbatt, VoltLevel_highbatt;

	//Temperaturschwellen für Heizstab (Single)
	float TempLevel_low, TempLevel_high;

	//enums als zeiger auf array mit bitsets
	HHW_INWORD_NAMES outnames;

	modflag mf_modflag;

}
TD_HappyHotwater;

#define COLDWTR_SPUELE 		BIT0
#define HOTWTR_SHOWER 		BIT1
#define COLDWTR_SHOWER 		BIT2
#define FREIG_HEIZEN		BIT3
#define TANK_MODE_ECO		BIT4
#define TANK_MODE_FULL 		BIT5
#define POWER_INV			BIT6
#define POWER_LV			BIT7
#define POWER_VB			BIT8
#define TANK_HOT_FULL		BIT9
#define TANK_HOT_MID		BIT10
#define TANK_TEMP_HL		BIT11
#define SPUELEN				BIT12
#define DUSCHEN				BIT13
#define FREIG_REFILL				BIT14
#define HOTWTR_SPUELE		BIT15

void mfinit_happyhotwater	(TD_HappyHotwater* hhw);
void mftask_happyhotwater	(TD_HappyHotwater* hhw);
void mftick_happyhotwater	(TD_HappyHotwater* hhw);

void hhw_lol_update			(TD_HappyHotwater* 	hhw,	Valuebuffer* db);
void hhw_lol_drain			(TD_HappyHotwater*	hhw, 	TD_MCP *mcp_io);
void hhw_lol_report			(TD_HappyHotwater*	hhw, 	Valuebuffer* db);
void hhw_lol_fillnheat		(TD_HappyHotwater* 	hhw,	Valuebuffer* db, TD_MCP *mcp_io);
void hhw_lol_battery_soc 	(TD_HappyHotwater* 	hhw,	Valuebuffer* db);
void hhw_set_state			(TD_HappyHotwater* hhw, 	int stateitr, int state);
int hhw_get_invstate		(TD_HappyHotwater* hhw, int stateitr);
int  hhw_get_state			(TD_HappyHotwater*  hhw, 	int stateitr);


void hhw_set_outstates		(TD_HappyHotwater* hhw,
							int ioitr,
							int stateitr,
							TD_MCP *mcp_io);

int hhw_set_instate			(TD_HappyHotwater* 	hhw,
							HHW_INWORD_NAMES 	ioitr,
							uint32_t				 	stateitr,
							uint32_t 			iobuffer,
							int 				inv);

void hhw_set_out			(TD_HappyHotwater* hhw,
							uint16_t outnamesmask,
							int state);

extern TD_HappyHotwater Hhw;

#endif /* PROPELLI_HAPPYHOTWATER_H_ */
