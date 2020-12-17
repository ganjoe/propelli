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
#include "fatfs.h"

/* jeder befehl hat eine feste linie im eeprom file */


void init_sdfile_initcmd		()
	{
	initcmd.filename=strdup("initcmd.hhw");
	initcmd.header=strdup("batch file");
	initcmd.maxchars =32;	//jede zeile ist gleichlang
	initcmd.maxlines =0;	//im sinne von maxcommands
	initcmd.maxfilename =32;

	//initcmd.bytesWrote = sd_lol_writeline(initcmd.filename, "setdate_9_12_20\r", initcmd.maxchars, initcmd.maxlines++);
	//initcmd.bytesWrote += sd_lol_writeline(initcmd.filename, "settime_00_24_00\r", initcmd.maxchars, initcmd.maxlines++);
	sdfile_lol_parsecmd(&initcmd);
	}

void init_sdfile_happylog		(HHW_FILE_FORMAT* file)
	{
	modflag_init(&file->this_mf, HALTICK, 0.5);
	file->filename=strdup("15.11.20 19:16:25.hhw");
	file->header=strdup("log_date tlog_time volt_mcu battvolt temp_mcu coldwatr hotwater mcpinput loglines\r");
	file->maxchars =120;	//jede zeile ist gleichlang
	file->maxfilename =32;
	file->maxlines =0xFFF;	//jede datei hat gleich viele zeilen
	file->flag = true;
	sdfile_lol_newhappylog();
	}

void mftask_sdfile_happylog		(HHW_FILE_FORMAT* file)
	{

	}

void mftick_sdfile_happylog		(HHW_FILE_FORMAT* file)
	{

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
	term_printf(&btTerm, "[sdfile_lol_newlog] ok\r");
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

void sdfile_lol_parsecmd		(HHW_FILE_FORMAT* file)
	{
	int bytesRead=-1;
	file->act_line = 0;
	//anzahl commands in file unbekannt
	while(file->act_line < 0xF )
		{
		bytesRead= sd_lol_readline(file->filename, btTerm.string_rx, file->maxchars, file->act_line);
		if (bytesRead <= 0)
			break;
		if(strchr(btTerm.string_rx, '#'))
			{
			file->maxlines = (file-> act_line) -1;
			term_printf(&btTerm, "\r[sdfile_lol_parsecmd] New maxlines: %d", file->maxlines);
			break;
			}
		else
			{

			term_printf(&btTerm, "\r[sdfile_lol_parsecmd] %d bytes Read:%s", bytesRead,   btTerm.string_rx);

			btTerm.sep = strdup("_");
			term_lol_parse(&btTerm);
			btTerm.sep = strdup(" ");
			}
		file->act_line++;
		}

	}

void sdfile_lol_write_init		(HHW_FILE_FORMAT* file, char* buffer, int line)
	{
	if (line > file->maxlines)
		{
		file->maxlines++;
		term_printf(&btTerm, "\rwritecmd:extend with new (%d)\r", file->maxlines);
		file->bytesWrote = sd_lol_writeline(file->filename, buffer, file->maxchars, file->maxlines);
		sd_lol_writeline(file->filename, "#", file->maxchars, (file->maxlines) + 1);
		}
	if (line <= file->maxlines)
		{
		file->bytesWrote = sd_lol_writeline(file->filename, buffer, file->maxchars, line);
		term_printf(&btTerm, "\rwritecmd:replace with new (%d)\r", line);
		}
	if (file->bytesWrote>0)
		{
		term_printf(&btTerm, "\rwritecmd: %d bytes Writen\r", file->bytesWrote);
		}
	else
		{
		term_printf(&btTerm, "\rwritecmd: not ready. wait reset\r", file->bytesWrote);
		file->flag = false;
		}

	}

void sdfile_lol_write_backup	(HHW_FILE_FORMAT* file, const char* buffer, int line)
	{

	if (line <= file->maxlines)
		{
		/* TODO: methode für leeren buffer im stack  */
		char* emptybuff = calloc(file->maxchars -1 ,1);
		memset(emptybuff,(int)' ', file->maxchars -1);

		sd_lol_readline(file->filename, emptybuff, file->maxchars, line);
		/*	nur schreiben wenn der string neu ist, sd-karte schonen */
		if (strcmp(emptybuff, buffer))
			{
			file->bytesWrote = sd_lol_writeline(file->filename, buffer, file->maxchars, line);
			term_printf(&btTerm, "\r[write_backup] Replace line %d with new backupcmd", line);

			term_printf(&btTerm, "\r[write_backup] %d bytes Writen", file->bytesWrote);

			}
		else
			{
			term_printf(&btTerm, "\r[write_backup] Skip line %d", line);
			}

		free (emptybuff);
		}

	if (line > file->maxlines)
		{
		file->maxlines++;
		term_printf(&btTerm, "\rwritecmd:extend with new (%d)", file->maxlines);
		file->bytesWrote = sd_lol_writeline(file->filename, buffer, file->maxchars, file->maxlines);
		sd_lol_writeline(file->filename, "#", file->maxchars, (file->maxlines) + 1);
		term_printf(&btTerm, "\rwritecmd: %d bytes Writen\r", file->bytesWrote);

		}

	if (file->bytesWrote<0)
		{
		term_printf(&btTerm, "\rwrite_backupcmd	: not ready. wait reset", file->bytesWrote);
		file->flag = false;
		}


	}


HHW_FILE_FORMAT initcmd, happylog;
