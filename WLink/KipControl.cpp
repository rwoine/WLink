/* ******************************************************************************** */
/*                                                                                  */
/* KipControl.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the utilities functions for the KipControl object	                */
/*                                                                                  */
/* History :  	15/08/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"KipControl"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "WLink.h"
#include "KipControl.h"
#include "EepromWire.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* External Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
unsigned char GL_pBuffer_UB[8];

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
KipControl::KipControl() {

}

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
boolean KipControl::getConfiguredFlag(void) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_GLOBAL_DATA_ADDR, GL_pBuffer_UB, 1) == 1)
		return ((GL_pBuffer_UB[0] & 0x01) == 0x01);
	else
		return false;
}

boolean KipControl::getRunningFlag(void) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_GLOBAL_DATA_ADDR, GL_pBuffer_UB, 1) == 1)
		return ((GL_pBuffer_UB[0] & 0x02) == 0x02);
	else
		return false;
}

unsigned char KipControl::getTolerance(void) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_TOLERANCE_ADDR, GL_pBuffer_UB, 1) == 1)
		return GL_pBuffer_UB[0];
	else
		return 0;
}

unsigned char KipControl::getReferenceDataId(void) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_REFERENCE_DATA_ID_ADDR, GL_pBuffer_UB, 1) == 1)
		return GL_pBuffer_UB[0];
	else
		return 0;
}

unsigned char KipControl::getBatchId(void) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_BATCH_ID_ADDR, GL_pBuffer_UB, 1) == 1)
		return GL_pBuffer_UB[0];
	else
		return 0;
}

unsigned char KipControl::getMaxDataNb(void) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_MAX_DATA_NB_ADDR, GL_pBuffer_UB, 1) == 1)
		return GL_pBuffer_UB[0];
	else
		return 0;
}

unsigned char KipControl::getStartIdx(void) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_START_IDX_ADDR, GL_pBuffer_UB, 1) == 1)
		return GL_pBuffer_UB[0];
	else
		return 0;
}

RTC_DATE_STRUCT KipControl::getStartDate(void) {
	RTC_DATE_STRUCT Date_X;

	if (GL_GlobalData_X.Eeprom_H.read(KC_START_DATE_ADDR, GL_pBuffer_UB, 3) == 3) {
		Date_X.Day_UB = GL_pBuffer_UB[0];
		Date_X.Month_UB = GL_pBuffer_UB[1];
		Date_X.Year_UB = GL_pBuffer_UB[2];
	}
	else {
		Date_X.Day_UB = 0;
		Date_X.Month_UB = 0;
		Date_X.Year_UB = 0;
	}

	return Date_X;
}

unsigned char KipControl::getCurrentIdx(void) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_CURRENT_IDX_ADDR, GL_pBuffer_UB, 1) == 1)
		return GL_pBuffer_UB[0];
	else
		return 0;
}

unsigned long KipControl::getTotalValue(void) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_TOTAL_VALUE_ADDR, GL_pBuffer_UB, 4) == 4)
		return ((GL_pBuffer_UB[3] << 24) + (GL_pBuffer_UB[2] << 16) + (GL_pBuffer_UB[1] << 8) + GL_pBuffer_UB[0]);
	else
		return 0;
}

unsigned long KipControl::getValueNb(void) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_VALUE_NB_ADDR, GL_pBuffer_UB, 4) == 4)
		return ((GL_pBuffer_UB[3] << 24) + (GL_pBuffer_UB[2] << 16) + (GL_pBuffer_UB[1] << 8) + GL_pBuffer_UB[0]);
	else
		return 0;
}


void KipControl::setRunningFlag(boolean Running_B) {
	if (GL_GlobalData_X.Eeprom_H.read(KC_GLOBAL_DATA_ADDR, GL_pBuffer_UB, 1) == 1) {
		if (Running_B)
			GL_pBuffer_UB[0] |= 0x02;
		else
			GL_pBuffer_UB[0] &= ~0x02;
		GL_GlobalData_X.Eeprom_H.write(KC_GLOBAL_DATA_ADDR, GL_pBuffer_UB, 1);
	}
}

void KipControl::setReferenceDataId(unsigned char ReferenceDataId_UB) {
	GL_GlobalData_X.Eeprom_H.write(KC_REFERENCE_DATA_ID_ADDR, &ReferenceDataId_UB, 1);
}

void KipControl::setMaxDataNb(unsigned char MaxDataNb_UB) {
	GL_GlobalData_X.Eeprom_H.write(KC_MAX_DATA_NB_ADDR, &MaxDataNb_UB, 1);
}

void KipControl::setStartIdx(unsigned char StartIdx_UB) {
	GL_GlobalData_X.Eeprom_H.write(KC_START_IDX_ADDR, &StartIdx_UB, 1);
}

void KipControl::setStartDate(RTC_DATE_STRUCT StartDate_X) {
	GL_pBuffer_UB[0] = StartDate_X.Day_UB;
	GL_pBuffer_UB[1] = StartDate_X.Month_UB;
	GL_pBuffer_UB[2] = StartDate_X.Year_UB;
	GL_GlobalData_X.Eeprom_H.write(KC_START_DATE_ADDR, GL_pBuffer_UB, 3);
}

void KipControl::setCurrentIdx(unsigned char CurrentIdx_UB) {
	GL_GlobalData_X.Eeprom_H.write(KC_CURRENT_IDX_ADDR, &CurrentIdx_UB, 1);
}

void KipControl::setTotalValue(unsigned long TotalValue_UL) {
	GL_pBuffer_UB[0] = (unsigned char)(TotalValue_UL % 256);
	GL_pBuffer_UB[1] = (unsigned char)((TotalValue_UL >> 8) % 256);
	GL_pBuffer_UB[2] = (unsigned char)((TotalValue_UL >> 16) % 256);
	GL_pBuffer_UB[3] = (unsigned char)((TotalValue_UL >> 24) % 256);
	GL_GlobalData_X.Eeprom_H.write(KC_TOTAL_VALUE_ADDR, GL_pBuffer_UB, 4);
}

void KipControl::setValueNb(unsigned long ValueNb_UL) {
	GL_pBuffer_UB[0] = (unsigned char)(ValueNb_UL % 256);
	GL_pBuffer_UB[1] = (unsigned char)((ValueNb_UL >> 8) % 256);
	GL_pBuffer_UB[2] = (unsigned char)((ValueNb_UL >> 16) % 256);
	GL_pBuffer_UB[3] = (unsigned char)((ValueNb_UL >> 24) % 256);
	GL_GlobalData_X.Eeprom_H.write(KC_VALUE_NB_ADDR, GL_pBuffer_UB, 4);
}


void KipControl::incCurrentIdx(void) {
	setCurrentIdx(getCurrentIdx() + 1);
}

void KipControl::appendTotalValue(unsigned int Value_UW) {
	setTotalValue(getTotalValue() + Value_UW);
}

void KipControl::incValueNb(void) {
	setValueNb(getValueNb() + 1);
}
