/*
 * delay.c
 *
 *  Created on: Oct 8, 2020
 *      Author: daniel
 *      https://stm32f4-discovery.net/2015/06/how-to-properly-enabledisable-interrupts-in-arm-cortex-m/
 */
#include "delay.h"
#include "utils.h"

void delay_init(DELAY_COUNTER *delay, TIM_HandleTypeDef *hwtimer)
    {
    delay->timerfreq = 80E6;
    delay->hwtimer = hwtimer;
    delay->hwtimer->Init.Period = 0xFFFFFFFF;
    delay->tickoffset = 40;	//feintuning, nich übertreiben sonst überlauf
    delay->overflows = 1;
    delay->maxdelay_us = (float) delay->hwtimer->Init.Period
	    / (float) delay->timerfreq * 1.0E6;

    HAL_TIM_Base_Start_IT(hwtimer);
    }

void delay_us(DELAY_COUNTER *delay, uint32_t value)
    {
    utils_truncate_number_int((int*) value, 1, delay->maxdelay_us);
    delay->hwtimer->Instance->CNT = 0;
    delay->tickdiff = (delay->timerfreq) / 1000000;
    delay->tickdiff *= value;
    delay->tickdiff -= delay->tickoffset;
    while (delay->hwtimer->Instance->CNT < delay->tickdiff)
	{

	}
    }
//TODO: nicht getestet
void delay_us_sisr(DELAY_COUNTER *delay, uint32_t value)
    {
    uint32_t prim;
    prim = __get_PRIMASK();
    __disable_irq();
    utils_truncate_number_int((int*) value, 1, delay->maxdelay_us);
    delay->hwtimer->Instance->CNT = 0;
    delay->tickdiff = (delay->timerfreq) / 1000000;
    delay->tickdiff *= value;
    delay->tickdiff -= delay->tickoffset;
    while (delay->hwtimer->Instance->CNT < delay->tickdiff)
	{

	}
    if (!prim)
	{
	__enable_irq();
	}
    }

void delay_ovf_callback(DELAY_COUNTER *delay)
    {

    ++delay->overflows;
    }

DELAY_COUNTER delay =  { 0};
