/* ******************************************************************************** */
/*                                                                                  */
/* RealTimeClock.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the RTC utilities functions										*/
/*                                                                                  */
/* History :  	24/01/2017  (RW)	Creation of this file                           */
/*																					*/
/* ******************************************************************************** */

#define MODULE_NAME		"RealTimeClock"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "RealTimeClock.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static TwoWire * GL_pRtcWire_H;
static RealTimeClockDS1307 GL_RtcDevice_H;
static unsigned char GL_PinSquareOut_UB;
static char GL_pRTCFormatString_UB[] = "00/00/00 00:00:00x";
static char GL_pRTCTimestampFormatString_UB[] = "0000-00-00+00:00:00x";

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
RealTimeClock::RealTimeClock() {
	GL_RealTimeClockParam_X.IsInitialized_B = false;
}

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void RealTimeClock::init(TwoWire * pWire_H, unsigned char PinSquareOut_UB) {
	GL_pRtcWire_H = pWire_H;
	GL_PinSquareOut_UB = PinSquareOut_UB;
	pinMode(GL_PinSquareOut_UB, INPUT);
    GL_RtcDevice_H.set24h();
	GL_RealTimeClockParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Real-Time Clock Module Initialized");
}

boolean RealTimeClock::isInitialized(void) {
	return GL_RealTimeClockParam_X.IsInitialized_B;
}

void RealTimeClock::setDate(RTC_DATE_STRUCT Date_X) {
    GL_RtcDevice_H.setDay(Date_X.Day_UB);
    GL_RtcDevice_H.setMonth(Date_X.Month_UB);
    GL_RtcDevice_H.setYear(Date_X.Year_UB);
    GL_RtcDevice_H.setDate();
}

void RealTimeClock::setTime(RTC_TIME_STRUCT Time_X) {
    GL_RtcDevice_H.setSeconds(Time_X.Sec_UB);
    GL_RtcDevice_H.setMinutes(Time_X.Min_UB);
    GL_RtcDevice_H.setHours(Time_X.Hour_UB);
    GL_RtcDevice_H.setTime();
}

void RealTimeClock::setDateTime(RTC_DATETIME_STRUCT DateTime_X) {
    GL_RtcDevice_H.setSeconds(DateTime_X.Time_X.Sec_UB);
    GL_RtcDevice_H.setMinutes(DateTime_X.Time_X.Min_UB);
    GL_RtcDevice_H.setHours(DateTime_X.Time_X.Hour_UB);
    GL_RtcDevice_H.setDay(DateTime_X.Date_X.Day_UB);
    GL_RtcDevice_H.setMonth(DateTime_X.Date_X.Month_UB);
    GL_RtcDevice_H.setYear(DateTime_X.Date_X.Year_UB);
    GL_RtcDevice_H.setClock();
}


RTC_DATE_STRUCT RealTimeClock::getDate(void) {
    RTC_DATE_STRUCT Date_X;

    GL_RtcDevice_H.readClock();

    Date_X.Day_UB = GL_RtcDevice_H.getDay();
    Date_X.Month_UB = GL_RtcDevice_H.getMonth();
    Date_X.Year_UB = GL_RtcDevice_H.getYear();

    return Date_X;
}

RTC_TIME_STRUCT RealTimeClock::getTime(void) {
    RTC_TIME_STRUCT Time_X;

    GL_RtcDevice_H.readClock();

    Time_X.Hour_UB = GL_RtcDevice_H.getHours();
    Time_X.Min_UB = GL_RtcDevice_H.getMinutes();
    Time_X.Sec_UB = GL_RtcDevice_H.getSeconds();

    return Time_X;
}

RTC_DATETIME_STRUCT RealTimeClock::getDateTime(void) {
    RTC_DATETIME_STRUCT DateTime_X;

    DateTime_X.Date_X = getDate();
    DateTime_X.Time_X = getTime();

    return DateTime_X;
}

String RealTimeClock::getDateTimeString(void) {
    getDateTime();
    GL_RtcDevice_H.getFormatted(GL_pRTCFormatString_UB, false);
    return GL_pRTCFormatString_UB;
}

String RealTimeClock::getTimestamp(void) {
    getDateTime();
    GL_RtcDevice_H.getFormatted2k(GL_pRTCTimestampFormatString_UB);
    return GL_pRTCTimestampFormatString_UB;
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
