/*
 * backup_command.c
 *
 *  Created on: 15.12.2020
 *      Author: danie
 */

/*
 * High-Level Funktionen um Befehle und deren Parameter in eeprom.hhw zu speichern.
 * Die parameter sind live-daten die nach reset geladen werden sollen
 */

#include "backup_command.h"
#include "PrettyLog.h"
#include "datatypes.h"
#include "sdfile.h"
#include "HappyHotWater.h"

void backup_datetime		(HHW_FILE_FORMAT* 	eeprom);
void backup_voltrange		(HHW_FILE_FORMAT*	eeprom);
void backup_trange			(HHW_FILE_FORMAT*	eeprom);
void backup_mode			(HHW_FILE_FORMAT*	eeprom);

void mfinit_sdfile_backup		()
	{
	modflag_init(&eeprom.this_mf, HALTICK, 0.01);
	eeprom.filename=strdup	("backupcmd.hhw");
	eeprom.header=strdup	("backupcmd batch file");
	eeprom.maxchars =32;	//jede zeile ist gleichlang
	eeprom.maxlines =0;	//im sinne von maxcommands
	eeprom.maxfilename =32;

	//initcmd.bytesWrote = sd_lol_writeline(initcmd.filename, "setdate_9_12_20\r", initcmd.maxchars, initcmd.maxlines++);
	//initcmd.bytesWrote += sd_lol_writeline(initcmd.filename, "settime_00_24_00\r", initcmd.maxchars, initcmd.maxlines++);
	sdfile_lol_parsecmd(&eeprom);
	}
void mftask_sdfile_backup		(HHW_FILE_FORMAT* file)
	{
	 if (file->this_mf.flag && file->this_mf.init_done)
		{
		 file->this_mf.repeat = modflag_tickdiff(&file->this_mf);
		 file->this_mf.rampcounter = file->this_mf.callcount % 4;

		 if (file->this_mf.rampcounter==0)
			 backup_datetime(file);
		 if (file->this_mf.rampcounter==1)
			 backup_voltrange(file);
		 if (file->this_mf.rampcounter==2)
			 backup_trange(file);
		 if (file->this_mf.rampcounter==3)
			 backup_mode(file);

		 file->this_mf.duration = modflag_tickdiff(&file->this_mf);
		 file->this_mf.callcount++;
		 file->this_mf.flag = false;
		}
	}
void mftick_sdfile_backup		(HHW_FILE_FORMAT* file)
	{
	modflag_upd_regular(&file->this_mf);
	}

void backup_all()
{
	backup_datetime(&eeprom);
	backup_trange(&eeprom);
	backup_voltrange(&eeprom);
	backup_mode(&eeprom);
}


void backup_datetime	(HHW_FILE_FORMAT	*eeprom)
	{
	char timestring[32];

	pl_rtc_timestring(timestring, DATECMD);
	sdfile_lol_write_backup(eeprom, timestring, LINE_SETDATE);
	pl_rtc_timestring(timestring, TIMECMD);
	sdfile_lol_write_backup(eeprom, timestring, LINE_SETTIME);
	}
void backup_voltrange	(HHW_FILE_FORMAT	*eeprom)
	{
	char cmdstring[32];
	sprintf(cmdstring, "vrange_%2.1f_%2.1f_%2.1f", Hhw.VoltLevel_lowbatt, Hhw.VoltLevel_highbatt, Hhw.VoltLevel_power_inv);
	sdfile_lol_write_backup(eeprom, cmdstring, LINE_VRANGE);
	}
void backup_trange		(HHW_FILE_FORMAT	*eeprom)
	{
	char cmdstring[32];
	sprintf(cmdstring, "trange_%2.1f_%2.1f", Hhw.TempLevel_low, Hhw.TempLevel_high);
	sdfile_lol_write_backup(eeprom, cmdstring, LINE_TRANGE);
	}
void backup_mode			(HHW_FILE_FORMAT*	eeprom)
	{
	char cmdstring[32];
	if(hhw_get_state(&Hhw, TANK_MODE_FULL))
		{
		sprintf(cmdstring, "mode_%d", 1);
		}
	if(hhw_get_state(&Hhw, TANK_MODE_ECO))
		{
		sprintf(cmdstring, "mode_%d", 0);
		}
	sdfile_lol_write_backup(eeprom, cmdstring, LINE_MODE);

	}
HHW_FILE_FORMAT eeprom;
