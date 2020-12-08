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
	initcmd.maxlines =0xF;	//im sinne von maxcommands
	initcmd.maxfilename =32;

	}
void init_sdfile_happylog		(HHW_FILE_FORMAT* file)
	{
	file->filename=strdup("15.11.20 19:16:25.hhw");
	file->header=strdup("log_date tlog_time volt_mcu battvolt temp_mcu coldwatr hotwater mcpinput loglines\r");
	file->maxchars =strlen(file->header);	//jede zeile ist gleichlang
	file->maxfilename =32;
	file->maxlines =0xFFFF;	//jede datei hat gleich viele zeilen
	file->flag = true;
	sdfile_lol_newhappylog(file);
	}
void sdfile_lol_set_filename	(HHW_FILE_FORMAT* file, 	char* filename)
{
	int stlen = strlen(filename);
	utils_truncate_number_int(&stlen, 1, file->maxfilename);
	strcpy(file->filename, filename);
}
void sdfile_lol_newhappylog		(HHW_FILE_FORMAT* happylog)
{
	char* buffer=calloc(happylog->maxfilename,1);
	pl_rtc_timestring(buffer, DATETIMEFAT);
	sdfile_lol_set_filename(happylog, buffer);
	happylog->bytesWrote = sd_lol_writeline(happylog->filename, happylog->header,strlen(happylog->header),0);
	if (happylog->bytesWrote <0)
		happylog->flag = false;
	happylog->act_line=1;
	free (buffer);
}
void sdfile_add_logline			(HHW_FILE_FORMAT* happylog, char* buffer)
	{
	if(happylog->flag)
		{
		int byteswrote = sd_lol_writeline(happylog->filename, buffer, happylog->maxchars, happylog->act_line);
		if ((byteswrote > 0))
			{
			happylog->bytesWrote +=	byteswrote;
			if (happylog->bytesWrote < happylog->maxlines)
				{
				happylog->act_line++;
				}
			else
				{
				sdfile_lol_newhappylog(&happylog);
				}
			}
		else if ((byteswrote < 0))
			{
			happylog->bytesWrote =666;
			happylog->flag = 0;
			}

		}
	}


HHW_FILE_FORMAT initcmd, happylog, eeprom;
