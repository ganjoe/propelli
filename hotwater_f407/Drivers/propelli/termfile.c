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

void init_cmdfile(TD_TERMFILE* cmd)
{
	cmd->maxarguments = 3;
	cmd->maxchars = 32;
	cmd->maxlines = 0xF;
	strcpy(cmd->cr, "+");
	strcpy(cmd->ef, "#");
	strcpy(cmd->sep, "_");
	char filename[] = "InitCommBatch.eeprom";

	cmd->linebuffer = malloc(cmd->maxchars);
	int fnlen = strlen(filename);
	cmd->filename = malloc(fnlen);
	strcpy(cmd->filename, filename);

	cmdfile_lol_open_create(cmd);
	char *strcmd;
	strcmd = strdup("setdate 29 11 20\r");	cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("settime 20 08 00\r");	cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("nlog\r");				cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("readpin 0 1\r");		cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("readpin 1 1\r");		cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);
	strcmd = strdup("readpin 2 1\r");		cmdfile_lol_writeln(cmd, strcmd, cmd->cmdcounter++);

	strcmd = strdup("0");
/*
	cmdfile_lol_readln(&initcmd, strcmd, 0);
	terminal_process_string(strcmd);
	cmdfile_lol_readln(&initcmd, strcmd, 1);
	terminal_process_string(strcmd);
	cmdfile_lol_readln(&initcmd, strcmd, 2);
	terminal_process_string(strcmd);
	cmdfile_lol_readln(&initcmd, strcmd, 3);
	terminal_process_string(strcmd);
	cmdfile_lol_readln(&initcmd, strcmd, 4);
	terminal_process_string(strcmd);
	cmdfile_lol_readln(&initcmd, strcmd, 5);
	terminal_process_string(strcmd);
	*/

}

void cmdfile_lol_open_create(TD_TERMFILE* initcmd)
{
	FRESULT stat;
	stat = f_open(&SDFile, initcmd->filename, FA_READ | FA_WRITE  );

	switch (stat)
	{
		/* für einen differenzierten status muss FAT und hardware fehlerfrei laufen
		dateisystem wird in fatfs.c gemountet */
		case FR_NO_FILE:
		{
		f_open(&SDFile, initcmd->filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
		f_close(&SDFile);
		commands_printf("/r neue initdatei erstellt/r");

		}break;
		case FR_OK:
		{
			initcmd->flag_initdone = true;
			stat  =	f_close(&SDFile);
		}break;

		default:
		{
			initcmd->flag_initdone = false;
		}break;

	}
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
void cmdfile_do_cmd	(TD_TERMFILE* initcmd)
	{
	int itr = initcmd->cmdcounter;
	if (initcmd->flag_initdone = true)
		{


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
