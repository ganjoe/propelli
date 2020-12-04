/*
 * datatypes.h
 *
 *  Created on: Jul 7, 2020
 *      Author: danie
 */

#ifndef INC_DATATYPES_H_
#define INC_DATATYPES_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "math.h"
#include "main.h"
#include "utils.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"


#define HALTICK 1000.0
#define SYSTICK 10000.0



typedef enum
    {		//reihenfolge egal
    DATE,
    TIME,
    MCUVOLT,
    BATTHWVOLT,
    BATTHWCURR,
    BATTHWPWR,
    TEMPMCU,
    TEMPCOLD,
    TEMPHOT,
    STATUS,
    LOGLINES,
    }
    colnames;

typedef struct
    {
    float 	battvolt,
			mcuvolt,
			temphot,
			tempcold,
			tempmcu,
			batthwcurr,
			batthwvolt,
			batthwpwr;
    uint16_t iostatus,
			loglines;
    colnames names;
    }
    Valuebuffer;

extern Valuebuffer db;


#endif /* INC_DATATYPES_H_ */
