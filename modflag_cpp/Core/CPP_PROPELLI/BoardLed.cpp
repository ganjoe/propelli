/*
 * BoardLed.cpp
 *
 *  Created on: Dec 22, 2020
 *      Author: danie
 */

#include "BoardLed.hpp"

uint32_t  ClassBoardLed::task()
	{
	 if(taskFlag)
	     {
	     Toggle();
	     }
	 }


