/* ******************************************************************************** */
/*                                                                                  */
/* KipControlMenu.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the necessary functions to manage the KipControl menu				*/
/*                                                                                  */
/* History :  	10/09/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"KipControlMenu"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "WLink.h"
#include "KipControlMenu.h"
#include "KipControlMenuItemText.h"
#include "KipControlMenuItemFunction.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define KCMENU_ITEM_TYPE_NULL	WMENU_ITEM_TYPE_NULL
#define KCMENU_ITEM_TYPE_INFO	WMENU_ITEM_TYPE_INFO
#define KCMENU_ITEM_TYPE_MENU	WMENU_ITEM_TYPE_MENU
#define KCMENU_ITEM_TYPE_PARAM	WMENU_ITEM_TYPE_PARAM

/* ******************************************************************************** */
/* External Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
static WMENU_ITEM_STRUCT GL_pKCMenuItem_X[KCMENU_ITEM_NUMBER];

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void KipControlMenu_Init(void) {

	/* -------------- */
	/* Default Values */
	/* -------------- */
	for (int i = 0; i < KCMENU_ITEM_NUMBER; i++) {

		GL_pKCMenuItem_X[i].Type_E = KCMENU_ITEM_TYPE_NULL;
		GL_pKCMenuItem_X[i].Id_UL = KCMENU_ITEM_NULL;
		GL_pKCMenuItem_X[i].NavIndex_UL = 0;
		GL_pKCMenuItem_X[i].IsFromApp_B = true;
		GL_pKCMenuItem_X[i].TimerValue_UL = 0;

		GL_pKCMenuItem_X[i].ppText_UB[0] = "";
		GL_pKCMenuItem_X[i].ppText_UB[1] = "";

        GL_pKCMenuItem_X[i].pErrorItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_ERROR_SCREEN]);

		GL_pKCMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_UP] = &(GL_pKCMenuItem_X[KCMENU_ITEM_NULL]);
		GL_pKCMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_DOWN] = &(GL_pKCMenuItem_X[KCMENU_ITEM_NULL]);
		GL_pKCMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pKCMenuItem_X[KCMENU_ITEM_NULL]);
		GL_pKCMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_NULL]);

		GL_pKCMenuItem_X[i].ppOnFctItem_X[WMENU_FCTBUTTON_F1] = &(GL_pKCMenuItem_X[WMENU_ITEM_NULL]);
		GL_pKCMenuItem_X[i].ppOnFctItem_X[WMENU_FCTBUTTON_F2] = &(GL_pKCMenuItem_X[WMENU_ITEM_NULL]);
		GL_pKCMenuItem_X[i].ppOnFctItem_X[WMENU_FCTBUTTON_F3] = &(GL_pKCMenuItem_X[WMENU_ITEM_NULL]);

		GL_pKCMenuItem_X[i].pOnConditionNavItem_X = &(GL_pKCMenuItem_X[WMENU_ITEM_NULL]);
		GL_pKCMenuItem_X[i].pOnTimerNavItem_X = &(GL_pKCMenuItem_X[WMENU_ITEM_NULL]);

		GL_pKCMenuItem_X[i].pFct_GetCondition = DefaultGetCondition;
		GL_pKCMenuItem_X[i].pFct_OnTransition = DefaultOnTransitionFct;
		GL_pKCMenuItem_X[i].pFct_OnProcess = DefaultOnProcessFct;
        GL_pKCMenuItem_X[i].pFct_OnEnter = DefaultOnProcessFct;
        GL_pKCMenuItem_X[i].pFct_OnTimerElapsed = DefaultOnProcessFct;
		GL_pKCMenuItem_X[i].pFct_OnValidateParam = DefaultOnValidateFct;
	}


	/* -------------------- */
	/* Initialize all items */
	/* -------------------- */

	/* 0. Idle */
	GL_pKCMenuItem_X[KCMENU_ITEM_IDLE_SCREEN].Type_E = KCMENU_ITEM_TYPE_INFO;
	GL_pKCMenuItem_X[KCMENU_ITEM_IDLE_SCREEN].Id_UL = KCMENU_ITEM_IDLE_SCREEN;
	GL_pKCMenuItem_X[KCMENU_ITEM_IDLE_SCREEN].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_IDLE_SCREEN].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_IDLE_SCREEN][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_IDLE_SCREEN].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_IDLE_SCREEN][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_IDLE_SCREEN].pOnConditionNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD]);
	GL_pKCMenuItem_X[KCMENU_ITEM_IDLE_SCREEN].pFct_GetCondition = KCMenuItem_WelcomeScreen_GetCondition;

    /* 0. Error */
    GL_pKCMenuItem_X[KCMENU_ITEM_ERROR_SCREEN].Type_E = KCMENU_ITEM_TYPE_INFO;
    GL_pKCMenuItem_X[KCMENU_ITEM_ERROR_SCREEN].Id_UL = KCMENU_ITEM_ERROR_SCREEN;
    GL_pKCMenuItem_X[KCMENU_ITEM_ERROR_SCREEN].NavIndex_UL = 0;

    GL_pKCMenuItem_X[KCMENU_ITEM_ERROR_SCREEN].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_ERROR_SCREEN][GL_GlobalConfig_X.Language_E].c_str();
    GL_pKCMenuItem_X[KCMENU_ITEM_ERROR_SCREEN].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_ERROR_SCREEN][GL_GlobalConfig_X.Language_E].c_str();


	/* 0. Continue Recording */
	GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].Type_E = KCMENU_ITEM_TYPE_INFO;
	GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].Id_UL = KCMENU_ITEM_CONTINUE_RECORD;
	GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_CONTINUE_RECORD][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_CONTINUE_RECORD][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER]);
	GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_NEW_RECORD]);

    GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].TimerValue_UL = 15000;
    GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].pOnTimerNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER]);

    GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].pFct_OnEnter = KCMenuItem_ContinueRecording_OnEnter;
    GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD].pFct_OnTimerElapsed = KCMenuItem_ContinueRecording_OnTimerElapsed;


	/* 0. Get Batch Number */
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER].Type_E = KCMENU_ITEM_TYPE_INFO;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER].Id_UL = KCMENU_ITEM_GET_BATCH_NUMBER;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_GET_BATCH_NUMBER][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_GET_BATCH_NUMBER][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER].TimerValue_UL = 2000;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER].pOnTimerNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_GET_REFERENCE_ID]);

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER].pFct_OnTransition = KCMenuItem_GetBatchNumber_Transition;    // Allow recording to be processed in this function


	/* 0. Get Reference ID */
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_REFERENCE_ID].Type_E = KCMENU_ITEM_TYPE_INFO;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_REFERENCE_ID].Id_UL = KCMENU_ITEM_GET_REFERENCE_ID;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_REFERENCE_ID].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_REFERENCE_ID].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_GET_REFERENCE_ID][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_REFERENCE_ID].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_GET_REFERENCE_ID][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_REFERENCE_ID].TimerValue_UL = 2000;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_REFERENCE_ID].pOnTimerNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_GET_TOLERANCE]);

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_REFERENCE_ID].pFct_OnTransition = KCMenuItem_GetReferenceId_Transition;


	/* 0. Get Tolerance */
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_TOLERANCE].Type_E = KCMENU_ITEM_TYPE_INFO;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_TOLERANCE].Id_UL = KCMENU_ITEM_GET_TOLERANCE;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_TOLERANCE].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_TOLERANCE].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_GET_TOLERANCE][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_TOLERANCE].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_GET_TOLERANCE][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_TOLERANCE].TimerValue_UL = 2000;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_TOLERANCE].pOnTimerNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_GET_MIN_WEIGHT]);

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_TOLERANCE].pFct_OnTransition = KCMenuItem_GetTolerance_Transition;


	/* 0. Get Min Weight */
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_MIN_WEIGHT].Type_E = KCMENU_ITEM_TYPE_INFO;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_MIN_WEIGHT].Id_UL = KCMENU_ITEM_GET_MIN_WEIGHT;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_MIN_WEIGHT].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_MIN_WEIGHT].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_GET_MIN_WEIGHT][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_MIN_WEIGHT].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_GET_MIN_WEIGHT][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_MIN_WEIGHT].TimerValue_UL = 2000;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_MIN_WEIGHT].pOnTimerNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_GET_CURRENT_DAY]);

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_MIN_WEIGHT].pFct_OnTransition = KCMenuItem_GetMinWeight_Transition;


	/* 0. Get Current Day */
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_CURRENT_DAY].Type_E = KCMENU_ITEM_TYPE_INFO;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_CURRENT_DAY].Id_UL = KCMENU_ITEM_GET_CURRENT_DAY;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_CURRENT_DAY].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_CURRENT_DAY].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_GET_CURRENT_DAY][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_CURRENT_DAY].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_GET_CURRENT_DAY][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_CURRENT_DAY].TimerValue_UL = 2000;
	GL_pKCMenuItem_X[KCMENU_ITEM_GET_CURRENT_DAY].pOnTimerNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD]);

	GL_pKCMenuItem_X[KCMENU_ITEM_GET_CURRENT_DAY].pFct_OnTransition = KCMenuItem_GetCurrentDay_Transition;



	/* 0. New Recording */
	GL_pKCMenuItem_X[KCMENU_ITEM_NEW_RECORD].Type_E = KCMENU_ITEM_TYPE_INFO;
	GL_pKCMenuItem_X[KCMENU_ITEM_NEW_RECORD].Id_UL = KCMENU_ITEM_NEW_RECORD;
	GL_pKCMenuItem_X[KCMENU_ITEM_NEW_RECORD].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_NEW_RECORD].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_NEW_RECORD][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_NEW_RECORD].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_NEW_RECORD][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_NEW_RECORD].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER]);
	GL_pKCMenuItem_X[KCMENU_ITEM_NEW_RECORD].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD]);

    GL_pKCMenuItem_X[KCMENU_ITEM_NEW_RECORD].pFct_OnEnter = KCMenuItem_NewRecording_OnEnter;


	/* 0. Set Batch Number */
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER].Type_E = KCMENU_ITEM_TYPE_PARAM;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER].Id_UL = KCMENU_ITEM_SET_BATCH_NUMBER;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_SET_BATCH_NUMBER][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_SET_BATCH_NUMBER][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID]);
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_NEW_RECORD]);

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER].pFct_OnProcess = KCMenuItem_SetBatchNumber_Process;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER].pFct_OnValidateParam = KCMenuItem_SetBatchNumber_OnValidate;


	/* 0. Set Reference ID */
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID].Type_E = KCMENU_ITEM_TYPE_PARAM;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID].Id_UL = KCMENU_ITEM_SET_REFERENCE_ID;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_SET_REFERENCE_ID][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_SET_REFERENCE_ID][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE]);
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_SET_BATCH_NUMBER]);

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID].pFct_OnProcess = KCMenuItem_SetReferenceId_Process;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID].pFct_OnValidateParam = KCMenuItem_SetReferenceId_OnValidate;



	/* 0. Set Tolerance */
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE].Type_E = KCMENU_ITEM_TYPE_PARAM;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE].Id_UL = KCMENU_ITEM_SET_TOLERANCE;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_SET_TOLERANCE][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_SET_TOLERANCE][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT]);
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_SET_REFERENCE_ID]);

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE].pFct_OnProcess = KCMenuItem_SetTolerance_Process;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE].pFct_OnValidateParam = KCMenuItem_SetTolerance_OnValidate;



	/* 0. Set Min Weight */
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT].Type_E = KCMENU_ITEM_TYPE_PARAM;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT].Id_UL = KCMENU_ITEM_SET_MIN_WEIGHT;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_SET_MIN_WEIGHT][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_SET_MIN_WEIGHT][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pKCMenuItem_X[KCMENU_ITEM_SET_START_DAY]);
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_SET_TOLERANCE]);

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT].pFct_OnProcess = KCMenuItem_SetMinWeight_Process;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT].pFct_OnValidateParam = KCMenuItem_SetMinWeight_OnValidate;



	/* 0. Set Start Day */
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_START_DAY].Type_E = KCMENU_ITEM_TYPE_PARAM;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_START_DAY].Id_UL = KCMENU_ITEM_SET_START_DAY;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_START_DAY].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_START_DAY].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_SET_START_DAY][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_START_DAY].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_SET_START_DAY][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_START_DAY].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD]);
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_START_DAY].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_SET_MIN_WEIGHT]);

	GL_pKCMenuItem_X[KCMENU_ITEM_SET_START_DAY].pFct_OnProcess = KCMenuItem_SetStartDay_Process;
	GL_pKCMenuItem_X[KCMENU_ITEM_SET_START_DAY].pFct_OnValidateParam = KCMenuItem_SetStartDay_OnValidate;
	



	/* 0. Actual Recording */
	GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].Type_E = KCMENU_ITEM_TYPE_INFO;
	GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].Id_UL = KCMENU_ITEM_ACTUAL_RECORD;
	GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_ACTUAL_RECORD][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_ACTUAL_RECORD][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_CONTINUE_RECORD]);

    GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].ppOnFctItem_X[WMENU_FCTBUTTON_F1] = &(GL_pKCMenuItem_X[KCMENU_ITEM_GET_BATCH_NUMBER]);
    GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].ppOnFctItem_X[WMENU_FCTBUTTON_F2] = &(GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT]); 

	GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].pOnConditionNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_RECORD]);
	GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].pFct_GetCondition = KCMenuItem_ActualRecording_GetCondition;

    GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].pFct_OnProcess = KCMenuItem_ActualRecording_Process;
    GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD].pFct_OnTransition = KCMenuItem_ActualRecording_Transition;


	/* 0. Current Record */	
	GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_RECORD].Type_E = KCMENU_ITEM_TYPE_INFO;
	GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_RECORD].Id_UL = KCMENU_ITEM_CURRENT_RECORD;
	GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_RECORD].NavIndex_UL = 0;

	GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_RECORD].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_CURRENT_RECORD][GL_GlobalConfig_X.Language_E].c_str();
	GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_RECORD].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_CURRENT_RECORD][GL_GlobalConfig_X.Language_E].c_str();

	GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_RECORD].TimerValue_UL = 4000;
	GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_RECORD].pOnTimerNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD]);

	GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_RECORD].pFct_OnProcess = KCMenuItem_CurrentRecord_Process;


    /* 0. Current Weight */
    GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT].Type_E = KCMENU_ITEM_TYPE_INFO;
    GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT].Id_UL = KCMENU_ITEM_CURRENT_WEIGHT;
    GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT].NavIndex_UL = 0;

    GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT].ppText_UB[0] = GL_ppKCMenuItemText_Str[KCMENU_ITEM_CURRENT_WEIGHT][GL_GlobalConfig_X.Language_E].c_str();
    GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT].ppText_UB[1] = GL_ppKCMenuItemText2_Str[KCMENU_ITEM_CURRENT_WEIGHT][GL_GlobalConfig_X.Language_E].c_str();

    GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD]);

    GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT].TimerValue_UL = 25000;
    GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT].pOnTimerNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_ACTUAL_RECORD]);

    GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT].pFct_OnProcess = KCMenuItem_CurrentWeight_Process;
    GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_WEIGHT].pFct_OnTransition = KCMenuItem_CurrentWeight_Transition;

}


WMENU_ITEM_STRUCT * KipControlMenu_GetFirstItem(void) {
	return (&(GL_pKCMenuItem_X[KCMENU_ITEM_IDLE_SCREEN]));
}
