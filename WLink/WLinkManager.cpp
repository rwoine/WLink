/* ******************************************************************************** */
/*                                                                                  */
/* WLinkManager.cpp														            */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the whole W-Link          			*/
/*                                                                                  */
/* History :  	25/02/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"WLinkManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WLinkManager.h"
#include "WConfigManager.h"
#include "WMenuManager.h"

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
enum W_STATE {
    W_IDLE,
    W_CONFIG,
    W_PROCESS_WLINK,
    W_ERROR
};

static W_STATE GL_WLinkManager_CurrentState_E = W_STATE::W_IDLE;
static boolean GL_WLinkManagerEnabled_B = false;


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessConfig(void);
static void ProcessWLink(void);
static void ProcessError(void);


static void TransitionToIdle(void);
static void TransitionToConfig(void);
static void TransitionToProcessWLink(void);
static void TransitionToError(void);

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void WLinkManager_Init() {
    GL_WLinkManagerEnabled_B = false;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "W-Link Manager Initialized");

    /* Init sub-entities Manager */
    WConfigManager_Init();
    WMenuManager_Init();
}

void WLinkManager_Enable() {
    GL_WLinkManagerEnabled_B = true;
}

void WLinkManager_Disable() {
    GL_WLinkManagerEnabled_B = false;
}

void WLinkManager_Process() {
    switch (GL_WLinkManager_CurrentState_E) {
    case W_IDLE:
        ProcessIdle();
        break;

    case W_CONFIG:
        ProcessConfig();
        break;

    case W_PROCESS_WLINK:
        ProcessWLink();
        break;

    case W_ERROR:
        ProcessError();
        break;
    }
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
    if (GL_WLinkManagerEnabled_B)
        TransitionToConfig();
}

void ProcessConfig(void) {
    if (WConfigManager_Process() != WCFG_STS_BUSY) {
        if (WConfigManager_Process() == WCFG_STS_OK) {
            WConfigManager_Disable();
            TransitionToProcessWLink();
        }
        else {
            TransitionToError();
        }
    }
}

void ProcessWLink(void) {

    // If Interface is enabled -> call process() from Interface Manager

    if (GL_GlobalConfig_X.EthConfig_X.isEnabled_B)                              NetworkAdapterManager_Process();
    if (GL_GlobalConfig_X.EthConfig_X.TcpServerConfig_X.isEnabled_B)            TCPServerManager_Process();
    if (GL_GlobalConfig_X.EthConfig_X.UdpServerConfig_X.isEnabled_B)            UDPServerManager_Process();
    if (GL_GlobalConfig_X.WCmdConfig_X.Medium_E != WLINK_WCMD_MEDIUM_NONE)      WCommandInterpreter_Process();
    if (GL_GlobalData_X.FlatPanel_H.isInitialized())                            FlatPanelManager_Process(); 
    WMenuManager_Process();

    // Call application
    if (GL_GlobalConfig_X.App_X.hasApplication_B) {
        if (GL_GlobalConfig_X.App_X.pFctIsEnabled())                            GL_GlobalConfig_X.App_X.pFctProcess();
    }
}

void ProcessError(void) {
    // Allow user to configure EEPROM through Debug port -> Need a restart after the configuration parameters are loaded
    if (GL_GlobalConfig_X.WCmdConfig_X.Medium_E != WLINK_WCMD_MEDIUM_NONE)      WCommandInterpreter_Process();
}


void TransitionToIdle(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_WLinkManager_CurrentState_E = W_STATE::W_IDLE;
}

void TransitionToConfig(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition to CONFIG");
    WConfigManager_Enable();
    GL_WLinkManager_CurrentState_E = W_STATE::W_CONFIG;
}

void TransitionToProcessWLink(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To PROCESS W-LINK");

    // Enable WMenu
    WMenuManager_Enable();

    // Enable Application if any
    if (GL_GlobalConfig_X.App_X.hasApplication_B)
        GL_GlobalConfig_X.App_X.pFctEnable();

    GL_WLinkManager_CurrentState_E = W_STATE::W_PROCESS_WLINK;
}

void TransitionToError(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To ERROR");
    if (GL_GlobalConfig_X.WCmdConfig_X.Medium_E != WLINK_WCMD_MEDIUM_COM0)  WConfigManager_BuildSerialGateway();    // Build Serial Gateway to EEPROM Configuration
    GL_WLinkManager_CurrentState_E = W_STATE::W_ERROR;
}

