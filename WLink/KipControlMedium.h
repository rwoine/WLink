/* ******************************************************************************** */
/*                                                                                  */
/* KipControlMedium.h												                */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for KipControlMedium.cpp				            	        */
/*		Defines functions to abstract the communication medium for the project.		*/
/*                                                                                  */
/* History :	31/07/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __KIPCONTROL_MEDIUM_H__
#define __KIPCONTROL_MEDIUM_H__

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

typedef enum {
    KC_MEDIUM_ETHERNET,
    KC_MEDIUM_GSM
} KC_MEDIUM_ENUM;

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void KipControlMedium_Init(KC_MEDIUM_ENUM Medium_E, void * pMedium_H);
void KipControlMedium_SetServerParam(String Name_Str, unsigned long Port_UL = 80);

boolean KipControlMedium_IsReady(void);

boolean KipControlMedium_Connect(void);
boolean KipControlMedium_IsConnected(void);

void KipControlMedium_SetupEnvironment(void);

void KipControlMedium_BeginTransaction(void);
void KipControlMedium_Print(unsigned char Data_UB);
void KipControlMedium_Print(int Data_SI);
void KipControlMedium_Print(unsigned long Data_UL);
void KipControlMedium_Print(char * pData_UB);
void KipControlMedium_Print(String Data_Str);
void KipControlMedium_EndTransaction(void);

void KipControlMedium_Flush(void);

int KipControlMedium_DataAvailable(void);
int KipControlMedium_GetServerResponse(void);
int KipControlMedium_GetDataSize(void);
void KipControlMedium_Read(char * pData_UB);




#endif // __KIPCONTROL_MEDIUM_H__

