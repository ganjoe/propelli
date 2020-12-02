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
void init_cmdfile(TD_TERMFILE* cmd)
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

	if (stat == FR_NO_FILE)
	{
	strcmd = strdup("setdate 29 11 20\r");	cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("settime 20 08 00\r");	cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("readpin 0 1\r");		cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("readpin 1 1\r");		cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("readpin 2 1\r");		cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("0");
	stat = FR_OK;
	}
	if (stat == FR_OK)
	{
		//TODO: unheil abwenden
		cmd->cmdcounter=5;
	cmdfile_do_cmds(&initcmd);
	}

}


FRESULT cmdfile_lol_open_create(TD_TERMFILE* initcmd)
{
	FRESULT stat;
	stat = f_open(&initcmd->SDFile, initcmd->filename, FA_READ | FA_WRITE  );

	switch (stat)
	{
		/* für einen differenzierten status muss FAT und hardware fehlerfrei laufen
		dateisystem wird in fatfs.c gemountet */
		case FR_NO_FILE:
		{
		f_open(&initcmd->SDFile, initcmd->filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
		f_close(&initcmd->SDFile);
		term_printf(&btTerm, "cmdfile_lol_open_create:\rneue initdatei erstellt\r");

		}break;
		case FR_OK:
		{
			initcmd->flag_initdone = true;
			stat  =	f_close(&initcmd->SDFile);
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
/*
https://c-for-dummies.com/blog/?p=1769
The strsep() function is defined in the string.h header file. Here is the man page format:
char * strsep(char **stringp, const char *delim);

The first argument, **stringp, is one of those annoying pointer-pointer things;
t’s the single string to search or an array of strings to search.
The second argument, *delim, is a string of one or more separator characters,
similar to the strtok() function.

Like strtok(), the strsep() function returns a pointer to the first string yanked out of **strngp.
Unlike strtok(), however, the function is called with the same arguments over and over until it returns a NULL pointer.
 */
void cmdfile_do_cmds	(TD_TERMFILE* initcmd)
	{
	if (initcmd->flag_initdone == true)
		{
		for (int var = 0; var < initcmd->cmdcounter; ++var)
			{
			cmdfile_lol_readln(initcmd, initcmd->linebuffer, var);
			//termfile_lol_parse(initcmd->linebuffer, initcmd->maxarguments);
			memcpy(btTerm.string_rx, initcmd->linebuffer, strlen(initcmd->linebuffer));
			term_lol_parse(&btTerm);
			}
		}
	}
static int callback_write = 0;
void termfile_lol_parse(/*cmd parser typedef*/char* buffer, int maxArguments )
    {
    int argc = 0;
    char *argv[maxArguments];
    //cmd ist der erste stringabschnitt von links
    char *p2 = strtok(buffer, " ");
    //argumente separieren, und in ptr-array speichern
    while (p2 && argc < maxArguments)
		{
		argv[argc++] = p2;
		//sachen passieren
		p2 = strtok(0," ");
		}
    for (int i = 0; i < callback_write; i++)
		{
    	if (callbacks[i].cbf != 0 && strcmp(argv[0], callbacks[i].command) == 0)
	    	{
    		callbacks[i].cbf(argc, (const char**) argv);
    		return;
	    	}
		}
    }

FRESULT cmdfile_lol_readln(TD_TERMFILE* initcmd, char* buffer, int linenr)
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
FRESULT cmdfile_lol_writeln(TD_TERMFILE* initcmd, char* buffer, int linenr)
{
	FRESULT stat;
	UINT byteswrote;
	int bufflen;

	bufflen = strlen(buffer);										//zu lange strings werden abgeschnitten gesendet
	utils_truncate_number_int(&bufflen, 0, initcmd->maxchars);
	utils_truncate_number_int(&linenr, 0, initcmd->maxlines);		//in der letzten zeile sollte nichts wichtiges stehen


	stat = f_open(&SDFile, initcmd->filename, FA_WRITE | FA_READ);
	stat = f_lseek(&SDFile, linenr * initcmd->maxchars);			// immer ab anfang der zeile schreiben.
	stat = f_write(&SDFile, buffer, bufflen, &byteswrote);
	stat = f_close(&SDFile);

	return stat;
}
