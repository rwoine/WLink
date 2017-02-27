/* ******************************************************************************** */
/*                                                                                  */
/* NetworkAdapterManager.cpp														*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the Network Adapter object			*/
/*                                                                                  */
/* History :  	22/02/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"NetworkAdapterManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "NetworkAdapterManager.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define NETWORK_ADAPTER_IS_ETHERNET_LINKED_TRY_NB    3
#define NETWORK_ADAPTER_IS_ETHERNET_LINKED_DELAY_MS  400


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum NETWORK_ADAPTER_STATE {
    NETWORK_ADAPTER_IDLE,
    NETWORK_ADAPTER_CONNECTING,
    NETWORK_ADAPTER_RUNNING
};

static NETWORK_ADAPTER_STATE GL_NetworkAdapterManager_CurrentState_E = NETWORK_ADAPTER_STATE::NETWORK_ADAPTER_IDLE;

static NetworkAdapter * GL_pNetworkAdapter_H;

static unsigned char GL_NetworkAdapterIsCableConnectedTryCnt_UB = 0;
static unsigned long GL_NetworkAdapterAbsoluteTime_UL = 0;
static boolean GL_NetworkAdapterManagerEnabled_B = false;


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

void NetworkAdapterManager_Init(NetworkAdapter * pNetworkAdapter_H) {
    GL_pNetworkAdapter_H = pNetworkAdapter_H;
    GL_NetworkAdapterManagerEnabled_B = false;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Network Adapter Manager Initialized");
}

void NetworkAdapterManager_Enable() {
    GL_NetworkAdapterManagerEnabled_B = true;
}

void NetworkAdapterManager_Disable() {
    GL_NetworkAdapterManagerEnabled_B = false;
}

void NetworkAdapterManager_Process() {
    switch (GL_NetworkAdapterManager_CurrentState_E) {
    case NETWORK_ADAPTER_IDLE:
        ProcessIdle();
        break;

    case NETWORK_ADAPTER_CONNECTING:
        ProcessConnecting();
        break;

    case NETWORK_ADAPTER_RUNNING:
        ProcessRunning();
        break;
    }
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
    if (GL_pNetworkAdapter_H->isInitialized() && GL_pNetworkAdapter_H->isEthernetLinked() && GL_NetworkAdapterManagerEnabled_B)
        TransitionToConnecting();
}

void ProcessConnecting(void) {
    if (!GL_NetworkAdapterManagerEnabled_B)
        TransitionToIdle();

    if (GL_pNetworkAdapter_H->isConnected())
        TransitionToRunning();
}

void ProcessRunning(void) {
    if (!IsEthernetStillLinked() || !GL_NetworkAdapterManagerEnabled_B)
        TransitionToIdle();
}


void TransitionToIdle(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_pNetworkAdapter_H->flush();
    GL_NetworkAdapterManager_CurrentState_E = NETWORK_ADAPTER_STATE::NETWORK_ADAPTER_IDLE;
}

void TransitionToConnecting(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONNECTING");
    GL_pNetworkAdapter_H->begin();
    GL_NetworkAdapterManager_CurrentState_E = NETWORK_ADAPTER_STATE::NETWORK_ADAPTER_CONNECTING;
}

void TransitionToRunning(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To RUNNING");
    GL_NetworkAdapterAbsoluteTime_UL = millis();
    GL_NetworkAdapterManager_CurrentState_E = NETWORK_ADAPTER_STATE::NETWORK_ADAPTER_RUNNING;
}


boolean IsEthernetStillLinked(void) {
    if (!GL_pNetworkAdapter_H->isEthernetLinked()) {
        if ((millis() - GL_NetworkAdapterAbsoluteTime_UL) >= NETWORK_ADAPTER_IS_ETHERNET_LINKED_DELAY_MS) {
            GL_NetworkAdapterAbsoluteTime_UL = millis();
            GL_NetworkAdapterIsCableConnectedTryCnt_UB++;
            //DBG_PRINT(DEBUG_SEVERITY_WARNING, "Cable Disconnected. Try Number = ");
            //DBG_PRINTDATA(GL_UdpIsCableConnectedTryCnt_UB);
            //DBG_ENDSTR();  
        }
        if (GL_NetworkAdapterIsCableConnectedTryCnt_UB == NETWORK_ADAPTER_IS_ETHERNET_LINKED_TRY_NB) {
            GL_NetworkAdapterIsCableConnectedTryCnt_UB = 0;
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Cable Disconnected");
            return false;
        }
    }
    else {
        GL_NetworkAdapterIsCableConnectedTryCnt_UB = 0;
    }

    return true;
}
