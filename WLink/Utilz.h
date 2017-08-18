/* ******************************************************************************** */
/*                                                                                  */
/* Utilz.h														                    */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for Utilz.cpp							                    	*/
/*		Utility functions.                                                          */
/*                                                                                  */
/* History :	28/02/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __UTILZ_H__
#define __UTILZ_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "RealTimeClock.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void Nop(void);
String HexArrayToString(unsigned char * pHexArray, unsigned long ItemNb_UL, String Separator_Str);
void DefaultKeyEvents(char * Key_UB);
void DefaultOnValidateFct(unsigned char * pParam_UB);
boolean isLeap(unsigned int Year_UI);
unsigned long getDeltaDay(RTC_DATE_STRUCT FromDate_X, RTC_DATE_STRUCT ToDate_X);
String dateToString(RTC_DATE_STRUCT Date_X);


#endif // __UTILZ_H__

