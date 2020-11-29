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
    extern TD_fatlog filelog, fileinit;
void Command_init()
    {
        terminal_register_command_callback("setword", "\nmcp outlatch\n",
	    "bool\n", setword);
        terminal_register_command_callback("writepin", "\nGPIOA,B Output no Pullup\n",
	    "bool\n", writepin);
        terminal_register_command_callback("readpin", "\nGPIOA,B Input no Pullup\n",
	    "bool\n", readpin);
        terminal_register_command_callback("setallin", "\nGPIOA,B Input no Pullup\n",
	    "bool\n", setallin);
        terminal_register_command_callback("setdate", "\nDD MM YY\n",
	    "bool\n", setdate);
        terminal_register_command_callback("settime", "\nhh mm ss\n",
	    "bool\n", settime);
        terminal_register_command_callback("nlogn", "\nfilename[32]\n",
	    "bool\n", nlogn);
        terminal_register_command_callback("newlog", "\nrtc filename\n",
	    "bool\n", newlog);
    }


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
	commands_printf("\ncmd ok\n");
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
	    commands_printf("\nread: %x\n", &mcp_io.inputstate);
	    }

	if (!e)
	    {
	    mcp_PinMode(&mcp_io, INPUT, d);
	    commands_printf("\ncmd ok\n");
	    }
	}
    }
void setallin(int argc, const char **argv)
    {
    int e = -1;	//pullup
    if (argc == 2)
	{
	sscanf(argv[1], "%d", &e);
	if (e)
	    {
	    mcp_set_all_input(&mcp_io);
	    commands_printf("\ncmd ok\n");
	    }

	if (!e)
	    {
	    mcp_set_all_input(&mcp_io);
	    commands_printf("\ncmd ok\n");
	    }
	}
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
	    commands_printf("\ncmd ok\n");
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
    		commands_printf("\nrange ist 32 12 65\n");
    		}
    	else
			{
			RTC_DateTypeDef date;
			date.Month = m;
			date.Date = d;
			date.Year = y;
			HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
			char buffer[]="\nrtc sagt: nein\n";
			pl_rtc_timestring(buffer, DATEMONO);
			commands_printf(buffer);
			pl_lol_newlog(&filelog);
			}
		}
    else
		{
    	commands_printf("\n3 argumente DD MM YY\n");
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
    	    commands_printf("\nrange ist 23 59 59\n");
    	else
	    {
	    RTC_TimeTypeDef time;
	    time.Hours = h;
	    time.Minutes = m;
	    time.Seconds = s;
	    HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
	    char buffer[]="\nrtc sagt: nein\n";
	    pl_rtc_timestring(buffer, TIMEMONO);
	    commands_printf(buffer);
	    pl_lol_newlog(&filelog);
	    }
	}
	else
	    commands_printf("\n3 argumente DD MM YY\n");
    }

void nlogn(int argc, const char **argv)
    {

        if (argc == 2)
    	{
            strcpy(filelog.sdinfo.Filename, argv[1]);
            pl_lol_newlogname(&filelog);
            commands_printf("\ncmd ok\n");
            commands_printf(filelog.sdinfo.Filename);
    	    }

   	}
void newlog(int argc, const char **argv)
    {

        if (argc == 1)
    	{
            pl_lol_newlog(&filelog);
            commands_printf("\ncmd ok\n");
            commands_printf(filelog.sdinfo.Filename);
    	    }

   	}
