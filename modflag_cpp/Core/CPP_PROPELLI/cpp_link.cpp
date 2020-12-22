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

    	ClassBoardLed LedRed(5);
    	ClassBoardLed LedGreen(2);

    	void cpp_tasks (void)
    	    {
    	    LedRed.task();
    	    LedGreen.task();
    	    }
    	void cpp_init (void)
    	    {
    	    init_mfSystick(&prop_systick, SYSTICK);
 	    }
    	void cpp_update (void)
 	    {
    	    LedGreen.updRegular();
    	    LedRed.updRegular();
    	    }

    #ifdef __cplusplus
    	}
    #endif



