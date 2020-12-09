/*
 * sdfile.c
 *
 *  Created on: Dec 7, 2020
 *      Author: daniel
 */

#include "datatypes.h"
#include "sdfile.h"
#include "PrettyLog.h"
#include "terminal.h"

void init_sdfile_initcmd		(HHW_FILE_FORMAT* file)
	{
	initcmd.filename=strdup("initcmd.hhw");
	initcmd.header=strdup("batch file");
	initcmd.maxchars =32;	//jede zeile ist gleichlang
	initcmd.maxlines =0;	//im sinne von maxcommands
	initcmd.maxfilename =32;

	//initcmd.bytesWrote = sd_lol_writeline(initcmd.filename, "setdate_9_12_20\r", initcmd.maxchars, initcmd.maxlines++);
	//initcmd.bytesWrote += sd_lol_writeline(initcmd.filename, "settime_00_24_00\r", initcmd.maxchars, initcmd.maxlines++);

	sdfile_parsecmds(&initcmd);

	}

void init_sdfile_happylog		(HHW_FILE_FORMAT* file)
	{
	file->filename=strdup("15.11.20 19:16:25.hhw");
	file->header=strdup("log_date tlog_time volt_mcu battvolt temp_mcu coldwatr hotwater mcpinput loglines\r");
	file->maxchars =120;	//jede zeile ist gleichlang
	file->maxfilename =32;
	file->maxlines =0xFFF;	//jede datei hat gleich viele zeilen
	file->flag = true;
	sdfile_lol_newhappylog();
	}

void sdfile_lol_set_filename	(HHW_FILE_FORMAT* file, 	char* filename)
{
	int stlen = strlen(filename);
	utils_truncate_number_int(&stlen, 1, file->maxfilename);
	strcpy(file->filename, filename);
}

void sdfile_lol_newhappylog		()
{
	char buffer[128] = {0};
	pl_rtc_timestring(buffer, DATETIMEFAT);
	sdfile_lol_set_filename(&happylog, buffer);
	happylog.bytesWrote = sd_lol_writeline(happylog.filename, happylog.header, happylog.maxchars,0);
	if (happylog.bytesWrote <0)
		happylog.flag = false;
	happylog.act_line=1;
}

void sdfile_add_logline			(HHW_FILE_FORMAT* happylog, char* buffer)
	{
	if(happylog->flag)
		{
		int byteswrote = sd_lol_writeline(happylog->filename, buffer, happylog->maxchars, happylog->act_line);
		if ((byteswrote > 0))
			{
			happylog->bytesWrote +=	byteswrote;
			if (happylog->act_line < happylog->maxlines)
				{
				happylog->act_line++;
				}
			else
				{
				sdfile_lol_newhappylog();
				}
			}
		else if ((byteswrote < 0))
			{
			happylog->bytesWrote =666;
			happylog->flag = 0;
			}
		}
	}

void sdfile_parsecmds			(HHW_FILE_FORMAT* file)
	{
	int bytesRead;
	file->act_line = 0;
	//anzahl commands in file unbekannt
	while(file->act_line < 0xF)
		{
		bytesRead= sd_lol_readline(file->filename, btTerm.string_rx, file->maxchars, file->act_line);
		if(strchr(btTerm.string_rx, '#'))
			{
			file->maxlines = (file-> act_line) -1;
			term_printf(&btTerm, "\rcmd sdfile_parsecmds:eof, new maxlines: %d\r", file->maxlines);
			break;
			}
		else
			{
			term_printf(&btTerm, "\rcmd sdfile_parsecmds: %d bytes Read:\r%s\r", bytesRead,   btTerm.string_rx);
			btTerm.sep = strdup("_");
			term_lol_parse(&btTerm);
			btTerm.sep = strdup(" ");
			}
		file->act_line++;
		}

	}

HHW_FILE_FORMAT initcmd, happylog, eeprom;
