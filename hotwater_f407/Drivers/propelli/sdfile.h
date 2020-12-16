/*
 * sdfile.h
 *
 *  Created on: Dec 7, 2020
 *      Author: danie
 */


#ifndef PROPELLI_SDFILE_H_
#define PROPELLI_SDFILE_H_

#include "datatypes_tasks.h"

typedef struct
{
	modflag this_mf;
	int16_t maxchars, maxlines, maxfilename;
	char* header;
	char* filename;
	uint16_t act_line, flag;
	int64_t bytesWrote;	//TODO:jetzt nocht pro sd-karte wär schön
}
HHW_FILE_FORMAT;
extern HHW_FILE_FORMAT initcmd, happylog;

void init_sdfile_initcmd	();

void init_sdfile_happylog	(HHW_FILE_FORMAT* file);

void mftick_sdfile_happylog		(HHW_FILE_FORMAT* file);

void sdfile_lol_write_backup(HHW_FILE_FORMAT* file,const char* buffer, int line);
void sdfile_add_logline		(HHW_FILE_FORMAT* happylog, char* buffer);
void sdfile_lol_parsecmd	(HHW_FILE_FORMAT* file);
#endif /* PROPELLI_SDFILE_H_ */
