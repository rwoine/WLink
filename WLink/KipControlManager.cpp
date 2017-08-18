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
/* External Variables
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

void KipControlManager_Init(void * pHanlde_H) {
    GL_KipControlManagerEnabled_B = false;
	GL_pKipControl_H = (KipControl *)pHanlde_H;
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

		// Get MAC Address
		GL_WorkingData_X.MacAddr_Str = HexArrayToString(GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB, sizeof(GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB), ":");
		DBG_PRINT(DEBUG_SEVERITY_INFO, "MAC Address for unique identification : ");
		DBG_PRINTDATA(GL_WorkingData_X.MacAddr_Str);
		DBG_ENDSTR();

		// Get fixed configuration (default or from LCD) -> stored in EEPROM
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get configuration stored in EEPROM");

		GL_WorkingData_X.ReferenceDataId_UB = GL_pKipControl_H->getReferenceDataId();		// Reference data of the current table
		GL_WorkingData_X.BatchId_UL = GL_pKipControl_H->getBatchId();						// Identifier of the current batch
		GL_WorkingData_X.Tolerance_UB = GL_pKipControl_H->getTolerance();					// Tolerance [%] regarding the reference weight to calculate the average
		GL_WorkingData_X.StartIdx_UB = GL_pKipControl_H->getStartIdx();						// Index of the reference table when the recording is started
		GL_WorkingData_X.StartDate_X = GL_pKipControl_H->getStartDate();					// Date of the beginning of the recording

		DBG_PRINT(DEBUG_SEVERITY_INFO, "- Reference Data ID = ");
		DBG_PRINTDATA(GL_WorkingData_X.ReferenceDataId_UB);
		DBG_ENDSTR();
		DBG_PRINT(DEBUG_SEVERITY_INFO, "- Batch ID = ");
		DBG_PRINTDATA(GL_WorkingData_X.BatchId_UL);
		DBG_ENDSTR();
		DBG_PRINT(DEBUG_SEVERITY_INFO, "- Tolerance = ");
		DBG_PRINTDATA(GL_WorkingData_X.Tolerance_UB);
		DBG_PRINTDATA(" [%]");
		DBG_ENDSTR();
		DBG_PRINT(DEBUG_SEVERITY_INFO, "- Start Index = ");
		DBG_PRINTDATA(GL_WorkingData_X.StartIdx_UB);
		DBG_ENDSTR();
		DBG_PRINT(DEBUG_SEVERITY_INFO, "- Start Date = ");
		DBG_PRINTDATA(dateToString(GL_WorkingData_X.StartDate_X));
		DBG_ENDSTR();

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

		GL_WorkingData_X.CurrentIdx_UB = GL_pKipControl_H->getCurrentIdx();					// Current index in the reference table
		GL_WorkingData_X.TotalValue_UL = GL_pKipControl_H->getTotalValue();					// Addition of all data
		GL_WorkingData_X.ValueNb_UL = GL_pKipControl_H->getValueNb();						// Total number of data

		DBG_PRINT(DEBUG_SEVERITY_INFO, "- Current Index = ");
		DBG_PRINTDATA(GL_WorkingData_X.CurrentIdx_UB);
		DBG_ENDSTR();
		DBG_PRINT(DEBUG_SEVERITY_INFO, "- Total Value = ");
		DBG_PRINTDATA(GL_WorkingData_X.TotalValue_UL);
		DBG_ENDSTR();
		DBG_PRINT(DEBUG_SEVERITY_INFO, "- Number of Value = ");
		DBG_PRINTDATA(GL_WorkingData_X.ValueNb_UL);
		DBG_ENDSTR();

		// Check if current index is still up-to-date
		unsigned char TempIndex_UB = getDeltaDay(GL_GlobalData_X.Rtc_H.getDate(), GL_WorkingData_X.StartDate_X) + GL_WorkingData_X.StartIdx_UB;
		if (TempIndex_UB == GL_WorkingData_X.CurrentIdx_UB) {
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Current Index is still up-to-date -> Work with it");
			DBG_PRINT(DEBUG_SEVERITY_INFO, "Average of the Day = ");
			DBG_PRINTDATA((GL_WorkingData_X.TotalValue_UL) / (GL_WorkingData_X.ValueNb_UL));
			DBG_PRINTDATA(" [g]");
			DBG_ENDSTR();
		}
		else {
			DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Current Index no more up-to-date -> Reset recovered data");
			GL_pKipControl_H->setCurrentIdx(TempIndex_UB);
			GL_pKipControl_H->setTotalValue(0);
			GL_pKipControl_H->setValueNb(0);
		}

	}
	else {
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Process was not running -> Reset working data");

		GL_pKipControl_H->setCurrentIdx(GL_WorkingData_X.StartIdx_UB);
		GL_pKipControl_H->setTotalValue(0);
		GL_pKipControl_H->setValueNb(0);

		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Set running flag");
		GL_pKipControl_H->setRunningFlag(true);		
	}

	TransitionToConnecting();
}

void ProcessConnecting(void) {



}

void ProcessWaitIndicator(void) {
	if (!(GL_GlobalData_X.Indicator_H.isFifoEmpty())) {

		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get Weight from Indicator");
		GL_WorkingData_X.Weight_SI = GL_GlobalData_X.Indicator_H.fifoPop();

		if (GL_WorkingData_X.Weight_SI != 0) {
			GL_WorkingData_X.TimeStamp_Str = GL_GlobalData_X.Rtc_H.getTimestamp();
			GL_WorkingData_X.CurrentDate_X = GL_GlobalData_X.Rtc_H.getLastDate();

			GL_WorkingData_X.CurrentIdx_UB = getDeltaDay(GL_WorkingData_X.StartDate_X, GL_WorkingData_X.CurrentDate_X) + GL_WorkingData_X.StartIdx_UB;
			if (GL_WorkingData_X.CurrentIdx_UB != GL_pKipControl_H->getCurrentIdx()) {
				DBG_PRINT(DEBUG_SEVERITY_INFO, "Current index has changed, save new value : ");
				DBG_PRINTDATA(GL_WorkingData_X.CurrentIdx_UB);
				DBG_ENDSTR();
				GL_pKipControl_H->setCurrentIdx(GL_WorkingData_X.CurrentIdx_UB);
			}

			TransitionToSendPacket();
		}
		else {
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Weight equals 0 -> do not process !");
		}

	}
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
