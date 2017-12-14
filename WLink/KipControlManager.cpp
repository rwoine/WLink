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
#define KC_MANAGER_SERVER_RESPONSE_OK				200

#define KC_MANAGER_CHECK_DATE_POLLING_TIME_MS		10000
#define KC_MANAGER_SERVER_RESPONSE_TIMEOUT_MS		10000


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum KC_STATE {
	KC_IDLE,
	KC_WAIT_USER,
	KC_GET_CONFIG,
	KC_RECOVER_DATA,
	KC_CONNECTING,
	KC_WAIT_INDICATOR,
	KC_CHECK_WEIGHT,
	KC_SEND_PACKET,
	KC_SERVER_RESPONSE,
	KC_END,
	KC_ERROR
};

static KC_STATE GL_KipControlManager_CurrentState_E = KC_STATE::KC_IDLE;
static boolean GL_KipControlManagerEnabled_B = false;

static KipControl * GL_pKipControl_H;
unsigned char GL_pKCBuffer_UB[128];
unsigned long GL_KipControlManagerAbsoluteTime_UL = 0;

KC_WORKING_DATA_STRUCT GL_WorkingData_X;
unsigned int GL_pReferenceData_UI[KC_MAX_DATA_NB];

typedef struct {
	int DataSize_SI;
	int Response_SI;
	int AccessCounter_SI;
	char pData_UB[256];
} KC_MANAGER_SERVER_PARAM;

KC_MANAGER_SERVER_PARAM GL_ServerParam_X;


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessWaitUser(void);
static void ProcessGetConfig(void);
static void ProcessRecoverData(void);
static void ProcessConnecting(void);
static void ProcessWaitIndicator(void);
static void ProcessCheckWeight(void);
static void ProcessSendPacket(void);
static void ProcessServerResponse(void);
static void ProcessEnd(void);
static void ProcessError(void);

static void TransitionToIdle(void);
static void TransitionToWaitUser(void);
static void TransitionToGetConfig(void);
static void TransitionToRecoverData(void);
static void TransitionToConnecting(void);
static void TransitionToWaitIndicator(void);
static void TransitionToCheckWeight(void);
static void TransitionToSendPacket(void);
static void TransitionToServerResponse(void);
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

    GL_WorkingData_X.IsConfigured_B = false;
    GL_WorkingData_X.IsRunning_B = false;

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "KipControl Manager Initialized");
}

void KipControlManager_Enable() {
    GL_KipControlManagerEnabled_B = true;
}

void KipControlManager_Disable() {
    GL_KipControlManagerEnabled_B = false;
}

void KipControlManager_Process() {

	/* Reset Condition */
	if ((GL_KipControlManager_CurrentState_E != KC_IDLE) && (!(KipControlMedium_IsReady() && GL_KipControlManagerEnabled_B))) {
		TransitionToIdle();
	}

	/* State Machine */
    switch (GL_KipControlManager_CurrentState_E) {
    case KC_IDLE:				ProcessIdle();				break;
	case KC_WAIT_USER:			ProcessWaitUser();			break;
    case KC_GET_CONFIG:			ProcessGetConfig();			break;
	case KC_RECOVER_DATA:		ProcessRecoverData();		break;
    case KC_CONNECTING:			ProcessConnecting();        break;
    case KC_WAIT_INDICATOR:     ProcessWaitIndicator();		break;
    case KC_CHECK_WEIGHT:       ProcessCheckWeight();       break;
	case KC_SEND_PACKET:		ProcessSendPacket();		break;
	case KC_SERVER_RESPONSE:	ProcessServerResponse();	break;
	case KC_END:				ProcessEnd();				break;
	case KC_ERROR:				ProcessError();				break;
    }
}

boolean KipControlManager_IsEnabled() {
    return (GL_KipControlManagerEnabled_B);
}

boolean KipControlManager_IsReady() {
	return ((GL_KipControlManager_CurrentState_E != KC_IDLE) ? true : false);
}

