/*
 * board_led.h
 *
 *  Created on: 25.09.2020
 *      Author: daniel
 */
#include "datatypes.h"
#include "datatypes_tasks.h"

#ifndef PROPELLI_BOARD_LED_H_
#define PROPELLI_BOARD_LED_H_

 void mfinit_boardled();
 void mftick_boardled();
 void mftask_boardled();

void task_toggle_green_led(modflag *thismf);	//nucleo board led
void task_toggle_red_led(modflag *thismf);

extern modflag mf_led_red_toggle;
extern modflag mf_led_green_toggle;

#endif /* PROPELLI_BOARD_LED_H_ */
