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


typedef enum
    {
    DATETIMEMONO = 0,	//"14.03.12 13:30:56"
    DATETIMEFAT,
    DATETIMESHORT,	//"14.3 13:30"
    DATEMONO,		//"14.03.12"
    TIMEMONO,		//"13:30:56"
    TIMENOSEC,		//"13:30"
    TIMEMONOSUBS,		//"13:30:56.323"
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
