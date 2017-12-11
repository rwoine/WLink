/* ******************************************************************************** */
/*                                                                                  */
/* FonaModuleManager.cpp															*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the Fona Module object				*/
/*                                                                                  */
/* History :  	17/07/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"FonaModuleManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "FonaModuleManager.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define FONA_MODULE_MANAGER_WAIT_REACTION_DELAY_MS				2500
#define FONA_MODULE_MANAGER_POWER_PULSE_LENGTH_MS				2000
#define FONA_MODULE_MANAGER_RESET_PULSE_LENGTH_MS				100
#define FONA_MODULE_MANAGER_NETWORK_REGISTRATION_TIMEOUT_MS		20000
#define FONA_MODULE_MANAGER_POLLING_INTERVAL_MS					2000


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum FONA_MODULE_MANAGER_STATE {
    FONA_MODULE_MANAGER_IDLE,
    FONA_MODULE_MANAGER_WAIT_REACTION,
    FONA_MODULE_MANAGER_APPLY_POWER_PULSE,
    FONA_MODULE_MANAGER_APPLY_RESET_PULSE,
    FONA_MODULE_MANAGER_CHECK_POWER_PIN,
    FONA_MODULE_MANAGER_BEGIN,
    FONA_MODULE_MANAGER_UNLOCK_SIM,
	FONA_MODULE_MANAGER_WAIT_NEWORK_REGISTRATION,
	FONA_MODULE_MANAGER_GET_BATTERY_STATE,
	FONA_MODULE_MANAGER_ENABLE_GPRS,
    FONA_MODULE_MANAGER_RUNNING,
    FONA_MODULE_MANAGER_ERROR
};

static FONA_MODULE_MANAGER_STATE GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_IDLE;
static FONA_MODULE_MANAGER_STATE GL_FonaModuleManager_NextState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_IDLE;

static FonaModule * GL_pFona_H;
static signed int GL_FonaModuleManagerRssi_SI = 0;
static boolean GL_FonaModuleManagerGprsState_B = false;
static unsigned int GL_FonaModuleManagerBatteryLeve_UI = 0;
static int GL_NetworkStatus_SI = 0;

static unsigned long GL_FonaManagerPowerSequenceNb_UL = 0;
static unsigned long GL_FonaAbsoluteTime_UL = 0;
static boolean GL_FonaModuleManagerEnabled_B = false;
static boolean GL_FonaModuleManagerEnableGprs_B = false;
static unsigned long GL_FonaModuleManagerPollingIndex_UL = 0;


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessWaitReaction(void);
static void ProcessApplyPowerPulse(void);
static void ProcessApplyResetPulse(void);
static void ProcessCheckPowerPin(void);
static void ProcessBegin(void);
static void ProcessUnlockSim(void);
static void ProcessWaitNetworkRegistration(void);
static void ProcessGetBatteryState(void);
static void ProcessEnableGprs(void);
static void ProcessRunning(void);
static void ProcessError(void);

static void TransitionToIdle(void);
static void TransitionToWaitReaction(void);
static void TransitionToApplyPowerPulse(void);
static void TransitionToApplyResetPulse(void);
static void TransitionToCheckPowerPin(void);
static void TransitionToBegin(void);
static void TransitionToUnlockSim(void);
static void TransitiontToWaitNetworkRegistration(void);
static void TransitionToGetBatteryState(void);
static void TransitionToEnableGprs(void);
static void TransitionToRunning(void);
static void TransitionToError(void);


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void FonaModuleManager_Init(FonaModule * pFona_H, boolean EnableGprs_B) {
    GL_pFona_H = pFona_H;
    GL_FonaModuleManagerEnabled_B = false;
	GL_FonaModuleManagerEnableGprs_B = EnableGprs_B;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "FONA Module Manager Initialized");
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "GPRS enabled in the current configuration");
}

void FonaModuleManager_Enable() {
    GL_FonaModuleManagerEnabled_B = true;
}

void FonaModuleManager_Disable() {
    GL_FonaModuleManagerEnabled_B = false;
}

void FonaModuleManager_Process() {
    /* Reset Condition */
    if (!GL_FonaModuleManagerEnabled_B) {
        TransitionToIdle();
    }

    /* State Machine */
    switch (GL_FonaModuleManager_CurrentState_E) {
    case FONA_MODULE_MANAGER_IDLE:
        ProcessIdle();
        break;

    case FONA_MODULE_MANAGER_WAIT_REACTION:
        ProcessWaitReaction();
        break;

    case FONA_MODULE_MANAGER_APPLY_POWER_PULSE:
        ProcessApplyPowerPulse();
        break;

    case FONA_MODULE_MANAGER_APPLY_RESET_PULSE:
        ProcessApplyResetPulse();
        break;

    case FONA_MODULE_MANAGER_CHECK_POWER_PIN:
        ProcessCheckPowerPin();
        break;

    case FONA_MODULE_MANAGER_BEGIN:
        ProcessBegin();
        break;

    case FONA_MODULE_MANAGER_UNLOCK_SIM:
        ProcessUnlockSim();
        break;

	case FONA_MODULE_MANAGER_WAIT_NEWORK_REGISTRATION:
		ProcessWaitNetworkRegistration();
		break;

	case FONA_MODULE_MANAGER_GET_BATTERY_STATE:
		ProcessGetBatteryState();
		break;

	case FONA_MODULE_MANAGER_ENABLE_GPRS:
		ProcessEnableGprs();
		break;

    case FONA_MODULE_MANAGER_RUNNING:
        ProcessRunning();
        break;

    case FONA_MODULE_MANAGER_ERROR:
        ProcessError();
        break;

    }
}

