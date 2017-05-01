/* ******************************************************************************** */
/*                                                                                  */
/* IndicatorManager.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the Indicator object					*/
/*                                                                                  */
/* History :  	02/06/2015  (RW)	Creation of this file                           */
/*				08/06/2016  (RW)	Re-mastered version								*/
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"IndicatorManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "IndicatorManager.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum INDICATOR_MANAGER_STATE {
	INDICATOR_MANAGER_IDLE,
    INDICATOR_MANAGER_WAIT_INTERRUPT,
	INDICATOR_MANAGER_WAIT_SCAN_PERIOD,
	INDICATOR_MANAGER_WAIT_RESPONSE_DELAY,
	INDICATOR_MANAGER_WAIT_RESET_DELAY
} ;

static INDICATOR_MANAGER_STATE GL_IndicatorManager_CurrentState_E = INDICATOR_MANAGER_STATE::INDICATOR_MANAGER_IDLE;

static Indicator * GL_pIndicator_H;


typedef struct {
	boolean IsEnabled_B;
    boolean HasInterrupt_B;
	boolean SetToZero_B;
	unsigned long Timer_UL;
	unsigned long ScanPeriod_UL;
	unsigned long ResponseDelay_UL;
	unsigned long ResetDelay_UL;
	unsigned char MaxTryNumber_UB;
	unsigned char TryNumber_UB;
	INDICATOR_INTERFACE_FRAME_ENUM FrameType_E;
} INDICATOR_MANAGER_PARAM;

static INDICATOR_MANAGER_PARAM GL_IndicatorManagerParam_X;

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessWaitInterrupt(void);
static void ProcessWaitScanPeriod(void);
static void ProcessWaitResponseDelay(void);
static void ProcessWaitResetDelay(void);

static void TransitionToIdle(void);
static void TransitionToWaitInterrupt(void);
static void TransitionToWaitScanPeriod(void);
static void TransitionToWaitResponseDelay(void);
static void TransitionToWaitResetDelay(void);

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void IndicatorManager_Init(Indicator * pIndicator_H) {
	GL_IndicatorManager_CurrentState_E = INDICATOR_MANAGER_STATE::INDICATOR_MANAGER_IDLE;
	GL_pIndicator_H = pIndicator_H;
	GL_IndicatorManagerParam_X.IsEnabled_B = false;
    GL_IndicatorManagerParam_X.HasInterrupt_B = false;
	GL_IndicatorManagerParam_X.SetToZero_B = false;
	GL_IndicatorManagerParam_X.ScanPeriod_UL = INDICATOR_MANAGER_DEFAULT_SCAN_PERIOD;		// TODO : Add function to make it programmable
	GL_IndicatorManagerParam_X.ResponseDelay_UL = INDICATOR_MANAGER_DEFAULT_RESPONSE_DELAY;	// TODO : Add function to make it programmable
	GL_IndicatorManagerParam_X.ResetDelay_UL = INDICATOR_MANAGER_DEFAULT_RESET_DELAY;		// TODO : Add function to make it programmable
	GL_IndicatorManagerParam_X.TryNumber_UB = 0;
	GL_IndicatorManagerParam_X.MaxTryNumber_UB = INDICATOR_MANAGER_DEFAULT_MAX_TRY_NUMBER;	// TODO : Add function to make it programmable
	GL_IndicatorManagerParam_X.FrameType_E = INDICATOR_INTERFACE_FRAME_ASK_WEIGHT;			// TODO : Add function to make it programmable
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Indicator Manager Initialized");
}

void IndicatorManager_Enable(INDICATOR_INTERFACE_FRAME_ENUM FrameType_E, boolean HasInterrupt_B) {
	GL_IndicatorManagerParam_X.FrameType_E = FrameType_E;
	GL_IndicatorManagerParam_X.IsEnabled_B = true;
    GL_IndicatorManagerParam_X.HasInterrupt_B = HasInterrupt_B;
}

void IndicatorManager_Disable() {
	GL_IndicatorManagerParam_X.IsEnabled_B = false;
}

void IndicatorManager_SetZeroIndicator() {
	GL_IndicatorManagerParam_X.SetToZero_B = true;
}

void IndicatorManager_Process() {
	switch (GL_IndicatorManager_CurrentState_E) {
	case INDICATOR_MANAGER_IDLE:
		ProcessIdle();
		break;

    case INDICATOR_MANAGER_WAIT_INTERRUPT:
        ProcessWaitInterrupt();
        break;

	case INDICATOR_MANAGER_WAIT_SCAN_PERIOD:
		ProcessWaitScanPeriod();
		break;

	case INDICATOR_MANAGER_WAIT_RESPONSE_DELAY:
		ProcessWaitResponseDelay();
		break;

	case INDICATOR_MANAGER_WAIT_RESET_DELAY:
		ProcessWaitResetDelay();
		break;
	}
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
    if (GL_pIndicator_H->isInitialized() && GL_IndicatorManagerParam_X.IsEnabled_B) {
        if (GL_IndicatorManagerParam_X.HasInterrupt_B)
            TransitionToWaitInterrupt();
        else
            TransitionToWaitResponseDelay();
    }
}

