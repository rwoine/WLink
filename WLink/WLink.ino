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

#define MODULE_NAME		"Main"

#define APP_USE_DEBUG

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <SPI.h>
#include <Ethernet.h>

#include "WLink.h"
#include "Hardware.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */


/* ******************************************************************************** */
/* Constant
/* ******************************************************************************** */

const unsigned char cGL_pWLinkRevisionId_UB[] = "16053101";	// YYMMDDVV - Year-Month-Day-Version


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

	/* Enable All UARTs by Default */
	pinMode(PIN_EN_SERIAL01, OUTPUT);
	digitalWrite(PIN_EN_SERIAL01, LOW);
	pinMode(PIN_EN_SERIAL23, OUTPUT);
	digitalWrite(PIN_EN_SERIAL23, LOW);

	/* Initialize Debug Module */
	Debug_Init(GL_PortComMap_X[PORT_COM0]);

	/* Assign Revision ID */
	for (int i = 0; i < sizeof(cGL_pWLinkRevisionId_UB); i++)
		GL_GlobalData_X.pRevisionId_UB[i] = cGL_pWLinkRevisionId_UB[i];

	/* Initialize LED */
	pinMode(GL_GlobalData_X.LedPin_UB, OUTPUT);
	//digitalWrite(GL_GlobalData_X.LedPin_UB, LOW);	// Turn-off by default
	analogWrite(GL_GlobalData_X.LedPin_UB, 128);	// 50% duty-cycle

	/* Initialize GPIO */
	for (int i = 0; i < 4; i++) {
		pinMode(GL_GlobalData_X.pGpioInputIndex_UB[i], INPUT);
		pinMode(GL_GlobalData_X.pGpioOutputIndex_UB[i], OUTPUT);
		digitalWrite(GL_GlobalData_X.pGpioOutputIndex_UB[i], LOW);
	}

	/* Initialiaze W-Link Command Management Modules */
	WCmdMedium_Init(WCMD_MEDIUM_SERIAL, GL_PortComMap_X[PORT_COM3]);	// Medium mapped on COM3
	WCommandInterpreter_Init(cGL_pFctDescr_X, WCMD_FCT_DESCR_SIZE);	



	// TODO : Add Output Management for Bug in SPI (additional output to maintain low ?)

}

/* ******************************************************************************** */
/* Loop
/* ******************************************************************************** */
void loop() {
	WCommandInterpreter_Process();
	
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Loop");

	DBG_PRINT(DEBUG_SEVERITY_WARNING, "Header String");
	DBG_PRINTDATA("yop");
	DBG_PRINTDATABASE(45, HEX);
	DBG_ENDSTR();
}

/* ******************************************************************************** */
/* Events
/* ******************************************************************************** */
// TODO : SerialEvent Management
