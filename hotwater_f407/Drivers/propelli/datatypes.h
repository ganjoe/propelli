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

#define BIT0 0x1
#define BIT1 0x2
#define BIT2 0x4
#define BIT3 0x8
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80
#define BIT8 0x100
#define BIT9 0x200
#define BIT10 0x400
#define BIT11 0x800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

typedef unsigned int	uint;

typedef enum
    {		//reihenfolge egal
    DATE,
    TIME,
    MCUVOLT,
    BATTHWVOLT,
	BATTSOC,
    BATTHWCURR,
    BATTHWPWR,
    TEMPMCU,
    TEMPCOLD,
    TEMPHOT,
    STATUS,
    LOGLINES,
	BYTESWROTE,
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
			battsoc,
			batthwpwr;
    uint16_t iostatus,
			loglines;
    colnames names;
    char	ppbuffer[120];
    }
    Valuebuffer;

extern Valuebuffer db;


#endif /* INC_DATATYPES_H_ */
