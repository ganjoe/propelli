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
#include "BoardLed.hpp"

    #ifdef __cplusplus
    	extern "C"
    	{
    #endif

/* call hier direkt nach resethandler wenn klasse im stack initialisiert wird */

    	ClassBoardLed *LedRed, *LedGreen;

    	void cpp_tasks ()
    	    {
    	    LedRed->task();
    	    LedGreen->task();
    	    }
    	void cpp_init ()
    	    {
    	    LedRed = new ClassBoardLed(10);
    	    LedGreen = new ClassBoardLed(2);
    	    init_mfSystick(&prop_systick, SYSTICK);
 	    }
    	void cpp_update ()
 	    {
    	    LedGreen->updRegular();
    	    LedRed->updRegular();
    	    }

    #ifdef __cplusplus
    	}
    #endif



