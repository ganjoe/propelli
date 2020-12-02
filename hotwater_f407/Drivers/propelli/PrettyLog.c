/*
 * PrettyLog.c
 *
 *  Created on: Nov 5, 2020
 *      Author: danie
 */

#include "PrettyLog.h"
#include "rtc.h"
#include "fatfs.h"
#include "terminal.h"
#include "usart.h"

void pl_ppValues	(char* buffer)
    {
    static char pbuffer[120]={0};
    char tabulator[2]= "\t\0";

	switch (db.names)
	    {
	    case DATE:
		pl_rtc_timestring(pbuffer, DATEMONO);
		break;
	    case TIME:
		pl_rtc_timestring(pbuffer,TIMEMONO);
		break;
	    case BATTHWVOLT:
		sprintf(pbuffer, "%2.2f\0", db.batthwvolt);
		break;
	    case BATTHWCURR:
		sprintf(pbuffer, "%7.3f\0", db.batthwcurr);
		break;
	    case BATTHWPWR:
		sprintf(pbuffer, "%1.3f\0", db.batthwpwr);
		break;
	    case TEMPHOT:
		sprintf(pbuffer, "%2.1f\0", db.temphot);
		break;
	    case TEMPCOLD:
		sprintf(pbuffer, "%2.1f\0", db.tempcold);
		break;
	    case TEMPMCU:
		sprintf(pbuffer, "%2.0f\0", db.tempmcu);
		break;
	    case MCUVOLT:
		sprintf(pbuffer, "%1.2f\0", db.mcuvolt);
		break;
	    case STATUS:
		sprintf(pbuffer, "%d\0", db.iostatus);
		break;
	    case LOGLINES:
		sprintf(pbuffer, "%d\0", db.loglines);
		break;

	    default:		break;
	}
	int valstrsize = strlen(pbuffer);
	valstrsize += strlen(tabulator);
	strncat(pbuffer,tabulator,1);
	strncat(buffer,pbuffer,valstrsize);
    }

int pl_rtc_timestring	(char* buffer, int fmt)
    {

    RTC_TimeTypeDef rtc= {0};
    RTC_DateTypeDef rtd= {0};
    // muss immer zusammen gecallt werden.
    // rtc taktquelle in cube einstellen und funktion prüfen
    HAL_RTC_GetTime(&hrtc, &rtc, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &rtd, RTC_FORMAT_BIN);

    switch (fmt)
	{
	case DATEMONO:
	    {
	    sprintf(buffer,"%2d.%2d.%2d\0", rtd.Date, rtd.Month, rtd.Year);
	    }break;
	case TIMEMONO:
	    {
	    sprintf(buffer,"%2d:%2d:%2d  ", rtc.Hours, rtc.Minutes, rtc.Seconds);
	    }break;
	case TIMEMONOSUBS:
	    {
	   //wird nur von stm-rtc v2, v2
	   //funktion selber stricken mit rtc-isr und timer
	    sprintf(buffer,"%.2d:%.2d:%.2d.%.3d", rtc.Hours, rtc.Minutes, rtc.Seconds, rtc.SubSeconds);
	    }break;
	case DATETIMEMONO:
	    {
	    sprintf(buffer,"%.2d.%.2d.%.2d %.2d:%.2d:%.2d", rtd.Date, rtd.Month, rtd.Year, rtc.Hours, rtc.Minutes, rtc.Seconds);
	    }
	case DATETIMEFAT:
	    {
	    sprintf(buffer,"log_%.2d.%.2d.%.2d-%d_%.2d_%.2d.hhw", rtd.Date, rtd.Month, rtd.Year, rtc.Hours, rtc.Minutes, rtc.Seconds);
	    }
	}

    return 1;
    }

void mfinit_prettylog(TD_prettylog *prettylog)
    {
    FRESULT fres;
    modflag_init(&prettylog->pp_modflag, HALTICK, 1);
    fres = filelog_init(&filelog);

    }
void mftick_prettylog(TD_prettylog *prettylog)
    {
    modflag_upd_regular(&prettylog->pp_modflag);
    }
void mftask_prettylog(modflag *thismf)
    {
    if (thismf->flag && thismf->init_done)
		{
		thismf->repeat = modflag_tickdiff(thismf);
		char buffer[80]={0};
		db.names=DATE; 		pl_ppValues( buffer);
		db.names=TIME;		pl_ppValues( buffer);
		//db.names=MCUVOLT;   	pl_ppValues( buffer);
		db.names=BATTHWVOLT;	pl_ppValues( buffer);
		db.names=BATTHWCURR;	pl_ppValues( buffer);
		db.names=BATTHWPWR;	pl_ppValues( buffer);
		//db.names=TEMPMCU;   	pl_ppValues( buffer);
		db.names=TEMPCOLD;  	pl_ppValues( buffer);
		db.names=TEMPHOT;   	pl_ppValues( buffer);
		db.names=STATUS;	pl_ppValues( buffer);
		db.names=LOGLINES;	pl_ppValues( buffer);
		strncat		(buffer,"\r",1);
		strcpy		(filelog.pbuffer.lastLine, buffer);

		pl_lol_update	(&filelog);

		int len = strlen(filelog.pbuffer.lastLine);
		term_printf(&btTerm, buffer);
		thismf->duration = modflag_tickdiff(thismf);
		thismf->flag = false;
		}
    }

TD_prettylog prettylog;

