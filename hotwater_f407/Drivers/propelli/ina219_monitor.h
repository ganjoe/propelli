/*
 * ina219_monitor.h
 *
 *  Created on: Nov 26, 2020
 *      Author: danie
 */

#ifndef PROPELLI_INA219_MONITOR_H_
#define PROPELLI_INA219_MONITOR_H_

#include "datatypes_tasks.h"

//shuntvoltage
#define INA219_RANGE_40mV 	0x0
#define INA219_RANGE_80mV	0x800
#define INA219_RANGE_160mV	0x1000
#define INA219_RANGE_320mV	0x1800
//busvoltage
#define INA219_VRANGE_32V	0x2000
#define INA219_VRANGE_16V	0x0		//default
//operating modes //TODO:auch mal testen
#define	INA219_PDOWN		0x0
#define INA219_ADCDOWN		0x4
#define INA219_RESET		0x8000

#define	INA219_SHUNTV_TRIG	0x1
#define INA219_SHUNT_CONT	0x5

#define INA219_BUSVOLT_TRIG	0x2
#define INA219_BUSVOLT_CONT	0x6
//	_SHUNT_BUS_VOLT_TRIGGERED_CONVERSION
#define INA219_SBVOLT_TRIG	0x3
#define INA219_SBVOLT_CONT	0x7		//default



/*
 * weitere kombinationen für verringerte auflösung nicht aufgezählt
 */

#define INA219_SAMPLES_128 	0x78	//68.1ms
#define INA219_SAMPLES_64 	0x70	//34.05ms
#define INA219_SAMPLES_32	0x68	//17.02ms
#define INA219_SAMPLES_16 	0x60	//8.51ms
#define INA219_SAMPLES_8 	0x58	//4.26ms
#define INA219_SAMPLES_4 	0x50	//2.13ms
#define INA219_SAMPLES_2	0x48	//1.06ms
#define INA219_SAMPLES_1	0x40	//0.532ms

//die vier bits für die samples kodieren auch die auflösung. beide einstellungen können nicht kombiniert werden

/*
 * die hexadressen der ic register sind ab 0x0 durchnummeriert, daher funktioniert das enum
 */
typedef enum
    {
    CONFIG=0,
    SHUNT,
    BUS,
    POWER,
    CURRENT,
    CALIBRATION,
    }
TE_INA219_REGISTER;

typedef struct
    {
    //taskcounter
    modflag mf_ina219;
    //bitshift auf 7bit für stm32 notwendig
    uint16_t addr;
    //hw instanz
    I2C_HandleTypeDef hi2c;
    //ergebnisse
    float pwerbuff, voltbuff, currbuff;
    //zum berechnen des calibration registers
    float shunt, max_current;
    //busvolt_lsb ist immer 0.004V
    //current_lsb ist abhängig von calibration
    //power_lsb ist immer faktor 20 current_lsb
    float current_lsb, power_lsb, busvolt_lsb;
    //wird nach setup-bitmanipulationen übertragen
    uint16_t configbuff;
    //merker für pga settings um current reading zu berechnen
    //Table 7. Shunt Voltage Register Format
    int range;

    uint16_t calibration;
    }
TD_INA219;


void mfinit_ina219	(TD_INA219 *batt_hw);
void mftick_ina219	(TD_INA219 *batt_hw);
void mftask_ina219	(TD_INA219 *batt_hw);

void ina_setup		(TD_INA219 *batt_hw);
void ina_lol_calc_cal	(TD_INA219 *batt_hw);
void ina_writeconfig	(TD_INA219 *batt_hw, uint16_t config);

float ina_getpower	(TD_INA219 *batt_hw);
float ina_getcurrent	(TD_INA219 *batt_hw);
float ina_getvoltage	(TD_INA219 *batt_hw);

HAL_StatusTypeDef ina_lolwriteword	(TD_INA219 *batt_hw, uint16_t buffer, uint8_t reg);
HAL_StatusTypeDef ina_lolreadword 	(TD_INA219 *batt_hw, int16_t *buffer, uint8_t reg);


extern TD_INA219 batt_hw;

#endif /* PROPELLI_INA219_MONITOR_H_ */
