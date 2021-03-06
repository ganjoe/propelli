/*
 * PrettyLog.c
 *
 *  Created on: Nov 5, 2020
 *      Author: danie
 */

#include "PrettyLog.h"
#include "rtc.h"
#include "sdfile.h"
#include "HappyHotWater.h"
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
		sprintf(pbuffer, "%2.2f", db.batthwvolt);
		break;
	    case BATTHWCURR:
		sprintf(pbuffer, "%7.3f", db.batthwcurr);
		break;
	    case BATTHWPWR:
		sprintf(pbuffer, "%1.3f", db.batthwpwr);
	    case BATTSOC:
		sprintf(pbuffer, "%3.1f", db.battsoc);
		break;
	    case TEMPHOT:
		sprintf(pbuffer, "%2.1f", db.temphot);
		break;
	    case TEMPCOLD:
		sprintf(pbuffer, "%2.1f", db.tempcold);
		break;
	    case TEMPMCU:
		sprintf(pbuffer, "%2.0f", db.tempmcu);
		break;
	    case MCUVOLT:
		sprintf(pbuffer, "%1.2f", db.mcuvolt);
		break;
	    case STATUS:
		sprintf(pbuffer, "%2d", db.iostatus);
		break;
	    case LOGLINES:
		sprintf(pbuffer, "%4d", db.loglines);
		break;
	    case BYTESWROTE:
		sprintf(pbuffer, "%1.3f", (float)(db.loglines / 1024.0));
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
	case TIMECMD:
	    {
	    sprintf(buffer,"settime_%d_%d_0", rtc.Hours, rtc.Minutes);
	    }break;
	case DATECMD:
	    {
	    sprintf(buffer,"setdate_%d_%d_%d", rtd.Date, rtd.Month, rtd.Year);
	    }break;
	case DATEMONO:
	    {
	    sprintf(buffer,"%2d.%2d.%2d", rtd.Date, rtd.Month, rtd.Year);
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
	    sprintf(buffer,"log_%.2d.%.2d.%.2d-%d_%.2d_%.2d.hhw\0", rtd.Date, rtd.Month, rtd.Year, rtc.Hours, rtc.Minutes, rtc.Seconds);
	    }
	}

    return 1;
    }

void mfinit_prettylog(TD_prettylog *prettylog)
    {

    modflag_init(&prettylog->pp_modflag, HALTICK, 1);

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
		char buffer[120]={0};
		db.names=DATE; 			pl_ppValues( buffer);
		db.names=TIME;			pl_ppValues( buffer);
		//db.names=MCUVOLT;   	pl_ppValues( buffer);
		db.names=BATTHWVOLT;	pl_ppValues( buffer);
		db.names=BATTSOC;		pl_ppValues( buffer);
		db.names=BATTHWCURR;	pl_ppValues( buffer);
		//db.names=BATTHWPWR;	pl_ppValues( buffer);
		//db.names=TEMPMCU;   	pl_ppValues( buffer);
		db.names=TEMPCOLD;  	pl_ppValues( buffer);
		db.names=TEMPHOT;   	pl_ppValues( buffer);
		//db.names=STATUS;		pl_ppValues( buffer);
		//db.names=LOGLINES;	pl_ppValues( buffer);
		db.names=BYTESWROTE;	pl_ppValues( buffer);
		strncat		(buffer,"\r",1);
		sdfile_add_logline(&happylog, buffer);
		term_printf(&btTerm, buffer);
		hhw_lol_report(&Hhw, &db);

		thismf->duration = modflag_tickdiff(thismf);
		thismf->flag = false;
		}
    }

TD_prettylog prettylog;

