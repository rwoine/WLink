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

#include "WCommand.h"
#include "WCommandMedium.h"
#include "WCommandInterpreter.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum WCMD_INTERPRETER_STATE {
	WCMD_INTERPRETER_STATE_IDLE,
	WCMD_INTERPRETER_STATE_CHECK_CMD,
	WCMD_INTERPRETER_STATE_PROCESS_CMD,
	WCMD_INTERPRETER_STATE_SEND_RESP,
	WCMD_INTERPRETER_STATE_SEND_NACK
};

static WCMD_INTERPRETER_STATE GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_IDLE;

static const WCMD_FCT_DESCR * GL_pWCmdFctDescr_X;
static unsigned long GL_WCmdFctNb_UL;


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

void WCommandInterpreter_Init(const WCMD_FCT_DESCR *pFctDescr_X, unsigned long NbFct_UL) {
	GL_pWCmdFctDescr_X = pFctDescr_X;
	GL_WCmdFctNb_UL = NbFct_UL;
}

void WCommandInterpreter_Process() {
	switch (GL_CurrentState_E) {
		case WCMD_INTERPRETER_STATE_IDLE :
			ProcessIdle();
			break;

		case WCMD_INTERPRETER_STATE_CHECK_CMD :
			ProcessCheckCmd();
			break;

		case WCMD_INTERPRETER_STATE_PROCESS_CMD :
			ProcessProcessCmd();
			break;

		case WCMD_INTERPRETER_STATE_SEND_RESP :
			ProcessSendResp();
			break;

		case WCMD_INTERPRETER_STATE_SEND_NACK :
			ProcessSendNack();
			break;
	}
}

void WCommandInterpreter_Restart() {
	TransitionToIdle();
}

/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
	if (WCmdMedium_IsConnected)
		TransitionToCheckCmd();
}

void ProcessCheckCmd(void) {

}

void ProcessProcessCmd(void) {

	int i = 0;
	WCMD_FCT_STS FctSts_E = WCMD_FCT_STS_ERROR;

	// Look for the command ID in the table of fonction handler
	for (i = 0; i < GL_WCmdFctNb_UL; i++) {
		if (GL_pWCmdFctDescr_X[i].CmdID_UB == 0x00) {	// TODO : Compare with gathered ID instead of 0x00
			//FctSts_E = GL_pWCmdFctDescr_X[i].FctHandler();
		}
		break;
	}

	// Check if the ID has been found and call the appropriate function
	if (i == GL_WCmdFctNb_UL) {
		FctSts_E = WCMD_FCT_STS_UNKNOWN;
		TransitionToSendNack();
	}
	else {
		TransitionToSendResp();
	}
}

void ProcessSendResp(void) {


	WCmdMedium_Flush();
	WCmdMedium_Stop();
	TransitionToIdle();
}

void ProcessSendNack(void) {

	WCmdMedium_Flush();
	WCmdMedium_Stop();
	TransitionToIdle();
}


void TransitionToIdle(void) {
	//DBG_PRINT("Transition To IDLE");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_IDLE;
}

void TransitionToCheckCmd(void) {
	//DBG_PRINT("Transition To CHECK CMD");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_CHECK_CMD;
}

void TransitionToProcessCmd(void) {
	//DBG_PRINT("Transition To PROCESS CMD");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_PROCESS_CMD;
}

void TransitionToSendResp(void) {
	//DBG_PRINT("Transition To SEND RESP");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_SEND_RESP;
}

void TransitionToSendNack(void) {
	//DBG_PRINT("Transition To SEND NACK");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_SEND_NACK;
}