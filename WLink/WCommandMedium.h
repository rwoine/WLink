/* ******************************************************************************** */
/*                                                                                  */
/* WCommandMedium.h                                                                 */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for WCommandMedium.cpp											*/
/*		Enables the selection of the communication medium for the commands channel	*/
/*                                                                                  */
/* History :	14/05/2016	(RW)	Creation of this file                           */
/*              01/03/2017  (RW)    Add GSM as possible medium                      */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __WCOMMAND_MEDIUM_H__
#define __WCOMMAND_MEDIUM_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include <Arduino.h>

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

typedef enum {
	WCMD_MEDIUM_SERIAL = 0,
	WCMD_MEDIUM_UDP,
	WCMD_MEDIUM_TCP,
    WCMD_MEDIUM_GSM
} WCMD_MEDIUM_ENUM;


/* ******************************************************************************** */
/* Functions Prototypes - Configuration
/* ******************************************************************************** */
void WCmdMedium_Init(WCMD_MEDIUM_ENUM WCmdMedium_E, void * pMedium_H, boolean isMonoClient_B);


/* ******************************************************************************** */
/* Functions Prototypes - Exported
/* ******************************************************************************** */
boolean WCmdMedium_IsMonoClient(void);
boolean WCmdMedium_IsRunning(void);
int WCmdMedium_DataAvailable(void);
unsigned char WCmdMedium_ReadByte(void);
void WCmdMedium_WriteByte(unsigned char Byte_UB);
void WCmdMedium_Write(unsigned char * pBuffer_UB, unsigned long NbData_UL);
void WCmdMedium_Flush(void);
void WCmdMedium_Stop(void);
void WCmdMedium_BeginPacket(void);
void WCmdMedium_EndPacket(void);


#endif // __WCOMMAND_MEDIUM_H__