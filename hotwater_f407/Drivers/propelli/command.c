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
#include "HappyHotWater.h"
#include "backup_command.h"

void Command_init()
    {
        term_lol_setCallback("setword", "mcp regs A und B mit wort setzen",	"dword",	setword);
        term_lol_setCallback("writepin", "pin, state",		"bits ab port a setzen",	writepin);
        term_lol_setCallback("readpin", "register lesen",	    "pinnr, pullupstate", 	readpin);
        term_lol_setCallback("setallin", "help",    "arghelp", 	setallin);

        term_lol_setCallback("setdate", "dd mm yy",	    "rtc setzen", setdate);
        term_lol_setCallback("settime", "hh mm ss",	    "rtc setzen", settime);

        term_lol_setCallback("sdwrite", "filename string chars lines","schreibt zu beginn einer zeile",	sdwrite);
        term_lol_setCallback("sdread", 	"help",	    "arghelp", 	sdread);

        term_lol_setCallback("readinit","help",	    "arghelp", 	readinit);
        term_lol_setCallback("writeinit","help",	"arghelp", 	writeinit);
        term_lol_setCallback("clearinit","write blanks to initfile","lines to write", 	clearinit);

        term_lol_setCallback("readrom","help",   "arghelp", readrom);
        term_lol_setCallback("writerom","help",	"arghelp", 	writerom);
        term_lol_setCallback("clearrom","write blanks to eeprom","lines to write", 	clearrom);

        term_lol_setCallback("nlogn",	"help",		"arghelp", 	nlogn);
        term_lol_setCallback("nlog",	"help",		"arghelp", 	nlog);

        term_lol_setCallback("vrange","USV Spannungslevel. Bsp: 10.5 12.8 13.6 ","lowlimit(SoC:0%) highlimit(SoC:100%) invlimit(only charging)",vrange);
        term_lol_setCallback("mode","Heizung Betriebsmodus","0: eco 1: full", 	mode);
        term_lol_setCallback("trange","Heizung Temperaturschwellen","lowlimit<float>_highlimit<float>", 	trange);

        term_lol_setCallback("selterm",	"help",		"arghelp", 	selterm);
        term_lol_setCallback("reset", 	"help",		"arghelp", 	reset);
        term_lol_setCallback("selhhw", 	"help",		"arghelp", 	selhhw);
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
		term_printf(&btTerm, "\rcmd readpin:pinnr %d pullup %d\r", d, e);
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
			date.WeekDay = RTC_WEEKDAY_SUNDAY;
			HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
			HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);
			term_printf(&btTerm, "[cmd setdate] ok\r");
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
    	    term_printf(&btTerm, "[cmd settime][Fehler] Range ist 23 59 59\r");
    	else
	    {
    	term_printf(&btTerm, "[cmd settime] ok\r");
    	time.Hours = h;
    	 time.Minutes = m;
    	 time.Seconds = s;
    	 time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    	 time.StoreOperation = RTC_STOREOPERATION_RESET;

	    HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN);
	    HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN);

	    sdfile_lol_newhappylog(&happylog);

	    }
	}
	else
	    term_printf(&btTerm, "\r3 argumente DD MM YY\r");
    }

void sdwrite(int argc, const char **argv)
    {
	if (argc == 4)
		{
		int lines=0;
		int chars=0;

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
		char linebuffer[initcmd.maxchars];
		bytesRead= sd_lol_readline(initcmd.filename, linebuffer, initcmd.maxchars, d);
		term_printf(&btTerm, "\rcmd readinit: (%2d/%2d)%s\r", d, initcmd.maxlines, linebuffer);
		//free (linebuffer);
		}
	}
void writeinit(int argc, const char **argv)
{
if (argc == 3)
	{
	int line;
	sscanf(argv[2], "%d", &line);

	if (line > initcmd.maxlines)
		{
		initcmd.maxlines++;
		term_printf(&btTerm, "\rcmd writeinit:extend list with new cmd (%d)\r", initcmd.maxlines);
		initcmd.bytesWrote = sd_lol_writeline(initcmd.filename, argv[1], initcmd.maxchars, initcmd.maxlines);
		sd_lol_writeline(initcmd.filename, "#", initcmd.maxchars, (initcmd.maxlines) + 1);

		}
	if (line <= initcmd.maxlines)
		{
		initcmd.bytesWrote = sd_lol_writeline(initcmd.filename, argv[1], initcmd.maxchars, line);
		term_printf(&btTerm, "\rcmd writeinit:replace cmd in list(%d)\r", line);
		}


	if (initcmd.bytesWrote>0)
		{
		term_printf(&btTerm, "\rcmd writeinit: %d bytes Writen\r", initcmd.bytesWrote);
		}
	else
		{
		term_printf(&btTerm, "\rcmd writeinit: not ready. disable initcmd.flag\r", initcmd.bytesWrote);
		initcmd.flag = false;
		}
	}
}
void clearinit(int argc, const char **argv)
{
	int d = -1;
	if (argc == 2)
		{
		sscanf(argv[1], "%d", &d);
		int bytesWrote;
		char* emptybuff = calloc(initcmd.maxchars -1 ,1);
		memset(emptybuff,(int)'#', initcmd.maxchars -1);
		for (int var = 0; var < d; ++var)
			{
			bytesWrote += sd_lol_writeline(initcmd.filename, emptybuff, initcmd.maxchars, var);
			}
		free(emptybuff);
		term_printf(&btTerm, "\rcmd clearinit: %d lines cleard with %d blanks ok\r",d, bytesWrote);
		}
	else
		{
		term_printf(&btTerm, "\rcmd clearinit: params");
		}
}

