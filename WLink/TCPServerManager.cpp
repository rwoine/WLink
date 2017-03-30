/* ******************************************************************************** */
/*                                                                                  */
/* TCPServerManager.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the TCP Server object					*/
/*                                                                                  */
/* History :  	02/06/2015  (RW)	Creation of this file                           */
/*				15/07/2016	(RW)	Add and manage Enable and Disable functions		*/
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

static NetworkAdapter * GL_pNetworkAdapter_H;
static TCPServer * GL_pTcpServer_H;

static boolean GL_TcpServerManagerEnabled_B = false;


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


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void TCPServerManager_Init(NetworkAdapter * pNetworkAdapter_H, TCPServer * pTCPServer_H) {
    GL_pNetworkAdapter_H = pNetworkAdapter_H;
	GL_pTcpServer_H = pTCPServer_H;
	GL_TcpServerManagerEnabled_B = false;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server Manager Initialized");
}

void TCPServerManager_Enable() {
	GL_TcpServerManagerEnabled_B = true;
}

void TCPServerManager_Disable() {
	GL_TcpServerManagerEnabled_B = false;
}

void TCPServerManager_Process() {

    /* Reset Condition */
    if ((GL_TCPServerManager_CurrentState_E != TCP_SERVER_MANAGER_IDLE) && (!(GL_pNetworkAdapter_H->isConnected() && GL_TcpServerManagerEnabled_B))) {
        TransitionToIdle();
    }

    /* State Machine */
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

boolean TCPServerManager_IsRunning(void) {
    return ((GL_TCPServerManager_CurrentState_E != TCP_SERVER_MANAGER_IDLE) ? true : false);
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
	if (GL_pTcpServer_H->isInitialized() && GL_pNetworkAdapter_H->isConnected() && GL_TcpServerManagerEnabled_B)
		TransitionToConnecting();
}

void ProcessConnecting(void) {
    if (GL_pTcpServer_H->isConnected())
        TransitionToWaitClient();
}

void ProcessWaitClient(void) {
    if (!(GL_pTcpServer_H->isConnected())) {
        TransitionToIdle();
    }
    else if (GL_pTcpServer_H->isClientAvailable()) {
        TransitionToRunning();
    }
}

void ProcessRunning(void) {
    if (!(GL_pTcpServer_H->isClientConnected())) {
        TransitionToWaitClient();
    }
}


void TransitionToIdle(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_pTcpServer_H->flushClient();
    GL_pTcpServer_H->stopClient();
	GL_TCPServerManager_CurrentState_E = TCP_SERVER_MANAGER_STATE::TCP_SERVER_MANAGER_IDLE;
}

void TransitionToConnecting(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONNECTING");
	GL_pTcpServer_H->begin();
	GL_TCPServerManager_CurrentState_E = TCP_SERVER_MANAGER_STATE::TCP_SERVER_MANAGER_CONNECTING;
}

void TransitionToWaitClient(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT CLIENT");
	GL_TCPServerManager_CurrentState_E = TCP_SERVER_MANAGER_STATE::TCP_SERVER_MANAGER_WAIT_CLIENT;
}

void TransitionToRunning(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To RUNNING");
	GL_TCPServerManager_CurrentState_E = TCP_SERVER_MANAGER_STATE::TCP_SERVER_MANAGER_RUNNING;
}
