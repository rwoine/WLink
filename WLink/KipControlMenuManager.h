/* ******************************************************************************** */
/*                                                                                  */
/* KipControlMenuManager.h  											            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for KipControlMenuManager.cpp							       	*/
/*		Process functions to manage the main menu for the KipControl application	*/
/*                                                                                  */
/* History :	17/04/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __KCMENU_MANAGER_H__
#define __KCMENU_MANAGER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "WLink.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define KCMENU_NAVBUTTON_UP                  0
#define KCMENU_NAVBUTTON_DOWN                1
#define KCMENU_NAVBUTTON_ENTER               2
#define KCMENU_NAVBUTTON_BACK                3

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
    KCMENU_ITEM_TYPE_NULL,       // Not an item
    KCMENU_ITEM_TYPE_INFO,       // Only display information
    KCMENU_ITEM_TYPE_MENU,       // Navigate through sub-menu
    KCMENU_ITEM_TYPE_PARAM       // Enter some parameters
} KCMENU_ITEM_TYPE_ENUM;


typedef struct KCMENU_ITEM_STRUCT KCMENU_ITEM_STRUCT;
struct KCMENU_ITEM_STRUCT {
    KCMENU_ITEM_TYPE_ENUM Type_E;
    unsigned long NavIndex_UL;
    const char * ppText_UB[2];
    KCMENU_ITEM_STRUCT * ppOnNavItem[4];
    unsigned long AdditionalParam_UL;
    void(*pFct_OnValidateParam)(unsigned char *);
};

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void KipControlMenuManager_Init();
void KipControlMenuManager_Enable();
void KipControlMenuManager_Disable();
void KipControlMenuManager_Process();


#endif // __KCMENU_MANAGER_H__

