/* ******************************************************************************** */
/*                                                                                  */
/* KipControlMenuITEMText.h													        */
/*                                                                                  */
/* Description :                                                                    */
/*		Defines the texts used for KipControlMenu, in several languages  			*/
/*                                                                                  */
/* History :	10/09/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __KCMENU_ITEM_TEXT_H__
#define __KCMENU_ITEM_TEXT_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "WLink.h"

/* ******************************************************************************** */
/* Enumeration
/* ******************************************************************************** */
typedef enum {
	KCMENU_ITEM_NULL = 0,
	KCMENU_ITEM_IDLE_SCREEN,
	KCMENU_ITEM_NUMBER
} KCMENU_ITEM_ENUM;


/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

//              Text for all menu item :        
const String GL_ppKCMenuItemText_Str[][3] = {
	//      EN                          FR                          NL
	{ "                    ",     "                    ",     "                    " },
	{ "  > KipControl  (EN)",     "  > KipControl  (FR)",     "  > KipControl  (NL)" },
};

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */

#endif // __KCMENU_ITEM_TEXT_H__
