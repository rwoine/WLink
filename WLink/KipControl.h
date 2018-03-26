/* ******************************************************************************** */
/*                                                                                  */
/* KipControl.h																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for KipControl.cpp												*/
/*                                                                                  */
/* History :	15/08/2017	(RW)	Creation of this file                           */
/*              26/03/2018  (RW)    Update BatchId to unsigned long                 */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __KIP_CONTROL_H__
#define __KIP_CONTROL_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include <Arduino.h>
#include "RealTimeClock.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define KC_WORKING_AREA_OFFSET          0x0400
#define KC_GLOBAL_DATA_ADDR             (KC_WORKING_AREA_OFFSET + 0x0001)
#define KC_TOLERANCE_ADDR               (KC_WORKING_AREA_OFFSET + 0x0002)
#define KC_WEIGHT_MIN_ADDR				(KC_WORKING_AREA_OFFSET + 0x0003)
#define KC_BATCH_ID_ADDR				(KC_WORKING_AREA_OFFSET + 0x0005)
#define KC_REFERENCE_DATA_ID_ADDR       (KC_WORKING_AREA_OFFSET + 0x0009)
#define KC_START_IDX_ADDR               (KC_WORKING_AREA_OFFSET + 0x000A)
#define KC_START_DATE_ADDR              (KC_WORKING_AREA_OFFSET + 0x000B)
#define KC_CURRENT_IDX_ADDR             (KC_WORKING_AREA_OFFSET + 0x000E)
#define KC_TOTAL_VALUE_ADDR             (KC_WORKING_AREA_OFFSET + 0x000F)
#define KC_VALUE_NB_ADDR                (KC_WORKING_AREA_OFFSET + 0x0013)

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class KipControl {
public:
	// Constructor
	KipControl();

	// Functions
	boolean getConfiguredFlag(void);
	boolean getRunningFlag(void);
	unsigned char getTolerance(void);
	signed int getWeightMin(void);
	unsigned char getReferenceDataId(void);
	unsigned long getBatchId(void);
	unsigned char getStartIdx(void);
	RTC_DATE_STRUCT getStartDate(void);
	unsigned char getCurrentIdx(void);
	unsigned long getTotalValue(void);
	unsigned long getValueNb(void);

    void setConfiguredFlag(boolean Configured_B);
	void setRunningFlag(boolean Running_B);
	void setTolerance(unsigned char Tolerance_UB);
	void setWeightMin(signed int WeightMin_SI);
	void setReferenceDataId(unsigned char ReferenceDataId_UB);
	void setBatchId(unsigned long BatchId_UL);
	void setStartIdx(unsigned char StartIdx_UB);
	void setStartDate(RTC_DATE_STRUCT StartDate_X);
	void setCurrentIdx(unsigned char CurrentIdx_UB);
	void setTotalValue(unsigned long TotalValue_UL);
	void setValueNb(unsigned long ValueNb_UL);

	void incCurrentIdx(void);
	void appendTotalValue(unsigned int Value_UW);
	void incValueNb(void);

};

#endif // __KIP_CONTROL_H__

