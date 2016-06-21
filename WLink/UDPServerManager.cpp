/* ******************************************************************************** */
/*                                                                                  */
/* UDPServerManager.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the UDP Server object					*/
/*                                                                                  */
/* History :  	21/06/2015  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"UDPServerManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "UDPServerManager.h"
#include "WCommandInterpreter.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define UDP_SERVER_MANAGER_IS_ETHERNET_LINKED_TRY_NB    3
#define UDP_SERVER_MANAGER_IS_ETHERNET_LINKED_DELAY_MS  400


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum UDP_SERVER_MANAGER_STATE {
	UDP_SERVER_MANAGER_IDLE,
	UDP_SERVER_MANAGER_CONNECTING,
	UDP_SERVER_MANAGER_WAIT_CLIENT,
	UDP_SERVER_MANAGER_RUNNING
};

static UDP_SERVER_MANAGER_STATE GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_IDLE;

static UDPServer * GL_pUdpServer_H;

static unsigned char GL_IsCableConnectedTryCnt_UB = 0;
static unsigned long GL_AbsoluteTime_UL = 0;


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessConnecting(void);
static void ProcessWaitClient(void);
static void ProcessRunning(void);

static void TransitionToIdle(void);
static void TransitionToConnecting(void);
static void TransitionToWaitClient(void);
static void TransitionToRunning(void);

static boolean IsEthernetStillLinked(void);


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void UDPServerManager_Init(UDPServer * pUDPServer_H) {
	GL_pUdpServer_H = pUDPServer_H;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "UDP Server Manager Initialized");
}

void UDPServerManager_Process() {
	switch (GL_UDPServerManager_CurrentState_E) {
	case UDP_SERVER_MANAGER_IDLE:
		ProcessIdle();
		break;

	case UDP_SERVER_MANAGER_CONNECTING:
		ProcessConnecting();
		break;

	case UDP_SERVER_MANAGER_WAIT_CLIENT:
		ProcessWaitClient();
		break;

	case UDP_SERVER_MANAGER_RUNNING:
		ProcessRunning();
		break;
	}
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
	if (GL_pUdpServer_H->isInitialized() && GL_pUdpServer_H->isEthernetLinked())
		TransitionToConnecting();
}

void ProcessConnecting(void) {
	if (GL_pUdpServer_H->GL_UdpServerParam_X.IsConnected_B)
		TransitionToWaitClient();
}

void ProcessWaitClient(void) {
	if (!IsEthernetStillLinked()) {
		WCommandInterpreter_Restart();
		TransitionToIdle();
	}

	//GL_pUdpServer_H->GL_UdpServerParam_X.Client_H = GL_pUdpServer_H->GL_UdpServerParam_X.Server_H.available();
	//if (GL_pUdpServer_H->GL_UdpServerParam_X.Client_H)
	//	TransitionToRunning();
}

void ProcessRunning(void) {
	boolean EnableCmd_B = true;

	if (!IsEthernetStillLinked()) {
		WCommandInterpreter_Restart();
		TransitionToIdle();
		EnableCmd_B = false;
	}

	//if (!GL_pUdpServer_H->GL_UdpServerParam_X.Client_H.connected()) {
	//	WCommandInterpreter_Restart();
	//	TransitionToWaitClient();
	//	EnableCmd_B = false;
	//}

	if (EnableCmd_B)
		WCommandInterpreter_Process();
}


void TransitionToIdle(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
	//GL_pUdpServer_H->GL_UdpServerParam_X.Client_H.flush();
	//GL_pUdpServer_H->GL_UdpServerParam_X.Client_H.stop();
	GL_pUdpServer_H->GL_UdpServerParam_X.Status_E = UDP_SERVER_IDLE;
	GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_IDLE;
}

void TransitionToConnecting(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONNECTING");
	GL_pUdpServer_H->begin();
	GL_pUdpServer_H->GL_UdpServerParam_X.Status_E = UDP_SERVER_AWAKE;
	GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_CONNECTING;
}

void TransitionToWaitClient(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT CLIENT");
	GL_pUdpServer_H->GL_UdpServerParam_X.Status_E = UDP_SERVER_CONNECTED;
	GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_WAIT_CLIENT;
}

void TransitionToRunning(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To RUNNING");
	GL_AbsoluteTime_UL = millis();
	GL_pUdpServer_H->GL_UdpServerParam_X.Status_E = UDP_SERVER_RUNNING;
	GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_RUNNING;
}


boolean IsEthernetStillLinked(void) {
	if (!GL_pUdpServer_H->isEthernetLinked()) {
		if ((millis() - GL_AbsoluteTime_UL) >= UDP_SERVER_MANAGER_IS_ETHERNET_LINKED_DELAY_MS) {
			GL_AbsoluteTime_UL = millis();
			GL_IsCableConnectedTryCnt_UB++;
			//DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Cable Disconnected. Number = ");
			//DBG_PRINTLN(DEBUG_SEVERITY_INFO, GL_IsCableConnectedTryCnt_UB);    
		}
		if (GL_IsCableConnectedTryCnt_UB == UDP_SERVER_MANAGER_IS_ETHERNET_LINKED_TRY_NB) {
			GL_IsCableConnectedTryCnt_UB = 0;
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Cable Disconnected");
			return false;
		}
	}
	else {
		GL_IsCableConnectedTryCnt_UB = 0;
	}

	return true;
}
