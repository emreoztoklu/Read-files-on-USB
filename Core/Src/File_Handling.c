/*
 * File_Handling_RTOS.c
 *
 *  Created on: 26-June-2020
 *      Author: Controllerstech.com
 */

#include "File_Handling.h"
#include "stm32f4xx_hal.h"
#include "usb_host.h"
#include <string.h>
#include <stdio.h>

//#include "retarget.h"

extern FILELIST_FileTypeDef FileList;
extern ApplicationTypeDef Appli_state;
USBH_HandleTypeDef hUSBHost;
uint16_t NumObs = 0;

extern UART_HandleTypeDef huart3;
#define UART &huart3



/* =============================>>>>>>>> NO CHANGES AFTER THIS LINE =====================================>>>>>>> */



extern char USBHPath[4];   /* USBH logical drive path */
extern FATFS USBHFatFS;    /* File system object for USBH logical drive */
extern FIL USBHFile;       /* File object for USBH */

FILINFO USBHfno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pUSBHFatFS;
DWORD fre_clust;
uint32_t total, free_space;

uint8_t rxBuffer[64] = {0};

void Send_Uart (char *string){

	HAL_UART_Transmit(UART, (uint8_t *)string, strlen(string), HAL_MAX_DELAY);

}

char * Get_FilenameUart (void) {
	Send_Uart("Enter a File name:");
	HAL_UART_Receive(UART, rxBuffer, 64, HAL_MAX_DELAY);
	return (char*)rxBuffer;
}


int Mount_USB (void){
	if ((fresult = f_mount(&USBHFatFS, USBHPath, 1)) != FR_OK){
		Send_Uart ("ERROR!!! in mounting USB ...\n\n");
		return 1;
	}
	else{
		Send_Uart("USB mounted successfully...\n");
		return 0;
	}
}

int Unmount_USB (void){
	if ((fresult = f_mount(NULL, USBHPath, 1)) == FR_OK){
		Send_Uart ("USB UNMOUNTED successfully...\n\n\n");
		return 0;
	}

	else{
		Send_Uart("ERROR!!! in UNMOUNTING USB \n\n\n");
		return 1;
	}
}

/* Start node to be scanned (***also used as work area***) */
FRESULT Scan_USB (char* pat){
    DIR dir;
    UINT i;
    char *path = malloc(20*sizeof (char));
    sprintf (path, "%s",pat);

                         /* Open the directory */
    if (( fresult = f_opendir(&dir, path)) == FR_OK){

    	for (;;){
                               /* Read a directory item */
            if ((fresult = f_readdir(&dir, &USBHfno)) != FR_OK || USBHfno.fname[0] == 0)
            	break;  /* Break on error or end of dir */

            if (USBHfno.fattrib & AM_DIR)     /* It is a directory */
            {
            	if (!(strcmp ("SYSTEM~1", USBHfno.fname)))
            		continue;
            	if (!(strcmp("System Volume Information", USBHfno.fname)))
            		continue;

            	char *buf = malloc(30*sizeof(char));
            	sprintf (buf, "Dir: %s\r\n", USBHfno.fname);
            	Send_Uart(buf);
            	free(buf);

                i = strlen(path);
                sprintf(&path[i], "/%s", USBHfno.fname);

                /* Enter the directory */
                if ((fresult = Scan_USB(path)) != FR_OK)
                	break;
                path[i] = 0;
            }

            else{   /* It is a file. */
           	   char *buf = malloc(50*sizeof(char));
               sprintf(buf,"File: %s/%s  %d KB\n", path, USBHfno.fname,(int)USBHfno.fsize/1024);
               Send_Uart(buf);
               free(buf);
            }
        }
        f_closedir(&dir);
    }
    free(path);
    return fresult;
}

/* Only supports removing files from home directory */
FRESULT Format_USB (void){
    DIR dir;
    char *path = malloc(20*sizeof (char));
    sprintf (path, "%s","/");

                           /* Open the directory */
    if ((fresult = f_opendir(&dir, path)) == FR_OK) {
        for (;;){
                               /* Read a directory item */
            if ((fresult = f_readdir(&dir, &USBHfno)) != FR_OK || USBHfno.fname[0] == 0)
            	break;  /* Break on error or end of dir */

            if (USBHfno.fattrib & AM_DIR)     /* It is a directory */
            {
            	if (!(strcmp ("SYSTEM~1", USBHfno.fname)))
            		continue;
            	if (!(strcmp("System Volume Information", USBHfno.fname)))
            		continue;


            	if ((fresult = f_unlink(USBHfno.fname)) == FR_DENIED) continue;
            }

            else{   /* It is a file. */
               fresult = f_unlink(USBHfno.fname);
            }
        }
        f_closedir(&dir);
    }
    free(path);
    return fresult;
}


