/* ******************************************************************************** */
/*                                                                                  */
/* KipControlManager.cpp												            */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the KipControl application   			*/
/*                                                                                  */
/* History :  	17/04/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"KipControlManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "KipControlManager.h"
#include "KipControlMedium.h"
#include "KipControlMenuManager.h"
#include "EepromWire.h"

#include "Utilz.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Extenral Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define KC_REFERENCE_TABLE_START_ADDR   0x0800
#define KC_REFERENCE_TABLE_OFFSET       0x0100

#define KC_MAX_DATA_NB					128

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum KC_STATE {
	KC_IDLE,
	KC_GET_CONFIG,
	KC_RECOVER_DATA,
	KC_CONNECTING,
	KC_WAIT_INDICATOR,
	KC_SEND_PACKET,
	KC_CHECK_WEIGHT,
	KC_END,
	KC_ERROR
};

static KC_STATE GL_KipControlManager_CurrentState_E = KC_STATE::KC_IDLE;
static boolean GL_KipControlManagerEnabled_B = false;

static KipControl * GL_pKipControl_H;

KC_WORKING_DATA_STRUCT GL_WorkingData_X;
unsigned int GL_pReferenceData_UI[KC_MAX_DATA_NB];


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessGetConfig(void);
static void ProcessRecoverData(void);
static void ProcessConnecting(void);
static void ProcessWaitIndicator(void);
static void ProcessSendPacket(void);
static void ProcessCheckWeight(void);
static void ProcessEnd(void);
static void ProcessError(void);

static void TransitionToIdle(void);
static void TransitionToGetConfig(void);
static void TransitionToRecoverData(void);
static void TransitionToConnecting(void);
static void TransitionToWaitIndicator(void);
static void TransitionToSendPacket(void);
static void TransitionToCheckWeight(void);
static void TransitionToEnd(void);
static void TransitionToError(void);

/* ******************************************************************************** */
/* Prototypes for Getters & Setters
/* ******************************************************************************** */


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void KipControlManager_Init(KipControl * pKipControl_H) {
    GL_KipControlManagerEnabled_B = false;
	GL_pKipControl_H = pKipControl_H;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "KipControl Manager Initialized");
}

void KipControlManager_Enable() {
    GL_KipControlManagerEnabled_B = true;
}

void KipControlManager_Disable() {
    GL_KipControlManagerEnabled_B = false;
}

void KipControlManager_Process() {
    switch (GL_KipControlManager_CurrentState_E) {
    case KC_IDLE:
        ProcessIdle();
        break;

    case KC_GET_CONFIG:
        ProcessGetConfig();
        break;

	case KC_RECOVER_DATA:
		ProcessRecoverData();
		break;

    case KC_CONNECTING:
        ProcessConnecting();
        break;

    case KC_WAIT_INDICATOR:
        ProcessWaitIndicator();
        break;

    case KC_SEND_PACKET:
        ProcessSendPacket();
        break;

    case KC_CHECK_WEIGHT:
        ProcessCheckWeight();
        break;

	case KC_END:
		ProcessEnd();
		break;

	case KC_ERROR:
		ProcessError();
		break;
    }
}

boolean KipControlManager_IsEnabled() {
    return (GL_KipControlManagerEnabled_B);
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
    if (GL_KipControlManagerEnabled_B)
        TransitionToGetConfig();
}

void ProcessGetConfig(void) {
	if (GL_pKipControl_H->getConfiguredFlag()) {

		// Get fixed configuration (default or from LCD) -> stored in EEPROM
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get configuration stored in EEPROM");

		GL_WorkingData_X.ReferenceDataId_UB = GL_pKipControl_H->getReferenceDataId();		// Reference data of the current table
		GL_WorkingData_X.BatchId_UL = GL_pKipControl_H->getBatchId();						// Identifier of the current batch
		GL_WorkingData_X.Tolerance_UB = GL_pKipControl_H->getTolerance();					// Tolerance [%] regarding the reference weight to calculate the average
		GL_WorkingData_X.StartIdx_UB = GL_pKipControl_H->getStartIdx();						// Index of the reference table when the recording is started
		GL_WorkingData_X.StartDate_X = GL_pKipControl_H->getStartDate();					// Date of the beginning of the recording

		TransitionToRecoverData();
	}
	else {
		DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Kip Control NOT Configured !");
		TransitionToError();
	}
}

void ProcessRecoverData(void) {
	if (GL_pKipControl_H->getRunningFlag()) {
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Process was running -> Recover all data from last time before shutdown");


	}
	else {
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Process was not running -> Reset working data");

		// TODO : Reset data

		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Set running flag");
		GL_pKipControl_H->setRunningFlag(true);

		TransitionToConnecting();
	}
}

void ProcessConnecting(void) {



}

void ProcessWaitIndicator(void) {



}

void ProcessSendPacket(void) {



}

void ProcessCheckWeight(void) {




}

void ProcessEnd(void) {
	// End of recording
	// TODO :	disable running flag
	//			reset param
	//			re-start recording ?
}

void ProcessError(void) {
	// Wait for further config or reset..
}


void TransitionToIdle(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_IDLE;
}

void TransitionToGetConfig(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET CONFIG");
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_GET_CONFIG;
}

void TransitionToRecoverData(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To RECOVER DATA");
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_RECOVER_DATA;
}

void TransitionToConnecting(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONNECTING");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_CONNECTING;
}

void TransitionToWaitIndicator(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT INDICATOR");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_WAIT_INDICATOR;
}

void TransitionToSendPacket(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To SEND PACKET");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_SEND_PACKET;
}

void TransitionToCheckWeight(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CHECK WEIGHT");
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_CHECK_WEIGHT;
}

void TransitionToEnd(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To END");
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_END;
}

void TransitionToError(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To ERROR");
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_ERROR;
}
