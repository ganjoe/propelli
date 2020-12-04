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

typedef enum
{
	/*versorgungsart*/

	STDBY_INV,
	STDBY_LV,

	/*Betriebsmodus*/
	HOTWTR_SPUELE,
	COLDWTR_SPUELE,
	HOTWTR_SHOWER,
	COLDWTR_SHOWER,

	/*Betriebszustand*/
	TANK_HOT_FULL,
	TANK_HOT_MID,
	TANK_HOT_EMPTY,
	HEIZEN,
}
HHW_STATES;

typedef enum
{
	//outputs
	VALVE_DRAIN,
	VALVE_SHOWR,
	VALVE_COLD,
	VALVE_HOT,
	PUMP_HOT,
	PUMP_COLD,
	HOTROD_300,	//heizung 300W
	//user inputs
	BTN_KALT,
	BTN_WARM,
	BTN_SPUELE,
	BTN_BRAUSE,
	//system inpus
	LVLSW_UL,
	LVLSW_LL,
}
HHW_WORD_NAMES;

typedef struct
{

	//schaltzustände und deren zahl an aufrufen
	uint16_t iowords[16];
	uint16_t iocount[16];

	//bitmaske für Betriebszustände
	uint16_t states;

	/*--hhw_lol_shower--*/
	/*--hhw_lol_drain--*/

	//zielwert regelung
	float hw_temperature;

	//hysterese zweipunktregler in grad
	float hw_tempdelta;

	//Spannungsschwellen für Betrieb
	float VoltLevel_power_ok, VoltLevel_lowbatt;

	//enums als zeiger auf array mit bitsets
	HHW_WORD_NAMES outnames;

	modflag mf_modflag;

}
TD_HappyHotwater;
void mfinit_happyhotwater	(TD_HappyHotwater* hhw);

void mftask_happyhotwater(TD_HappyHotwater* hhw, Valuebuffer* db);

int hhw_lol_bimetal(float *value, float delta, float setpoint);

extern TD_HappyHotwater Hhw;

#endif /* PROPELLI_HAPPYHOTWATER_H_ */
