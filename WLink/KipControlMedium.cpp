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
static boolean GL_TransactionStatus_B = false;


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

void KipControlMedium_SetServerParam(String Name_Str, unsigned long Port_UL) {
	GL_ServerName_Str = Name_Str;
	GL_ServerPort_UL = Port_UL;
}


boolean KipControlMedium_IsReady(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		return NetworkAdapterManager_IsRunning();
		break;

	case KC_MEDIUM_GSM:
		return (FonaModuleManager_IsRunning() && FonaModuleManager_GetCurrentGprsState());
		break;
	}
}

boolean KipControlMedium_IsError(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		return false;   // No error management in Ethernet (so far)
		break;

	case KC_MEDIUM_GSM:
		return FonaModuleManager_IsError();
		break;
	}
}


boolean KipControlMedium_Connect(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		return (((GL_pMediumEthernet_H->connect(GL_ServerName_Str.c_str(), (int)GL_ServerPort_UL)) == 1) ? true : false);
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpTerm();	// Make sure all HTTP transactions are terminated
		return (GL_pMediumGsm_H->httpInit());
		break;
	}
}

boolean KipControlMedium_IsConnected(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		return (GL_pMediumEthernet_H->connected());
		break;

	case KC_MEDIUM_GSM:
		// No status available for GSM
		return true;
		break;
	}
}


void KipControlMedium_SetupEnvironment(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		// Do nothing for Ethernet
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpParam(FONA_MODULE_HTTP_PARAM_CID, 1);
		GL_pMediumGsm_H->httpParam(FONA_MODULE_HTTP_PARAM_REDIR, 1);
		GL_pMediumGsm_H->httpParam(FONA_MODULE_HTTP_PARAM_UA, "WLINK");
		break;
	}
}


void KipControlMedium_BeginTransaction(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->print("GET http://");
		GL_pMediumEthernet_H->print(GL_ServerName_Str);
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpParamStart(FONA_MODULE_HTTP_PARAM_URL);
		GL_pMediumGsm_H->httpParamAdd("http://");
		GL_pMediumGsm_H->httpParamAdd((char *)(GL_ServerName_Str.c_str()));
		break;
	}
}


void KipControlMedium_Print(unsigned char Data_UB) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->print(Data_UB);
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpParamAdd(Data_UB);
		break;
	}
}

void KipControlMedium_Print(int Data_SI) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->print(Data_SI);
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpParamAdd(Data_SI);
		break;
	}
}

void KipControlMedium_Print(unsigned long Data_UL) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->print(Data_UL);
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpParamAdd(Data_UL);
		break;
	}
}

void KipControlMedium_Print(char * pData_UB) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->print(pData_UB);
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpParamAdd(pData_UB);
		break;
	}
}

void KipControlMedium_Print(String Data_Str) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->print(Data_Str);
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpParamAdd(Data_Str);
		break;
	}
}

void KipControlMedium_EndTransaction(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->println("&submitted=1&action=validate HTTP/1.1");
		GL_pMediumEthernet_H->print("Host: ");
		GL_pMediumEthernet_H->println(GL_ServerName_Str);
		GL_pMediumEthernet_H->println("Connection: close");
		GL_pMediumEthernet_H->println();
		delay(1);
		GL_pMediumEthernet_H->flush();
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpParamAdd("&submitted=1&action=validate");
		GL_pMediumGsm_H->httpParamEnd();
        GL_TransactionStatus_B = GL_pMediumGsm_H->httpAction(FONA_MODULE_HTTP_ACTION_METHOD_GET, &GL_ServerResponse_SI, &GL_ServerData_SI);
		break;
	}
}

boolean KipControlMedium_IsTransactionOk(void) {
    switch (GL_Medium_E) {
    case KC_MEDIUM_ETHERNET:
        return true;
        break;

    case KC_MEDIUM_GSM:
        return GL_TransactionStatus_B;
        break;
    }
}

void KipControlMedium_Flush(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		GL_pMediumEthernet_H->flush();
		delay(500);
		while (GL_pMediumEthernet_H->available())
			GL_pMediumEthernet_H->read();
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->flush();
		break;
	}
}

int KipControlMedium_DataAvailable(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		return GL_pMediumEthernet_H->available();
		break;

	case KC_MEDIUM_GSM:
		return GL_ServerData_SI;
		break;
	}
}

int KipControlMedium_GetServerResponse(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "To be implemented !");
		return -1;
		break;

	case KC_MEDIUM_GSM:
		return GL_ServerResponse_SI;
		break;
	}
}

int KipControlMedium_GetDataSize(void) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "To be implemented !");
		return -1;
		break;

	case KC_MEDIUM_GSM:
		return GL_ServerData_SI;
		break;
	}
}

void KipControlMedium_Read(char * pData_UB) {
	switch (GL_Medium_E) {
	case KC_MEDIUM_ETHERNET:
		pData_UB[0] = 0x00;
		DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "To be implemented !");
		break;

	case KC_MEDIUM_GSM:
		GL_pMediumGsm_H->httpRead(pData_UB);
		break;
	}
}

