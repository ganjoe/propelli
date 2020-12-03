/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
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

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
TD_fatlog filelog;

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
   FRESULT fres;
   int i;
   HAL_Delay(1000);
   fres = 	f_mount(&SDFatFS, SDPath, 1);	//1=sofortmount wenn das dateisystem bereits passt
   fres = 	f_setlabel("HAPPYLOG");
   filelog.sdinfo.notPresent = fres;
   filelog.sdinfo.filenamesize = 32;

  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */
FRESULT pl_lol_writeheader	(TD_fatlog *fatlog)
{

    fatlog->errcode =	f_open(&SDFile, fatlog->sdinfo.Filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    fatlog->errcode  =	f_write(&SDFile, fatlog->pbuffer.firstLine, strlen( fatlog->pbuffer.firstLine), &fatlog->pbuffer.bytesWrote);
    fatlog->errcode  =	f_close(&SDFile);

    fatlog->pbuffer.pos_x	= fatlog->pbuffer.bytesWrote;
    fatlog->pbuffer.pos_y	=1;


    return fatlog->errcode;
}
FRESULT pl_lol_writeheader_ptr	(TD_fatlog *log)
{

    log->errcode =	f_open(&log->sdinfo.SDFile, log->sdinfo.Filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    log->errcode  =	f_write(&log->sdinfo.SDFile, log->pbuffer.firstLine, strlen( log->pbuffer.firstLine), &log->pbuffer.bytesWrote);
    log->errcode  =	f_close(&log->sdinfo.SDFile);

    log->pbuffer.pos_x	= log->pbuffer.bytesWrote;
    log->pbuffer.pos_y	=1;


    return log->errcode;
}
FRESULT pl_lol_writebuffer	(TD_fatlog *fatlog)
{
    fatlog->errcode =	f_open(&SDFile, fatlog->sdinfo.Filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    fatlog->errcode =	f_lseek(&SDFile,fatlog->pbuffer.pos_x);
    fatlog->errcode  =	f_write(&SDFile, fatlog->pbuffer.lastLine, strlen( fatlog->pbuffer.lastLine), &fatlog->pbuffer.bytesWrote);
    fatlog->errcode  =	f_close(&SDFile);
    fatlog->pbuffer.pos_x	+= fatlog->pbuffer.bytesWrote;
    fatlog->pbuffer.pos_y	+=1;
    if (fatlog->pbuffer.pos_y > fatlog->pbuffer.maxlines)
    	pl_lol_newlog(fatlog);
    return fatlog->errcode;
}
FRESULT pl_lol_writebuffer_ptr	(TD_fatlog *log)
{
    log->errcode =	f_open(&log->sdinfo.SDFile, log->sdinfo.Filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    log->errcode =	f_lseek(&log->sdinfo.SDFile,log->pbuffer.pos_x);
    log->errcode  =	f_write(&log->sdinfo.SDFile, log->pbuffer.lastLine, strlen( log->pbuffer.lastLine), &log->pbuffer.bytesWrote);
    log->errcode  =	f_close(&log->sdinfo.SDFile);
    log->pbuffer.pos_x	+= log->pbuffer.bytesWrote;
    log->pbuffer.pos_y	+=1;
    if (log->pbuffer.pos_y > log->pbuffer.maxlines)
    	pl_lol_newlog(log);
    return log->errcode;
}
FRESULT pl_lol_readbuffer	(TD_fatlog *log)
    {
    log->errcode =	f_open(&SDFile, log->sdinfo.Filename, FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
    log->pbuffer.pos_x = log->pbuffer.pos_y * log->pbuffer.maxchars;
    log->errcode =	f_lseek(&SDFile,log->pbuffer.pos_x);

    }
FRESULT pl_lol_update		(TD_fatlog *fatlog)
{
	if (!fatlog->sdinfo.notPresent)
	    {
	    fatlog->errcode = pl_lol_writebuffer(fatlog);
	    return fatlog->errcode;
	    }
	return fatlog->errcode;
}
FRESULT filelog_init		(TD_fatlog *fatlog)
{
	pl_rtc_timestring(fatlog->sdinfo.Filename, 1);

	//prettylog->mf_prettylog.init_done = 1;

	fatlog->pbuffer.maxchars = 120;
	fatlog->pbuffer.maxlines = 0xFFFF;

	fatlog->sdinfo.LABEL="HAPPYLOG";
	char buffer[]=	"log_date\t"
			"log_time\t"
			"volt_mcu\t"
			"battvolt\t"
			"temp_mcu\t"
			"coldwatr\t"
			"hotwater\t"
			"mcpinput\t"
			"loglines\t"
			"\r";
	strcpy(fatlog->pbuffer.firstLine, buffer);
	fatlog->errcode = pl_lol_writeheader(fatlog);
	return fatlog->errcode;
}

FRESULT	pl_lol_newlog		(TD_fatlog *log)
{
	/*	*/
	 pl_rtc_timestring(&log->sdinfo.Filename, 1);
	//pl_lol_writeheader(log);
	log->pbuffer.pos_y = 0;
	log->pbuffer.filecount +=1;
	log->pbuffer.bytesWrote=0;

}
FRESULT	pl_lol_newlogname	(TD_fatlog *log)
{
	FRESULT err;
	//err=pl_lol_writeheader	(log);
	log->pbuffer.pos_y = 0;
	log->pbuffer.filecount +=1;
	log->pbuffer.bytesWrote=0;
	return err;
}


/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
