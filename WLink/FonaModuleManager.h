/* ******************************************************************************** */
/*                                                                                  */
/* FonaModuleManager.h																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for FonaModuleManager.cpp										*/
/*		Process functions to manage the Fona Module object							*/
/*                                                                                  */
/* History :	17/07/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __FONA_MODULE_MANAGER_H__
#define __FONA_MODULE_MANAGER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "FonaModule.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void FonaModuleManager_Init(FonaModule * pFona_H, boolean EnableGprs_B = true);
void FonaModuleManager_Enable();
void FonaModuleManager_Disable();
void FonaModuleManager_Process();
boolean FonaModuleManager_IsRunning();
boolean FonaModuleManager_IsError();

int FonaModuleManager_GetCurrentSignalStrength();
boolean FonaModuleManager_GetCurrentGprsState();
unsigned int FonaModuleManager_GetCurrentBatteryLevel();

#endif // __FONA_MODULE_MANAGER_H__