FRESULT Write_File (char *name, char *data){

	/**** check whether the file exists or not ****/
	if ((fresult = f_stat (name, &USBHfno)) != FR_OK){
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* does not exists\n\n", name);
		Send_Uart (buf);
	    free(buf);
	    return fresult;
	}

	else{
	    /* Create a file with read write access and open it */
	    if ((fresult = f_open(&USBHFile, name, FA_OPEN_EXISTING | FA_WRITE)) != FR_OK)
	    {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
	    	Send_Uart(buf);
	        free(buf);
	        return fresult;
	    }

	    else{
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "Opening file-->  *%s*  To WRITE data in it\n", name);
	    	Send_Uart(buf);
	        free(buf);

	    	if ((fresult = f_write(&USBHFile, data, strlen(data), &bw)) != FR_OK){
	    		char *buf = malloc(100*sizeof(char));
	    		sprintf (buf, "ERROR!!! No. %d while writing to the FILE *%s*\n\n", fresult, name);
	    		Send_Uart(buf);
	    		free(buf);
	    	}

	    	/* Close file */
	    	if ((fresult = f_close(&USBHFile)) != FR_OK){
	    		char *buf = malloc(100*sizeof(char));
	    		sprintf (buf, "ERROR!!! No. %d in closing file *%s* after writing it\n\n", fresult, name);
	    		Send_Uart(buf);
	    		free(buf);
	    	}

	    	else{
	    		char *buf = malloc(100*sizeof(char));
	    		sprintf (buf, "File *%s* is WRITTEN and CLOSED successfully\n\n", name);
	    		Send_Uart(buf);
	    		free(buf);
	    	}
	    }
	    return fresult;
	}
}

FRESULT Read_File (char *name){
	/**** check whether the file exists or not ****/

	if ((fresult = f_stat (name, &USBHfno)) != FR_OK){
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERRROR!!! *%s* does not exists\n\n", name);
		Send_Uart (buf);
		free(buf);
	    return fresult;
	}

	else{
/********************************************************************************/
/* Open file to read */
		if ((fresult = f_open(&USBHFile, name, FA_READ)) != FR_OK){
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
		    Send_Uart(buf);
		    free(buf);
		    return fresult;
		}

/* Read data from the file
 *
 ** see the function details for the arguments */

    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "Opening file-->  *%s*  To READ data from it\n", name);
    	Send_Uart(buf);
        free(buf);
/********************************************************************************/
		char buffer[4096];
		char buffer1[4096];
		int isOn;
		int delay;
	    FRESULT fr;          /* FatFs function common result code */
	    UINT br, bw;         /* File read/write count */

		memset((void*)buffer, 0,sizeof(buffer));
		memset((void*)buffer1, 0,sizeof(buffer));


//		while(f_gets(buffer,sizeof(buffer), &USBHFile)){
		for(;;){
			fr = f_read(&USBHFile, buffer, sizeof (buffer), &br); /* Read a chunk of data from the source file */
		        if (br == 0)
		        	break; /* error or eof */


			isOn = !isOn;
			if(isOn == 1)
			  delay = 100;
			else
			  delay = 100;

			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, isOn);
			HAL_Delay(delay);

			Send_Uart("\n");
			for ( int i = 0; i < sizeof(buffer); i++){
				snprintf(buffer1,sizeof(buffer),"%02X", buffer[i]);

				if(i!=0 && !( i % 1)){
					Send_Uart(" ");
					if(!(i % 16)){
						Send_Uart("\n");
					}
				}

				Send_Uart(buffer1);

			}
		}
		Send_Uart("\n\n");

/********************************************************************************/
/* Close file */
		if ((fresult = f_close(&USBHFile)) != FR_OK){
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR!!! No. %d in closing file *%s*\n\n", fresult, name);
			Send_Uart(buf);
			free(buf);
		}
		else{
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "File *%s* CLOSED successfully\n\n", name);
			Send_Uart(buf);
			free(buf);
		}

	    return fresult;
	}
}

FRESULT Create_File (char *name){

	if ((fresult = f_stat (name, &USBHfno)) == FR_OK){
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* already exists!!!!\n use Update_File \n\n",name);
		Send_Uart(buf);
		free(buf);
	    return fresult;
	}

	else{

		if ((fresult = f_open(&USBHFile, name, FA_CREATE_ALWAYS|FA_READ|FA_WRITE)) != FR_OK){
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR!!! No. %d in creating file *%s*\n\n", fresult, name);
			Send_Uart(buf);
			free(buf);
		    return fresult;
		}
		else{
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "*%s* created successfully\n Now use Write_File to write data\n",name);
			Send_Uart(buf);
			free(buf);
		}


		if ((fresult = f_close(&USBHFile)) != FR_OK){
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR No. %d in closing file *%s*\n\n", fresult, name);
			Send_Uart(buf);
			free(buf);
		}
		else{
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "File *%s* CLOSED successfully\n\n", name);
			Send_Uart(buf);
			free(buf);
		}
	}
    return fresult;
}

