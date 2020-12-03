/*
 * termfile.c
 *
 *  Created on: 29.11.2020
 *      Author: danie
 */


#include "termfile.h"
#include "terminal.h"
#include "utils.h"

TD_TERMFILE initcmd;
/* ließt terminal commandos aus textdatei. im sinne einer init-batch*/
void 	init_cmdfile(TD_TERMFILE* cmd)
{
	cmd->maxarguments = 3;
	cmd->maxchars = 32;
	cmd->maxlines = 0xF;
	char *filename = strdup("InitCommBatch.eeprom");
	char *strcmd;
	cmd->linebuffer = calloc(cmd->maxchars,1);
	cmd->filename 	= calloc(strlen(filename),1);
	strcpy(cmd->filename, filename);

	FRESULT stat = 	cmdfile_lol_open_create(cmd);
	cmd->cmdcounter = 0;

	if (stat == FR_NO_FILE)
	{
	strcmd = strdup("setdate 29 11 20\r");	cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("settime 20 08 00\r");	cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("readpin 0 1\r");		cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("readpin 1 1\r");		cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("readpin 2 1\r");		cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("#");					cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter);
	strcmd = strdup("0");
	stat = FR_OK;
	}
	if (stat == FR_OK)
	{
		//TODO: unheil abwenden

	cmdfile_do_cmds(&initcmd);
	}
	term_printf(&btTerm, "\rinit_cmdfile:\r%d done\r",cmd->cmdcounter);

}

void 	cmdfile_do_cmds			(TD_TERMFILE* initcmd)
	{
	if (initcmd->flag_initdone == true)
		{
		while (initcmd->cmdcounter<initcmd->maxlines)
			{
			cmdfile_lol_readln(initcmd, initcmd->linebuffer, initcmd->cmdcounter);
			if(strchr(initcmd->linebuffer, '#'))
				{
				break;
				}
			initcmd->cmdcounter++;
			memcpy(btTerm.string_rx, initcmd->linebuffer, strlen(initcmd->linebuffer));
			term_lol_parse(&btTerm);
			}
		}
	}

FRESULT cmdfile_lol_open_create	(TD_TERMFILE* initcmd)
{
	FRESULT stat;
	stat = f_open(&initcmd->InitFIle, initcmd->filename, FA_READ | FA_WRITE  );

	switch (stat)
	{
		/* für einen differenzierten status muss FAT und hardware fehlerfrei laufen
		dateisystem wird in fatfs.c gemountet */
		case FR_NO_FILE:
		{
		f_open(&initcmd->InitFIle, initcmd->filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
		f_close(&initcmd->InitFIle);
		term_printf(&btTerm, "cmdfile_lol_open_create:\rneue initdatei erstellt\r");

		}break;
		case FR_OK:
		{
			initcmd->flag_initdone = true;
			stat  =	f_close(&initcmd->InitFIle);
			term_printf(&btTerm, "cmdfile_lol_open_create:\r initdatei vorhanden\r");
		}break;

		default:
		{
			term_printf(&btTerm, "cmdfile_lol_open_create:\r Status FATFS != (FR_OK | FR_NO_FILE)\r");
			initcmd->flag_initdone = false;
		}break;

	}
	return stat;
}

FRESULT cmdfile_lol_readln		(TD_TERMFILE* initcmd, char* buffer, int linenr)
{
	utils_truncate_number_int(&linenr, 0, initcmd->maxlines);
	FRESULT stat;
	UINT bytesread;
	stat = f_open(&SDFile, initcmd->filename, FA_READ);
	stat = f_lseek(&SDFile, linenr * initcmd->maxchars);
	stat = f_read(&SDFile, buffer, initcmd->maxchars, &bytesread);
	stat = f_close(&SDFile);
	return stat;
}

FRESULT cmdfile_lol_writeln		(TD_TERMFILE* initcmd, char* buffer, int linenr)
{
	FRESULT stat;
	UINT byteswrote;
	int bufflen;

	bufflen = strlen(buffer);										//zu lange strings werden abgeschnitten gesendet
	utils_truncate_number_int(&bufflen, 0, initcmd->maxchars);
	utils_truncate_number_int(&linenr, 	0, initcmd->maxlines);		//in der letzten zeile sollte nichts wichtiges stehen

	stat = f_open	(&SDFile, initcmd->filename, FA_WRITE | FA_READ);
	stat = f_lseek	(&SDFile, linenr * initcmd->maxchars);			// immer ab anfang der zeile schreiben.
	stat = f_write	(&SDFile, buffer, bufflen, &byteswrote);
	stat = f_close	(&SDFile);

	return stat;
}

