/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sm_adp-flual.h"
#include "stdbool.h"
#include "string.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SUCCESS_TEXT (const uint8_t*) "\033[0;30;42mTest Success!\033[0;39;49m\n\r"
#define FAIL_TEXT	 (const uint8_t*) "\033[0;37;41mTest Fail!\033[0;39;49m\n\r"


#define SETUP_EXECUTED			0x00000001
#define NEW_FILE_EXECUTED 		0x00000002
#define BLINK_RED_LED			0x00000004
#define SEEK_WIFI_NETWORK		0x00000008
#define PLAY_LAST_SOUND			0x00000010
#define CLOSE_FILE_RECORDED		0x00000020
#define START_SEND				0x00000040
#define FINISH_SEND				0x00000080
#define CONNECTION_FAIL_ALERT	0x00000100
#define SEND_MESSAGE_ALERT  	0x00000200
#define REBOOT_CAPTURE_FUNCTION 0x00000400

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/*Events status variables
 *
 * */

bool TurnOnEvent = false;
bool VoiceCommandEvent = false;
bool TimerOutEvent = false;
bool ConnectionEvent = false;
bool SendStatusEvent = false;
bool PlayStatusEvent = false;
/*Actions status variables
 *
 * */
voiceCommandEnum_t voiceCommand = VC_INVALID;
connectionStatusEnum_t connectionStatus = CONNECTION_STATUS_OFF;
sendStatusEnum_t sendStatus = SEND_OFF;
playStatusEvent_t playStatus = STOP_PLAY;
uint32_t executed_routines = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void send_Serial(const uint8_t * text){
	HAL_UART_Transmit(&huart2, (uint8_t *)text, strlen((const char *)text), 1000);
}

/*****************************************************************************
 * MOC for Actions application
 *
 */
void setup(void){
	executed_routines |= SETUP_EXECUTED;
}

void newFileExecuted(void){
	executed_routines |= NEW_FILE_EXECUTED;
}

void blinkRedLed(void){
	executed_routines |= BLINK_RED_LED;
}

void seekWifiNetwork(void){
	executed_routines |= SEEK_WIFI_NETWORK;
}

void playLastSound(void){
	executed_routines |= PLAY_LAST_SOUND;
}

void stopRecord(void){
	executed_routines |= CLOSE_FILE_RECORDED;
}

void startSend(void){
	executed_routines |= START_SEND;
}

void connectionFailAlert(void){
	executed_routines |= CONNECTION_FAIL_ALERT;
}

void sendMessageAlert(void){
	executed_routines |= SEND_MESSAGE_ALERT;
}

void rebootCaptureFunction(void){
	executed_routines |= REBOOT_CAPTURE_FUNCTION;
}
/*****************************************************************************
 * Mocs for Events application
 *
 */
void generateTurnOnEvent(void){
	TurnOnEvent = true;
}

bool getTurnOnEvent(void){
	bool retVal = TurnOnEvent;

	TurnOnEvent = false;
	return retVal;
}

void generateVoiceCommandEvent(voiceCommandEnum_t value){
	voiceCommand = value;
	VoiceCommandEvent = true;
}

bool getVoiceCommandEvent(voiceCommandEnum_t *trigger){
	bool retVal = VoiceCommandEvent;
	*trigger = voiceCommand;
	VoiceCommandEvent = false;
	voiceCommand = VC_INVALID;
	return retVal;
}

void generateTimerOutEvent(void){
	TimerOutEvent = true;
}

bool getTimerOutEvent(void){
	bool retVal = TimerOutEvent;
	TimerOutEvent = false;
	return retVal;
}

void generateConnectionEvent(connectionStatusEnum_t value){
	connectionStatus = value;
	ConnectionEvent = true;
}

bool getConnectionEvent(connectionStatusEnum_t *trigger){
	bool retVal = ConnectionEvent;
	*trigger = connectionStatus;
	ConnectionEvent = false;
	connectionStatus = CONNECTION_STATUS_OFF;
	return retVal;
}

void generateSendStatusEvent(sendStatusEnum_t value){
	sendStatus = value;
	SendStatusEvent = true;
}

bool getSendStatusEvent(sendStatusEnum_t *trigger){
	bool retVal = SendStatusEvent;
	*trigger = sendStatus;
	SendStatusEvent = false;
	sendStatus = SEND_OFF;
	return retVal;
}

void generatePlayStatusEvent(playStatusEvent_t value){
	playStatus = value;
	PlayStatusEvent = true;
}

bool getPlayStatusEvent(playStatusEvent_t *trigger){
	bool retVal = PlayStatusEvent;
	*trigger = playStatus;
	PlayStatusEvent = false;
	playStatus = STOP_PLAY;
	return retVal;
}


