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
	char* header;
	const char* filename;
	uint16_t act_line;
	uint64_t bytesWrote;	//TODO:jetzt nocht pro sd-karte wär schön
}
HHW_FILE_FORMAT;
extern HHW_FILE_FORMAT eeprom, happylog;
void init_sdfile_eeprom(HHW_FILE_FORMAT* file);
void init_sdfile_happylog(HHW_FILE_FORMAT* file);
void sdfile_happylog_new(HHW_FILE_FORMAT* happylog);

#endif /* PROPELLI_SDFILE_H_ */