boolean KipControlManager_IsWaitingWeight() {
	return ((GL_KipControlManager_CurrentState_E == KC_WAIT_INDICATOR) ? true : false);
}

boolean KipControlManager_IsProcessingWeight() {
	return ((GL_KipControlManager_CurrentState_E == KC_CHECK_WEIGHT) ? true : false);
}

signed int KipControlManager_GetCurrentWeight() {
	return (GL_WorkingData_X.Weight_SI);
}

void KipControlManager_SetConfiguredFlag() {
    GL_WorkingData_X.IsConfigured_B = true;
}

/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
	if (GL_KipControlManagerEnabled_B && KipControlMedium_IsReady()) {
		
		// Set Default Server Param
		GL_ServerParam_X.DataSize_SI = 0;
		GL_ServerParam_X.Response_SI = 0;
		GL_ServerParam_X.AccessCounter_SI = 0;
        		
		TransitionToWaitUser();
	}
}

void ProcessWaitUser(void) {
    if (GL_WorkingData_X.IsConfigured_B) {
        TransitionToGetConfig();
    }
}

void ProcessGetConfig(void) {

	unsigned int EepromReferenceDataAddr_UW = 0;

	// Get MAC Address
	GL_WorkingData_X.MacAddr_Str = HexArrayToString(GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB, sizeof(GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB), ":");
	DBG_PRINT(DEBUG_SEVERITY_INFO, "MAC Address for unique identification : ");
	DBG_PRINTDATA(GL_WorkingData_X.MacAddr_Str);
	DBG_ENDSTR();

	// Setup offline mode
	GL_WorkingData_X.OfflineMode_B = false;

	// Get fixed configuration (default or from LCD) -> stored in EEPROM
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get configuration stored in EEPROM");

	GL_WorkingData_X.ReferenceDataId_UB = GL_pKipControl_H->getReferenceDataId();		// Reference data of the current table
	GL_WorkingData_X.BatchId_UL = GL_pKipControl_H->getBatchId();						// Identifier of the current batch
	GL_WorkingData_X.Tolerance_UB = GL_pKipControl_H->getTolerance();					// Tolerance [%] regarding the reference weight to calculate the average
	GL_WorkingData_X.WeightMin_SI = GL_pKipControl_H->getWeightMin();					// Weight not taken into account below this value
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
	DBG_PRINT(DEBUG_SEVERITY_INFO, "- Weight Minimum = ");
	DBG_PRINTDATA(GL_WorkingData_X.WeightMin_SI);
	DBG_PRINTDATA(" [g]");
	DBG_ENDSTR();
	DBG_PRINT(DEBUG_SEVERITY_INFO, "- Start Index = ");
	DBG_PRINTDATA(GL_WorkingData_X.StartIdx_UB);
	DBG_ENDSTR();
	DBG_PRINT(DEBUG_SEVERITY_INFO, "- Start Date = ");
	DBG_PRINTDATA(dateToString(GL_WorkingData_X.StartDate_X));
	DBG_ENDSTR();


	// Build Reference Data Address
	EepromReferenceDataAddr_UW = KC_REFERENCE_TABLE_START_ADDR + ((GL_WorkingData_X.ReferenceDataId_UB - 1) * KC_REFERENCE_TABLE_OFFSET);
	DBG_PRINT(DEBUG_SEVERITY_INFO, "Build address for Reference Data Table = 0x");
	DBG_PRINTDATABASE(EepromReferenceDataAddr_UW, HEX);
	DBG_ENDSTR();

	// Get Reference Data table
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get Reference Data table");
	if (GL_GlobalData_X.Eeprom_H.read(EepromReferenceDataAddr_UW, GL_pKCBuffer_UB, 2) == 2) {

		if (GL_pKCBuffer_UB[0] == GL_WorkingData_X.ReferenceDataId_UB) {
			DBG_PRINT(DEBUG_SEVERITY_INFO, "Reference table found, ID = ");
			DBG_PRINTDATA(GL_WorkingData_X.ReferenceDataId_UB);
			DBG_ENDSTR();

			GL_WorkingData_X.MaxDataNb_UB = GL_pKCBuffer_UB[1];
			DBG_PRINT(DEBUG_SEVERITY_INFO, "Number of reference data = ");
			DBG_PRINTDATA(GL_WorkingData_X.MaxDataNb_UB);
			DBG_ENDSTR();

			if (GL_WorkingData_X.MaxDataNb_UB > KC_MAX_DATA_NB) {
				DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Total number of data exceeds the maximum allowed -> value cropped");
				GL_WorkingData_X.ReferenceDataId_UB = KC_MAX_DATA_NB;
				DBG_PRINT(DEBUG_SEVERITY_WARNING, "KipControl handles only ");
				DBG_PRINTDATA(GL_WorkingData_X.ReferenceDataId_UB);
				DBG_PRINTDATA("reference values");
				DBG_ENDSTR();
			}

			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Print out Reference Data table : ");
			GL_GlobalData_X.Eeprom_H.read(KC_REFERENCE_TABLE_START_ADDR + ((GL_WorkingData_X.ReferenceDataId_UB - 1) * KC_REFERENCE_TABLE_OFFSET) + 2, GL_pKCBuffer_UB, 2 * GL_WorkingData_X.MaxDataNb_UB);
			for (int i = 0; i < GL_WorkingData_X.MaxDataNb_UB; i++) {
				GL_pReferenceData_UI[i] = (GL_pKCBuffer_UB[2 * i] << 8) + (GL_pKCBuffer_UB[2 * i + 1]);
				DBG_PRINT(DEBUG_SEVERITY_INFO, "- ");
				DBG_PRINTDATABASE(i, DEC);
				DBG_PRINTDATA(" = ");
				DBG_PRINTDATABASE(GL_pReferenceData_UI[i], DEC);
				DBG_ENDSTR();
			}


			// Transition to get other data
			TransitionToRecoverData();
		}
		else {
			DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Reference Data table NOT found !");
			DBG_PRINT(DEBUG_SEVERITY_INFO, "Wrong Batch ID found : 0x");
			DBG_PRINTDATABASE(GL_pKCBuffer_UB[0], HEX);
			DBG_ENDSTR();
			TransitionToError();
		}

	}
	else {
		DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Can NOT read Reference Data table !");
		TransitionToError();
	}

}

