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
#define FONA_MODULE_MANAGER_WAIT_REACTION_DELAY_MS      2000
#define FONA_MODULE_MANAGER_POWER_PULSE_LENGTH_MS       2000
#define FONA_MODULE_MANAGER_RESET_PULSE_LENGTH_MS       100
#define FONA_MODULE_MANAGER_ENABLE_GPRS_DELAY_MS		20000
#define FONA_MODULE_MANAGER_RSSI_POLLING_INTERVAL_MS    5000


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
	FONA_MODULE_MANAGER_ENABLE_GPRS,
    FONA_MODULE_MANAGER_RUNNING,
    FONA_MODULE_MANAGER_ERROR
};

static FONA_MODULE_MANAGER_STATE GL_FonaModuleManager_CurrentState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_IDLE;
static FONA_MODULE_MANAGER_STATE GL_FonaModuleManager_NextState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_IDLE;

static FonaModule * GL_pFona_H;
static boolean GL_FonaModuleManagerToggle_B = false;
static signed int GL_FonaModuleManagerRssi_SI = 0;
static signed int GL_FonaModuleManagerRssiOld_SI = 0;
static boolean GL_FonaModuleManagerGprsState_B = false;
static boolean GL_FonaModuleManagerGprsStateOld_B = false;
static unsigned int GL_FonaModuleManagerBatteryLeve_UI = 0;
static int GL_NetworkStatus_SI = 0;

static unsigned long GL_FonaManagerPowerSequenceNb_UL = 0;
static unsigned long GL_FonaAbsoluteTime_UL = 0;
static boolean GL_FonaModuleManagerEnabled_B = false;
static boolean GL_FonaModuleManagerEnableGprs_B = false;


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
    GL_pFona_H->begin();
    GL_FonaModuleManager_NextState_E = FONA_MODULE_MANAGER_STATE::FONA_MODULE_MANAGER_UNLOCK_SIM;
    TransitionToWaitReaction();
}

void ProcessUnlockSim(void) {
    if (GL_pFona_H->enterPinCode())
		if (GL_FonaModuleManagerEnableGprs_B )
			TransitionToEnableGprs();
		else
			TransitionToRunning();
    else
        TransitionToError();
}

void ProcessEnableGprs(void) {
	if ((millis() - GL_FonaAbsoluteTime_UL) >= FONA_MODULE_MANAGER_ENABLE_GPRS_DELAY_MS) {
		if (GL_pFona_H->enableGprs())
			TransitionToRunning();
		else
			TransitionToError();
	}
}

void ProcessRunning(void) {
    // Keep running

    // Poll RSSI
    if ((millis() - GL_FonaAbsoluteTime_UL) >= FONA_MODULE_MANAGER_RSSI_POLLING_INTERVAL_MS) {
        
        GL_FonaModuleManagerRssi_SI = GL_pFona_H->getSignalStrength();

        // Print Debug only if signal strength is different
        if (GL_FonaModuleManagerRssiOld_SI != GL_FonaModuleManagerRssi_SI) {
            GL_FonaModuleManagerRssiOld_SI = GL_FonaModuleManagerRssi_SI;
            DBG_PRINT(DEBUG_SEVERITY_INFO, "RSSI Value : ");
            DBG_PRINTDATA(GL_FonaModuleManagerRssi_SI);
            DBG_PRINTDATA("[dBm]");
            DBG_ENDSTR();
        }


        GL_FonaModuleManagerGprsState_B = GL_pFona_H->isGprsEnabled();

        // Print Debug only if GPRS state has changed
        if (GL_FonaModuleManagerGprsStateOld_B != GL_FonaModuleManagerGprsState_B) {
            GL_FonaModuleManagerGprsStateOld_B = GL_FonaModuleManagerGprsState_B;
            if (GL_FonaModuleManagerGprsState_B)
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "GPRS Enabled");
            else
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "GPRS Disabled");
        }

        if (GL_FonaModuleManagerToggle_B) {
            GL_FonaModuleManagerToggle_B = false;
            GL_FonaModuleManagerBatteryLeve_UI = GL_pFona_H->getBatteryLevel();
            DBG_PRINT(DEBUG_SEVERITY_INFO, "Battery Capacity : ");
            DBG_PRINTDATA(GL_FonaModuleManagerBatteryLeve_UI);
            DBG_PRINTDATA("[%]");
            DBG_ENDSTR();
        }
        else {
            GL_FonaModuleManagerToggle_B = true;
            GL_pFona_H->getNetworkStatus(&GL_NetworkStatus_SI);
        }



        GL_FonaAbsoluteTime_UL = millis();
    }
}

void ProcessError(void) {
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

void TransitionToEnableGprs(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To ENABLE GPRS");
	GL_FonaAbsoluteTime_UL = millis();
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
