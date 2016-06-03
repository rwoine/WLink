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

/* Badge Reader ******************************************************************* */
/* ******************************************************************************** */

/* LCD  *************************************************************************** */
/* ******************************************************************************** */



