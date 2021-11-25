/*
 * adp_card.c
 *
 *  Created on: Oct 24, 2021
 *      Author: Jhulliane
 */

#include <stdint.h>
#include "adp_card.h"
#include "fatfs.h"
#include "fatfs_sd.h"
#include <math.h>

#define MAX_BLOCKS_RECORD 11

#define SAMPLE_RATE		48000

#define BITS_PER_SAMPLE	32

#define NUM_CHANNELS	1

uint8_t DataFromADC[MAX_BLOCKS_RECORD] = {48};

char buffer[MAX_BLOCKS_RECORD];	// To store data read, increase here the size of the sample

UINT bytes_written;

UINT bytes_read;

UINT bytes_sent;

uint8_t ID_block;

State_SD_Card SD_State = Setup;

SD_Command_Enum SD_Command = Start_Record_Command;

__IO uint32_t  SDCardTimeout = 0;

SD_Command_Enum SD_Card_Command = No_Command;

uint8_t SD_Recorded_Files = 0;

HAL_StatusTypeDef Status_SPI;

TCHAR * File_Name = "File00.txt";

FATFS fs; 			// File System
FIL fil;			// File
FRESULT fresult;	// To store the result
FILINFO file_info;  // To store file information




/****************************************************************
 * The function controls the state machine of ADP FLual SD Card
 * @ param lastTick is the last Tick increment
 ***************************************************************/
void SD_State_Machine(uint32_t lastTick, SPI_HandleTypeDef hspi){
	static uint8_t lastID_debug;
	WAV_Header Audio_Header;

	switch(SD_State){
	case Setup:
		//f_mount(0,"", 0);
		lastID_debug = 0x01;
		Status_SPI = HAL_SPI_Init(&hspi);
		fresult = f_mount(&fs,"",1);
		if (fresult == FR_OK && Status_SPI == HAL_OK){
			SD_State = Ready;
		}
		else
		{
			SDCardTimeout = lastTick + 1000;
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|LD3_Pin, GPIO_PIN_SET);
			SD_State = Setup_Failure;
		}
		break;
	case Setup_Failure:
		if (SDCardTimeout >= lastTick)
		{
			SD_State = Setup;
		}
		break;
	case Ready:
		if( Get_Command_SD_Card() == Record_Command ){
			/* Open file to write/create a file if it doesn't exist */
			File_Name = SD_Create_File_Name(SD_Recorded_Files);
			fresult = f_open(&fil, File_Name, FA_CREATE_NEW | FA_WRITE);
			if (fresult == FR_OK){
				//SD_State = Waiting_Data;
				SD_Recorded_Files++;

				Create_WAV_Header(&Audio_Header);

				fresult = f_write(&fil, &Audio_Header, sizeof(Audio_Header), &bytes_written);

				SD_State = Record_File;

				SDCardTimeout = lastTick + 1000;
			}
			else if (fresult == FR_EXIST){
				SD_Recorded_Files++;
				break;
			}
			else{
				SD_State = Setup;
			}
		}
		else if (Get_Command_SD_Card() == Play_Command){
			// File_Name is the last file recorded, if "play file" is commanded right after the device initialization, there will be a failure.
			// This issue will be fixed in further versions
		  	fresult = f_open(&fil, File_Name, FA_READ);
		  	if (fresult == FR_OK){
		  		SD_State = Play_File;
		  	}
			else{
				fresult = f_close(&fil);
				if(fresult == FR_OK){
					SD_State = Ready;
				}
				else{
					SD_State = Setup;
				}
		  	}
		}
		else{
			/* Keep the same state if the command is No_Command */
		}
		break;
	case Record_File:
		// If there are no new blocks to be recorded, SD Card will close the file opened
		if (Get_Command_SD_Card() == Record_Command && ID_block != lastID_debug){
			bytes_written = 0;
			fresult = f_write(&fil, DataFromADC, sizeof(DataFromADC), &bytes_written);
			if (fresult == FR_OK){
				// Waiting for the command to record a new 4096 bytes block
				SDCardTimeout = lastTick + 1000;
			}
			else{
				fresult = f_close(&fil);
				if(fresult == FR_OK){
					SD_State = Ready;
				}
				else{
					SD_State = Setup;
				}
				f_stat(File_Name, &file_info);
			}
		}
		else if (SDCardTimeout >= lastTick || Get_Command_SD_Card() == Stop_Recording_Command){
			fresult = f_close(&fil);
			if(fresult == FR_OK){
				SD_State = Ready;
			}
			else{
				SD_State = Setup;
			}
			f_stat(File_Name, &file_info);
			f_open(&fil, File_Name, FA_OPEN_EXISTING | FA_WRITE);
			f_lseek(&fil,4);
			//file_info.fsize = 4000000;
			f_write(&fil, &file_info.fsize, 4, &bytes_written);
			f_lseek(&fil,40);
			f_write(&fil, &file_info.fsize, 4, &bytes_written);
			fresult = f_close(&fil);
			if(fresult == FR_OK){
				SD_State = Ready;
			}
			else{
				SD_State = Setup;
			}
		}
		break;
	case Play_File:
		// Add here RTOS sync mechanism to read data from the file

		// Reading some attributes of the file, here we are interested in its size
		f_stat(File_Name, &file_info);

		// Checking if the file has been fully read
		if(bytes_sent < file_info.fsize){
			fresult = f_read(&fil, buffer, sizeof(buffer), &bytes_read);
			if(fresult == FR_OK){
				bytes_sent += sizeof(buffer);
			}
			else{
				SD_State = Setup;
			}
		}
		else{
			fresult = f_close(&fil);
			if(fresult == FR_OK){
				SD_State = Ready;
			}
			else{
				SD_State = Setup;
			}
		}
	break;
	}
}



