/*
 * ClassModflag.h
 *
 *  Created on: Dec 22, 2020
 *      Author: danie
 */

#ifndef INC_CLASSMODFLAG_H_
#define INC_CLASSMODFLAG_H_

#include "PROP_START.hpp"

class ClassModflag
    {
public:
    ClassModflag(uint32_t systick, float setpoint_hz, mfSystick* timer);

    float setpoint_hz;

private:

bool initDone, taskFlag;


uint32_t systick, newTick, oldTick, counter, ovf;

mfSystick* ptrSystick;



   float divisor;

    void  updRegular();

    uint32_t tickDiff();

    void setStatus(bool enable);

    };

#endif /* INC_CLASSMODFLAG_H_ */
