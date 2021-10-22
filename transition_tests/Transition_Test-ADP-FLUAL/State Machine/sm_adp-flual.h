/**
 ******************************************************************************
 * @file           : sm_adp-flual.h
 * @brief          : ADP-FLUAL State Machine
 ******************************************************************************
 * @attention
 *
 *State Machine
 *
 ******************************************************************************
 */

typedef enum{
	SHUTDOWN,
	SM_READY,
	SM_RECORD_ID,
	SM_RECORD_FLUENCY,
	SM_HARD_FAULT,
	SM_CONNECT,
	SM_SEND,
	SM_PLAY
} adpflual_State_t;

#ifdef DEBUG
void adpflual_setState (adpflual_State_t value);

adpflual_State_t adpflual_getState (void);

void adpflual_setEvent (void);

void adpflual_setTrigger (void);

#endif

void adpflual_State_Machine(void);
