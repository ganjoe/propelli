/*
 * termfile.h
 *
 *  Created on: 29.11.2020
 *      Author: danie
 */

#ifndef PROPELLI_TERMFILE_H_
#define PROPELLI_TERMFILE_H_
#include "datatypes.h"
#include "fatfs.h"
/*
 * empfängt und schreibt strings aus einer datei
 *  und schickt sie zum parser in terminal.c
 *
*/
typedef struct
{
	//zeilenbuffer/dateiname für schreiben/lesen
	char* linebuffer;
	char* filename;
	//dateiformat
	int maxchars, maxlines, maxarguments;
	//steuerzeichen
	char sep[2];
	char cr[2];
	char ef[2];

	int flag_initdone;

}
TD_TERMFILE;

void init_cmdfile(TD_TERMFILE* initcmd);
void cmdfile_lol_open_create(TD_TERMFILE* initcmd);
FRESULT cmdfile_lol_readln(TD_TERMFILE* initcmd, char* buffer, int linenr);
FRESULT cmdfile_lol_writeln(TD_TERMFILE* initcmd, char* buffer, int linenr);
extern TD_TERMFILE initcmd;

#endif /* PROPELLI_TERMFILE_H_ */
