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

/*******************************************************************************
 * Events
 *
 * */
#define ADPFLUIAL_GETVOICECOMMAND(VALUE) getVoiceCommandEvent(VALUE)
bool getVoiceCommandEvent(voiceCommandEnum_t *trigger);

#define ADPFLUAL_VC_LIST_TYPEDEF voiceCommandEnum_t

#define ADPFLUAL_TIMEROUT() 	getTimerOutEvent()
bool getTimerOutEvent(void);