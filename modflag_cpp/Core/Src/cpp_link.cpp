/**
 * cpp_link.cpp
 *
 *  Created on: 19.12.2020
 *      Author: daniel
 *
 * https://community.st.com/s/question/0D50X0000BFER0CSQX/cube32-feature-request-need-to-handle-cpp-files
 */

#include "cpp_link.hpp"
#include "PROP_START.hpp"

    #ifdef __cplusplus
    	extern "C"
    	{
    #endif
	#include "BoardLed.hpp"
    	void cpp_tasks (void)
    		{

    		}
    	void cpp_init (void)
    		{
    		init_mfSystick(&prop_systick, SYSTICK, 3.33);
    		ClassBoardLed LedRed;


    		}

    #ifdef __cplusplus
    	}
    #endif



