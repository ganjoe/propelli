/*
 * datatypes_tasks.c
 *
 *  Created on: 10.09.2020
 *      Author: daniel
 */

#include "datatypes_tasks.h"
#include "tim.h"
#include "stdlib.h"

/* task management */
void modflag_upd_regular(modflag *thismf)
    {
    if (thismf->init_done)
	{
	thismf->counter++;
	if (!(thismf->counter % thismf->divisor))
	    {
	    thismf->flag = true;
	   // thismf->counter = false;
	    }
	}
    }

uint64_t modflag_tickdiff(modflag *cnt)
    {
    uint64_t counter = TIM6->CNT;
    cnt->ovf = mf_global_systick.ovf;

    cnt->systick = counter + (cnt->ovf * 0xFFFF);
    cnt->newtick = cnt->systick;
    cnt->tickdiff = cnt->newtick - cnt->oldtick;
    cnt->oldtick = cnt->newtick;
    return cnt->tickdiff;
    }

void modflag_enable(modflag *thismf)
	{
	thismf->init_done = true;
	}

void modflag_disable(modflag *thismf)
{
thismf->init_done = false;
}

void modflag_init(modflag *thismf, float systick, float setpoint_hz)
    {
    utils_truncate_number(&setpoint_hz, 0, systick); // max 1h minimum: systick
    thismf->freq = systick;
    thismf->divisor = systick / setpoint_hz;
    if (setpoint_hz==0)
		{
		thismf->init_done = false;
		}
    else
		{
		thismf->init_done = true;
		}
    }

void modflag_ovf_callback()
    {
    ++mf_global_systick.ovf;
    }

modflag mf_global_systick;

