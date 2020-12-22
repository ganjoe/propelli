/*
 * PROP_START.hpp
 *
 *  Created on: Dec 22, 2020
 *      Author: danie
 */

#ifndef INC_PROP_START_HPP_
#define INC_PROP_START_HPP_

#include "stdint-gcc.h"
#include "tim.h"
#include "utils.hpp"



#define HALTICK 1000
#define SYSTICK 10000

/**
 * Systick with high Resolution.
 * An approbiate Timer have to be choose for each Platform.
 */

typedef struct
    {
    TIM_HandleTypeDef mfTimer;
    uint64_t counter, ovf;
    }
mfSystick;

void init_mfSystick(mfSystick* thismf, uint32_t setpoint_hz);

extern mfSystick prop_systick;

#endif /* INC_PROP_START_HPP_ */
