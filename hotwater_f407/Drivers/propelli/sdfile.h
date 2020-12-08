/*
 * sdfile.h
 *
 *  Created on: Dec 7, 2020
 *      Author: danie
 */

#ifndef PROPELLI_SDFILE_H_
#define PROPELLI_SDFILE_H_

typedef struct
{
	uint16_t maxchars, maxlines, maxfilename;
	const char* header;
	const char* filename;
	uint16_t act_line, flag;
	int64_t bytesWrote;	//TODO:jetzt nocht pro sd-karte wär schön
}
HHW_FILE_FORMAT;
extern HHW_FILE_FORMAT initcmd, happylog, eeprom;

void init_sdfile_initcmd	(HHW_FILE_FORMAT* file);
void init_sdfile_happylog	(HHW_FILE_FORMAT* file);
void sdfile_lol_newhappylog	(HHW_FILE_FORMAT* happylog);
void sdfile_add_logline		(HHW_FILE_FORMAT* happylog, char* buffer);

#endif /* PROPELLI_SDFILE_H_ */
