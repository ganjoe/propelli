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

	/*Betriebsmodi für kalt und warm*/
	HOTWTR_SPUELE,
	COLDWTR_SPUELE,
	HOTWTR_SHOWER,
	COLDWTR_SHOWER,
	/*Betriebsmodi für Tank nachfüllen*/
	TANK_ALWAYS_EMPTY,
	TANK_ALWAYS_MID,
	TANK_ALWAYS_FULL,
	//HOTROD_HALF_POWER, //taktender Betrieb oder pwm

	/*Betriebszustand*/
	POWER_INV,
	POWER_LV,
	POWER_VB,
	/*Tank*/
	TANK_HOT_FULL,
	TANK_HOT_MID,
	TANK_HOT_EMPTY,
	/*Wasser Marsch*/
	SPUELEN,
	DUSCHEN,
	REFILL,
}
HHW_STATES;
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
	uint16_t inwords[16];
	uint16_t outwords[16];
	uint16_t iocount[16];

	//bitmaske für Betriebszustände
	uint16_t states;

	//zielwert regelung
	float hw_temperature;

	//hysterese zweipunktregler in grad
	float hw_tempdelta;

	//Spannungsschwellen für Betrieb
	float VoltLevel_power_inv, VoltLevel_lowbatt, VoltLevel_highbatt;

	//Batteriekapazität in Prozent zwischen lowbatt und batt
	float state_of_charge;


	//enums als zeiger auf array mit bitsets
	HHW_INWORD_NAMES outnames;

	modflag mf_modflag;

}
TD_HappyHotwater;

void mfinit_happyhotwater	(TD_HappyHotwater* hhw);
void mftask_happyhotwater	(TD_HappyHotwater* hhw);
void mftick_happyhotwater	(TD_HappyHotwater* hhw);

extern TD_HappyHotwater Hhw;

#endif /* PROPELLI_HAPPYHOTWATER_H_ */
