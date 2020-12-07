/*
 * sdfile.c
 *
 *  Created on: Dec 7, 2020
 *      Author: danie
 */


#include "datatypes.h"
#include "sdfile.h"
#include "PrettyLog.h"


void init_sdfile_eeprom()
	{
	eeprom.filename=strdup("eeprom.hhw");
	eeprom.header=strdup("batch file");
	eeprom.maxchars =32;	//jede zeile ist gleichlang
	eeprom.maxlines =0xF;	//im sinne von maxcommands
	eeprom.maxfilename =8+1+3+1;
	}
void init_sdfile_happylog()
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
	happylog.maxchars =128;	//jede zeile ist gleichlang
	happylog.maxfilename =8+1+3+1;
	happylog.maxlines =0xFFFF;	//jede datei hat gleich viele zeilen
	}
void sdfile_lol_set_filename(HHW_FILE_FORMAT* file, char* filename)
{
	int stlen = strlen(filename);
	utils_truncate_number_int(&stlen, 1, file->maxfilename);
	strcpy(file->filename, file);
}

void sdfile_happylog_new()
{
	char* buffer=calloc(32,1);
	pl_rtc_timestring(buffer, DATETIMEFAT);
	sdfile_lol_set_filename(&happylog, buffer);
	free (buffer);
}








HHW_FILE_FORMAT eeprom, happylog;
