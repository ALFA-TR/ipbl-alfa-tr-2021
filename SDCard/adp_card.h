/*
 * adp_sdcard.h
 *
 *  Created on: Oct 24, 2021
 *      Author: Jhulliane
 */

#ifndef INC_ADP_CARD_H_
#define INC_ADP_CARD_H_

#include "stm32l4xx_hal.h"


// Defining the states of the SD CARD state machine
typedef enum{
	Setup,
	Setup_Failure,
	Ready,
	Record_File,
	Play_File,
	Send_File
}State_SD_Card;

typedef enum{
	 Start_Record_Command = 0,
	 Play_Command,
	 Record_Command,
	 Stop_Recording_Command,
	 No_Command
}SD_Command_Enum;

typedef struct{
	// RIFF Chunk
	char RIFF_ID[4];			// 4 bytes
	uint32_t RIFF_Size;			// 4 bytes
	char RIFF_Type[4];			// 4 bytes

	// Format Chunk
	char FMT_ID[4];				// 4 bytes
	uint32_t FMT_Size;			// 4 bytes
	uint16_t Compression_Code;	// 2 bytes
	uint16_t Channels;
	uint32_t Sample_Rate;
	uint32_t Bytes_Rate;
	uint16_t Bytes_Per_Sample;
	uint16_t Bits_Per_Sample;
	//uint16_t Extra_Format;

	// Data Chunk
	char DATA_ID[4];
	uint32_t Data_Size;

}WAV_Header;

/****************************************************************
 * Prototypes of SD Card State Machine functions
 ***************************************************************/
void SD_State_Machine(uint32_t lastTick, SPI_HandleTypeDef hspi);

uint8_t Get_Command_SD_Card();

char * SD_Create_File_Name(uint8_t File_Number);

SD_Command_Enum Get_SD_State();

void Set_SD_Command(SD_Command_Enum Command_SD_Card);

void Set_SD_State_Machine(State_SD_Card state);

void Record_New_Block(uint8_t * SD_Block, uint8_t size, uint8_t id_block);

void Create_WAV_Header(WAV_Header * Audio_Header);

#endif /* INC_ADP_CARD_H_ */
