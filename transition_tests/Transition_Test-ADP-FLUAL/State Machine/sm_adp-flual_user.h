/**
 ******************************************************************************
 * @file           : sm_adp-flual_user.h
 * @brief          : ADP-FLUAL State Machine
 ******************************************************************************
 * @attention
 *
 *State Machine User definitions.
 *
 ******************************************************************************
 */
#include "main.h"
#include "stdbool.h"

/*******************************************************************************
 * Actions
 *
 * */
#define ADPFLUAL_SETUP()				setup()
void setup(void);

#define ADPFLUAL_NEWFILE()				newFileExecuted()
void newFileExecuted(void);

#define ADPFLUAL_BLINK_RED_LED()		blinkRedLed()
void blinkRedLed(void);

#define ADPFLUAL_SEEK_WIFI_NETWORK()	seekWifiNetwork()
void seekWifiNetwork(void);

#define ADPFLUAL_PLAY_LAST_SOUND() 		playLastSound()
void playLastSound(void);

#define ADPFLUAL_STOP_RECORD()			stopRecord()
void stopRecord(void);

#define ADPFLUAL_START_SEND()			startSend()
void startSend(void);

#define ADPFLUAL_CONNECTION_FAIL()		connectionFailAlert()
void connectionFailAlert(void);

#define ADPFLUAL_SEND_MESSAGE()			sendMessageAlert()
void sendMessageAlert(void);

#define ADPFLUAL_REBOOT_CAPTURE_FUNCTION() rebootCaptureFunction()
void rebootCaptureFunction(void);

/*******************************************************************************
 * Events
 *
 * */
/********EVENTS VOICE COMMANDS*************************************************/
#define ADPFLUIAL_GET_VOICECOMMAND(VALUE) 	 getVoiceCommandEvent(VALUE)
bool getVoiceCommandEvent(voiceCommandEnum_t *trigger);

#define ADPFLUAL_VC_LIST_TYPEDEF voiceCommandEnum_t
/********EVENTS VOICE COMMANDS*************************************************/

/********EVENTS TIMER OUT*****************************************************/
#define ADPFLUAL_TIMEROUT() 				 getTimerOutEvent()
bool getTimerOutEvent(void);
/********EVENTS TIMER OUT*****************************************************/

/********EVENTS CONNECTION****************************************************/
#define ADPFLUAL_GET_CONNECTION_STATUS(VALUE) getConnectionEvent(VALUE)
bool getConnectionEvent(connectionStatusEnum_t *trigger);

#define ADPFLUAL_CONNECTION_LIST_TYPEDEF connectionStatusEnum_t
/********EVENTS CONNECTION****************************************************/

/********EVENTS SEND STATUS****************************************************/
#define ADPFLUAL_GET_SEND_STATUS(VALUE) getSendStatusEvent(VALUE)
bool getSendStatusEvent(sendStatusEnum_t *trigger);

#define ADPFLUAL_SEND_STATUS_LIST_TYPEDEF sendStatusEnum_t
/********EVENTS SEND STATUS****************************************************/

/********EVENTS PLAY STATUS****************************************************/
#define ADPFLUAL_GET_PLAY_STATUS(VALUE) getPlayStatusEvent(VALUE)
bool getPlayStatusEvent(playStatusEvent_t *trigger);

#define ADPFLUAL_PLAY_STATUS_LIST_TYPEDEF playStatusEvent_t
/********EVENTS PLAY STATUS****************************************************/
