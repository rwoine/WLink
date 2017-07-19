/* ******************************************************************************** */
/*                                                                                  */
/* WCommand.cpp																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the functions related to each module                              */
/*                                                                                  */
/* History :  	16/02/2015  (RW)	Creation of this file                           */
/*				14/05/2016	(RW)	Re-mastered version								*/
/*				17/10/2016	(RW)	Add port COM functions							*/
/*									Add EEPROM functions							*/
/*              25/01/2017  (RW)    Add RTC functions                               */
/*              04/06/2017  (RW)    Add COM port tunnel functions                   */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"WCommand"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WLink.h"
#include "WCommand.h"

#include "SerialHandler.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

/* Global ************************************************************************* */
/* ******************************************************************************** */

WCMD_FCT_STS WCmdProcess_GetRevisionId(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_GetRevisionId");
	*pAnsNb_UL = 8;

	for (int i = 0; i < 8; i++)
		pAns_UB[i] = GL_GlobalData_X.pRevisionId_UB[i];

	return WCMD_FCT_STS_OK;
}

/* GPIO *************************************************************************** */
/* ******************************************************************************** */

WCMD_FCT_STS WCmdProcess_GpioRead(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_GpioRead");
	*pAnsNb_UL = 1;

	pAns_UB[0] = 0x00;

	// Read Inputs
	for (int i = 0; i < 4; i++) {
		if (digitalRead(GL_GlobalData_X.pGpioInputIndex_UB[i]))
			pAns_UB[0] |= (0x01 << i);
	}

	// Read Otuputs
	for (int i = 0; i < 4; i++) {
		if (digitalRead(GL_GlobalData_X.pGpioOutputIndex_UB[i]))
			pAns_UB[0] |= (0x01 << (i + 4));
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Read Values = ");
	DBG_PRINTDATABASE(pAns_UB[0], HEX);
	DBG_ENDSTR();

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_GpioWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_GpioWrite");
	*pAnsNb_UL = 0;

	if (ParamNb_UL == 0)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	// Write Outputs Only
	for (int i = 0; i < 4; i++) {
		if ((pParam_UB[0] & (0x01 << (i + 4))) == (0x01 << (i + 4)))
			digitalWrite(GL_GlobalData_X.pGpioOutputIndex_UB[i], HIGH);
		else
			digitalWrite(GL_GlobalData_X.pGpioOutputIndex_UB[i], LOW);
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Write Values = ");
	DBG_PRINTDATABASE((pParam_UB[0] & 0xF0), HEX);
	DBG_ENDSTR();

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_GpioSetBit(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_GpioSetBit");
	*pAnsNb_UL = 0;

	if (ParamNb_UL == 0)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	// Write Outputs Only if bit High
	for (int i = 0; i < 4; i++) {
		if ((pParam_UB[0] & (0x01 << (i + 4))) == (0x01 << (i + 4))) {
			digitalWrite(GL_GlobalData_X.pGpioOutputIndex_UB[i], HIGH);
		}
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Write Values (Set-Bit) = ");
	DBG_PRINTDATABASE((pParam_UB[0] & 0xF0), HEX);
	DBG_ENDSTR();

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_GpioClrBit(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_GpioClrBit");
	*pAnsNb_UL = 0;

	if (ParamNb_UL == 0)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	// Write Outputs Only if bit High
	for (int i = 0; i < 4; i++) {
		if ((pParam_UB[0] & (0x01 << (i + 4))) == (0x01 << (i + 4)))
			digitalWrite(GL_GlobalData_X.pGpioOutputIndex_UB[i], LOW);
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Write Values (Clr-Bit) = ");
	DBG_PRINTDATABASE((pParam_UB[0] & 0xF0), HEX);
	DBG_ENDSTR();

	return WCMD_FCT_STS_OK;
}

/* Indicator ********************************************************************** */
/* ******************************************************************************** */
WCMD_FCT_STS WCmdProcess_IndicatorGetWeight(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_IndicatorGetWeight");
	*pAnsNb_UL = 4;

	pAns_UB[0] = (unsigned char)(GL_GlobalData_X.Indicator_H.getWeightStatus());
	pAns_UB[1] = (unsigned char)(GL_GlobalData_X.Indicator_H.getWeightSign());
	pAns_UB[2] = (unsigned char)(GL_GlobalData_X.Indicator_H.getWeightUnsignedValue());
	pAns_UB[3] = (unsigned char)(GL_GlobalData_X.Indicator_H.getWeightUnsignedValue() >> 8);

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_IndicatorGetWeightAlibi(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_IndicatorGetWeightAlibi");
	*pAnsNb_UL = 0;

	return WCMD_FCT_STS_ERROR;
}

WCMD_FCT_STS WCmdProcess_IndicatorSetZero(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_IndicatorSetZero");
	*pAnsNb_UL = 0;

	IndicatorManager_SetZeroIndicator();

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_IndicatorGetWeightAscii(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_IndicatorGetWeightAscii");
	*pAnsNb_UL = 8;

	unsigned long Remaining1_UL = GL_GlobalData_X.Indicator_H.getWeightUnsignedValue();  // 987.654
	unsigned long Remaining2_UL = 0;

	pAns_UB[0] = (unsigned char)(GL_GlobalData_X.Indicator_H.getWeightStatus());
	pAns_UB[1] = (unsigned char)(GL_GlobalData_X.Indicator_H.getWeightSign());

	Remaining2_UL = (Remaining1_UL - Remaining1_UL % 100000);       // 900.000 = 987.654 - 87.654
	pAns_UB[2] = (unsigned char)(0x30 + (Remaining2_UL / 100000));  // 0x30 + 9

	Remaining1_UL = Remaining1_UL - Remaining2_UL;                // 87.654 = 987.654 - 900.000
	Remaining2_UL = (Remaining1_UL - Remaining1_UL % 10000);        // 80.000 = 87.854 - 7.854 
	pAns_UB[3] = (unsigned char)(0x30 + (Remaining2_UL / 10000));   // 0x30 + 8

	Remaining1_UL = Remaining1_UL - Remaining2_UL;
	Remaining2_UL = (Remaining1_UL - Remaining1_UL % 1000);
	pAns_UB[4] = (unsigned char)(0x30 + (Remaining2_UL / 1000));

	Remaining1_UL = Remaining1_UL - Remaining2_UL;
	Remaining2_UL = (Remaining1_UL - Remaining1_UL % 100);
	pAns_UB[5] = (unsigned char)(0x30 + (Remaining2_UL / 100));

	Remaining1_UL = Remaining1_UL - Remaining2_UL;
	Remaining2_UL = (Remaining1_UL - Remaining1_UL % 10);
	pAns_UB[6] = (unsigned char)(0x30 + (Remaining2_UL / 10));

	Remaining1_UL = Remaining1_UL - Remaining2_UL;
	Remaining2_UL = (Remaining1_UL - Remaining1_UL % 1);
	pAns_UB[7] = (unsigned char)(0x30 + (Remaining2_UL / 1));

	return WCMD_FCT_STS_OK;
}

/* Badge Reader ******************************************************************* */
/* ******************************************************************************** */
WCMD_FCT_STS WCmdProcess_BadgeReaderGetBadgeId(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_BadgeReaderGetBadgeId");
	*pAnsNb_UL = 0;

	if (!BadgeReaderManager_IsBadgeAvailable()) {
		*pAnsNb_UL = 0;
	}
	else {
		*pAnsNb_UL = 10;
		for (int i = 0; i < 10; i++)
			pAns_UB[i] = BadgeReaderManager_GetBadgeChar(i);
	}

	return WCMD_FCT_STS_OK;
}


/* LCD **************************************************************************** */
/* ******************************************************************************** */
WCMD_FCT_STS WCmdProcess_LcdWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_LcdWrite");
	*pAnsNb_UL = 0;

	if (ParamNb_UL < 2)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	GL_GlobalData_X.Lcd_H.writeDisplay((LCD_DISPLAY_LINE_ENUM) pParam_UB[0], (unsigned char *) &(pParam_UB[1]), (ParamNb_UL-1));

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_LcdRead(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_LcdRead");
	*pAnsNb_UL = 0;

	if (ParamNb_UL != 1)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	GL_GlobalData_X.Lcd_H.readDisplayShadowContent((LCD_DISPLAY_LINE_ENUM)pParam_UB[0], pAns_UB, pAnsNb_UL);

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_LcdClear(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_LcdClear");
	*pAnsNb_UL = 0;

	if (ParamNb_UL != 1)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	GL_GlobalData_X.Lcd_H.clearDisplay((LCD_DISPLAY_LINE_ENUM)pParam_UB[0]);

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_LcdSetBacklight(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_LcdSetBacklight");
	*pAnsNb_UL = 0;

	if (ParamNb_UL != 1)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	GL_GlobalData_X.Lcd_H.setBacklight(pParam_UB[0]);

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_LcdEnableExternalWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_LcdEnableExternalWrite");
	*pAnsNb_UL = 0;

	if (ParamNb_UL != 2)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	GL_GlobalData_X.Lcd_H.enableExternalWrite(pParam_UB[0], pParam_UB[1]);

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_LcdDisableExternalWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_LcdDisableExternalWrite");
	*pAnsNb_UL = 0;

	GL_GlobalData_X.Lcd_H.disableExternalWrite();

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_LcdGetExternalWriteStatus(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_LcdGetExternalWriteStatus");
	*pAnsNb_UL = 1;

	pAns_UB[0] = (GL_GlobalData_X.Lcd_H.isExternalWriteEnabled()) ? 0x01 : 0x00;

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_LcdGetExternalWriteData(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_LcdGetExternalWriteData");
	*pAnsNb_UL = 1 + GL_GlobalData_X.Lcd_H.getExternalWriteData().length();

	// Get external write status
	pAns_UB[0] = (GL_GlobalData_X.Lcd_H.isExternalWriteEnabled()) ? 0x01 : 0x00;

	// Get external write data
	for (int i = 0; i < GL_GlobalData_X.Lcd_H.getExternalWriteData().length(); i++)
		pAns_UB[i + 1] = GL_GlobalData_X.Lcd_H.getExternalWriteData().charAt(i);

	return WCMD_FCT_STS_OK;
}


/* EEPROM ************************************************************************* */
/* ******************************************************************************** */
WCMD_FCT_STS WCmdProcess_EepromWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_EepromWrite");
	*pAnsNb_UL = 0;

	// At least 4 parameters: Address for write (2 bytes), Length to write and Data
	if (ParamNb_UL < 4)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	GL_GlobalData_X.Eeprom_H.write(((pParam_UB[0] << 8) + pParam_UB[1]), (unsigned char *)&pParam_UB[3], (unsigned long)pParam_UB[2]);

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_EepromRead(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_EepromRead");
	*pAnsNb_UL = 0;

	// Must have 3 parameters: Address for read (2 bytes), Length to read
	if (ParamNb_UL != 3)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	*pAnsNb_UL = GL_GlobalData_X.Eeprom_H.read(((pParam_UB[0] << 8) + pParam_UB[1]), pAns_UB, (unsigned long)pParam_UB[2]);

	return WCMD_FCT_STS_OK;
}


/* RTC **************************************************************************** */
/* ******************************************************************************** */
WCMD_FCT_STS WCmdProcess_RtcSetDateTime(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_RtcSetDateTime");
    *pAnsNb_UL = 0;

    RTC_DATETIME_STRUCT DateTime_X;

    // Must have 6 parameters
    if (ParamNb_UL != 6)
        return WCMD_FCT_STS_BAD_PARAM_NB;

    DateTime_X.Time_X.Hour_UB = pParam_UB[0];
    DateTime_X.Time_X.Min_UB = pParam_UB[1];
    DateTime_X.Time_X.Sec_UB = pParam_UB[2];
    DateTime_X.Date_X.Day_UB = pParam_UB[3];
    DateTime_X.Date_X.Month_UB = pParam_UB[4];
    DateTime_X.Date_X.Year_UB = pParam_UB[5];

    // Call low-level function
    GL_GlobalData_X.Rtc_H.setDateTime(DateTime_X);

    // Debug print actual time
    DBG_PRINT(DEBUG_SEVERITY_INFO, "Actual time : ");
    DBG_PRINTDATA(GL_GlobalData_X.Rtc_H.getDateTimeString());
    DBG_ENDSTR();

    return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_RtcGetDateTime(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_RtcGetDateTime");
    *pAnsNb_UL = 0;

    RTC_DATETIME_STRUCT DateTime_X;

    // Call low-level function
    DateTime_X = GL_GlobalData_X.Rtc_H.getDateTime();

    pAns_UB[(*pAnsNb_UL)++] = DateTime_X.Time_X.Hour_UB;
    pAns_UB[(*pAnsNb_UL)++] = DateTime_X.Time_X.Min_UB;
    pAns_UB[(*pAnsNb_UL)++] = DateTime_X.Time_X.Sec_UB;
    pAns_UB[(*pAnsNb_UL)++] = DateTime_X.Date_X.Day_UB;
    pAns_UB[(*pAnsNb_UL)++] = DateTime_X.Date_X.Month_UB;
    pAns_UB[(*pAnsNb_UL)++] = DateTime_X.Date_X.Year_UB;

    // Debug print actual time
    DBG_PRINT(DEBUG_SEVERITY_INFO, "Actual time : ");
    DBG_PRINTDATA(GL_GlobalData_X.Rtc_H.getDateTimeString());
    DBG_ENDSTR();

    return WCMD_FCT_STS_OK;
}


/* COM Port *********************************************************************** */
/* ******************************************************************************** */
WCMD_FCT_STS WCmdProcess_ComPortOpen(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_ComPortOpen");
	*pAnsNb_UL = 0;

	unsigned long pPortComSpeedLut_UL[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };

	// Must have 3 parameters: Port COM Number, Baudrate and Config
	if (ParamNb_UL != 3)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	// Port COM from 0x00 to 0x03
	if (pParam_UB[0] > 0x03)
		return WCMD_FCT_STS_BAD_DATA;

	// Baudrate from 0x00 (1200[bps]) to 0x07 (115200[bps])
	if (pParam_UB[1] > 0x07)
		return WCMD_FCT_STS_BAD_DATA;

	// Config should be 0x06 for now
	if (pParam_UB[2] != 0x06)
		return WCMD_FCT_STS_BAD_DATA;

	// Open Serial
	GetSerialHandle(pParam_UB[0])->begin(pPortComSpeedLut_UL[pParam_UB[1]]);

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_ComPortClose(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_ComPortClose");
	*pAnsNb_UL = 0;

	// Must have 1 parameter: Port COM Number
	if (ParamNb_UL != 1)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	// Port COM from 0x00 to 0x03
	if (pParam_UB[0] > 0x03)
		return WCMD_FCT_STS_BAD_DATA;

	// Close Serial
    GetSerialHandle(pParam_UB[0])->end();

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_ComPortWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_ComPortWrite");
	*pAnsNb_UL = 0;

	// At least 3 parameters: Port COM Number, Number of Bytes to Send, Byte(s) to Send
	if (ParamNb_UL < 3)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	// Check if number of bytes to send are > 0
	if (pParam_UB[1] == 0)
		return WCMD_FCT_STS_BAD_DATA;

	// Send Bytes
	for (int i = 0; i < pParam_UB[1]; i++)
        GetSerialHandle(pParam_UB[0])->write(pParam_UB[2+i]);

	return WCMD_FCT_STS_OK;
}


WCMD_FCT_STS WCmdProcess_ComPortEnableTunnel(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_ComPortEnableTunnel");
    *pAnsNb_UL = 0;

    // At least 2 parameters: Port COM A, Port COM B
    if (ParamNb_UL < 2)
        return WCMD_FCT_STS_BAD_PARAM_NB;

    // Check if port number A is < 4
    if (pParam_UB[0] >= 4)
        return WCMD_FCT_STS_BAD_DATA;

    // Check if port number B is < 4
    if (pParam_UB[1] >= 4)
        return WCMD_FCT_STS_BAD_DATA;

    // Call low-level function
    SerialManager_SetTunnel((unsigned long)pParam_UB[0], (unsigned long)pParam_UB[1]);

    return WCMD_FCT_STS_OK;
}


WCMD_FCT_STS WCmdProcess_ComPortDisableTunnel(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_ComPortDisableTunnel");
    *pAnsNb_UL = 0;

    // Call low-level function
    SerialManager_StopTunnel();    

    return WCMD_FCT_STS_OK;
}


/* Test *************************************************************************** */
/* ******************************************************************************** */
WCMD_FCT_STS WCmdProcess_TestCommand(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_TestCommand");
	*pAnsNb_UL = 0;

	/* --> Start of Test Code Section */

    GL_GlobalData_X.Fona_H.disabeGprs();

	/* <-- End of Test Code Section */

	return WCMD_FCT_STS_OK;
}

