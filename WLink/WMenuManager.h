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
#include "WLink.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define WMENU_NAVBUTTON_UP                  0
#define WMENU_NAVBUTTON_DOWN                1
#define WMENU_NAVBUTTON_ENTER               2
#define WMENU_NAVBUTTON_BACK                3
#define WMENU_NAVBUTTON_NONE                99

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
    WMENU_ITEM_TYPE_NULL,       // Not an item
    WMENU_ITEM_TYPE_INFO,       // Only display information
    WMENU_ITEM_TYPE_MENU,       // Navigate through sub-menu
    WMENU_ITEM_TYPE_PARAM       // Enter some parameters
} WMENU_ITEM_TYPE_ENUM;

typedef struct {
    WMENU_ITEM_TYPE_ENUM Type_E;
    unsigned long NavIndex_UL;
    const char * ppText_UB[2];
    void * ppOnNavItem_X[4];
} WMENU_ITEM_STRUCT;

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void WMenuManager_Init();
void WMenuManager_Enable();
void WMenuManager_Disable();
void WMenuManager_Process();


#endif // __WMENU_MANAGER_H__

