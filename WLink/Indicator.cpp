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
#include "IndicatorInterface.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static HardwareSerial * GL_pIndicatorSerial_H;
static HardwareSerial * GL_pIndicatorEcho_H;

static INDICATOR_INTERFACE_DEVICES_ENUM GL_IndicatorDevice_E;

static unsigned char GL_pIndicatorBuffer_UB[INDICATOR_INTERFACE_MAX_RESP_SIZE];

extern INDICATOR_INTERFACE_STRUCT GL_pIndicatorInterface_X[INDICATOR_INTERFACE_DEVICES_NUM];

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


void Indicator::sendFrame(INDICATOR_INTERFACE_FRAME_ENUM Frame_E) {
	DBG_PRINT(DEBUG_SEVERITY_INFO, "Send Frame to Indicator [");	
	DBG_PRINTDATA(pIndicatorInterfaceDeviceLut_Str[GL_IndicatorDevice_E]);
	DBG_PRINTDATA("] : ");
	DBG_PRINTDATA(pIndicatorInterfaceFrameLut_Str[Frame_E]);
	DBG_ENDSTR();
	for (int i = 0; i < GL_pIndicatorInterface_X[GL_IndicatorDevice_E].pFrame[Frame_E].Size_UB; i++)
		GL_pIndicatorSerial_H->write(GL_pIndicatorInterface_X[GL_IndicatorDevice_E].pFrame[Frame_E].pWords_UB[i]);
}

boolean Indicator::isResponseAvailable(INDICATOR_INTERFACE_FRAME_ENUM Frame_E) {
	if (GL_pIndicatorSerial_H->available() >= GL_pIndicatorInterface_X[GL_IndicatorDevice_E].pFrame[Frame_E].RespSize_UB)
		return true;
	else
		return false;
}

void Indicator::processFrame(INDICATOR_INTERFACE_FRAME_ENUM Frame_E) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive Data from Serial Buffer");
	if (GL_IndicatorParam_X.HasEcho_B)
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "(With ECHO)");
	for (int i = 0; i < GL_pIndicatorInterface_X[GL_IndicatorDevice_E].pFrame[Frame_E].RespSize_UB; i++) {
		GL_pIndicatorBuffer_UB[i] = (unsigned char)GL_pIndicatorSerial_H->read();
		if (GL_IndicatorParam_X.HasEcho_B)
			GL_pIndicatorEcho_H->write(GL_pIndicatorBuffer_UB[i]);
	}

	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Process Data with Low-Level Function");
	GL_pIndicatorInterface_X[GL_IndicatorDevice_E].FctHandler(GL_pIndicatorBuffer_UB, Frame_E, &(GL_IndicatorParam_X.Weight_X));

	if (GL_IndicatorParam_X.IsAlibi_B) {
		DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Reset Alibi Flag");
		GL_IndicatorParam_X.IsAlibi_B = false;
	}
}


void Indicator::flushIndicator(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Flush Serial Buffer of Indicator");
	GL_pIndicatorSerial_H->flush();
}


INDICATOR_WEIGHT_STATUS_ENUM Indicator::getWeightStatus() {
	return GL_IndicatorParam_X.Weight_X.Status_E;
}

INDICATOR_WEIGHT_SIGN_ENUM Indicator::getWeightSign() {
	return GL_IndicatorParam_X.Weight_X.Sign_E;
}

signed int Indicator::getWeightValue() {
	if(GL_IndicatorParam_X.Weight_X.Sign_E == INDICATOR_WEIGHT_SIGN_NEG)
		return (-GL_IndicatorParam_X.Weight_X.Value_UI);
	else
		return GL_IndicatorParam_X.Weight_X.Value_UI;
}

unsigned int Indicator::getWeightUnsignedValue() {
	return GL_IndicatorParam_X.Weight_X.Value_UI;
}

unsigned int Indicator::getAlibiValue() {
	return GL_IndicatorParam_X.Weight_X.Alibi_UI;
}
