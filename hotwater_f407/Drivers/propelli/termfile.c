/*
 * termfile.c
 *
 *  Created on: 29.11.2020
 *      Author: danie
 */


#include "termfile.h"
#include "fatfs.h"

TD_TERMFILE initcmd;

void init_cmdfile(TD_TERMFILE* initcmd)
{
	initcmd->maxarguments = 3;
	initcmd->maxchars = 32;
	initcmd->maxlines = 0xF;
	strcpy(initcmd->cr, "+");
	strcpy(initcmd->ef, "#");
	strcpy(initcmd->sep, "_");
	const char* filename = "InitCommBatch.eeprom";

	/* keine userparameter mehr ab hier */
	initcmd->linebuffer = malloc(initcmd->maxchars);
	initcmd->filename = malloc(sizeof(filename));
	strcpy(initcmd->filename, filename);
}

void cmdfile_open_create(TD_TERMFILE* initcmd)
{
	/* dateisystem wird in fatfs.c gemountet */
	/* nur neu erstellen wenn nicht vorhanden */
	FRESULT stat;
	stat = f_open(&SDFile, initcmd->filename, FA_WRITE);

	switch (stat) {
		case FR_NO_FILE:
		{
		stat =	f_open(&SDFile, initcmd->filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
		}break;
		case FR_OK:
		{
			initcmd->flag_initdone = true;

		}break;
		case FR_NO_FILESYSTEM:
		{

		}break;
		case FR_INVALID_NAME:
		{

		}break;

	}
}
