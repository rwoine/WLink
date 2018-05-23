/* ******************************************************************************** */
/*                                                                                  */
/* KipControlMenuItemFunction.h											            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for KipControlMenuItemFunction.cpp						       	*/
/*		Defines the specific functions for the KipControl Menu Items.				*/
/*                                                                                  */
/* History :	03/09/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __KCMENU_ITEM_FUNCTION_H__
#define __KCMENU_ITEM_FUNCTION_H__

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
boolean KCMenuItem_WelcomeScreen_GetCondition(void * Handler_H);

void KCMenuItem_ContinueRecording_OnTransition(void * Handler_H);
void KCMenuItem_ContinueRecording_OnEnter(void * Handler_H);
void KCMenuItem_ContinueRecording_OnTimerElapsed(void * Handler_H);

void KCMenuItem_GetBatchNumber_Transition(void * Handler_H);
void KCMenuItem_GetReferenceId_Transition(void * Handler_H);
void KCMenuItem_GetTolerance_Transition(void * Handler_H);
void KCMenuItem_GetMinWeight_Transition(void * Handler_H);
void KCMenuItem_GetCurrentDay_Transition(void * Handler_H);
void KCMenuItem_GetStartDate_Transition(void * Handler_H);

void KCMenuItem_SetBatchNumber_Process(void * Handler_H);
void KCMenuItem_SetBatchNumber_OnValidate(unsigned char * pParam_UB);
void KCMenuItem_SetReferenceId_Process(void * Handler_H);
void KCMenuItem_SetReferenceId_OnValidate(unsigned char * pParam_UB);
void KCMenuItem_SetTolerance_Process(void * Handler_H);
void KCMenuItem_SetTolerance_OnValidate(unsigned char * pParam_UB);
void KCMenuItem_SetMinWeight_Process(void * Handler_H);
void KCMenuItem_SetMinWeight_OnValidate(unsigned char * pParam_UB);
void KCMenuItem_SetStartDay_Process(void * Handler_H);
void KCMenuItem_SetStartDay_OnValidate(unsigned char * pParam_UB);
void KCMenuItem_SetStartDate_Process(void * Handler_H);
void KCMenuItem_SetStartDate_OnValidate(unsigned char * pParam_UB);

void KCMenuItem_NewRecording_OnEnter(void * Handler_H);

void KCMenuItem_ActualRecording_Process(void * Handler_H);
void KCMenuItem_ActualRecording_Transition(void * Handler_H);
boolean KCMenuItem_ActualRecording_GetCondition(void * Hander_H);

void KCMenuItem_CurrentRecord_Process(void * Handler_H);

void KCMenuItem_CurrentWeight_Process(void * Handler_H);
void KCMenuItem_CurrentWeight_Transition(void * Handler_H);

void KCMenuItem_ResetWeight_OnEnter(void * Handler_H);


#endif // __KCMENU_ITEM_FUNCTION_H__

