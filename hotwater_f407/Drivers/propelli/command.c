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
    extern TD_fatlog filelog, fileinit;
void Command_init()
    {
        term_lol_setCallback("setword", "\nmcp outlatch\n",
	    "bool\n", setword);
        term_lol_setCallback("writepin", "\nGPIOA,B Output no Pullup\n",
	    "bool\n", writepin);
        term_lol_setCallback("readpin", "\nGPIOA,B Input no Pullup\n",
	    "bool\n", readpin);
        term_lol_setCallback("setallin", "\nGPIOA,B Input no Pullup\n",
	    "bool\n", setallin);
        term_lol_setCallback("setdate", "\nDD MM YY\n",
	    "bool\n", setdate);
        term_lol_setCallback("settime", "\nhh mm ss\n",
	    "bool\n", settime);
        term_lol_setCallback("nlogn", "\nfilename[32]\n",
	    "bool\n", nlogn);
        term_lol_setCallback("newlog", "\nrtc filename\n",
	    "bool\n", newlog);
        term_lol_setCallback("showconf", "\nrtc filename\n",
	    "bool\n", showconf);
        term_lol_setCallback("writeconf", "\nrtc filename\n",
	    "bool\n", writeconf);
        term_lol_setCallback("readconf", "\nrtc filename\n",
	    "bool\n", readconf);
        term_lol_setCallback("selterm", "\nlog upd speed\n",
	    "bool\n", selterm);
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
		term_printf(&cmdkeen, "\rcmd writepin:pinnr %d state %d\r", &d, &e);
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
		term_printf(&cmdkeen, "\rcmd readpin:pinnr %d pullup %d\r", &d, &e);
		}
    }
void setallin(int argc, const char **argv)
    {
    mcp_set_all_input(&mcp_io);
    term_printf(&cmdkeen, "\rcmd mcp_set_all_input ok\r");
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
	    term_printf(&cmdkeen, "\rcmd setword: 0x%x ok\r",e);
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
    		term_printf(&cmdkeen, "\nrange ist 32 12 65\n");
    		}
    	else
			{

			date.Month = m;
			date.Date = d;
			date.Year = y;
			HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
			char buffer[]="\nrtc sagt: nein\n";
			pl_rtc_timestring(buffer, DATEMONO);
			term_printf(&cmdkeen, buffer);
			pl_lol_newlog(&filelog);
			}
		}
    else
		{
    	term_printf(&cmdkeen, "\n3 argumente DD MM YY\n");
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
    	    term_printf(&cmdkeen, "\nrange ist 23 59 59\n");
    	else
	    {
    	term_printf(&cmdkeen, "\rsettime\r");
	    time.Hours = h;
	    time.Minutes = m;
	    time.Seconds = s;
	    HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
	    HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
	    char buffer[]="\nrtc sagt: nein\n";
	    pl_rtc_timestring(buffer, TIMEMONO);
	    term_printf(&cmdkeen, buffer);
	    pl_lol_newlog(&filelog);
	    }
	}
	else
	    term_printf(&cmdkeen, "\n3 argumente DD MM YY\n");
    }

void nlogn(int argc, const char **argv)
    {
	if (argc == 3)
		{
		pl_lol_newlogname(&filelog);
		term_printf(&cmdkeen, "\rcmd nlogn ok\r");
		term_printf(&cmdkeen, filelog.sdinfo.Filename);
		}

   	}
void newlog(int argc, const char **argv)
    {

        if (argc == 1)
    	{
            pl_lol_newlog(&filelog);
            term_printf(&cmdkeen, "\ncmd ok\n");
            term_printf(&cmdkeen, filelog.sdinfo.Filename);
    	    }

   	}

void showconf(int argc, const char **argv)
{
	int d = -1;	//pinnr
	int itr=0;
		sscanf(argv[1], "%d", &d);
		term_printf("\rcmd readconf\r:batchcounter: %d\r", &initcmd.cmdcounter);

		while(itr < initcmd.cmdcounter)
			{
			cmdfile_lol_readln(&initcmd, initcmd.linebuffer, itr);
			term_printf("[%d] %s\r",itr, initcmd.linebuffer);
			itr++;
			}

}
void writeconf(int argc, const char **argv)
{

}
void readconf(int argc, const char **argv)
{

}
void selterm(int argc, const char **argv)
{
	float f = -1;
	if (argc == 2)
	{
	sscanf(argv[1], "%f", &f);
	modflag_init(&prettylog.pp_modflag, HALTICK, f);
	term_printf(&cmdkeen, "\rcmd selterm ok\r");
	}
}
