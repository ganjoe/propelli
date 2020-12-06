/*
 * command.c
 *
 *  Created on: Nov 15, 2020
 *      Author: danie
 */

#include "command.h"
#include "mcp23.h"
#include "terminal.h"
#include "utils.h"
#include "rtc.h"
#include "PrettyLog.h"
#include "fatfs.h"
#include "termfile.h"
#include "delay.h"
    extern TD_fatlog filelog;
void Command_init()
    {
        term_lol_setCallback("setword", "\rmcp outlatch\r",	    "bool\r", setword);
        term_lol_setCallback("writepin", "\rGPIOA,B Output no Pullup\r",	    "bool\r", writepin);
        term_lol_setCallback("readpin", "\rGPIOA,B Input no Pullup\r",	    "bool\r", readpin);
        term_lol_setCallback("setallin", "\rGPIOA,B Input no Pullup\r",	    "bool\r", setallin);
        term_lol_setCallback("setdate", "\rDD MM YY\r",	    "bool\r", setdate);
        term_lol_setCallback("settime", "\rhh mm ss\r",	    "bool\r", settime);
        term_lol_setCallback("nlogn", "\rfilename[32]\r",	    "bool\r", nlogn);
        term_lol_setCallback("newlog", "\rrtc filename\r",	    "bool\r", newlog);
        term_lol_setCallback("showconf", "\rrtc filename\r",	    "bool\r", showconf);
        term_lol_setCallback("writeconf", "\rrtc filename\r",	    "bool\r", writeconf);
        term_lol_setCallback("readconf", "\rrtc filename\r",	    "bool\r", readconf);
        term_lol_setCallback("selterm", "\rlog upd speed\r",	    "bool\r", selterm);
        term_lol_setCallback("reset", "\rreset mit countdown\r",	    "bool\r", reset);
    }

RTC_DateTypeDef date;
RTC_TimeTypeDef time;

void writepin(int argc, const char **argv)
    {
    int d = -1;	//pinnr
    int e = -1;	//state
    if (argc == 3)
		{
		sscanf(argv[1], "%d", &d);
		sscanf(argv[2], "%d", &e);

		mcp_PinMode(&mcp_io, OUTPUT, d);
		mcp_WritePin(&mcp_io, d, e);
		term_printf(&btTerm, "\rcmd writepin:pinnr %d state %d\r", d, e);
		}
    }
void readpin(int argc, const char **argv)
    {
    int d = -1;	//pinnr
    int e = -1;	//pullup
    if (argc == 3)
		{
		sscanf(argv[1], "%d", &d);
		sscanf(argv[2], "%d", &e);
		if (e)
			{
			mcp_PinMode(&mcp_io, PULLUP, d);
			}

		if (!e)
			{
			mcp_PinMode(&mcp_io, INPUT, d);
			}
		term_printf(&btTerm, "\rcmd readpin:pinnr %d pullup %d\r", &d, &e);
		}
    }
void setallin(int argc, const char **argv)
    {
    mcp_set_all_input(&mcp_io);
    term_printf(&btTerm, "\rcmd mcp_set_all_input ok\r");
	}

void setword(int argc, const char **argv)
    {
    int e = -1;	//
    if (argc == 2)
	{
	sscanf(argv[1], "%d", &e);
	if (e<=0xFFFF)
	    {
	    mcp_WriteWord(&mcp_io, e);
	    term_printf(&btTerm, "\rcmd setword: 0x%x ok\r",e);
	    }

	}
    }

void setdate(int argc, const char **argv)
    {
    int d = -1;	//
    int m = -1;	//
    int y = -1;	//
    if (argc == 4)
		{
    	int wparam;
    	sscanf(argv[1], "%d", &d);
    	sscanf(argv[2], "%d", &m);
    	sscanf(argv[3], "%d", &y);
    	wparam = utils_truncate_number_int(&d, 0, 32);
    	wparam = utils_truncate_number_int(&m, 0, 12);
    	wparam = utils_truncate_number_int(&y, 20, 65);
    	if (wparam)
    		{
    		term_printf(&btTerm, "\rrange ist 32 12 65\r");
    		}
    	else
			{

			date.Month = m;
			date.Date = d;
			date.Year = y;
			HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
			char buffer[]="\rrtc sagt: nein\r";
			pl_rtc_timestring(buffer, DATEMONO);

			term_printf(&btTerm, buffer);
			//pl_lol_newlog(&filelog);


			}
		}
    else
		{
    	term_printf(&btTerm, "\r3 argumente DD MM YY\r");
		}

	}
void settime(int argc, const char **argv)
    {
    int h = -1;	//
    int m = -1;	//
    int s = -1;	//
    if (argc == 4)
	{
    	sscanf(argv[1], "%d", &h);
    	sscanf(argv[2], "%d", &m);
    	sscanf(argv[3], "%d", &s);
    	int wparam=0;
    	wparam = utils_truncate_number_int(&h, 0, 23);
    	wparam = utils_truncate_number_int(&m, 0, 59);
    	wparam = utils_truncate_number_int(&s, 0, 59);
    	if (wparam)
    	    term_printf(&btTerm, "\rrange ist 23 59 59\r");
    	else
	    {
    	term_printf(&btTerm, "\rsettime\r");
	    time.Hours = h;
	    time.Minutes = m;
	    time.Seconds = s;
	    HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
	    HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
	    char buffer[]="\rrtc sagt: nein\r";
	    pl_rtc_timestring(buffer, TIMEMONO);
	    term_printf(&btTerm, buffer);
	    //pl_lol_newlog(&filelog);

	    }
	}
	else
	    term_printf(&btTerm, "\r3 argumente DD MM YY\r");
    }

void nlogn(int argc, const char **argv)
    {
	if (argc == 2)
		{
		char* filename = calloc(32,1);
		//strcpy(filename, argv[1]);
		filename = strtok(argv[1], "\r");
		//filelog.sdinfo.Filename = memset()
		strncpy(filelog.sdinfo.Filename, filename,32);
		pl_lol_newlogname(&filelog);
		term_printf(&btTerm, "\rcmd nlogn ok\r");
		term_printf(&btTerm, filelog.sdinfo.Filename);
		free (filename);
		}

   	}
void newlog(int argc, const char **argv)
    {
	pl_lol_newlog(&filelog);
	term_printf(&btTerm, "\rcmd ok\r");
	//term_printf(&btTerm, filelog.sdinfo.Filename);
   	}

void showconf(int argc, const char **argv)
{
	int d = -1;	//
	int itr=0;
		sscanf(argv[1], "%d", &d);
		term_printf(&btTerm, "\rcmd ok\r");

		cmdfile_scan_cmd(&initcmd);

}
void writeconf(int argc, const char **argv)
{

}
void readconf(int argc, const char **argv)
{
//cmdfile_scan_cmd(&initcmd);
	cmdfile_lol_readln(&initcmd, initcmd.linebuffer, initcmd.cmdcounter);
}
void selterm(int argc, const char **argv)
{
	float f = -1;
	if (argc == 2)
	{
	sscanf(argv[1], "%f", &f);
	modflag_init(&prettylog.pp_modflag, HALTICK, f);
	term_printf(&btTerm, "\rcmd selterm:%3.1fHz ok\r", f);
	}
}

void reset(int argc, const char **argv)
{

	if (argc == 2)
	{
	float f = -1;
	float counter;
	float cntdownms = 300;
	sscanf(argv[1], "%f", &f);
	term_printf(&btTerm, "\rcmd reset ok:%5fs ok\r", f);

	HAL_NVIC_SystemReset();
	}
}
