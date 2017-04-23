/* ******************************************************************************** */
/*                                                                                  */
/* KipControlManager.cpp												            */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the KipControl application   			*/
/*                                                                                  */
/* History :  	17/04/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"KipControlManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "KipControlManager.h"
#include "KipControlMenuManager.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Extenral Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum KC_STATE {
    KC_IDLE,
    KC_CONNECTING,
    KC_RUNNING
};

static KC_STATE GL_KipControlManager_CurrentState_E = KC_STATE::KC_IDLE;
static boolean GL_KipControlManagerEnabled_B = false;


EthernetClient KipClient_H;


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

void KipControlManager_Init() {
    GL_KipControlManagerEnabled_B = false;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "KipControl Manager Initialized");

}

void KipControlManager_Enable() {
    GL_KipControlManagerEnabled_B = true;
}

void KipControlManager_Disable() {
    GL_KipControlManagerEnabled_B = false;
}

void KipControlManager_Process() {
    switch (GL_KipControlManager_CurrentState_E) {
    case KC_IDLE:
        ProcessIdle();
        break;

    case KC_CONNECTING:
        ProcessConnecting();
        break;

    case KC_RUNNING:
        ProcessRunning();
        break;
    }
}

boolean KipControlManager_IsEnabled() {
    return (GL_KipControlManagerEnabled_B);
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
    if (GL_KipControlManagerEnabled_B)
        TransitionToConnecting();
}

void ProcessConnecting(void) {

    if (GL_GlobalData_X.Network_H.isConnected()) {
        if (KipClient_H.connect("www.balthinet.be", 80)) {

            KipClient_H.print("GET /kipcontrol/import?");
            KipClient_H.print("data[0][Weight]=70&data[0][BalanceSerial]=02000001000C&data[0][DateTime]=2017-04-20+15:30:24");
            KipClient_H.println("&submitted=1&action=validate HTTP/1.1");
            KipClient_H.println("Host: www.balthinet.be");
            KipClient_H.println("Connection: close");
            KipClient_H.println();

            TransitionToRunning();
        }
    }

}

void ProcessRunning(void) {
    
    while (KipClient_H.available()) {
        Serial.print((char)(KipClient_H.read()));
    }

}


void TransitionToIdle(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_IDLE;
}

void TransitionToConnecting(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONNECTING");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_CONNECTING;
}

void TransitionToRunning(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To RUNNING");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_RUNNING;
}

