/**
  ******************************************************************************
  * @file   fatfs.h
  * @brief  Header for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __fatfs_H
#define __fatfs_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h" /* defines SD_Driver as external */

/* USER CODE BEGIN Includes */
#include "rtc.h"
#include "stdio.h"

/* USER CODE END Includes */

extern uint8_t retSD; /* Return value for SD */
extern char SDPath[4]; /* SD logical drive path */
extern FATFS SDFatFS; /* File system object for SD logical drive */
extern FIL SDFile; /* File object for SD */

void MX_FATFS_Init(void);

/* USER CODE BEGIN Prototypes */


typedef struct
    {
	//int Lines, Cols;

	int pos_x, pos_y, filecount;
	UINT bytesWrote;
	int maxlines, maxchars;
	//TODO: dynamisch
	char firstLine [120];
	char lastLine [120];

    }
    PageBuffer;

typedef struct
    {
	int files, freebyte, totalbyte;
	char *LABEL;
	char Filename[32];
	FIL SDFile;
	int notPresent;
    }
    storageinfo;

typedef struct
    {
	PageBuffer 	pbuffer;
	storageinfo 	sdinfo;
	FRESULT errcode;


    }
    TD_fatlog;

    FRESULT filelog_init		(TD_fatlog *fatlog);
    FRESULT fileinit_init		(TD_fatlog *init);
    //TODO: soll pl_lol_writeheader_ptr ersetzt werden
    FRESULT pl_lol_writeheader		(TD_fatlog *fatlog);
    FRESULT pl_lol_writeheader_ptr	(TD_fatlog *log);
    FRESULT pl_lol_newlog		(TD_fatlog *fatlog);
    FRESULT pl_lol_newlogname		(TD_fatlog *log);
    FRESULT pl_lol_writebuffer		(TD_fatlog *fatlog);
    FRESULT pl_lol_writebuffer_ptr	(TD_fatlog *log);
    FRESULT pl_lol_update		(TD_fatlog *fatlog);

    extern TD_fatlog filelog, fileinit;

/* USER CODE END Prototypes */
#ifdef __cplusplus
}
#endif
#endif /*__fatfs_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
