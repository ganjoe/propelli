/*
 * ClassModflag.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: danie
 */

#include "ClassModflag.h"

ClassModflag::ClassModflag(uint32_t systick, float setpoint_hz, mfSystick* timer)
    {
    utils_truncate_number(&setpoint_hz, 0, systick);

    divisor = (float) systick / setpoint_hz;

    if (setpoint_hz == 0)
	initDone = false;

    else
	initDone = true;
    }

void ClassModflag::updRegular()
    {
    if (initDone)
	{
	counter++;
	if (counter % (uint32_t) divisor)
	    {
	    taskFlag = true;
	    }
	}

    }

void ClassModflag::tickDiff()
    {
    /**
     * Calculates the duration for a task-call.
     * Timervalues for max and actual value have to be provided,
     */
    uint32_t counter = ptrSystick->mfTimer.Instance->CNT;
    uint32_t tickdiff;
    systick = counter + (ptrSystick->ovf * ptrSystick->mfTimer.Instance->ARR);
    newTick = systick;
    tickdiff = newTick - oldTick;
    oldTick = newTick;
    repeat = tickdiff;
    rampcounter = callcount % rampmod;
    }

void ClassModflag::setStatus(bool enable)
    {
    if (enable)
	initDone = true;
    else
	initDone = false;
    }
