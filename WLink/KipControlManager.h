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
#include "RealTimeClock.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
    boolean IsConfigured_B;             // Configured flag
    boolean IsRunning_B;                // Running flag
    unsigned char Tolerance_UB;         // Tolerance in [%]
    unsigned char ReferenceDataId_UB;   // ID of the reference data table
    unsigned char MaxDataNb_UB;         // Total number of data stored in the table
    unsigned char StartIdx_UB;          // Index at which the recording should start
    RTC_DATE_STRUCT StartDate_X;        // Start date of recording
    unsigned char CurrentIdx_UB;        // Current index
    unsigned long TotalValue_UL;        // Total value (addition of all current data)
    unsigned long ValueNb_UL;           // Number of values in the total value field
} KC_HANDLE_STRUCT;

typedef struct {
    signed int Weigh_SI;
    String MacAddr_Str;
    String TimeStamp_Str;
} KC_DATA_TO_SEND_STRUCT;

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void KipControlManager_Init();
void KipControlManager_Enable();
void KipControlManager_Disable();
void KipControlManager_Process();

boolean KipControlManager_IsEnabled();


#endif // __KIPCONTROL_MANAGER_H__

