/* ******************************************************************************** */
/*                                                                                  */
/* Utilz.cpp														                */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes some utility functions                                  			*/
/*                                                                                  */
/* History :  	28/02/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"Utilz"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "Utilz.h"
#include "Debug.h"
#include <time.h>
#include "RealTimeClock.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void Nop(void) {
    // Empty function
}


byte DecToBcd(byte b) {
    return (((b / 10) << 4) + (b % 10));
}

byte BcdToDec(byte b) {
    return (((b >> 4) * 10) + (b % 16));
}

char HighNybbleToAscii(byte b) {
    return LowNybbleToAscii(b >> 4);
}

char LowNybbleToAscii(byte b) {
    b = b & 0x0f;
    if (b < 10) {
        //0 is ASCII 48
        return 48 + b;
    }
    //A is ASCII 55
    return 55 + b;
}


String HexArrayToString(unsigned char * pHexArray, unsigned long ItemNb_UL, String Separator_Str) {
    String Temp_Str = "";
    unsigned int Temp_UW = 0;

    for (int i = 0; i < ItemNb_UL; i++) {
        Temp_UW = pHexArray[i];
        Temp_Str += String((Temp_UW / 16), HEX);
        Temp_Str += String((Temp_UW % 16), HEX);
        if (i < ItemNb_UL - 1)
            Temp_Str += Separator_Str;
    }

    Temp_Str.toUpperCase();

    return Temp_Str;
}


int GetIndexOfChar(const char * pCharArray_UB, char CharToSearch_UB, int FromIndex_SI) {
	int Index_SI = FromIndex_SI;
	boolean Found_B = false;

	while ((pCharArray_UB[Index_SI] != 0x00) && (!Found_B)) {

		if (pCharArray_UB[Index_SI] == CharToSearch_UB)
			Found_B = true;

		if(!Found_B)	
			Index_SI++;
	}	

	if (!Found_B)
		Index_SI = -1;

	return Index_SI;
}


void DefaultKeyEvents(char * Key_UB) {
    DBG_PRINT(DEBUG_SEVERITY_WARNING, "[");
    DBG_PRINTDATA((*Key_UB));
    DBG_PRINTDATA("] key pressed -> No callback assigned!");
    DBG_ENDSTR();
}

boolean DefaultGetCondition(void * Handler_H) {
	return false;
}

void DefaultOnTransitionFct(void * Handler_H) {
	//DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "No function on Transition");
}

void DefaultOnProcessFct(void * Handler_H) {

}

void DefaultOnValidateFct(unsigned char * pParam_UB) {
	//DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "No function on Validate");
}


boolean isLeap(unsigned int Year_UI) {
    return ((((Year_UI % 4) == 0) && (((Year_UI % 100) != 0) || ((Year_UI % 400) == 0))) ? true : false);
}

signed long getDeltaDay(RTC_DATE_STRUCT FromDate_X, RTC_DATE_STRUCT ToDate_X) {
    struct tm FromTime_X = { 0 };
    struct tm ToTime_X = { 0 };

    double Diff_D = 0;

    FromTime_X.tm_hour = 12;
    FromTime_X.tm_min = 0;
    FromTime_X.tm_sec = 0;
    FromTime_X.tm_year = (FromDate_X.Year_UB + 2000) - 1900;
    FromTime_X.tm_mon = FromDate_X.Month_UB - 1;
    FromTime_X.tm_mday = FromDate_X.Day_UB;
    FromTime_X.tm_isdst = -1;

    ToTime_X.tm_hour = 12;
    ToTime_X.tm_min = 0;
    ToTime_X.tm_sec = 0;
    ToTime_X.tm_year = (ToDate_X.Year_UB + 2000) - 1900;
    ToTime_X.tm_mon = ToDate_X.Month_UB - 1;
    ToTime_X.tm_mday = ToDate_X.Day_UB;
    ToTime_X.tm_isdst = -1;

    Diff_D = difftime(mktime(&ToTime_X), mktime(&FromTime_X));

    return ((signed long)(Diff_D / 86400));
}

String dateToString(RTC_DATE_STRUCT Date_X) {	
	char pDate_UB[9];
	sprintf(pDate_UB, "%02d/%02d/%02d", Date_X.Day_UB, Date_X.Month_UB, Date_X.Year_UB);

	return String(pDate_UB);
}
