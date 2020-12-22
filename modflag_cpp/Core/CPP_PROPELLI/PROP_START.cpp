/*
 * PROP_START.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: danie
 */

#include "PROP_START.hpp"

/**Datatype for Softwaretimers
 *
 */

void init_mfSystick(mfSystick* thismf,uint32_t setpoint_hz )
    {
    /**Timer Configuration done by CubeMX/HAL*/
    thismf->mfTimer = htim6;
    }

mfSystick prop_systick;
