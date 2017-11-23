/* ******************************************************************************** */
/*                                                                                  */
/* RealTimeClock.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the RTC utilities functions										*/
/*                                                                                  */
/* History :  	24/01/2017  (RW)	Creation of this file                           */
/*              23/11/2017  (RW)    Remove external library                         */
/*																					*/
/* ******************************************************************************** */

#define MODULE_NAME		"RealTimeClock"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "Utilz.h"
#include "RealTimeClock.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define REAL_TIME_CLOCK_DS1339C_ADDR    0x68

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static TwoWire * GL_pRtcWire_H;
static unsigned char GL_PinSquareOut_UB;

static RTC_DATETIME_STRUCT GL_InternalDateTime_X;
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
    GL_pRtcWire_H->begin();
	GL_PinSquareOut_UB = PinSquareOut_UB;
	pinMode(GL_PinSquareOut_UB, INPUT);
    GL_RealTimeClockParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Real-Time Clock Module Initialized");
}

boolean RealTimeClock::isInitialized(void) {
	return GL_RealTimeClockParam_X.IsInitialized_B;
}

void RealTimeClock::setDate(RTC_DATE_STRUCT Date_X) {

    byte Day_UB = DecToBcd(Date_X.Day_UB);
    byte Month_UB = DecToBcd(Date_X.Month_UB);
    byte Year_UB = DecToBcd(Date_X.Year_UB);

    GL_pRtcWire_H->beginTransmission(REAL_TIME_CLOCK_DS1339C_ADDR);
    GL_pRtcWire_H->write(0x04);
    GL_pRtcWire_H->write(Day_UB);
    GL_pRtcWire_H->write(Month_UB);
    GL_pRtcWire_H->write(Year_UB);
    GL_pRtcWire_H->endTransmission();
}

void RealTimeClock::setTime(RTC_TIME_STRUCT Time_X) {

    byte Sec_UB = DecToBcd(Time_X.Sec_UB);
    byte Min_UB = DecToBcd(Time_X.Min_UB);
    byte Hour_UB = DecToBcd(Time_X.Hour_UB);

    GL_pRtcWire_H->beginTransmission(REAL_TIME_CLOCK_DS1339C_ADDR);
    GL_pRtcWire_H->write(0x00);
    GL_pRtcWire_H->write(Sec_UB);
    GL_pRtcWire_H->write(Min_UB);
    GL_pRtcWire_H->write(Hour_UB);
    GL_pRtcWire_H->endTransmission();
}

void RealTimeClock::setDateTime(RTC_DATETIME_STRUCT DateTime_X) {

    byte Sec_UB = DecToBcd(DateTime_X.Time_X.Sec_UB);
    byte Min_UB = DecToBcd(DateTime_X.Time_X.Min_UB);
    byte Hour_UB = DecToBcd(DateTime_X.Time_X.Hour_UB);
    byte Day_UB = DecToBcd(DateTime_X.Date_X.Day_UB);
    byte Month_UB = DecToBcd(DateTime_X.Date_X.Month_UB);
    byte Year_UB = DecToBcd(DateTime_X.Date_X.Year_UB);

    GL_pRtcWire_H->beginTransmission(REAL_TIME_CLOCK_DS1339C_ADDR);
    GL_pRtcWire_H->write(0x00);
    GL_pRtcWire_H->write(Sec_UB);
    GL_pRtcWire_H->write(Min_UB);
    GL_pRtcWire_H->write(Hour_UB);
    GL_pRtcWire_H->write(0x01); // force day of the week to 1 as it in not use here
    GL_pRtcWire_H->write(Day_UB);
    GL_pRtcWire_H->write(Month_UB);
    GL_pRtcWire_H->write(Year_UB);
    GL_pRtcWire_H->endTransmission();
}


RTC_DATE_STRUCT RealTimeClock::getDate(void) {
    RTC_DATE_STRUCT Date_X;

    GL_pRtcWire_H->beginTransmission(REAL_TIME_CLOCK_DS1339C_ADDR);
    GL_pRtcWire_H->write(0x04);
    GL_pRtcWire_H->endTransmission();
    GL_pRtcWire_H->requestFrom(REAL_TIME_CLOCK_DS1339C_ADDR, 3);

    Date_X.Day_UB = BcdToDec(GL_pRtcWire_H->read());
    Date_X.Month_UB = BcdToDec(GL_pRtcWire_H->read());
    Date_X.Year_UB = BcdToDec(GL_pRtcWire_H->read());

    // Record
    GL_InternalDateTime_X.Date_X = Date_X;

    return Date_X;
}

