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

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void IndicatorManager_Init(Indicator* Indicator_H);
void IndicatorManager_Enable();
void IndicatorManager_Disable();
void IndicatorManager_SetZeroIndicator();
void IndicatorManager_Process();

#endif // __INDICATOR_MANAGER_H__

