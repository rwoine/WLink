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
    unsigned char ppText_UB[2][20];
    WMENU_ITEM_STRUCT * pPreviousItem_X;    // TODO : Need Text on one line to display and navigate. Need EnterItem_X ? Have to think...
    WMENU_ITEM_STRUCT * pNextItem_X;
} WMENU_ITEM_STRUCT;

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void WMenuManager_Init();
void WMenuManager_Enable();
void WMenuManager_Disable();
void WMenuManager_Process();


#endif // __WMENU_MANAGER_H__