FRESULT Update_File (char *name, char *data)
{
	/**** check whether the file exists or not ****/

	if ((fresult = f_stat (name, &USBHfno)) != FR_OK){
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* does not exists\n\n", name);
		Send_Uart (buf);
		free(buf);
	    return fresult;
	}

	else{
		 /* Create a file with read write access and open it */

	    if ((fresult = f_open(&USBHFile, name, FA_OPEN_APPEND | FA_WRITE)) != FR_OK){
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in opening file *%s*\n\n", fresult, name);
	    	Send_Uart(buf);
	        free(buf);
	        return fresult;
	    }

    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "Opening file-->  *%s*  To UPDATE data in it\n", name);
    	Send_Uart(buf);
        free(buf);

	    /* Writing text */
	    if ((fresult = f_write(&USBHFile, data, strlen (data), &bw)) != FR_OK){
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in writing file *%s*\n\n", fresult, name);
	    	Send_Uart(buf);
	    	free(buf);
	    }

	    else {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "*%s* UPDATED successfully\n", name);
	    	Send_Uart(buf);
	    	free(buf);
	    }

	    /* Close file */
	    if ((fresult = f_close(&USBHFile)) != FR_OK){
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "ERROR!!! No. %d in closing file *%s*\n\n", fresult, name);
	    	Send_Uart(buf);
	    	free(buf);
	    }
	    else {
	    	char *buf = malloc(100*sizeof(char));
	    	sprintf (buf, "File *%s* CLOSED successfully\n\n", name);
	    	Send_Uart(buf);
	    	free(buf);
	     }
	}
    return fresult;
}

FRESULT Remove_File (char *name)
{
	/**** check whether the file exists or not ****/
	if ((fresult = f_stat (name, &USBHfno)) != FR_OK){
		char *buf = malloc(100*sizeof(char));
		sprintf (buf, "ERROR!!! *%s* does not exists\n\n", name);
		Send_Uart (buf);
		free(buf);
		return fresult;
	}

	else{

		if ((fresult = f_unlink (name)) == FR_OK){
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "*%s* has been removed successfully\n\n", name);
			Send_Uart (buf);
			free(buf);
		}

		else{
			char *buf = malloc(100*sizeof(char));
			sprintf (buf, "ERROR No. %d in removing *%s*\n\n",fresult, name);
			Send_Uart (buf);
			free(buf);
		}
	}
	return fresult;
}

FRESULT Create_Dir (char *name){

    if ((fresult = f_mkdir(name)) == FR_OK){
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "*%s* has been created successfully\n\n", name);
    	Send_Uart (buf);
    	free(buf);
    }
    else{
    	char *buf = malloc(100*sizeof(char));
    	sprintf (buf, "ERROR No. %d in creating directory *%s*\n\n", fresult,name);
    	Send_Uart(buf);
    	free(buf);
    }
    return fresult;
}

void Check_USB_Details (void){
    /* Check free space */
    f_getfree("", &fre_clust, &pUSBHFatFS);

    total = (uint32_t)((pUSBHFatFS->n_fatent - 2) * pUSBHFatFS->csize * 0.5);
    char *buf = malloc(30*sizeof(char));
    sprintf (buf, "USB  Total Size KB: \t%lu\n",total/1024);
    Send_Uart(buf);
    free(buf);


    free_space = (uint32_t)(fre_clust * pUSBHFatFS->csize * 0.5);
    buf = malloc(30*sizeof(char));
    sprintf (buf, "USB Free Space KB: \t%lu\n",free_space/1024);
    Send_Uart(buf);
    free(buf);
}


FRESULT StorageParse(void){
  FRESULT res = FR_OK;
  FILINFO fno;
  DIR dir;
  char *fn;


  res = f_opendir(&dir, USBHPath);
  FileList.ptr = 0;

  if(res == FR_OK)
  {
    while(Appli_state == APPLICATION_READY)
    {
      res = f_readdir(&dir, &fno);
      if(res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if(fno.fname[0] == '.')
      {
        continue;
      }

      fn = fno.fname;

      if(FileList.ptr < FILEMGR_LIST_DEPDTH)
      {
        if((fno.fattrib & AM_DIR) == 0)
        {
          if((strstr(fn, "wav")) || (strstr(fn, "WAV")))
          {
            strncpy((char *)FileList.file[FileList.ptr].name, (char *)fn, FILEMGR_FILE_NAME_SIZE);
            FileList.file[FileList.ptr].type = FILETYPE_FILE;
            FileList.ptr++;
          }
        }
      }
    }
  }
  NumObs = FileList.ptr;
  f_closedir(&dir);
  return res;
}

uint16_t GetObjectNumber(void){
	if (StorageParse() != FR_OK)
		return -1;
	return NumObs;
}

