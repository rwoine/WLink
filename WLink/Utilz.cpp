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

String HexArrayToString(unsigned char * pHexArray, unsigned long ItemNb_UL, String Separator_Str) {
    String Temp_Str = "";
    unsigned int Temp_UW = 0;

    for (int i = 0; i < ItemNb_UL; i++) {
        Temp_UW = pHexArray[i];
        Temp_Str += String((Temp_UW / 128), HEX);
        Temp_Str += String((Temp_UW % 128), HEX);
        if (i < ItemNb_UL - 1)
            Temp_Str += Separator_Str;
    }

    Temp_Str.toUpperCase();

    return Temp_Str;
}


void DefaultKeyEvents(char * Key_UB) {
    DBG_PRINT(DEBUG_SEVERITY_WARNING, "[");
    DBG_PRINTDATA((*Key_UB));
    DBG_PRINTDATA("] key pressed -> No callback assigned!");
    DBG_ENDSTR();
}


void DefaultOnValidateFct(unsigned char * pParam_UB) {
    DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "No function on Validate");
}

boolean isLeap(unsigned int Year_UI) {
    return ((((Year_UI % 4) == 0) && (((Year_UI % 100) != 0) || ((Year_UI % 400) == 0))) ? true : false);
}

unsigned long getDeltaDay(RTC_DATE_STRUCT FromDate_X, RTC_DATE_STRUCT ToDate_X) {
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

    return ((unsigned long)(Diff_D / 86400));
}
