/* ******************************************************************************** */
/*                                                                                  */
/* WCommand.cpp																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the functions related to each module                              */
/*                                                                                  */
/* History :  	16/02/2015  (RW)	Creation of this file                           */
/*				14/05/2016	(RW)	Re-mastered version								*/
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"WCommand"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WLink.h"
#include "WCommand.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern HardwareSerial * GL_PortComMap_X[4];

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

	if (ParamNb_UL < 3)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	GL_GlobalData_X.Lcd_H.writeDisplay((LCD_DISPLAY_LINE_ENUM) pParam_UB[0], (unsigned char *) &(pParam_UB[1]), (ParamNb_UL-1));

	return WCMD_FCT_STS_OK;
}

WCMD_FCT_STS WCmdProcess_LcdRead(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_LcdRead");
	*pAnsNb_UL = 0;

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


/* COM Port *********************************************************************** */
/* ******************************************************************************** */
WCMD_FCT_STS WCmdProcess_ComPortWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCmdProcess_ComPortWrite");
	*pAnsNb_UL = 0;

	// At least 3 parameters: Port COM Number, Number of Bytes to Send, Byte(s) to Send
	if (ParamNb_UL < 3)
		return WCMD_FCT_STS_BAD_PARAM_NB;

	// Send Bytes
	for (int i = 0; i < pParam_UB[1]; i++)
		GL_PortComMap_X[pParam_UB[0]]->write(pParam_UB[2+i]);

	return WCMD_FCT_STS_OK;
}