void ProcessRecoverData(void) {
	unsigned char TempIndex_UB = 0;
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

		GL_WorkingData_X.CurrentDate_X = GL_GlobalData_X.Rtc_H.getDate();

		DBG_PRINT(DEBUG_SEVERITY_INFO, "- Current Date = ");
		DBG_PRINTDATA(dateToString(GL_WorkingData_X.CurrentDate_X));
		DBG_ENDSTR();

		// Check if current index is still up-to-date
		TempIndex_UB = getDeltaDay(GL_WorkingData_X.StartDate_X, GL_WorkingData_X.CurrentDate_X) + GL_WorkingData_X.StartIdx_UB;
		if (TempIndex_UB == GL_WorkingData_X.CurrentIdx_UB) {
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Current Index is still up-to-date -> Work with it");

			if (GL_WorkingData_X.ValueNb_UL != 0) {
				DBG_PRINT(DEBUG_SEVERITY_INFO, "Average of the Day = ");
				DBG_PRINTDATA((GL_WorkingData_X.TotalValue_UL) / (GL_WorkingData_X.ValueNb_UL));
				DBG_PRINTDATA(" [g]");
				DBG_ENDSTR();
			}
		}
		else {
			DBG_PRINT(DEBUG_SEVERITY_WARNING, "Current Index no more up-to-date (new index = ");
			DBG_PRINTDATA(TempIndex_UB);
			DBG_PRINTDATA(") -> reset recovered data");
			DBG_ENDSTR();
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
	if (KipControlMedium_IsConnected()) {
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Turn to online mode -> recording to portal allowed");
		GL_WorkingData_X.OfflineMode_B = false;

		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Setup connectivity environment");
		KipControlMedium_SetupEnvironment();
	}
	else {
		DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Cannot connect to portal");
		DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Turn to offline mode -> recording to portal disabled");
		GL_WorkingData_X.OfflineMode_B = true;
	}

	// Flush Indicator Serial and FIFO
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Flush Indicator Serial and FIFO");
	GL_GlobalData_X.Indicator_H.flushIndicator();
	while (!(GL_GlobalData_X.Indicator_H.isFifoEmpty()))
		GL_GlobalData_X.Indicator_H.fifoPop();

	// Go to Wait Indicator (real process)
	TransitionToWaitIndicator();
}

void ProcessWaitIndicator(void) {

	// Wait for new Weight
	if (!(GL_GlobalData_X.Indicator_H.isFifoEmpty())) {

		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get Weight from Indicator");
		GL_WorkingData_X.Weight_SI = GL_GlobalData_X.Indicator_H.fifoPop();

		GL_WorkingData_X.TimeStamp_Str = GL_GlobalData_X.Rtc_H.getTimestamp();
		GL_WorkingData_X.CurrentDate_X = GL_GlobalData_X.Rtc_H.getLastDate();

		GL_WorkingData_X.CurrentIdx_UB = getDeltaDay(GL_WorkingData_X.StartDate_X, GL_WorkingData_X.CurrentDate_X) + GL_WorkingData_X.StartIdx_UB;
		if (GL_WorkingData_X.CurrentIdx_UB != GL_pKipControl_H->getCurrentIdx()) {
			DBG_PRINT(DEBUG_SEVERITY_INFO, "Current index has changed, save new value : ");
			DBG_PRINTDATA(GL_WorkingData_X.CurrentIdx_UB);
			DBG_ENDSTR();
			GL_pKipControl_H->setCurrentIdx(GL_WorkingData_X.CurrentIdx_UB);

			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Reset working data (average calculation)");
			GL_pKipControl_H->setTotalValue(0);
			GL_pKipControl_H->setValueNb(0);

			if (GL_WorkingData_X.CurrentIdx_UB >= GL_WorkingData_X.MaxDataNb_UB) {
				// End of recording
				DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Reached end of Reference Data table");
				DBG_PRINTLN(DEBUG_SEVERITY_INFO, "-> End of recording");
				TransitionToEnd();
			}
			else {
				// Check weight according to Reference Data
				TransitionToCheckWeight();
			}
		}
		else {
			// Check weight according to Reference Data
			TransitionToCheckWeight();
		}

	}

	// Check if reached the end of recording
	if ((millis() - GL_KipControlManagerAbsoluteTime_UL) >= KC_MANAGER_CHECK_DATE_POLLING_TIME_MS) {
		GL_KipControlManagerAbsoluteTime_UL = millis();

		GL_WorkingData_X.CurrentDate_X = GL_GlobalData_X.Rtc_H.getDate();
		GL_WorkingData_X.CurrentIdx_UB = getDeltaDay(GL_WorkingData_X.StartDate_X, GL_WorkingData_X.CurrentDate_X) + GL_WorkingData_X.StartIdx_UB;

		if (GL_WorkingData_X.CurrentIdx_UB >= GL_WorkingData_X.MaxDataNb_UB) {
			// End of recording
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Reached end of Reference Data table");
			DBG_PRINT(DEBUG_SEVERITY_INFO, "Calculated Current Index = ");
			DBG_PRINTDATA(GL_WorkingData_X.CurrentIdx_UB);
			DBG_ENDSTR();
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "-> End of recording");
			TransitionToEnd();
		}
	}
}

