/* ******************************************************************************** */
/*                                                                                  */
/* WCommandIntepreter.cpp															*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the commands processing               */
/*                                                                                  */
/* History :  	16/02/2015  (RW)	Creation of this file                           */
/*				14/05/2016	(RW)	Re-mastered version								*/
/*                                                                                  */
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WLink.h"
#include "WCommandInterpreter.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

typedef enum {
	WCMD_INTERPRETER_STATE_IDLE,
	WCMD_INTERPRETER_STATE_CHECK_CMD,
	WCMD_INTERPRETER_STATE_PROCESS_CMD,
	WCMD_INTERPRETER_STATE_SEND_RESP,
	WCMD_INTERPRETER_STATE_SEND_NACK
} WCMD_INTERPRETER_STATE;

static WCMD_INTERPRETER_STATE GL_CurrentState_E = WCMD_INTERPRETER_STATE_IDLE;


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessCheckCmd(void);
static void ProcessProcessCmd(void);
static void ProcessSendResp(void);
static void ProcessSendNack(void);

static void TransitionToIdle(void);
static void TransitionToCheckCmd(void);
static void TransitionToProcessCmd(void);
static void TransitionToSendResp(void);
static void TransitionToSendNack(void);


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */




/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void TransitionToIdle(void) {
	//DBG_PRINT("Transition To IDLE");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE_IDLE;
}

void TransitionToCheckCmd(void) {
	//DBG_PRINT("Transition To CHECK CMD");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE_CHECK_CMD;
}

void TransitionToProcessCmd(void) {
	//DBG_PRINT("Transition To PROCESS CMD");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE_PROCESS_CMD;
}

void TransitionToSendResp(void) {
	//DBG_PRINT("Transition To SEND RESP");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE_SEND_RESP;
}

void TransitionToSendNack(void) {
	//DBG_PRINT("Transition To SEND NACK");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE_SEND_NACK;
}