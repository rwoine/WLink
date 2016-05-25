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

//#include "WCommand.h"
//#include "WCommandInterpreter.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

const unsigned char cGL_pWLinkRevisionId_UB[] = "16051401";	// YYMMDDVV - Year-Month-Day-Version


/* ******************************************************************************** */
/* Global
/* ******************************************************************************** */

GLOBAL_PARAM_STRUCT GL_GlobalData_X;
HardwareSerial* GL_PortComMap_X[] = { &Serial, &Serial1, &Serial2, &Serial3 };


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
	
}

/* ******************************************************************************** */
/* Loop
/* ******************************************************************************** */
void loop() {
  
}
