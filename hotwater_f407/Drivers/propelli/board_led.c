/*
 * board_led.c
 *
 *  Created on: 25.09.2020
 *      Author: daniel
 */

#include "board_led.h"
#include "terminal.h"
#include "mcp23.h"


 modflag mf_led_red_toggle;
 modflag mf_led_green_toggle;

 void mfinit_boardled()
 {
	  modflag_init(&mf_led_red_toggle, HALTICK, 10);
	  modflag_init(&mf_led_green_toggle, HALTICK, 5);
 }
 void mftick_boardled()
 {
	    modflag_upd_regular(&mf_led_red_toggle);
	    modflag_upd_regular(&mf_led_green_toggle);
 }
 void mftask_boardled()
     {
     task_toggle_green_led(&mf_led_green_toggle);
     task_toggle_red_led(&mf_led_red_toggle);
     }
void task_toggle_green_led(modflag *thismf)
    {
    if (thismf->flag && thismf->init_done)
	{
	thismf->repeat = modflag_tickdiff(thismf);
	HAL_GPIO_TogglePin(led_green_GPIO_Port, led_green_Pin);

	thismf->duration = modflag_tickdiff(thismf);
	thismf->flag = false;
	}
    }

void task_toggle_red_led(modflag *thismf)
    {
    if (thismf->flag && thismf->init_done)
	{
	thismf->repeat = modflag_tickdiff(thismf);
	HAL_GPIO_TogglePin(led_red_GPIO_Port, led_red_Pin);
	thismf->flag = false;
	thismf->duration = modflag_tickdiff(thismf);
	}
    }
