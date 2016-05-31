/* ******************************************************************************** */
/*                                                                                  */
/* Debug.h																			*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for Debug.cpp													*/
/*		Gathers the several functions to output debug information					*/
/*                                                                                  */
/* History :	31/05/2016	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __DEBUG_H__
#define __DEBUG_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include <Arduino.h>
#include <HardwareSerial.h>

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

#ifdef APP_USE_DEBUG
#define DBG_PRINT(Severity, Message)	(Debug_Print(Severity,MODULE_NAME,__LINE__,__FUNCTION__))->print(Message)
#define DBG_PRINTDATA(Data) 			(Debug_GetHandle())->print(Data)
#define DBG_PRINTDATABASE(Data, Base) 	(Debug_GetHandle())->print(Data, Base)
#define DBG_ENDSTR()					(Debug_GetHandle())->println()
#define DBG_PRINTLN(Severity, Message)	(Debug_Print(Severity,MODULE_NAME,__LINE__,__FUNCTION__))->println(Message)
#else
#define DBG_PRINT(Severity, Message)	/* Do Nothing */
#define DBG_PRINTDATA(Data)				/* Do Nothing */
#define DBG_PRINTDATABASE(Data, Base)	/* Do Nothing */
#define DBG_ENDSTR()					/* Do Nothing */
#define DBG_PRINTLN(Severity, Message)	/* Do Nothing */
#endif

#define DEBUG_DEFAULT_BAUDRATE	9600


/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
	DEBUG_SEVERITY_INFO,
	DEBUG_SEVERITY_WARNING,
	DEBUG_SEVERITY_ERROR
} DEBUG_SEVERITY_ENUM;

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void Debug_Init(HardwareSerial * pSerial_H);
void Debug_Init(HardwareSerial * pSerial_H, unsigned long Baudrate_UL);

HardwareSerial * Debug_GetHandle(void);
HardwareSerial * Debug_Print(DEBUG_SEVERITY_ENUM Severity_E, String pModuleName_cstr, unsigned long LineNb_UL, const String pFunctionName_cstr);

#endif // __DEBUG_H__