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
	initcmd.filename=strdup("eeprom.hhw");
	initcmd.header=strdup("batch file");
	initcmd.maxchars =32;	//jede zeile ist gleichlang
	initcmd.maxlines =0xF;	//im sinne von maxcommands
	initcmd.maxfilename =32;
	}
void init_sdfile_happylog		(HHW_FILE_FORMAT* file)
	{
	happylog.filename=strdup("nodate.hhw");
	happylog.header=strdup("log_date\t"
						"log_time\t"
						"volt_mcu\t"
						"battvolt\t"
						"temp_mcu\t"
						"coldwatr\t"
						"hotwater\t"
						"mcpinput\t"
						"loglines\t"
						"\r");
	happylog.maxchars =80;	//jede zeile ist gleichlang
	happylog.maxfilename =32;
	happylog.maxlines =0xFFFF;	//jede datei hat gleich viele zeilen
	sdfile_lol_newhappylog(&happylog);
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
	happylog->act_line=1;
	free (buffer);
}
void sdfile_add_logline			(HHW_FILE_FORMAT* happylog, char* buffer)
	{
		happylog->bytesWrote +=	sd_lol_writeline(happylog->filename, buffer, happylog->maxchars, happylog->act_line);
		if (happylog->bytesWrote < happylog->maxlines)
			{
			happylog->act_line++;
			}
		else
			{
			sdfile_lol_newhappylog(&happylog);
			}
   	}

HHW_FILE_FORMAT initcmd, happylog, eeprom;
