/* ******************************************************************************** */
/*                                                                                  */
/* KipControlLinkManager.h												            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for KipControlLinkManager.cpp					            	*/
/*		Process functions to manage the KipControl application  					*/
/*                                                                                  */
/* History :	17/04/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __KIPCONTROL_MANAGER_H__
#define __KIPCONTROL_MANAGER_H__

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
void KipControlManager_Init();
void KipControlManager_Enable();
void KipControlManager_Disable();
void KipControlManager_Process();

boolean KipControlManager_IsEnabled();


#endif // __KIPCONTROL_MANAGER_H__

