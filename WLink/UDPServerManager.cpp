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


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum UDP_SERVER_MANAGER_STATE {
	UDP_SERVER_MANAGER_IDLE,
	UDP_SERVER_MANAGER_CONNECTING,
	UDP_SERVER_MANAGER_RUNNING
};

static UDP_SERVER_MANAGER_STATE GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_IDLE;

static NetworkAdapter * GL_pNetworkAdapter_H;
static UDPServer * GL_pUdpServer_H;

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


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void UDPServerManager_Init(NetworkAdapter * pNetworkAdapter_H, UDPServer * pUDPServer_H) {
    GL_pNetworkAdapter_H = pNetworkAdapter_H;
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
    /* Reset Condition */
    if (!(GL_pNetworkAdapter_H->isConnected() && GL_UdpServerManagerEnabled_B)) {
        TransitionToIdle();
    }

    /* State Machine */
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

boolean UDPServerManager_IsRunning(void) {
    return ((GL_UDPServerManager_CurrentState_E != UDP_SERVER_MANAGER_IDLE) ? true : false);
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
	if (GL_pUdpServer_H->isInitialized() && GL_pNetworkAdapter_H->isConnected() && GL_UdpServerManagerEnabled_B)
		TransitionToConnecting();
}

void ProcessConnecting(void) {
	if (GL_pUdpServer_H->isConnected())
		TransitionToRunning();
}

void ProcessRunning(void) {
    // Keep running
}


void TransitionToIdle(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_pUdpServer_H->flushServer();
    GL_pUdpServer_H->stopServer();
	GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_IDLE;
}

void TransitionToConnecting(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONNECTING");
	GL_pUdpServer_H->begin();
	GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_CONNECTING;
}

void TransitionToRunning(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To RUNNING");
	GL_UDPServerManager_CurrentState_E = UDP_SERVER_MANAGER_STATE::UDP_SERVER_MANAGER_RUNNING;
}
