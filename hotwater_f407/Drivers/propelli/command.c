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
#include "sdfile.h"
#include "delay.h"

void Command_init()
    {
        term_lol_setCallback("setword", "help", 	"arghelp",	setword);
        term_lol_setCallback("writepin", "help",    "arghelp", 	writepin);
        term_lol_setCallback("readpin", "help",	    "arghelp", 	readpin);
        term_lol_setCallback("setallin", "help",    "arghelp", 	setallin);

        term_lol_setCallback("setdate", "help",	    "barghelp", setdate);
        term_lol_setCallback("settime", "help",	    "arghelp", 	settime);

        term_lol_setCallback("sdwrite", "help",		"arghelp", 	sdwrite);
        term_lol_setCallback("sdread", 	"help",	    "arghelp", 	sdread);
        term_lol_setCallback("readinit","help",	    "arghelp", 	readinit);
        term_lol_setCallback("writeinit","help",	"arghelp", 	writeinit);
        term_lol_setCallback("nlogn",	"help",		"arghelp", 	nlogn);
        term_lol_setCallback("nlog",	"help",		"arghelp", 	nlog);

        term_lol_setCallback("selterm",	"help",		"arghelp", 	selterm);
        term_lol_setCallback("reset", 	"help",		"arghelp", 	reset);
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
			sdfile_lol_newhappylog(&happylog);
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
	    char* buffptr = &buffer;
	    pl_rtc_timestring(buffptr, TIMEMONO);
	    term_printf(&btTerm, buffptr);
	    strcpy(happylog.filename,	buffer );
	   // sdfile_lol_newhappylog(&happylog);

	    }
	}
	else
	    term_printf(&btTerm, "\r3 argumente DD MM YY\r");
    }

void sdwrite(int argc, const char **argv)
    {
	if (argc == 4)
		{
		int lines, chars;

		sscanf(argv[3], "%d", &chars);
		sscanf(argv[4], "%d", &lines);

		int bytesWrote = sd_lol_writeline(argv[1], argv[2], chars, lines);
		if (bytesWrote>0)
			{
			term_printf(&btTerm, "\rcmd nlogn: %d bytes Writen\r", bytesWrote);
			}
		else
			{
			term_printf(&btTerm, "\rcmd nlogn: not ready\r", bytesWrote);
			}
		}
   	}
void sdread(int argc, const char **argv)
	{
	if (argc == 4)
		{

		int bytesRead;
		int lines, chars;

		sscanf(argv[2], "%d", &chars);
		sscanf(argv[3], "%d", &lines);
		char* linebuffer = calloc(chars,1);

		bytesRead = sd_lol_readline(argv[1], linebuffer, chars, lines);
		if (bytesRead>0)
			{
			term_printf(&btTerm, "\rcmd sdread: %d bytes Read:\r%s", bytesRead, linebuffer);
			//term_printf(&btTerm, linebuffer);
			}
		else
			{
			term_printf(&btTerm, "\rcmd sdread: empty:\r%s", bytesRead, linebuffer);
			}
		free (linebuffer);
		}

	}

void readinit(int argc, const char **argv)
	{
	int d = -1;
	if (argc == 2)
	{
	sscanf(argv[1], "%d", &d);
	int bytesRead;
	char* linebuffer = calloc(initcmd.maxchars,1);
	bytesRead = sd_lol_readline(initcmd.filename, linebuffer, initcmd.maxchars, d);
	term_printf(&btTerm, "\rcmd sdread: %d bytes Read:\r%s", bytesRead, linebuffer);
	free (linebuffer);
	}
}
void writeinit(int argc, const char **argv)
{
if (argc == 3)
	{
	int line;
	sscanf(argv[2], "%d", &line);

	initcmd.bytesWrote = sd_lol_writeline(initcmd.filename, argv[1], initcmd.maxchars, line);
	if (initcmd.bytesWrote>0)
		{
		term_printf(&btTerm, "\rcmd writeinit: %d bytes Writen\r", initcmd.bytesWrote);
		}
	else
		{
		term_printf(&btTerm, "\rcmd writeinit: not ready\r", initcmd.bytesWrote);
		}

	}


}

void nlogn(int argc, const char **argv)
{
if (argc == 2)
	{
	strcpy(happylog.filename,	argv[1] );
	happylog.act_line = 0;
	happylog.bytesWrote = 0;

	term_printf(&btTerm, "\rcmd nlogn ok\r");
	}
}
void nlog(int argc, const char **argv)
{
if (argc == 2)
	{
	pl_rtc_timestring(happylog.filename, DATETIMEFAT);
	//strcpy(happylog.filename,	argv[1] );
	happylog.act_line = 0;
	happylog.bytesWrote = 0;

	term_printf(&btTerm, "\rcmd nlogn ok\r");

	}
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
