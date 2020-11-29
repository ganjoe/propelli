/*
 * delay.h
 *
 *  Created on: Oct 8, 2020
 *      Author: danie
 */

#ifndef PROPELLI_DELAY_H_
#define PROPELLI_DELAY_H_
#include "datatypes.h"
#include "tim.h"

#endif /* PROPELLI_DELAY_H_ */

typedef struct
{
    uint32_t period,timerfreq,timercapacity, overflows, tickdiff, tickoffset, maxdelay_us;

    TIM_HandleTypeDef* hwtimer;
}
DELAY_COUNTER;
void delay_init(DELAY_COUNTER *delay,  TIM_HandleTypeDef *hwtimer);
int delay_s(DELAY_COUNTER *delay, uint32_t value, uint32_t exp);
void delay_us(DELAY_COUNTER *delay, uint32_t value);
void delay_ovf_callback(DELAY_COUNTER *delay);

extern DELAY_COUNTER delay;
