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
typedef enum {
	INDICATOR_WEIGHT_STATUS_STABLE,
	INDICATOR_WEIGHT_STATUS_UNSTABLE,
	INDICATOR_WEIGHT_STATUS_OVERRANGE,
	INDICATOR_WEIGHT_STATUS_UNDEFINED
} INDICATOR_WEIGHT_STATUS_ENUM;

typedef enum {
	INDICATOR_WEIGHT_SIGN_POS,
	INDICATOR_WEIGHT_SIGN_NEG,
	INDICATOR_WEIGHT_SIGN_UNDEFINED
} INDICATOR_WEIGHT_SIGN_ENUM;

typedef struct {
	INDICATOR_WEIGHT_STATUS_ENUM Status_E;
	INDICATOR_WEIGHT_SIGN_ENUM Sign_E;
	unsigned int Value_UI;
	unsigned int Alibi_UI;
} INDICATOR_WEIGHT_STRUCT;

typedef struct {
	boolean IsInitialized_B;
	boolean HasEcho_B;
	boolean IsAlibi_B;
	boolean IsMsa_B;
	INDICATOR_WEIGHT_STRUCT Weight_X;
} INDICATOR_PARAM;

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

	boolean isInitialized();
	boolean isAlibi();
	boolean isMsa();
	boolean hasEcho();

	//void askWeight();
	//void askWeightMsa();
	//void askWeightWithAlibi();
	//void askLastAlibi();
	//void setToZero();


	INDICATOR_PARAM GL_IndicatorParam_X;
};

#endif // __INDICATOR_H__