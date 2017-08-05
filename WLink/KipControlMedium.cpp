/* ******************************************************************************** */
/*                                                                                  */
/* KipControlMedium.cpp												                */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the functions to abstract the communication interface   			*/
/*                                                                                  */
/* History :  	31/07/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"KipControlMedium"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "KipControlMedium.h"
#include "EthernetClient.h"
#include "FonaModule.h"

#include "Debug.h"


/* ******************************************************************************** */
/* External Variables
/* ******************************************************************************** */


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static EthernetClient * GL_pMediumEthernet_H;
static FonaModule * GL_pMediumGsm_H;

static const String GL_pMediumLut_cstr[] = { "Ethernet", "GSM" };
static KC_MEDIUM_ENUM GL_Medium_E;
static String GL_ServerName_Str = "";
static unsigned long GL_ServerPort_UL = 80;

static int GL_ServerResponse_SI = 0;
static int GL_ServerData_SI = 0;


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void KipControlMedium_Init(KC_MEDIUM_ENUM Medium_E, void * pMedium_H) {
	GL_Medium_E = Medium_E;

	switch (GL_Medium_E) {

	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H = (EthernetClient *)pMedium_H;
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H = (FonaModule *)pMedium_H;
		break;

	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "KipControl Medium Assigned -> ");
	DBG_PRINTDATA(GL_pMediumLut_cstr[Medium_E]);
	DBG_ENDSTR();
}

void KipControlMedium_SetServerParam(String Name_Str, unsigned long Port_UL = 80) {
	GL_ServerName_Str = Name_Str;
	GL_ServerPort_UL = Port_UL;
}

boolean KipControlMedium_Connect(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		return (((GL_pMediumEthernet_H->connect(GL_ServerName_Str, GL_ServerPort_UL)) == 1) ? true : false);
		break;

	case KC_MEDIUM_GSM:
		return (GL_pMediumGsm_H->enableGprs());
		break;
	}
}

void KipControlMedium_BeginTransaction(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:

		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpTerm();	// Make sure all HTTP transactions are terminated
		GL_pMediumGsm_H->httpInit();
		GL_pMediumGsm_H->httpParam(FONA_MODULE_HTTP_PARAM_CID, 1);
		GL_pMediumGsm_H->httpParam(FONA_MODULE_HTTP_PARAM_REDIR, 1);
		GL_pMediumGsm_H->httpParam(FONA_MODULE_HTTP_PARAM_UA, "WLINK");
		break;
	}
}

void KipControlMedium_Print(char * pData_UB) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->print(pData_UB);
		break;

	case KC_MEDIUM_GSM:

		break;
	}
}

void KipControlMedium_Print(String Data_Str) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->print(Data_Str);
		break;

	case KC_MEDIUM_GSM:

		break;
	}
}

void KipControlMedium_EndTransaction(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->println();
		delay(1);
		GL_pMediumEthernet_H->flush();
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpAction(FONA_MODULE_HTTP_ACTION_METHOD_GET, &GL_ServerResponse_SI, &GL_ServerData_SI);
		break;
	}
}