void ProcessCheckWeight(void) {

	/* Check if Weight is above limit */
	if (GL_WorkingData_X.Weight_SI >= GL_WorkingData_X.WeightMin_SI) {

		/* Get Reference Data according to current day */
		DBG_PRINT(DEBUG_SEVERITY_INFO, "Day #");
		DBG_PRINTDATA(GL_WorkingData_X.CurrentIdx_UB + 1);	// Day starting at 1 -> index starting at 0
		DBG_PRINTDATA(" -> ");
		DBG_PRINTDATA("Reference Weight is ");
		DBG_PRINTDATA(GL_pReferenceData_UI[GL_WorkingData_X.CurrentIdx_UB]);
		DBG_PRINTDATA("[g]");
		DBG_ENDSTR();

		/* Check if Weight is in Tolerance */
		float LimitHigh_F = (float)GL_pReferenceData_UI[GL_WorkingData_X.CurrentIdx_UB] * (1 + (float)GL_WorkingData_X.Tolerance_UB / 100);
		float LimitLow_F = (float)GL_pReferenceData_UI[GL_WorkingData_X.CurrentIdx_UB] * (1 - (float)GL_WorkingData_X.Tolerance_UB / 100);
		DBG_PRINT(DEBUG_SEVERITY_INFO, "Weight should be within [");
		DBG_PRINTDATA(LimitHigh_F);
		DBG_PRINTDATA(":");
		DBG_PRINTDATA(LimitLow_F);
		DBG_PRINTDATA("]");
		DBG_ENDSTR();

		if ((GL_WorkingData_X.Weight_SI <= LimitHigh_F) && (GL_WorkingData_X.Weight_SI >= LimitLow_F)) {
			GL_WorkingData_X.IsValid_B = true;
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Weight within boundaries -> taken into account to calculate average.");
			GL_pKipControl_H->appendTotalValue(GL_WorkingData_X.Weight_SI);
			GL_pKipControl_H->incValueNb();

			GL_WorkingData_X.TotalValue_UL = GL_pKipControl_H->getTotalValue();
			GL_WorkingData_X.ValueNb_UL = GL_pKipControl_H->getValueNb();

			DBG_PRINT(DEBUG_SEVERITY_INFO, "Average of the Day = ");
			DBG_PRINTDATA((GL_WorkingData_X.TotalValue_UL) / (GL_WorkingData_X.ValueNb_UL));
			DBG_PRINTDATA(" [g]");
			DBG_ENDSTR();

		}
		else {
			GL_WorkingData_X.IsValid_B = false;
			DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Weight outside boundaries -> ignored!");
		}


		//  Manage Transition
		if (GL_WorkingData_X.OfflineMode_B) {
			// Wait or new Weight
			TransitionToWaitIndicator();
		}
		else {
			// Send all data to portal
			TransitionToSendPacket();
		}

	} /* Discard Weight */
	else {	
		DBG_PRINT(DEBUG_SEVERITY_INFO, "Weight equals ");
		DBG_PRINTDATA(GL_WorkingData_X.Weight_SI);
		DBG_PRINTDATA("[g] and it is under ");
		DBG_PRINTDATA(GL_WorkingData_X.WeightMin_SI);
		DBG_PRINTDATA("[g] -> do not process !");
		DBG_ENDSTR();

		TransitionToWaitIndicator();
	}
}


