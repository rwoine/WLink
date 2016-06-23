/* ******************************************************************************** */
/*                                                                                  */
/* TCPServerManager.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the TCP Server object					*/
/*                                                                                  */
/* History :  	02/06/2015  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"TCPServerManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "TCPServerManager.h"
#include "WCommandInterpreter.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define TCP_SERVER_MANAGER_IS_ETHERNET_LINKED_TRY_NB    3
#define TCP_SERVER_MANAGER_IS_ETHERNET_LINKED_DELAY_MS  400


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum TCP_SERVER_MANAGER_STATE {
	TCP_SERVER_MANAGER_IDLE,
	TCP_SERVER_MANAGER_CONNECTING,
	TCP_SERVER_MANAGER_WAIT_CLIENT,
	TCP_SERVER_MANAGER_RUNNING
};

static TCP_SERVER_MANAGER_STATE GL_TCPServerManager_CurrentState_E = TCP_SERVER_MANAGER_STATE::TCP_SERVER_MANAGER_IDLE;

static TCPServer * GL_pTcpServer_H;

static unsigned char GL_TcpIsCableConnectedTryCnt_UB = 0;
static unsigned long GL_TcpAbsoluteTime_UL = 0;


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

void TCPServerManager_Init(TCPServer * pTCPServer_H) {
	GL_pTcpServer_H = pTCPServer_H;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server Manager Initialized");
}

void TCPServerManager_Process() {
	switch (GL_TCPServerManager_CurrentState_E) {
	case TCP_SERVER_MANAGER_IDLE:
		ProcessIdle();
		break;

	case TCP_SERVER_MANAGER_CONNECTING:
		ProcessConnecting();
		break;

	case TCP_SERVER_MANAGER_WAIT_CLIENT:
		ProcessWaitClient();
		break;

	case TCP_SERVER_MANAGER_RUNNING:
		ProcessRunning();
		break;
	}
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
	if (GL_pTcpServer_H->GL_TcpServerParam_X.IsInitialized_B && GL_pTcpServer_H->isEthernetLinked())
		TransitionToConnecting();
}

void ProcessConnecting(void) {
	if (GL_pTcpServer_H->GL_TcpServerParam_X.IsConnected_B)
		TransitionToWaitClient();
}

void ProcessWaitClient(void) {
	if (!IsEthernetStillLinked()) {
		WCommandInterpreter_Restart();
		TransitionToIdle();
	}

	GL_pTcpServer_H->GL_TcpServerParam_X.Client_H = GL_pTcpServer_H->GL_TcpServerParam_X.Server_H.available();
	if (GL_pTcpServer_H->GL_TcpServerParam_X.Client_H)
		TransitionToRunning();
}

void ProcessRunning(void) {
	boolean EnableCmd_B = true;

	if (!IsEthernetStillLinked()) {
		WCommandInterpreter_Restart();
		TransitionToIdle();
		EnableCmd_B = false;
	}

	if (!GL_pTcpServer_H->GL_TcpServerParam_X.Client_H.connected()) {
		WCommandInterpreter_Restart();
		TransitionToWaitClient();
		EnableCmd_B = false;
	}

	if (EnableCmd_B)
		WCommandInterpreter_Process();
}


void TransitionToIdle(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
	GL_pTcpServer_H->GL_TcpServerParam_X.Client_H.flush();
	GL_pTcpServer_H->GL_TcpServerParam_X.Client_H.stop();
	GL_pTcpServer_H->GL_TcpServerParam_X.Status_E = TCP_SERVER_IDLE;
	GL_TCPServerManager_CurrentState_E = TCP_SERVER_MANAGER_STATE::TCP_SERVER_MANAGER_IDLE;
}

void TransitionToConnecting(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONNECTING");
	GL_pTcpServer_H->begin();
	GL_pTcpServer_H->GL_TcpServerParam_X.Status_E = TCP_SERVER_AWAKE;
	GL_TCPServerManager_CurrentState_E = TCP_SERVER_MANAGER_STATE::TCP_SERVER_MANAGER_CONNECTING;
}

void TransitionToWaitClient(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT CLIENT");
	GL_pTcpServer_H->GL_TcpServerParam_X.Status_E = TCP_SERVER_CONNECTED;
	GL_TCPServerManager_CurrentState_E = TCP_SERVER_MANAGER_STATE::TCP_SERVER_MANAGER_WAIT_CLIENT;
}

void TransitionToRunning(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To RUNNING");
	GL_TcpAbsoluteTime_UL = millis();
	GL_pTcpServer_H->GL_TcpServerParam_X.Status_E = TCP_SERVER_RUNNING;
	GL_TCPServerManager_CurrentState_E = TCP_SERVER_MANAGER_STATE::TCP_SERVER_MANAGER_RUNNING;
}


boolean IsEthernetStillLinked(void) {
	if (!GL_pTcpServer_H->isEthernetLinked()) {
		if ((millis() - GL_TcpAbsoluteTime_UL) >= TCP_SERVER_MANAGER_IS_ETHERNET_LINKED_DELAY_MS) {
			GL_TcpAbsoluteTime_UL = millis();
			GL_TcpIsCableConnectedTryCnt_UB++;
			//DBG_PRINT(DEBUG_SEVERITY_WARNING, "Cable Disconnected. Try Number = ");
			//DBG_PRINTDATA(GL_TcpIsCableConnectedTryCnt_UB);
			//DBG_ENDSTR();
		}
		if (GL_TcpIsCableConnectedTryCnt_UB == TCP_SERVER_MANAGER_IS_ETHERNET_LINKED_TRY_NB) {
			GL_TcpIsCableConnectedTryCnt_UB = 0;
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Cable Disconnected");
			return false;
		}
	}
	else {
		GL_TcpIsCableConnectedTryCnt_UB = 0;
	}

	return true;
}
