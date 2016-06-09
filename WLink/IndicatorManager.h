/* ******************************************************************************** */
/*                                                                                  */
/* IndicatorManager.h																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for IndicatorManager.cpp										*/
/*		Process functions to manager to Indicator object							*/
/*                                                                                  */
/* History :  	22/12/2014  (RW)	Creation of this file                           */
/*				12/01/2015  (RW)	Add disable() function                          */
/*				07/06/2016	(RW)	Re-mastered version								*/	
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __INDICATOR_MANAGER_H__
#define __INDICATOR_MANAGER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <Arduino.h>

#include "Indicator.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define INDICATOR_MANAGER_DEFAULT_SCAN_PERIOD	    1000
#define INDICATOR_MANAGER_DEFAULT_RESPONSE_DELAY    100
#define INDICATOR_MANAGER_DEFAULT_RESET_DELAY	    100
#define INDICATOR_MANAGER_DEFAULT_MAX_TRY_NUMBER	2

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void IndicatorManager_Init(Indicator* Indicator_H);
void IndicatorManager_Enable(INDICATOR_INTERFACE_FRAME_ENUM FrameType_E = INDICATOR_INTERFACE_FRAME_ASK_WEIGHT);
void IndicatorManager_Disable();
void IndicatorManager_SetZeroIndicator();
void IndicatorManager_Process();

#endif // __INDICATOR_MANAGER_H__

