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

		GL_pKCMenuItem_X[i].ppText_UB[0] = "";
		GL_pKCMenuItem_X[i].ppText_UB[1] = "";

		GL_pKCMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_UP] = &(GL_pKCMenuItem_X[KCMENU_ITEM_NULL]);
		GL_pKCMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_DOWN] = &(GL_pKCMenuItem_X[KCMENU_ITEM_NULL]);
		GL_pKCMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pKCMenuItem_X[KCMENU_ITEM_NULL]);
		GL_pKCMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pKCMenuItem_X[KCMENU_ITEM_NULL]);

		GL_pKCMenuItem_X[i].pFct_OnTransition = DefaultOnTransitionFct;
		GL_pKCMenuItem_X[i].pFct_OnProcess = DefaultOnProcessFct;
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

	//GL_pKCMenuItem_X[WMENU_ITEM_IDLE_SCREEN].pFct_OnProcess = ;

}


WMENU_ITEM_STRUCT * KipControlMenu_GetFirstItem(void) {
	return (&(GL_pKCMenuItem_X[KCMENU_ITEM_IDLE_SCREEN]));
}