void ProcessSendPacket(void) {

	KipControlMedium_BeginTransaction();
	KipControlMedium_Print("/kipcontrol/import?");
	KipControlMedium_Print("data[0][Weight]=");
	KipControlMedium_Print(GL_WorkingData_X.Weight_SI);
	KipControlMedium_Print("&data[0][BalanceSerial]=");
	KipControlMedium_Print((char *)(GL_WorkingData_X.MacAddr_Str).c_str());
	KipControlMedium_Print("&data[0][Batch]=");
	KipControlMedium_Print(GL_WorkingData_X.BatchId_UL);
	KipControlMedium_Print("&data[0][Tolerance]=");
	KipControlMedium_Print(GL_WorkingData_X.Tolerance_UB);
	KipControlMedium_Print("&data[0][Age]=");
	KipControlMedium_Print(GL_WorkingData_X.CurrentIdx_UB + 1);
	KipControlMedium_Print("&data[0][DateTime]=");
	KipControlMedium_Print((char *)(GL_WorkingData_X.TimeStamp_Str).c_str());
	KipControlMedium_Print("&data[0][Valid]=");
	KipControlMedium_Print(((GL_WorkingData_X.IsValid_B) ? 1 : 0));
	KipControlMedium_EndTransaction();

	TransitionToServerResponse();
}

