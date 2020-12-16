/*
 * PrettyLog.h
 *
 *  Created on: Nov 5, 2020
 *      Author: daniel
 */

#include "datatypes_tasks.h"
#include "datatypes.h"



#ifndef PROPELLI_PRETTYLOG_H_
#define PROPELLI_PRETTYLOG_H_
//FATFS/APP/

/* Jahreszahl,bzw. Jahrzehnt immer ab 20 */
typedef enum
    {

    DATETIMESHORT,	//"14.3 13:30"
    DATETIMEMONO,	//"14.03.12 13:30:56"
	DATETIMEFAT,	//"14.03.12 13:30:56.hhw"
    DATESHORT,		//"14.3.20"
    DATEMONO,		//"14.03.20"
	DATECMD,		//"14_3_20"
	WEEKDAY,		//"Sunday"
	TIMECMD,		//"18_37_00"
    TIMEMONO,		//"13:30:56"
    TIMENOSEC,		//"13:30"
    TIMEMONOSUBS,	//"13:30:56.323"
    }
    pp_timestringstyle;



typedef struct
    {
	colnames 			colums;
	Valuebuffer 		values;
	pp_timestringstyle timefmt;
	modflag 				pp_modflag;
    }
    TD_prettylog;

void mfinit_prettylog(TD_prettylog *prettylog);

void mftick_prettylog(TD_prettylog *prettylog);

void mftask_prettylog(modflag *thismf);

void pl_ppValues(char* buffer);

int pl_rtc_timestring	(char* buffer, int fmt);

extern TD_prettylog prettylog;
#endif /* PROPELLI_PRETTYLOG_H_ */
