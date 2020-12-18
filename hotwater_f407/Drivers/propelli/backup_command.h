/*
 * backup_command.h
 *
 *  Created on: 15.12.2020
 *      Author: danie
 */

#ifndef PROPELLI_BACKUP_COMMAND_H_
#define PROPELLI_BACKUP_COMMAND_H_

#include "sdfile.h"

extern HHW_FILE_FORMAT eeprom;
typedef enum
{
	LINE_SETTIME,
	LINE_SETDATE,
	LINE_VRANGE,		//spannungsreigabe für ventile, pumpen..
	LINE_MODE,
	LINE_TRANGE,		//zweipunktregler heizung
	LINE_CNT_FLOW,		//gesamtzähler hot, cold
	LINE_LEVEL,			//aktueller geschätzer füllstand hot, cold
	LINE_PARMS_FLOW,	//polynome für ausgleichsfunktion. signifikanteste polynome links xn, xn-1
}
BACKUP_CMD_LIST;

void backup_all();


void mfinit_sdfile_backup	();
void mftick_sdfile_backup	(HHW_FILE_FORMAT* file);
void mftask_sdfile_backup	(HHW_FILE_FORMAT* file);

int backup_read				(HHW_FILE_FORMAT*	eeprom, BACKUP_CMD_LIST line, float* argbuffer);

void backup_datetime		(HHW_FILE_FORMAT* 	eeprom);
void backup_voltrange		(HHW_FILE_FORMAT*	eeprom);
void backup_trange			(HHW_FILE_FORMAT*	eeprom);
void backup_mode			(HHW_FILE_FORMAT*	eeprom);

#endif /* PROPELLI_BACKUP_COMMAND_H_ */

