/*
 * PROP_START.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: danie
 */

#include "PROP_START.hpp"



void init_mfSystick(mfSystick* thismf, uint32_t systick, uint32_t setpoint_hz)
    {
    utils_truncate_number_int((int*)setpoint_hz, 0, systick); // max 1h minimum: systick
    thismf->systick = systick;
    thismf->divisor = systick / setpoint_hz;
    }

mfSystick prop_systick;
