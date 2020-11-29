/*
 * datatypes_tasks.h
 *
 *  Created on: 22.09.2020
 *      Author: daniel
 */

#ifndef DATATYPES_TASKS_H_
#define DATATYPES_TASKS_H_


#include "datatypes.h"
#include "tim.h"


typedef struct
    {
    int divisor;

    uint64_t rampcounter, callcount;
    uint64_t counter, ovf;
    uint64_t oldtick, systick, newtick, tickdiff;
    uint64_t duration, repeat;
    float freq, duty_sp;
    /*
     * flag		funktion wird ausgeführt, danach rückgesetzt. wird durch mftask gepollt
     * flag_delay	funktion wird nach ablauf über callback-isr ausgeführt. gehört zu mf_tse
     * flag_reset	error-flag, wird ggf. durch mfinit-funktion rückgesetzt
     * init_done	enable für die funktion
     */
    int flag, flag_delay, flag_reset, init_done;
    const char name[8];
    }
modflag;



void modflag_init(modflag *thismf, float systick, float setpoint_hz);
void modflag_upd_regular(modflag *thismf);
void modflag_enable(modflag *thismf);
void modflag_disable(modflag *thismf);
void modflag_set_delay_until_callback(modflag *thismf);

uint64_t modflag_tickdiff(modflag *cnt);
void modflag_ovf_callback();
extern modflag mf_global_systick;




#endif /* DATATYPES_TASKS_H_ */
