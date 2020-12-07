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
#include "string.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  FRESULT fres = 	f_mount(&SDFatFS, SDPath, 1);
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
int sd_lol_writeline(char* filename, char* linebuff, uint8_t chars, uint8_t line)
{
	UINT bytesWrote;
	FRESULT fres;
	int slen;
	char* linebuffer;
/*---------------------------------------------------------*/
	linebuffer = calloc(chars + 2, 1);	// + \r und platz für die letzte \0 von calloc
	strcpy(linebuffer, linebuff);
	memcpy(linebuffer+chars, "\r",1);

	if (line == 0)
	{
		//write header
	}
	slen =strlen(linebuffer);
	if (slen>chars)
		{
		linebuffer =strdup("string not valid");
		}

/*---------------------------------------------------------*/
	fres = 	f_mount(&SDFatFS, SDPath, 1);
	fres =	f_open(&SDFile, filename, FA_READ| FA_WRITE);
	if (fres == FR_NO_FILE)
		 {
		 fres =	f_open(&SDFile, filename, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
		 }
	 fres =	f_lseek(&SDFile, chars * line);
	 f_write(&SDFile, linebuffer, chars, &bytesWrote);
	 f_close(&SDFile);
	 f_mount(&SDFatFS, SDPath, 0);
/*---------------------------------------------------------*/
	 if (fres == FR_OK)
		 return bytesWrote;
	 free (linebuffer);
	 return -1;
}

int sd_lol_readline(char* filename, char* linebuff, uint8_t chars, uint8_t line)
{
	UINT bytesWrote;
	FRESULT fres;
	int slen;
	char* linebuffer = calloc(chars+2,1);
	/*---------------------------------------------------------*/
		fres = 	f_mount(&SDFatFS, SDPath, 1);
		fres =	f_open(&SDFile, filename, FA_READ);
		if (fres == FR_NO_FILE)
			 {
			 return 0;
			 }
		 fres =	f_lseek(&SDFile, chars * line);
		 fres = f_read(&SDFile, linebuffer, chars, &bytesWrote);
		 f_close(&SDFile);
		 f_mount(&SDFatFS, SDPath, 0);
	/*---------------------------------------------------------*/

}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