void ProcessServerResponse(void) {

	boolean AnotherTryNeeded_B = false;

	// Check Data
	if (KipControlMedium_DataAvailable()) {

		GL_ServerParam_X.Response_SI = KipControlMedium_GetServerResponse();

		if (GL_ServerParam_X.Response_SI == KC_MANAGER_SERVER_RESPONSE_OK) {
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Response OK from Server!");

			GL_ServerParam_X.DataSize_SI = KipControlMedium_GetDataSize();
			KipControlMedium_Read(GL_ServerParam_X.pData_UB);

			AnotherTryNeeded_B = false;
		}
		else {
			DBG_PRINT(DEBUG_SEVERITY_WARNING, "The Server did not respond OK -> Response = ");
			DBG_PRINTDATA(GL_ServerParam_X.Response_SI);
			DBG_ENDSTR();

			AnotherTryNeeded_B = true;
		}
	}

	// Check Timeout
	if ((millis() - GL_KipControlManagerAbsoluteTime_UL) >= KC_MANAGER_SERVER_RESPONSE_TIMEOUT_MS) {
		DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Timeout while accessing to portal");
		AnotherTryNeeded_B = true;
	}


	// Flush Medium for clean-up
	KipControlMedium_Flush();

	// Check what to do
	if (!AnotherTryNeeded_B) {
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Continue processing");

		// Reset Access Counter
		GL_ServerParam_X.AccessCounter_SI = 0;

		// Wait for new Weight
		TransitionToWaitIndicator();

	} else if (GL_ServerParam_X.AccessCounter_SI >= 2) {
		DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Maximum number of try reached -> Value not sent to portal..");

		// Reset Access Counter
		GL_ServerParam_X.AccessCounter_SI = 0;

		// Wait for new Weight
		TransitionToWaitIndicator();
	}
	else {
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Try to send value again..");

		// Increment Access Counter
		GL_ServerParam_X.AccessCounter_SI++;

		// Try to re-send the data
		TransitionToSendPacket();
	}

}


void ProcessEnd(void) {
	// End of recording
	// TODO :	reset param
	//			re-start recording ?
}

void ProcessError(void) {
	// Wait for further config or reset..
}


void TransitionToIdle(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_IDLE;
}

void TransitionToWaitUser(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT USER");
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_WAIT_USER;
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
	KipControlMedium_Connect();
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_CONNECTING;
}

void TransitionToWaitIndicator(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT INDICATOR");
	GL_KipControlManagerAbsoluteTime_UL = millis();
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_WAIT_INDICATOR;
}

void TransitionToCheckWeight(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CHECK WEIGHT");
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_CHECK_WEIGHT;
}

void TransitionToSendPacket(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To SEND PACKET");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_SEND_PACKET;
}

void TransitionToServerResponse(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To SERVER RESPONSE");
	GL_KipControlManagerAbsoluteTime_UL = millis();
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_SERVER_RESPONSE;
}

void TransitionToEnd(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To END");
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Reset running flag");
	GL_pKipControl_H->setRunningFlag(false);
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_END;
}

void TransitionToError(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To ERROR");
	GL_KipControlManager_CurrentState_E = KC_STATE::KC_ERROR;
}
