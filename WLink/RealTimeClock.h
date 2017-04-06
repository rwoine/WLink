/* ******************************************************************************** */
/*                                                                                  */
/* RealTimeClock.h		                                                            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for RealTimeClock.cpp											*/
/*                                                                                  */
/* History :  	24/01/2017  (RW)	Creation of this file							*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __REAL_TIME_CLOCK_H__
#define __REAL_TIME_CLOCK_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <Arduino.h>
#include "RealTimeClockDS1307.h"
#include <Wire.h>

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
	boolean IsInitialized_B;
} RTC_PARAM;

typedef struct {
    unsigned char Hour_UB;
    unsigned char Min_UB;
    unsigned char Sec_UB;
} RTC_TIME_STRUCT;

typedef struct {
    unsigned char Day_UB;
    unsigned char Month_UB;
    unsigned char Year_UB;
} RTC_DATE_STRUCT;

typedef struct {
    RTC_TIME_STRUCT Time_X;
    RTC_DATE_STRUCT Date_X;
} RTC_DATETIME_STRUCT;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class RealTimeClock {
public:
	// Constructor
	RealTimeClock();

	// Functions
	void init(TwoWire * pWire_H, unsigned char PinSquareOut_UB);
	boolean isInitialized(void);

    void setDate(RTC_DATE_STRUCT Date_X);
    void setTime(RTC_TIME_STRUCT Time_X);
    void setDateTime(RTC_DATETIME_STRUCT DateTime_X);

    RTC_DATE_STRUCT getDate(void);
    RTC_TIME_STRUCT getTime(void);
    RTC_DATETIME_STRUCT getDateTime(void);
    String getDateTimeString(void);

	RTC_PARAM GL_RealTimeClockParam_X;
};

#endif // __REAL_TIME_CLOCK_H__