void Record_New_Block(uint8_t * SD_Block, uint8_t size, uint8_t id_block){
	for(int i = 0; i < size; i++){
		DataFromADC[i] = SD_Block[i];
	}
	ID_block = id_block;
}

/****************************************************************
 * The function returns the current command to control the SD Card
 ***************************************************************/
uint8_t Get_Command_SD_Card(){
	switch(SD_Card_Command){
	case 0:
		SD_Command = Start_Record_Command;
		break;
	case 1:
		SD_Command = Play_Command;
		break;
	case 2:
		SD_Command = Record_Command;
		break;
	case 3:
		SD_Command = Stop_Recording_Command;
		break;
	default:
		SD_Command = No_Command;
		break;
	}
	return SD_Command;
}

/****************************************************************
 * The function returns the name of a new created file
 ***************************************************************/
char * SD_Create_File_Name(uint8_t File_Number){
	  static char Name_File[] = "File00.txt";
	  if(File_Number < 10 && File_Number > 0){
		  Name_File[5] =  48 + File_Number;
	  }
	  else if (File_Number >= 10){
		  Name_File[4] = 48 + (int)(File_Number/10);
		  Name_File[5] = 48 + fmod(File_Number,10);
	  }
	  return Name_File;
}


SD_Command_Enum Get_SD_State(){
	return SD_State;
}

void Set_SD_Command(SD_Command_Enum Command_SD_Card){
	SD_Card_Command = Command_SD_Card;
}

void Set_SD_State_Machine(State_SD_Card state){
	SD_State = state;
}

void Create_File(TCHAR * file_name){

}

uint8_t * Get_SD_Data(){
	return buffer;
}

void Create_WAV_Header(WAV_Header * Audio_Header){
	// RIFF chunk
	Audio_Header -> RIFF_ID[0]		= 'R';
	Audio_Header -> RIFF_ID[1]		= 'I';
	Audio_Header -> RIFF_ID[2]		= 'F';
	Audio_Header -> RIFF_ID[3]		= 'F';

	Audio_Header -> RIFF_Size		= -1;

	Audio_Header -> RIFF_Type[0] 	= 'W';
	Audio_Header -> RIFF_Type[1] 	= 'A';
	Audio_Header -> RIFF_Type[2] 	= 'V';
	Audio_Header -> RIFF_Type[3] 	= 'E';

	// Format Chunk
	Audio_Header -> FMT_ID[0]		= 'f';
	Audio_Header -> FMT_ID[1]		= 'm';
	Audio_Header -> FMT_ID[2]		= 't';
	Audio_Header -> FMT_ID[3]		= ' ';

	Audio_Header -> FMT_Size			= 16;
	Audio_Header -> Compression_Code	= 0;
	Audio_Header -> Channels			= NUM_CHANNELS;
	Audio_Header -> Bytes_Rate			= SAMPLE_RATE;
	Audio_Header -> Bits_Per_Sample		= NUM_CHANNELS * BITS_PER_SAMPLE/8;
	Audio_Header -> Bytes_Per_Sample	= SAMPLE_RATE * Audio_Header->Bits_Per_Sample;


	// Data Chunk
	Audio_Header -> DATA_ID[0]			= 'D';
	Audio_Header -> DATA_ID[1]			= 'A';
	Audio_Header -> DATA_ID[2]			= 'T';
	Audio_Header -> DATA_ID[3]			= 'A';

	Audio_Header -> Data_Size			= -1;

}
