/* ******************************************************************************** */
/*                                                                                  */
/* FlatPanelManager.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the Flat Panel object					*/
/*                                                                                  */
/* History :  	18/12/2015  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"FlatPanelManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "FlatPanelManager.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum FLAT_PANEL_MANAGER_STATE {
	FLAT_PANEL_MANAGER_IDLE,
	FLAT_PANEL_MANAGER_SCAN_KEY
};

static FLAT_PANEL_MANAGER_STATE GL_FlatPanelManager_CurrentState_E = FLAT_PANEL_MANAGER_STATE::FLAT_PANEL_MANAGER_IDLE;

static FlatPanel * GL_pFlatPanel_H;

typedef struct {
	boolean IsEnabled_B;
} FLAT_PANEL_MANAGER_PARAM;

static FLAT_PANEL_MANAGER_PARAM GL_FlatPanelManagerParam_X;


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessScanKey(void);

static void TransitionToIdle(void);
static void TransitionToScanKey(void);

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void FlatPanelManager_Init(FlatPanel * pFlatPanel_H) {
	GL_pFlatPanel_H = pFlatPanel_H;
	GL_FlatPanelManager_CurrentState_E = FLAT_PANEL_MANAGER_STATE::FLAT_PANEL_MANAGER_IDLE;
	GL_FlatPanelManagerParam_X.IsEnabled_B = false;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Flat Panel Manager Initialized");
}

void FlatPanelManager_Enable() {
	GL_FlatPanelManagerParam_X.IsEnabled_B = true;
}

void FlatPanelManager_Disable() {
	GL_FlatPanelManagerParam_X.IsEnabled_B = false;
}

void FlatPanelManager_Process() {
	switch (GL_FlatPanelManager_CurrentState_E) {
	case FLAT_PANEL_MANAGER_IDLE:
		ProcessIdle();
		break;

	case FLAT_PANEL_MANAGER_SCAN_KEY:
		ProcessScanKey();
		break;
	}
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void) {
	if (GL_pFlatPanel_H->isInitialized() && GL_FlatPanelManagerParam_X.IsEnabled_B)
		TransitionToScanKey();
}

static void ProcessScanKey(void) {
	GL_pFlatPanel_H->getKey();
	if (!(GL_FlatPanelManagerParam_X.IsEnabled_B))
		TransitionToIdle();
}


void TransitionToIdle() {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
	GL_FlatPanelManager_CurrentState_E = FLAT_PANEL_MANAGER_STATE::FLAT_PANEL_MANAGER_IDLE;
}

void TransitionToScanKey(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To SCAN KEY");
	GL_FlatPanelManager_CurrentState_E = FLAT_PANEL_MANAGER_STATE::FLAT_PANEL_MANAGER_SCAN_KEY;
}

