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
    bool initDone, taskFlag;

    uint32_t systick, newTick, oldTick, counter, ovf, repeat, rampcounter, rampmod, callcount;

    mfSystick* ptrSystick;

    float divisor;

    void  updRegular();

    void tickDiff();

    void setStatus(bool enable);
private:
    };

#endif /* INC_CLASSMODFLAG_H_ */