void readrom(int argc, const char **argv)
	{
	int d = -1;
	if (argc == 2)
		{
		sscanf(argv[1], "%d", &d);
		int bytesRead;
		char linebuffer[eeprom.maxchars];
		bytesRead= sd_lol_readline(eeprom.filename, linebuffer, eeprom.maxchars, d);
		term_printf(&btTerm, "\rcmd readrom: (%2d/%2d)%s", d, eeprom.maxlines, linebuffer);
		}
	}
void writerom(int argc, const char **argv)
	{
	if (argc == 3)
		{
		int line;
		sscanf(argv[2], "%d", &line);
		sdfile_lol_write_backup(&eeprom, argv[1], line);
		}
	}
void clearrom(int argc, const char **argv)
{
	int d = -1;
	if (argc == 2)
		{
		sscanf(argv[1], "%d", &d);
		int bytesWrote;
		char* emptybuff = calloc(eeprom.maxchars -1 ,1);
		memset(emptybuff,(int)'#', eeprom.maxchars -1);
		for (int var = 0; var < d; ++var)
			{
			bytesWrote += sd_lol_writeline(eeprom.filename, emptybuff, eeprom.maxchars, var);
			}
		free(emptybuff);
		term_printf(&btTerm, "\rcmd clearrom: %d cmds cleard with %d blanks ok\r",d, bytesWrote);
		}
	else
		{
		term_printf(&btTerm, "\rcmd clearrom: params");
		}
}

void vrange(int argc, const char **argv)
{
	float min = -1;	float max = -1;	float inv = -1;

	if (argc == 4)
		{
		sscanf(argv[1], "%f", &min);
		sscanf(argv[2], "%f", &max);
		sscanf(argv[3], "%f", &inv);

		Hhw.VoltLevel_highbatt = max;
		Hhw.VoltLevel_lowbatt = min;
		Hhw.VoltLevel_power_inv = inv;

		term_printf(&btTerm, "\r[cmd vrange]: ok\r");
		backup_voltrange(&eeprom);
		}
	else
		{
		term_printf(&btTerm, "\rcmd vrange: 3 params needed");
		}
}
void mode(int argc, const char **argv)
{
	int mode = -1;

	if (argc == 2)
		{
		sscanf(argv[1], "%d", &mode);
		if (mode==1)
			{
			hhw_set_state(&Hhw, TANK_MODE_FULL, true);
			hhw_set_state(&Hhw, TANK_MODE_ECO, false);
			term_printf(&btTerm, "\r[cmd mode] new: TANK_MODE_FULL");
			}
		if (mode==0)
			{
			hhw_set_state(&Hhw, TANK_MODE_FULL, false);
			hhw_set_state(&Hhw, TANK_MODE_ECO, true);
			term_printf(&btTerm, "\r[cmd mode] new: TANK_MODE_ECO");
			}
		backup_all();
		}
	else
		{
		term_printf(&btTerm, "\rcmd mode: 1 params needed");
		}
}
void trange(int argc, const char **argv)
{
	float min = -1;	float max = -1;
	if (argc == 3)
		{
		sscanf(argv[1], "%f", &min);
		sscanf(argv[2], "%f", &max);

		Hhw.TempLevel_high = max;
		Hhw.TempLevel_low = min;

		term_printf(&btTerm, "\rcmd trange: ok\r");
		backup_all();

		}
	else
		{
		term_printf(&btTerm, "\rcmd trange: 2 params needed");
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
void selhhw(int argc, const char **argv)
{
	float f = -1;
	if (argc == 2)
		{
		sscanf(argv[1], "%f", &f);
		modflag_init(&Hhw.mf_modflag, HALTICK, f);
		term_printf(&btTerm, "\rcmd selhhw:%3.1fHz ok\r", f);
		}
}
void reset(int argc, const char **argv)
{

	if (argc == 2)
	{
	backup_all();
	float f = -1;
	float counter;
	float cntdownms = 300;
	sscanf(argv[1], "%f", &f);
	term_printf(&btTerm, "\rcmd reset ok:%5fs ok\r", f);

	HAL_NVIC_SystemReset();
	}
}
