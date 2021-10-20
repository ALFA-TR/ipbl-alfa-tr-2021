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
#include "sm_adp-flual_user.h"
#include "sm_adp-flual.h"

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
		adpflual_State = SM_READY;
		break;
	case SM_READY:{
		ADPFLUAL_VC_LIST_TYPEDEF temp;
		if(ADPFLUIAL_GETVOICECOMMAND(&temp)){
			if(temp == VC_RECORD_ID){
				ADPFLUAL_NEWFILE();
				adpflual_State = SM_RECORD_ID;
			}else if(temp == VC_RECORD_FLUENCY){
				ADPFLUAL_NEWFILE();
				adpflual_State = SM_RECORD_FLUENCY;
			}else if(temp == VC_SEND){
				ADPFLUAL_SEEK_WIFI_NETWORK();
				adpflual_State = SM_CONNECT;
			}else if(temp == VC_PLAY_RECORDED){
				ADPFLUAL_PLAY_LAST_SOUND();
				adpflual_State = SM_PLAY;
			}
		} else if(ADPFLUAL_TIMEROUT()){
			ADPFLUAL_BLINK_RED_LED();
			adpflual_State = SM_HARD_FAULT;
		}
	}
	break;
	case SM_RECORD_ID:{
		ADPFLUAL_VC_LIST_TYPEDEF temp;
		if(ADPFLUIAL_GETVOICECOMMAND(&temp)){
			if(temp == VC_STOP_RECORD){
				ADPFLUAL_STOP_RECORD();
				adpflual_State = SM_READY;
			}
		}
	}
	break;
	case SM_RECORD_FLUENCY:{

	}
	break;
	case SM_HARD_FAULT:{

	}
	break;
	case SM_CONNECT:{

	}
	break;
	case SM_SEND:{

	}
	break;
	case SM_PLAY:{

	}
	break;
	}
}