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
	KCMENU_ITEM_CONTINUE_RECORD,
	KCMENU_ITEM_GET_BATCH_NUMBER,
	KCMENU_ITEM_GET_REFERENCE_ID,
	KCMENU_ITEM_GET_TOLERANCE,
	KCMENU_ITEM_GET_MIN_WEIGHT,
	KCMENU_ITEM_GET_CURRENT_DAY,
	KCMENU_ITEM_SET_BATCH_NUMBER,
	KCMENU_ITEM_SET_REFERENCE_ID,
	KCMENU_ITEM_SET_TOLERANCE,
	KCMENU_ITEM_SET_MIN_WEIGHT,
	KCMENU_ITEM_SET_START_DAY,
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
	{ "  Continue          ",     " Continuer          ",     " Doorgaan           " },
	{ "Current setting     ",     "Parametre actuel    ",     "Huidige instelling  " },
	{ "Current setting     ",     "Parametre actuel    ",     "Huidige instelling  " },
	{ "Current setting     ",     "Parametre actuel    ",     "Huidige instelling  " },
	{ "Current setting     ",     "Parametre actuel    ",     "Huidige instelling  " },
	{ "Current setting     ",     "Parametre actuel    ",     "Huidige instelling  " },
	{ "Enter setting       ",     "Entrez parametre    ",     "Voeren parameter    " },
	{ "Enter setting       ",     "Entrez parametre    ",     "Voeren parameter    " },
	{ "Enter setting       ",     "Entrez parametre    ",     "Voeren parameter    " },
	{ "Enter setting       ",     "Entrez parametre    ",     "Voeren parameter    " },
	{ "Enter setting       ",     "Entrez parametre    ",     "Voeren parameter    " },
};

//              Text for all menu item :        
const String GL_ppKCMenuItemText2_Str[][3] = {
	//      EN                          FR                          NL
	{ "                    ",     "                    ",     "                    " },
	{ "    starting...     ",     "    demarrage...    ",     "    opstarten...    " },
	{ " recording ?		   ",     " enregistrement ?   ",     " aanmelding ?       " },
	{ "Batch : XX          ",     "Lot : XXX           ",     "Batch : XX          " },
	{ "Reference : XX      ",     "Reference : XX      ",     "Referentie : XX     " },
	{ "Tolerance : XX %    ",     "Tolerance : XX %    ",     "Tolerantie : XX %   " },
	{ "Min weight : XXXX g ",     "Poids min : XXXX g  ",     "Min gewicht : XXXX g" },
	{ "Day : XX            ",     "Jour : XX           ",     "Dag : XX            " },
	{ "Batch : XX          ",     "Lot : XXX           ",     "Batch : XX          " },
	{ "Reference : XX      ",     "Reference : XX      ",     "Referentie : XX     " },
	{ "Tolerance : XX %    ",     "Tolerance : XX %    ",     "Tolerantie : XX %   " },
	{ "Min weight : XXXX g ",     "Poids min : XXXX g  ",     "Min gewicht : XXXX g" },
	{ "Start day : XX      ",     "Jour debut : XX     ",     "Startdag : XX       " },
};

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */

#endif // __KCMENU_ITEM_TEXT_H__

