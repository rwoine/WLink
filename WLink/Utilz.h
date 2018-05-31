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

byte DecToBcd(byte b);
byte BcdToDec(byte b);
char HighNybbleToAscii(byte b);
char LowNybbleToAscii(byte b);

String HexArrayToString(unsigned char * pHexArray, unsigned long ItemNb_UL, String Separator_Str);
int GetIndexOfChar(const char * pCharArray_UB, char CharToSearch_UB, int FromIndex_SI = 0);

void DefaultKeyEvents(char * Key_UB);
boolean DefaultGetCondition(void * Handler_H);
void DefaultOnTransitionFct(void * Handler_H);
void DefaultOnProcessFct(void * Handler_H);
void DefaultOnValidateFct(unsigned char * pParam_UB);

boolean isLeap(unsigned int Year_UI);
signed long getDeltaDay(RTC_DATE_STRUCT FromDate_X, RTC_DATE_STRUCT ToDate_X);
String dateToString(RTC_DATE_STRUCT Date_X);


#endif // __UTILZ_H__