RTC_DATE_STRUCT RealTimeClock::getLastDate(void) {
    return GL_InternalDateTime_X.Date_X;
}

RTC_TIME_STRUCT RealTimeClock::getTime(void) {
    RTC_TIME_STRUCT Time_X;

    GL_pRtcWire_H->beginTransmission(REAL_TIME_CLOCK_DS1339C_ADDR);
    GL_pRtcWire_H->write(0x00);
    GL_pRtcWire_H->endTransmission();
    GL_pRtcWire_H->requestFrom(REAL_TIME_CLOCK_DS1339C_ADDR, 3);

    Time_X.Sec_UB = BcdToDec((GL_pRtcWire_H->read()) & 0x7F);   // mask oscillator bit
    Time_X.Min_UB = BcdToDec(GL_pRtcWire_H->read());
    Time_X.Hour_UB = BcdToDec((GL_pRtcWire_H->read()) & 0x3F);  // get in 24h-mode

    // Record
    GL_InternalDateTime_X.Time_X = Time_X;

    return Time_X;
}

RTC_DATETIME_STRUCT RealTimeClock::getDateTime(void) {
    RTC_DATETIME_STRUCT DateTime_X;

    GL_pRtcWire_H->beginTransmission(REAL_TIME_CLOCK_DS1339C_ADDR);
    GL_pRtcWire_H->write(0x00);
    GL_pRtcWire_H->endTransmission();
    GL_pRtcWire_H->requestFrom(REAL_TIME_CLOCK_DS1339C_ADDR, 7);

    DateTime_X.Time_X.Sec_UB = BcdToDec((GL_pRtcWire_H->read()) & 0x7F);   // mask oscillator bit
    DateTime_X.Time_X.Min_UB = BcdToDec(GL_pRtcWire_H->read());
    DateTime_X.Time_X.Hour_UB = BcdToDec((GL_pRtcWire_H->read()) & 0x3F);  // get in 24h-mode
    GL_pRtcWire_H->read(); // drop day of week
    DateTime_X.Date_X.Day_UB = BcdToDec(GL_pRtcWire_H->read());
    DateTime_X.Date_X.Month_UB = BcdToDec(GL_pRtcWire_H->read());
    DateTime_X.Date_X.Year_UB = BcdToDec(GL_pRtcWire_H->read());

    // Record
    GL_InternalDateTime_X = DateTime_X;

    return DateTime_X;
}

String RealTimeClock::getDateTimeString(void) {
    int i = 0;

    // Call function to have the record
    getDateTime();

    // Target string format: DD/MM/YY hh:mm:ss
    GL_pRTCFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Day_UB));
    GL_pRTCFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Day_UB));
    i++;
    GL_pRTCFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Month_UB));
    GL_pRTCFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Month_UB));
    i++;
    GL_pRTCFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Year_UB));
    GL_pRTCFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Year_UB));
    i++;

    GL_pRTCFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Hour_UB));
    GL_pRTCFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Hour_UB));
    i++;
    GL_pRTCFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Min_UB));
    GL_pRTCFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Min_UB));
    i++;
    GL_pRTCFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Sec_UB));
    GL_pRTCFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Sec_UB));

    GL_pRTCFormatString_UB[i++] = 0x00;

    return GL_pRTCFormatString_UB;
}

String RealTimeClock::getTimestamp(void) {
    int i = 0;

    // Call function to have the record
    getDateTime();

    // Target string format: YYYY/MM/DD hh:mm:ss
    GL_pRTCTimestampFormatString_UB[i++] = '2';
    GL_pRTCTimestampFormatString_UB[i++] = '0';
    GL_pRTCTimestampFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Year_UB));
    GL_pRTCTimestampFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Year_UB));
    i++;
    GL_pRTCTimestampFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Month_UB));
    GL_pRTCTimestampFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Month_UB));
    i++;
    GL_pRTCTimestampFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Day_UB));
    GL_pRTCTimestampFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Date_X.Day_UB));
    i++;

    GL_pRTCTimestampFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Hour_UB));
    GL_pRTCTimestampFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Hour_UB));
    i++;
    GL_pRTCTimestampFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Min_UB));
    GL_pRTCTimestampFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Min_UB));
    i++;
    GL_pRTCTimestampFormatString_UB[i++] = HighNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Sec_UB));
    GL_pRTCTimestampFormatString_UB[i++] = LowNybbleToAscii(DecToBcd(GL_InternalDateTime_X.Time_X.Sec_UB));

    GL_pRTCTimestampFormatString_UB[i++] = 0x00;

    return GL_pRTCTimestampFormatString_UB;
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
