/* ******************************************************************************** */
/*                                                                                  */
/* Indicator.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the indicator utilities functions					                */
/*                                                                                  */
/* History :  	01/12/2014  (RW)	Creation of this file                           */
/*				12/01/2015  (RW)	Manage indicator with low-level functions       */
/*				06/06/2016	(RW)	Re-mastered version								*/	
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"Indicator"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "Indicator.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static HardwareSerial * GL_pIndicatorSerial_H;
static HardwareSerial * GL_pIndicatorEcho_H;

static INDICATOR_INTERFACE_DEVICES_ENUM GL_IndicatorDevice_E;

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
Indicator::Indicator() {
	GL_IndicatorParam_X.IsInitialized_B = false;
	GL_IndicatorParam_X.HasEcho_B = false;
	GL_IndicatorParam_X.IsAlibi_B = false;
	GL_IndicatorParam_X.IsMsa_B = false;
	GL_IndicatorParam_X.Weight_X.Status_E = INDICATOR_WEIGHT_STATUS_UNDEFINED;
	GL_IndicatorParam_X.Weight_X.Sign_E = INDICATOR_WEIGHT_SIGN_UNDEFINED;
	GL_IndicatorParam_X.Weight_X.Value_UI = 0;
	GL_IndicatorParam_X.Weight_X.Alibi_UI = 0;
}

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void Indicator::init(HardwareSerial * pSerial_H) {
	GL_pIndicatorSerial_H = pSerial_H;
	GL_pIndicatorSerial_H->begin(INDICATOR_DEFAULT_BAUDRATE);
	GL_IndicatorParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Indicator Initialized");
}

void Indicator::init(HardwareSerial * pSerial_H, unsigned long BaudRate_UL) {
	GL_pIndicatorSerial_H = pSerial_H;
	GL_pIndicatorSerial_H->begin(BaudRate_UL);
	GL_IndicatorParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Indicator Initialized");
}

void Indicator::setIndicatorDevice(INDICATOR_INTERFACE_DEVICES_ENUM Device_E) {
	GL_IndicatorDevice_E = Device_E;
}

void Indicator::attachEcho(HardwareSerial * pSerial_H) {
	GL_pIndicatorEcho_H = pSerial_H;
	GL_pIndicatorEcho_H->begin(INDICATOR_ECHO_DEFAULT_BAUDRATE);
	GL_IndicatorParam_X.HasEcho_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Attach Echo to Indicator");
}

void Indicator::attachEcho(HardwareSerial * pSerial_H, unsigned long BaudRate_UL) {
	GL_pIndicatorEcho_H = pSerial_H;
	GL_pIndicatorEcho_H->begin(BaudRate_UL);
	GL_IndicatorParam_X.HasEcho_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Attach Echo to Indicator");
}

void Indicator::detachEcho(void) {
	GL_pIndicatorEcho_H->flush();
	GL_pIndicatorEcho_H->end();
	GL_IndicatorParam_X.HasEcho_B = false;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Detach Echo from Indicator");
}

void Indicator::enableMsa(boolean Enable_B) {
	GL_IndicatorParam_X.IsMsa_B = Enable_B;
}

void Indicator::enableAlibi(boolean Enable_B) {
	GL_IndicatorParam_X.IsAlibi_B = Enable_B;
}

boolean Indicator::isInitialized() {
	return GL_IndicatorParam_X.IsInitialized_B;
}

boolean Indicator::isAlibi() {
	return GL_IndicatorParam_X.IsAlibi_B;
}

boolean Indicator::isMsa() {
	return GL_IndicatorParam_X.IsMsa_B;
}

boolean Indicator::hasEcho() {
	return GL_IndicatorParam_X.HasEcho_B;
}
