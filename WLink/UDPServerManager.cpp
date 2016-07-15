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

static unsigned char GL_UdpIsCableConnectedTryCnt_UB = 0;
static unsigned long GL_UdpAbsoluteTime_UL = 0;
static boolean GL_UdpServerManagerEnabled_B = false;


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessConnecting(void);
static void ProcessRunning(void);

static void TransitionToIdle(void);
static void TransitionToConnecting(void);
static void TransitionToRunning(void);

static boolean IsEthernetStillLinked(void);


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void UDPServerManager_Init(UDPServer * pUDPServer_H) {
	GL_pUdpServer_H = pUDPServer_H;
	GL_UdpServerManagerEnabled_B = false;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "UDP Server Manager Initialized");
}

void UDPServerManager_Enable() {
	GL_UdpServerManagerEnabled_B = true;
}

void UDPServerManager_Disable() {
	GL_UdpServerManagerEnabled_B = false;
}

void UDPServerManager_Process() {
	switch (GL_UDPServerManager_CurrentState_E) {
	case UDP_SERVER_MANAGER_IDLE:
		ProcessIdle();
		break;

	case UDP_SERVER_MANAGER_CONNECTING:
		ProcessConnecting();
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
	if (GL_pUdpServer_H->isInitialized() && GL_pUdpServer_H->isEthernetLinked() && GL_UdpServerManagerEnabled_B)
		TransitionToConnecting();
}

void ProcessConnecting(void) {
	if (GL_pUdpServer_H->GL_UdpServerParam_X.IsConnected_B)
		TransitionToRunning();

	if (!GL_UdpServerManagerEnabled_B)
		TransitionToIdle();
}

void ProcessRunning(void) {
	boolean EnableCmd_B = true;

	if (!IsEthernetStillLinked() || !GL_UdpServerManagerEnabled_B) {
		WCommandInterpreter_Restart();
		TransitionToIdle();
		EnableCmd_B = false;
	}

	if (EnableCmd_B)
		WCommandInterpreter_Process();
}


void TransitionToIdle(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
	GL_pUdpServer_H->GL_UdpServerParam_X.Server_H.flush();
	GL_pUdpServer_H->GL_UdpServerParam_X.Server_H.stop();
	GL_pUdpServer_H->GL_UdpServerParam_X.Status_E = UDP_SERVER_IDLE;
	GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_IDLE;
}

void TransitionToConnecting(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONNECTING");
	GL_pUdpServer_H->begin();
	GL_pUdpServer_H->GL_UdpServerParam_X.Status_E = UDP_SERVER_AWAKE;
	GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_CONNECTING;
}

void TransitionToRunning(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To RUNNING");
	GL_UdpAbsoluteTime_UL = millis();
	GL_pUdpServer_H->GL_UdpServerParam_X.Status_E = UDP_SERVER_RUNNING;
	GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_RUNNING;
}


boolean IsEthernetStillLinked(void) {
	if (!GL_pUdpServer_H->isEthernetLinked()) {
		if ((millis() - GL_UdpAbsoluteTime_UL) >= UDP_SERVER_MANAGER_IS_ETHERNET_LINKED_DELAY_MS) {
			GL_UdpAbsoluteTime_UL = millis();
			GL_UdpIsCableConnectedTryCnt_UB++;
			//DBG_PRINT(DEBUG_SEVERITY_WARNING, "Cable Disconnected. Try Number = ");
			//DBG_PRINTDATA(GL_UdpIsCableConnectedTryCnt_UB);
			//DBG_ENDSTR();  
		}
		if (GL_UdpIsCableConnectedTryCnt_UB == UDP_SERVER_MANAGER_IS_ETHERNET_LINKED_TRY_NB) {
			GL_UdpIsCableConnectedTryCnt_UB = 0;
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Cable Disconnected");
			return false;
		}
	}
	else {
		GL_UdpIsCableConnectedTryCnt_UB = 0;
	}

	return true;
}
