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

#define MODULE_NAME		"WCommandInterpreter"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WCommand.h"
#include "WCommandMedium.h"
#include "WCommandInterpreter.h"

#include "Debug.h"

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

typedef struct {
	unsigned char CmdId_UB;
	boolean HasParam_B;
	unsigned long ParamNb_UL;
	unsigned long AnsNb_UL;
	unsigned char pParamBuffer_UB[WCMD_MAX_PARAM_NB];
	unsigned char pAnsBuffer_UB[WCMD_MAX_ANS_NB];
} WCMD_PARAM_STRUCT;

static WCMD_PARAM_STRUCT GL_WCmdParam_X;

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
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "W-Command Interpreter Initialized");
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
	boolean FoundStx_B = false;
	unsigned char Temp_UB = 0x00;

	while (WCmdMedium_DataAvailable()) {
		// Look for the Start Of Transmit byte
		if (WCmdMedium_ReadByte() == WCMD_STX) {
			FoundStx_B = true;
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Found STX");
			break;
		}
	}

	if (FoundStx_B && (WCmdMedium_DataAvailable() > 0)) {

		// Get Command ID and Parameters bit
		Temp_UB = WCmdMedium_ReadByte();
		GL_WCmdParam_X.CmdId_UB = Temp_UB & WCMD_CMD_ID_MASK;
		GL_WCmdParam_X.HasParam_B = ((Temp_UB & WCMD_PARAM_BIT_MASK) == WCMD_PARAM_BIT_MASK) ? true : false;

		DBG_PRINT(DEBUG_SEVERITY_INFO, "Command ID = ");
		DBG_PRINTDATABASE(GL_WCmdParam_X.CmdId_UB, HEX);
		DBG_ENDSTR();
		DBG_PRINT(DEBUG_SEVERITY_INFO, "Has Param ? = ");
		DBG_PRINTDATABASE(GL_WCmdParam_X.HasParam_B, BIN);
		DBG_ENDSTR();

		// Get Parameters if any
		if (GL_WCmdParam_X.HasParam_B) {
			GL_WCmdParam_X.ParamNb_UL = WCmdMedium_ReadByte();

			DBG_PRINT(DEBUG_SEVERITY_INFO, "Param Number = ");
			DBG_PRINTDATA(GL_WCmdParam_X.ParamNb_UL);
			DBG_ENDSTR();

			for (int i = 0; i < GL_WCmdParam_X.ParamNb_UL; i++)
				GL_WCmdParam_X.pParamBuffer_UB[i] = WCmdMedium_ReadByte();
		}

		// Check for End Of Transmit byte
		if (WCmdMedium_ReadByte() == WCMD_ETX) {
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Found ETX");
			TransitionToProcessCmd();
		}
		else {
			DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Do NOT Found ETX");
			TransitionToSendNack();
		}

	}

}

void ProcessProcessCmd(void) {

	int i = 0;
	WCMD_FCT_STS FctSts_E = WCMD_FCT_STS_ERROR;

	// Look for the command ID in the table of fonction handler
	for (i = 0; i < GL_WCmdFctNb_UL; i++) {
		if (GL_pWCmdFctDescr_X[i].CmdID_UB == GL_WCmdParam_X.CmdId_UB) {
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Found Command ID. Call Function...");
			FctSts_E = GL_pWCmdFctDescr_X[i].FctHandler(GL_WCmdParam_X.pParamBuffer_UB, GL_WCmdParam_X.ParamNb_UL, GL_WCmdParam_X.pAnsBuffer_UB, &(GL_WCmdParam_X.AnsNb_UL));
			DBG_PRINT(DEBUG_SEVERITY_INFO, "Status = ");
			DBG_PRINTDATA(FctSts_E);
			DBG_PRINTDATA(" - Number of Bytes in Answer = ");
			DBG_PRINTDATA(GL_WCmdParam_X.AnsNb_UL);
			DBG_ENDSTR();
		}
		break;
	}

	// Check if the ID has been found and call the appropriate function
	if (i == GL_WCmdFctNb_UL) {
		FctSts_E = WCMD_FCT_STS_UNKNOWN;
		DBG_PRINT(DEBUG_SEVERITY_WARNING, "Command ID Unknown [0x");
		DBG_PRINTDATABASE(GL_WCmdParam_X.CmdId_UB, HEX);
		DBG_PRINTDATA("]");
		DBG_ENDSTR();
		TransitionToSendNack();
	}
	else {
		TransitionToSendResp();
	}
}

void ProcessSendResp(void) {
	unsigned char pBuffer_UB[WCMD_MAX_ANS_NB];
	unsigned long Offset_UL = 0;

	// Start Of Transmit
	pBuffer_UB[0] = WCMD_STX;

	// Command ID
	if (GL_WCmdParam_X.AnsNb_UL == 0) {
		pBuffer_UB[1] = GL_WCmdParam_X.CmdId_UB;
		Offset_UL = 2;
	}
	else { // .. and Data
		pBuffer_UB[1] = GL_WCmdParam_X.CmdId_UB | WCMD_PARAM_BIT_MASK;
		pBuffer_UB[2] = (unsigned char)GL_WCmdParam_X.ParamNb_UL;
		for (int i = 0; i < GL_WCmdParam_X.AnsNb_UL; i++)
			pBuffer_UB[3 + i] = GL_WCmdParam_X.pAnsBuffer_UB[i];
		Offset_UL = GL_WCmdParam_X.AnsNb_UL + 3;
	}

	// Acknowledge
	pBuffer_UB[Offset_UL++] = WCMD_ACK;

	// End Of Transmit
	pBuffer_UB[Offset_UL++] = WCMD_ETX;

	// Send Response
	WCmdMedium_Write(pBuffer_UB, Offset_UL);
	delay(1);

	WCmdMedium_Flush();
	WCmdMedium_Stop();
	TransitionToIdle();
}

void ProcessSendNack(void) {
	unsigned char pBuffer_UB[4];
	unsigned long Offset_UL = 0;

	// Start Of Transmit
	pBuffer_UB[0] = WCMD_STX;

	// Command ID
	pBuffer_UB[1] = GL_WCmdParam_X.CmdId_UB;

	// Not Acknowledge
	pBuffer_UB[2] = WCMD_NACK;

	// End Of Transmit
	pBuffer_UB[3] = WCMD_ETX;

	// Send Response
	WCmdMedium_Write(pBuffer_UB, 4);
	delay(1);

	WCmdMedium_Flush();
	WCmdMedium_Stop();
	TransitionToIdle();
}


void TransitionToIdle(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_IDLE;
}

void TransitionToCheckCmd(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CHECK CMD");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_CHECK_CMD;
}

void TransitionToProcessCmd(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To PROCESS CMD");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_PROCESS_CMD;
}

void TransitionToSendResp(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To SEND RESP");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_SEND_RESP;
}

void TransitionToSendNack(void) {
	//DBG_PRINT("Transition To SEND NACK");
	GL_CurrentState_E = WCMD_INTERPRETER_STATE::WCMD_INTERPRETER_STATE_SEND_NACK;
}