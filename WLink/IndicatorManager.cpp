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
	INDICATOR_MANAGER_WAIT_SCAN_PERIOD,
	INDICATOR_MANAGER_WAIT_RESPONSE_DELAY,
	INDICATOR_MANAGER_WAIT_RESET_DELAY
} ;

static INDICATOR_MANAGER_STATE GL_IndicatorManager_CurrentState_E = INDICATOR_MANAGER_STATE::INDICATOR_MANAGER_IDLE;

static Indicator * GL_pIndicator_H;


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessWaitScanPeriod(void);
static void ProcessWaitResponseDelay(void);
static void ProcessWaitResetDelay(void);

static void TransitionToIdle(void);
static void TransitionToWaitScanPeriod(void);
static void TransitionToWaitResponseDelay(void);
static void TransitionToWaitResetDelay(void);

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void IndicatorManager_Init(Indicator * pIndicator_H) {

}

void IndicatorManager_Enable() {

}

void IndicatorManager_Disable() {

}

void IndicatorManager_SetZeroIndicator() {

}

void IndicatorManager_Process() {

}


void IndicatorManager_Init() {
	switch (GL_IndicatorManager_CurrentState_E) {
	case INDICATOR_MANAGER_IDLE:
		ProcessIdle();
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

}

void ProcessWaitScanPeriod(void) {

}

void ProcessWaitResponseDelay(void) {

}

void ProcessWaitResetDelay(void) {

}


void TransitionToIdle(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");

}

static void TransitionToWaitScanPeriod(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT SCAN PERIOD");

}

static void TransitionToWaitResponseDelay(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT RESPONSE DELAY");

}

static void TransitionToWaitResetDelay(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT RESET DELAY");

}
