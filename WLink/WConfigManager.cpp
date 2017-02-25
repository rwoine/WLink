/* ******************************************************************************** */
/*                                                                                  */
/* WConfigManager.cpp													            */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the W-Link configuration    			*/
/*                                                                                  */
/* History :  	25/02/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"WConfigManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WConfigManager.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define WCONFIG_PARAMETER_BUFFER_SIZE   128

#define WCONFIG_CONFIG_TAG0         0xAA
#define WCONFIG_CONFIG_TAG1         0x55


#define WCONFIG_ADDR_CONFIG_TAG     0x00
#define WCONFIG_ADDR_BOARD_REV      0x02
#define WCONFIG_ADDR_IO             0x08


/* ******************************************************************************** */
/* Extenral Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum WCFG_STATE {
    WCFG_IDLE,
    WCFG_INIT_EEPROM,
    WCFG_CHECK,
    WCFG_GET_BOARD_REVISION, 
    WCFG_GET_IO_CONFIG,
    WCFG_ERROR_READING
};

static WCFG_STATE GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_IDLE;
static boolean GL_WConfigManagerEnabled_B = false;

static WCFG_STATUS GL_WConfigStatus_E = WCFG_STS_ERROR;

static unsigned char GL_pWConfigBuffer_UB[WCONFIG_PARAMETER_BUFFER_SIZE];


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void TransitionToErrorReading(void);

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void WConfigManager_Init() {
    GL_WConfigManagerEnabled_B = false;
    GL_WConfigStatus_E = WCFG_STS_NOT_ENABLED;
    for (int i = 0; i < WCONFIG_PARAMETER_BUFFER_SIZE; i++)
        GL_pWConfigBuffer_UB[i] = 0x00;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "W-Link Configuration Manager Initialized");
}

void WConfigManager_Enable() {
    GL_WConfigManagerEnabled_B = true;
}

void WConfigManager_Disable() {
    GL_WConfigManagerEnabled_B = false;
}

WCFG_STATUS WConfigManager_Process() { 

    /* Reset Condition */
    if (!GL_WConfigManagerEnabled_B) {
        GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_IDLE;
    }

    /* W-Link Configuration State Machine */
    switch (GL_WConfigManager_CurrentState_E) {

    /* IDLE */
    /* > Wait for enabling command. */
    case WCFG_IDLE:

        GL_WConfigStatus_E = WCFG_STS_NOT_ENABLED;

        if (GL_WConfigManagerEnabled_B) {
            GL_WConfigStatus_E = WCFG_STS_BUSY;
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To INIT EEPROM");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_INIT_EEPROM;
        }

        break;


    /* INIT EEPROM */
    /* > Initialize EEPROM. It is the configuration storage device. */
    case WCFG_INIT_EEPROM:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize EEPROM Modules");
        GL_GlobalData_X.Eeprom_H.init(&Wire1, 0x50);

        if (GL_GlobalData_X.Eeprom_H.isInitialized()) {
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CHECK");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_CHECK;
        }
        else {
            // TODO : Manage error if EEPROM not initialized
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "EEPROM cannot be initialized");
        }

        break;


    /* CHECK */
    /* > Check if EEPROM has been configured or not. */
    case WCFG_CHECK:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive configuration tag");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_CONFIG_TAG, GL_pWConfigBuffer_UB, 2) == 2) {
            if ((GL_pWConfigBuffer_UB[0] == WCONFIG_CONFIG_TAG0) && (GL_pWConfigBuffer_UB[1] == WCONFIG_CONFIG_TAG1)) {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Configuration tag ok!");
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET BOARD REVISION");
                GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_BOARD_REVISION;
            }
            else {
                // TODO : Manage error if no or bad configuration tag
                DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Error in configuration tag (no tag or bad tag)");
            }
        }
        else {
            TransitionToErrorReading();
        }

        break;


    /* GET BOARD REVISION */
    /* > Retreive board revision. Informational state. */
    case WCFG_GET_BOARD_REVISION:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive board revision");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_BOARD_REV, GL_pWConfigBuffer_UB, 2) == 2) {

            GL_GlobalConfig_X.MajorRev_UB = GL_pWConfigBuffer_UB[0];
            GL_GlobalConfig_X.MinorRev_UB = GL_pWConfigBuffer_UB[1];
            DBG_PRINT(DEBUG_SEVERITY_INFO, "Board Revision = ");
            DBG_PRINTDATA(GL_GlobalConfig_X.MajorRev_UB);
            DBG_PRINTDATA(".");
            DBG_PRINTDATA(GL_GlobalConfig_X.MinorRev_UB);
            DBG_ENDSTR();

            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET IO CONFIG");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_IO_CONFIG;
        }
        else {
            TransitionToErrorReading();
        }

        break;


    /* GET IO CONFIG */
    /* > Retreive digital I/O's configuration. */
    case WCFG_GET_IO_CONFIG:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive digital I/O'ss configuration");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_IO, GL_pWConfigBuffer_UB, 8) == 8) {

            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Configure Inputs:");
            for (int i = 0; i < 4; i++) {
                if ((GL_pWConfigBuffer_UB[i] && 0x01) == 0x01) {
                    DBG_PRINT(DEBUG_SEVERITY_INFO, "Configure IN");

                }
            }



            //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET IO CONFIG");
            //GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_IO_CONFIG;
        }
        else {
            TransitionToErrorReading();
        }

        break;



    /* GET ERROR READING */
    /* > Error while reading a configuration in a previous state. */
    case WCFG_ERROR_READING:

        GL_WConfigStatus_E = WCFG_STS_ERROR_READING;

        break;


    }

    return GL_WConfigStatus_E;
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void TransitionToErrorReading(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Error when reading from EEPROM");
    GL_WConfigStatus_E = WCFG_STS_ERROR_READING;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To ERROR READING");
    GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_ERROR_READING;
}
