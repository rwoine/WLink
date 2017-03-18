/* ******************************************************************************** */
/*                                                                                  */
/* WMenuItemText.h														            */
/*                                                                                  */
/* Description :                                                                    */
/*		Defines the texts used for WMenu, in several languages  					*/
/*                                                                                  */
/* History :	18/03/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __WMENU_ITEM_TEXT_H__
#define __WMENU_ITEM_TEXT_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "WLink.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

#define WMENU_ITEM_TEXT_NULL                0
#define WMENU_ITEM_TEXT_WELCOME_SCREEN      1
#define WMENU_ITEM_TEXT_PARAMETERS          2
#define WMENU_ITEM_TEXT_LCD                 3
#define WMENU_ITEM_TEXT_MENU1               4

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

//              Text for all menu item :        
const String GL_ppWMenuItemText_Str[][3] = {
//      EN                          FR                          NL
    { "                    ",     "                    ",     "                    " },
    { "--- W-Link ---  (EN)",     "--- W-Link ---  (FR)",     "--- W-Link ---  (NL)" },
    { "  Settings          ",     "  Paramètres        ",     "  Instellingen      " },
    { "  LCD               ",     "  LCD               ",     "  LCD               " },
    { "  Menu1 - EN        ",     "  Menu1 - FR        ",     "  Menu1 - FR        " },
};

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */

#endif // __WMENU_ITEM_TEXT_H__

