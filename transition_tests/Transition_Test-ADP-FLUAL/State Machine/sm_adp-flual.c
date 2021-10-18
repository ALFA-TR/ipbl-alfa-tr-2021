/**
  ******************************************************************************
  * @file           : sm_adp-flual.c
  * @brief          : ADP-FLUAL State Machine
  ******************************************************************************
  * @attention
  *
  *State Machine
  *
  ******************************************************************************
  */

#include "main.h"
#include "sm_adp-flual.h"
#include "sm_adp-flual_user.h"

adpflual_State_t adpflual_State = SHUTDOWN;

#ifdef DEBUG
void adpflual_setState (adpflual_State_t value) {
	adpflual_State = value;
};

adpflual_State_t adpflual_getState (void) {
	return adpflual_State;
};

void adpflual_setEvent (void) {

};

void adpflual_setTrigger (void) {

};

#endif

void adpflual_State_Machine(void) {
	switch(adpflual_State) {

	case SHUTDOWN:
		ADPFLUAL_SETUP();
		adpflual_State = READY;
		break;
	case READY:
		break;
	case RECORD_ID:
		break;
	case RECORD_FLUENCY:
		break;
	case HARD_FAULT:
		break;
	case CONNECT:
		break;
	case SEND:
		break;
	case PLAY:
		break;
	}
}
