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
	SPUELE_KALT,
	SPUELE_WARM,
	BRAUSE_KALT,
	BRAUSE_WARM,
	DRAIN,
	REFILL_HOT,	//von kalt nach warmwassertank
	REFILL_COLD,
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
	//zielwert regelung
	float hw_temperature;

	//hysterese zweipunktregler in grad
	float hw_tempdelta;

	//array mit bitmasken für schaltzustände
	uint16_t outwords[16];

	//bitmaske für Betriebszustände
	uint16_t states;
	float VoltLevel_pwrok, VoltLevel_lowbatt;

	//enums als zeiger auf array mit bitsets
	HHW_WORD_NAMES outnames;

	modflag mf_modflag;

}
TD_HappyHotwater;

void mftask_happyhotwater(TD_HappyHotwater* hhw, Valuebuffer* db);

int hhw_lol_bimetal(float *value, float delta, float setpoint);

extern TD_HappyHotwater Hhw;

#endif /* PROPELLI_HAPPYHOTWATER_H_ */
