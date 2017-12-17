/* ******************************************************************************** */
/*                                                                                  */
/* WMenuManager.h														            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for WMenuManager.cpp							            	*/
/*		Process functions to manage the main menu for the W-Link					*/
/*                                                                                  */
/* History :	16/03/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __WMENU_MANAGER_H__
#define __WMENU_MANAGER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "WMenuItemText.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define WMENU_NAVBUTTON_UP                  0
#define WMENU_NAVBUTTON_DOWN                1
#define WMENU_NAVBUTTON_ENTER               2
#define WMENU_NAVBUTTON_BACK                3

#define WMENU_FCTBUTTON_F1					0
#define WMENU_FCTBUTTON_F2					1
#define WMENU_FCTBUTTON_F3					2

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
    WMENU_ITEM_TYPE_NULL,       // Not an item
    WMENU_ITEM_TYPE_INFO,       // Only display information
    WMENU_ITEM_TYPE_MENU,       // Navigate through sub-menu
    WMENU_ITEM_TYPE_PARAM       // Enter some parameters
} WMENU_ITEM_TYPE_ENUM;


typedef struct WMENU_ITEM_STRUCT WMENU_ITEM_STRUCT;
struct WMENU_ITEM_STRUCT {
	WMENU_ITEM_TYPE_ENUM Type_E;
	unsigned long Id_UL;
    unsigned long NavIndex_UL;
	boolean IsFromApp_B;
	unsigned long TimerValue_UL;
    const char * ppText_UB[2];
    WMENU_ITEM_STRUCT * pErrorItem_X;
	WMENU_ITEM_STRUCT * ppOnNavItem_X[4];
	WMENU_ITEM_STRUCT * ppOnFctItem_X[3];
	WMENU_ITEM_STRUCT * pOnConditionNavItem_X;
	WMENU_ITEM_STRUCT * pOnTimerNavItem_X;
	boolean (*pFct_GetCondition)(void *);
	void (*pFct_OnTransition)(void *);
	void (*pFct_OnProcess)(void *);
    void (*pFct_OnEnter)(void *);
    void (*pFct_OnTimerElapsed)(void *);
    void (*pFct_OnValidateParam)(unsigned char *);
};


typedef struct {
	WMENU_ITEM_STRUCT * pSenderItem_H;
	boolean KeyPressed_B;
	char Key_UB;
	unsigned long ParamIndex_UL;
	unsigned char * pParam_UB;
} WMENU_ITEM_PARAM_STRUCT;


/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void WMenuManager_Init();
void WMenuManager_Enable();
void WMenuManager_Disable();
void WMenuManager_Process();

void WMenuManager_PushKey(char * pKey_UB);


#endif // __WMENU_MANAGER_H__

