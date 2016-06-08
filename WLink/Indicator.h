/* ******************************************************************************** */
/*                                                                                  */
/* Indicator.h			                                                            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for Indicator.cpp												*/
/*                                                                                  */
/* History :  	06/06/2015  (RW)	Creation of this file							*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __INDICATOR_H__
#define __INDICATOR_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <Arduino.h>
#include <HardwareSerial.h>

#include "IndicatorInterface.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define INDICATOR_DEFAULT_BAUDRATE			2400
#define INDICATOR_ECHO_DEFAULT_BAUDRATE		9600

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class Indicator {
public:
	// Constructor
	Indicator();

	// Functions
	void init(HardwareSerial * pSerial_H);
	void init(HardwareSerial * pSerial_H, unsigned long BaudRate_UL);

	void setIndicatorDevice(INDICATOR_INTERFACE_DEVICES_ENUM Device_E);

	void attachEcho(HardwareSerial * pSerial_H);
	void attachEcho(HardwareSerial * pSerial_H, unsigned long BaudRate_UL);
	void detachEcho(void);

	void enableMsa(boolean Enable_B);
	void enableAlibi(boolean Enable_B);

	boolean isInitialized(void);
	boolean isAlibi(void);
	boolean isMsa(void);
	boolean hasEcho(void);

	void sendFrame(INDICATOR_INTERFACE_FRAME_ENUM Frame_E);
	boolean isResponseAvailable(INDICATOR_INTERFACE_FRAME_ENUM Frame_E);
	void processFrame(INDICATOR_INTERFACE_FRAME_ENUM Frame_E);

	void flushIndicator(void);

	INDICATOR_WEIGHT_STATUS_ENUM getWeightStatus();
	INDICATOR_WEIGHT_SIGN_ENUM getWeightSign();
	signed int getWeightValue();
	unsigned int getWeightUnsignedValue();
	unsigned int getAlibiValue();


	INDICATOR_PARAM GL_IndicatorParam_X;
};

#endif // __INDICATOR_H__