void ProcessWaitInterrupt(void) {
    if (GL_IndicatorManagerParam_X.IsEnabled_B) {
        // Check if interrupt is received
        if (GL_pIndicator_H->isInterruptReceived()) {
            // Get frame
            if (GL_pIndicator_H->isResponseAvailable(GL_IndicatorManagerParam_X.FrameType_E)) {
                GL_pIndicator_H->processFrame(GL_IndicatorManagerParam_X.FrameType_E);
            }
            // Reset interrupt flag
            GL_pIndicator_H->resetIrq();
        }
    }
    else {
        TransitionToIdle();
    }
}

void ProcessWaitScanPeriod(void) {
	if (GL_IndicatorManagerParam_X.IsEnabled_B) {
		if ((millis() - GL_IndicatorManagerParam_X.Timer_UL) >= GL_IndicatorManagerParam_X.ScanPeriod_UL)
			TransitionToWaitResponseDelay();
		else if (GL_IndicatorManagerParam_X.SetToZero_B)
			TransitionToWaitResetDelay();
	} else {
		TransitionToIdle();
	}
}

void ProcessWaitResponseDelay(void) {
	boolean ResponseDelayCondition_B = false;

	if ((millis() - GL_IndicatorManagerParam_X.Timer_UL) >= (GL_IndicatorManagerParam_X.ResponseDelay_UL * (GL_IndicatorManagerParam_X.TryNumber_UB + 1))) {
		ResponseDelayCondition_B = true;
		DBG_PRINT(DEBUG_SEVERITY_INFO, "Response Delay with Try Number = ");
		DBG_PRINTDATA((GL_IndicatorManagerParam_X.TryNumber_UB+1)); // add 1 because start from 0
		DBG_PRINTDATA(" - Delay = ");
		DBG_PRINTDATA((millis() - GL_IndicatorManagerParam_X.Timer_UL));
		DBG_PRINTDATA("[ms]");
		DBG_ENDSTR();
	}

	if (ResponseDelayCondition_B) {
		if (GL_pIndicator_H->isResponseAvailable(GL_IndicatorManagerParam_X.FrameType_E)) {
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Weight Available");
			GL_pIndicator_H->processFrame(GL_IndicatorManagerParam_X.FrameType_E);
			TransitionToWaitScanPeriod();
		} else {
			if (GL_IndicatorManagerParam_X.TryNumber_UB >= (GL_IndicatorManagerParam_X.MaxTryNumber_UB - 1)) {
				DBG_PRINT(DEBUG_SEVERITY_WARNING, "Max Try Number Reached [");
				DBG_PRINTDATA(GL_IndicatorManagerParam_X.MaxTryNumber_UB);
				DBG_PRINTDATA("] -> Flush Indicator");
				DBG_ENDSTR();
				GL_pIndicator_H->flushIndicator();
				TransitionToWaitScanPeriod();
			} else {
				DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Ask Again");
				GL_pIndicator_H->sendFrame(GL_IndicatorManagerParam_X.FrameType_E);
				GL_IndicatorManagerParam_X.TryNumber_UB++;
			}
		}
	}
}

void ProcessWaitResetDelay(void) {
	if ((millis() - GL_IndicatorManagerParam_X.Timer_UL) >= GL_IndicatorManagerParam_X.ResetDelay_UL) {
		GL_IndicatorManagerParam_X.SetToZero_B = false;
		TransitionToWaitScanPeriod();
	}
}


void TransitionToIdle(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
	GL_IndicatorManager_CurrentState_E = INDICATOR_MANAGER_STATE::INDICATOR_MANAGER_IDLE;
}

void TransitionToWaitInterrupt(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT INTERRUPT");
    GL_IndicatorManager_CurrentState_E = INDICATOR_MANAGER_STATE::INDICATOR_MANAGER_WAIT_INTERRUPT;
}

static void TransitionToWaitScanPeriod(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT SCAN PERIOD");
	GL_IndicatorManager_CurrentState_E = INDICATOR_MANAGER_STATE::INDICATOR_MANAGER_WAIT_SCAN_PERIOD;
}

static void TransitionToWaitResponseDelay(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT RESPONSE DELAY");
	GL_IndicatorManagerParam_X.Timer_UL = millis();
	GL_IndicatorManagerParam_X.TryNumber_UB = 0;
	GL_pIndicator_H->sendFrame(GL_IndicatorManagerParam_X.FrameType_E);
	GL_IndicatorManager_CurrentState_E = INDICATOR_MANAGER_STATE::INDICATOR_MANAGER_WAIT_RESPONSE_DELAY;
}

static void TransitionToWaitResetDelay(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT RESET DELAY");
	GL_pIndicator_H->sendFrame(INDICATOR_INTERFACE_FRAME_SET_ZERO);
	GL_IndicatorManager_CurrentState_E = INDICATOR_MANAGER_STATE::INDICATOR_MANAGER_WAIT_RESET_DELAY;
}
