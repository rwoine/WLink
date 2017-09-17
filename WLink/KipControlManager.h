/* ******************************************************************************** */
/*                                                                                  */
/* KipControlManager.h												                */
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
#include "KipControl.h"
#include "RealTimeClock.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define KC_REFERENCE_TABLE_START_ADDR   0x0800
#define KC_REFERENCE_TABLE_OFFSET       0x0100

#define KC_MAX_DATA_NB					120

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
	String MacAddr_Str;					// MAC address for unique identification
	boolean OfflineMode_B;				// Offline mode flag (false = record data to portal)

    boolean IsConfigured_B;             // Configured flag
    boolean IsRunning_B;                // Running flag
    unsigned long BatchId_UL;           // ID of the current batch
    unsigned char Tolerance_UB;         // Tolerance in [%]
	signed int WeightMin_SI;			// Minimum weight to take into account
    unsigned char ReferenceDataId_UB;   // ID of the reference data table
    unsigned char MaxDataNb_UB;         // Total number of data stored in the table
    unsigned char StartIdx_UB;          // Index at which the recording should start
    RTC_DATE_STRUCT StartDate_X;        // Start date of recording

    unsigned char CurrentIdx_UB;        // Current index
    unsigned long TotalValue_UL;        // Total value (addition of all current data)
    unsigned long ValueNb_UL;           // Number of values in the total value field

	signed int Weight_SI;				// Current weight
	RTC_DATE_STRUCT CurrentDate_X;      // Current date
	String TimeStamp_Str;				// Current timestamp
	boolean IsValid_B;					// Check if weight is valid (within tolerance)

} KC_WORKING_DATA_STRUCT;


/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void KipControlManager_Init(void * pHanlde_H);
void KipControlManager_Enable();
void KipControlManager_Disable();
void KipControlManager_Process();

boolean KipControlManager_IsEnabled();
boolean KipControlManager_IsRunning();

#endif // __KIPCONTROL_MANAGER_H__

