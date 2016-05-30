/* ******************************************************************************** */
/*                                                                                  */
/*                           W-LINK APPLICATIVE CODE                                */
/*                                                                                  */
/*                                                                                  */
/* Description :                                                                    */
/*                                                                                  */
/* History :	01/12/2014  (RW)	Creation of this file                           */
/*				14/05/2016	(RW)	Re-mastered version								*/
/*                                                                                  */
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WLink.h"
#include "Hardware.h"

#include "WCommand.h"
#include "WCommandMedium.h"
#include "WCommandInterpreter.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

const unsigned char cGL_pWLinkRevisionId_UB[] = "16052501";	// YYMMDDVV - Year-Month-Day-Version


/* ******************************************************************************** */
/* Global
/* ******************************************************************************** */

GLOBAL_PARAM_STRUCT GL_GlobalData_X;
HardwareSerial* GL_PortComMap_X[] = { &Serial, &Serial1, &Serial2, &Serial3 };


/* ******************************************************************************** */
/* Functions Mapping
/* ******************************************************************************** */

const WCMD_FCT_DESCR cGL_pFctDescr_X[] =
{
	{ WCMD_GET_REVISION_ID, WCmdProcess_GetRevisionId }
};

#define WCMD_FCT_DESCR_SIZE (sizeof(cGL_pFctDescr_X)/sizeof(WCMD_FCT_DESCR))


/* ******************************************************************************** */
/* Setup
/* ******************************************************************************** */
void setup() {

	/* Assign Revision ID */
	for (int i = 0; i < sizeof(cGL_pWLinkRevisionId_UB); i++)
		GL_GlobalData_X.pRevisionId_UB[i] = cGL_pWLinkRevisionId_UB[i];

	/* Initialize GPIO */
	for (int i = 0; i < 4; i++) {
		pinMode(GL_GlobalData_X.pGpioInputIndex_UB[i], INPUT);
		pinMode(GL_GlobalData_X.pGpioOutputIndex_UB[i], OUTPUT);
		digitalWrite(GL_GlobalData_X.pGpioOutputIndex_UB[i], LOW);
	}


	/* Initialiaze W-Link Command Management Modules */
	WCmdMedium_Init(WCMD_MEDIUM_SERIAL, &Serial3);	// Medium mapped on Serial3
	WCommandInterpreter_Init(cGL_pFctDescr_X, WCMD_FCT_DESCR_SIZE);	
}

/* ******************************************************************************** */
/* Loop
/* ******************************************************************************** */
void loop() {
	WCommandInterpreter_Process();
}