boolean FonaModuleManager_IsRunning() {
	return ((GL_FonaModuleManager_CurrentState_E == FONA_MODULE_MANAGER_RUNNING) ? true : false);
}


int FonaModuleManager_GetCurrentSignalStrength() {
	return GL_FonaModuleManagerRssi_SI;
}

boolean FonaModuleManager_GetCurrentGprsState() {
    return GL_FonaModuleManagerGprsState_B;
}

unsigned int FonaModuleManager_GetCurrentBatteryLevel() {
	return GL_FonaModuleManagerBatteryLeve_UI;
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
    if (GL_pFona_H->isInitialized()) {
        GL_FonaModuleManager_NextState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_APPLY_POWER_PULSE;
        TransitionToWaitReaction();
    }
}

void ProcessWaitReaction(void) {
    if ((millis() - GL_FonaAbsoluteTime_UL) >= FONA_MODULE_MANAGER_WAIT_REACTION_DELAY_MS) {
        switch (GL_FonaModuleManager_NextState_E) {
        case FONA_MODULE_MANAGER_APPLY_POWER_PULSE: TransitionToApplyPowerPulse();  break;
        case FONA_MODULE_MANAGER_APPLY_RESET_PULSE: TransitionToApplyResetPulse();  break;
        case FONA_MODULE_MANAGER_CHECK_POWER_PIN:   TransitionToCheckPowerPin();    break;
        case FONA_MODULE_MANAGER_BEGIN:             TransitionToBegin();            break;
        case FONA_MODULE_MANAGER_UNLOCK_SIM:        TransitionToUnlockSim();        break;
		case FONA_MODULE_MANAGER_GET_BATTERY_STATE:	TransitionToGetBatteryState();	break;
		case FONA_MODULE_MANAGER_ENABLE_GPRS:		TransitionToEnableGprs();		break;
        case FONA_MODULE_MANAGER_RUNNING:           TransitionToRunning();          break;
        default:                                    TransitionToIdle();             break;
        }
    }
}

void ProcessApplyPowerPulse(void) {
    if ((millis() - GL_FonaAbsoluteTime_UL) >= FONA_MODULE_MANAGER_POWER_PULSE_LENGTH_MS) {
        GL_pFona_H->setPinKey();    // default state for Power Key pin
        GL_FonaModuleManager_NextState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_CHECK_POWER_PIN;
        TransitionToWaitReaction();
    }
}

void ProcessApplyResetPulse(void) {
    if ((millis() - GL_FonaAbsoluteTime_UL) >= FONA_MODULE_MANAGER_RESET_PULSE_LENGTH_MS) {
        GL_pFona_H->setPinRst();    // default state for Reset pin
        GL_FonaModuleManager_NextState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_BEGIN;
        TransitionToWaitReaction();
    }
}

void ProcessCheckPowerPin(void) {
    if (GL_pFona_H->isPowered()) {
        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "FONA Module Powered");
        TransitionToApplyResetPulse();
    }
    else {
        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "FONA Module NOT Powered !");
        if (GL_FonaManagerPowerSequenceNb_UL < 2) {
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Try to power it again..");
            TransitionToApplyPowerPulse();
        }
        else {
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Try to power too many times -> Error (reset needed) !");
            TransitionToError();
        }
    }
}

void ProcessBegin(void) {
	if (GL_pFona_H->begin()) {
		GL_FonaModuleManager_NextState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_UNLOCK_SIM;
		TransitionToWaitReaction();
	}
	else {
		DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Error while trying to establish/begin communication with FONA module !");
		TransitionToError();
	}
}

void ProcessUnlockSim(void) {
	if (GL_pFona_H->enterPinCode())
		TransitiontToWaitNetworkRegistration();
    else
        TransitionToError();
}