/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();

	/* USER CODE BEGIN 2 */
	/*
	 * Clear terminal prompt.
	 */
	send_Serial((const uint8_t *)"\033c");

	/*
	 * 1o teste de transi????o quando sai do Shutdown e configura o ambiente para READY.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test1: ");
	executed_routines = 0;
	adpflual_setState(SHUTDOWN);
	generateTurnOnEvent();
	adpflual_State_Machine();

	if(adpflual_getState()== SM_READY && executed_routines == (SETUP_EXECUTED)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}

	/*
	 * 2o teste de transi????o quando sai do Ready e entra em Record_id para come??ar
	 * uma grava????o de arquivo no cart??o SD.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test2: ");
	executed_routines = 0;
	adpflual_setState(SM_READY);
	generateVoiceCommandEvent(VC_RECORD_ID);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_RECORD_ID && executed_routines == (NEW_FILE_EXECUTED)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}

	/*
	 * 3o teste de transi????o quando sai do Ready e entra em Record_Fluency para come??ar
	 * uma grava????o de arquivo no cart??o SD.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test3: ");
	executed_routines = 0;
	adpflual_setState(SM_READY);
	generateVoiceCommandEvent(VC_RECORD_FLUENCY);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_RECORD_FLUENCY && executed_routines == (NEW_FILE_EXECUTED)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}

	/*
	 * 4o teste de transi????o quando sai do Ready e entra em Hard_fault para come??ar
	 * uma grava????o de arquivo no cart??o SD.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test4: ");
	executed_routines = 0;
	adpflual_setState(SM_READY);
	generateTimerOutEvent();
	adpflual_State_Machine();

	if(adpflual_getState()== SM_HARD_FAULT && executed_routines == (BLINK_RED_LED)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}

	/*
	 * 5o teste de transi????o quando sai do Ready e entra em Connect para come??ar uma grava????o
	 * do arquivo no cart??o SD.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test5: ");
	executed_routines = 0;
	adpflual_setState(SM_READY);
	generateVoiceCommandEvent(VC_SEND);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_CONNECT && executed_routines == (SEEK_WIFI_NETWORK)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}

	/*
	 * 6o teste de transi????o quando sai do Ready e entra em PLay para come??ar uma reprodu????o
	 * do arquivo no cart??o SD.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test6: ");
	executed_routines = 0;
	adpflual_setState(SM_READY);
	generateVoiceCommandEvent(VC_PLAY_RECORDED);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_PLAY && executed_routines == (PLAY_LAST_SOUND)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}
	/*
	 * 7o teste de transi????o quando sai do Record_ID e entra em Ready para finalizar a grava????o
	 * do arquivo no cart??o SD.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test7: ");
	executed_routines = 0;
	adpflual_setState(SM_RECORD_ID);
	generateVoiceCommandEvent(VC_STOP_RECORD);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_READY && executed_routines == (CLOSE_FILE_RECORDED)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}
	/*
	 * 8o teste de transi????o quando sai do Record Fluency Sound
	 * e entra em Ready para finalizar a grava????o de flu??ncia no
	 * arquivo no cart??o SD.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test8: ");
	executed_routines = 0;
	adpflual_setState(SM_RECORD_FLUENCY);
	generateVoiceCommandEvent(VC_STOP_RECORD);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_READY && executed_routines == (CLOSE_FILE_RECORDED)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}
	/*
	 * 9o teste de transi????o quando sai do Connect e entra em SEND
	 * para enviar os arquivos de ??udio para o servidor.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test9: ");
	executed_routines = 0;
	adpflual_setState(SM_CONNECT);
	generateConnectionEvent(CONNECTION_STATUS_SUCCESS);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_SEND && executed_routines == (START_SEND)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}
	/*
	 * 10o teste de transi????o quando sai do Connect e volta para READY
	 * para enviar os arquivos de ??udio para o servidor.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test10: ");
	executed_routines = 0;
	adpflual_setState(SM_CONNECT);
	generateConnectionEvent(CONNECTION_STATUS_FAIL);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_READY && executed_routines == (CONNECTION_FAIL_ALERT)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}
	/*
	 * 11o teste de transi????o quando sai do SEND e volta para READY
	 * informando o sucesso do envio dos arquivos de ??udio para o servidor.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test11: ");
	executed_routines = 0;
	adpflual_setState(SM_SEND);
	generateSendStatusEvent(SEND_SUCCESS);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_READY && executed_routines == (SEND_MESSAGE_ALERT)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}

	/*
	 * 12o teste de transi????o quando sai do SEND e volta para READY
	 * informando a falha de envio dos arquivos de ??udio para o servidor.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test12: ");
	executed_routines = 0;
	adpflual_setState(SM_SEND);
	generateSendStatusEvent(SEND_FAIL);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_READY && executed_routines == (SEND_MESSAGE_ALERT)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}
	/*
	 * 13o teste de transi????o quando sai do PLAY e volta para READY
	 * finalizando a reprodu????o do ??udio.
	 *
	 * */
	send_Serial((const uint8_t *)"Transition Test13: ");
	executed_routines = 0;
	adpflual_setState(SM_PLAY);
	generatePlayStatusEvent(STOP_PLAY);
	adpflual_State_Machine();

	if(adpflual_getState()== SM_READY && executed_routines == (REBOOT_CAPTURE_FUNCTION)){
		//Sucesso no teste.
		send_Serial(SUCCESS_TEXT);
	}
	else {
		//Falha no teste.
		send_Serial(FAIL_TEXT);
	}


	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure LSE Drive Capability
	 */
	HAL_PWR_EnableBkUpAccess();
	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 16;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
	/** Enable MSI Auto calibration
	 */
	HAL_RCCEx_EnableMSIPLLMode();
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
