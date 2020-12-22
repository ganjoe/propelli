/*
 * BoardLed.hpp
 *
 *  Created on: Dec 22, 2020
 *      Author: danie
 */

#ifndef INC_BOARDLED_HPP_
#define INC_BOARDLED_HPP_

#include "ClassModflag.h"
#include "gpio.h"
#include "PROP_START.hpp"


class ClassBoardLed : public ClassModflag
    {
public:
    ClassBoardLed()
    :ClassModflag(HALTICK, 10000, &prop_systick)
    {    }
    uint32_t task()
	{
	 if(taskFlag)
	     {

	     }
	    Toggle();
	 }


private:
   void Toggle()    { HAL_GPIO_TogglePin( led_green_GPIO_Port, led_green_Pin); }
};


#endif /* INC_BOARDLED_HPP_ */
