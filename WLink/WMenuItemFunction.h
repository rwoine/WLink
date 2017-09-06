/* ******************************************************************************** */
/*                                                                                  */
/* WMenuItemFunction.h													            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for WMenuItemFunction.cpp						            	*/
/*		Defines the specific functions for the W-Link Menu Items.					*/
/*                                                                                  */
/* History :	03/09/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __WMENU_ITEM_FUNCTION_H__
#define __WMENU_ITEM_FUNCTION_H__

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

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */

void WMenuItem_WelcomeScreen_Transition(void * Hander_H);

void WMenuItem_Idle_Process(void * Handler_H);

void WMenuItem_LanguageSelect_Process(void * Handler_H);

void WMenuItem_DateSet_Process(void * Handler_H);

void WMenuItem_TimeSet_Process(void * Handler_H);


#endif // __WMENU_ITEM_FUNCTION_H__