void ProcessWaitNetworkRegistration(void) {

	GL_pFona_H->getNetworkStatus(&GL_NetworkStatus_SI);
	if ((GL_NetworkStatus_SI == FONA_MODULE_NETWORK_STATUS_REGISTERED) || (GL_NetworkStatus_SI == FONA_MODULE_NETWORK_STATUS_ROAMING)) {
		GL_FonaModuleManager_NextState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_GET_BATTERY_STATE;
		TransitionToWaitReaction();
	}

	// Timeout on registration
	if ((millis() - GL_FonaAbsoluteTime_UL) >= FONA_MODULE_MANAGER_NETWORK_REGISTRATION_TIMEOUT_MS) {
		DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Fail to register to Network");
		TransitionToError();
	}

}

void ProcessGetBatteryState(void) {
	unsigned int BatteryState_UI = 0;
	BatteryState_UI = GL_pFona_H->getBatteryLevel();

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Battery level = ");
	DBG_PRINTDATA(BatteryState_UI);
	DBG_PRINTDATA(" [%]");
	DBG_ENDSTR();

	if (GL_FonaModuleManagerEnableGprs_B) {
		GL_FonaModuleManager_NextState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_ENABLE_GPRS;
		TransitionToWaitReaction();
	}
	else
		TransitionToRunning();
}

void ProcessEnableGprs(void) {
	if (GL_pFona_H->enableGprs())
		TransitionToRunning();
	else
		TransitionToError();
}

void ProcessRunning(void) {
    // Keep running

    // Poll Statuses
    if ((millis() - GL_FonaAbsoluteTime_UL) >= FONA_MODULE_MANAGER_POLLING_INTERVAL_MS) {

		switch (GL_FonaModuleManagerPollingIndex_UL) {
		case 0:	GL_FonaModuleManagerRssi_SI = GL_pFona_H->getSignalStrength();		break;
		case 1:	GL_FonaModuleManagerGprsState_B = GL_pFona_H->isGprsEnabled();		break;
		//case 2:	GL_FonaModuleManagerBatteryLeve_UI = GL_pFona_H->getBatteryLevel();	break;
		default:	break;
		}

		GL_FonaModuleManagerPollingIndex_UL++;
		GL_FonaModuleManagerPollingIndex_UL = GL_FonaModuleManagerPollingIndex_UL % 3;

        GL_FonaAbsoluteTime_UL = millis();
    }
}

void ProcessError(void) {

	// Reset Data
	GL_FonaModuleManagerRssi_SI = 0;
	GL_FonaModuleManagerGprsState_B = false;
	GL_FonaModuleManagerBatteryLeve_UI = 0;

    // Keep holding
    // > reset needed
}


void TransitionToIdle(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_pFona_H->flush();        // flush Serial communication
    GL_pFona_H->setPinKey();    // default state for Power Key pin
    GL_pFona_H->setPinRst();    // default state for Reset pin
    GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_IDLE;
}

void TransitionToWaitReaction(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT REACTION");
    GL_FonaAbsoluteTime_UL = millis();
    GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_WAIT_REACTION;
}

void TransitionToApplyPowerPulse(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To APPLY POWER PULSE");
    GL_FonaManagerPowerSequenceNb_UL++; // increment the amount of power sequence
    GL_pFona_H->clearPinKey();          // tie low for at least 2[s] to apply power sequence
    GL_FonaAbsoluteTime_UL = millis();
    GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_APPLY_POWER_PULSE;
}

void TransitionToApplyResetPulse(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To APPLY RESET PULSE");
    GL_pFona_H->clearPinRst();          // tie low for at least 100[ms] to apply power sequence
    GL_FonaAbsoluteTime_UL = millis();
    GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_APPLY_RESET_PULSE;
}

void TransitionToCheckPowerPin(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CHECK POWER PIN");
    GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_CHECK_POWER_PIN;
}

void TransitionToBegin(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To BEGIN");
    GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_BEGIN;
}

void TransitionToUnlockSim(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To UNLOCK SIM");
    GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_UNLOCK_SIM;
}

void TransitiontToWaitNetworkRegistration(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT NETWORK REGISTRATION");
	GL_FonaAbsoluteTime_UL = millis();
	GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_WAIT_NEWORK_REGISTRATION;
}

void TransitionToGetBatteryState(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET BATTERY STATE");
	GL_FonaAbsoluteTime_UL = millis();
	GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_GET_BATTERY_STATE;
}

void TransitionToEnableGprs(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To ENABLE GPRS");
	GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_ENABLE_GPRS;
}

void TransitionToRunning(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To RUNNING");
    GL_FonaAbsoluteTime_UL = millis();
    GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_RUNNING;
}

void TransitionToError(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Transition To ERROR");
    GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_ERROR;
}
