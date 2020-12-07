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
}
HHW_FILE_FORMAT;
extern HHW_FILE_FORMAT eeprom, happylog;
void init_sdfile_eeprom();
void init_sdfile_happylog();
void sdfile_happylog_new();

#endif /* PROPELLI_SDFILE_H_ */
