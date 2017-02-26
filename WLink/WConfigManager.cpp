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

#define WCONFIG_CONFIG_TAG0             0xAA
#define WCONFIG_CONFIG_TAG1             0x55

    
#define WCONFIG_ADDR_CONFIG_TAG         0x0000
#define WCONFIG_ADDR_BOARD_REV          0x0002
#define WCONFIG_ADDR_IO                 0x0008
#define WCONFIG_ADDR_COM                0x0010


/* ******************************************************************************** */
/* LUT
/* ******************************************************************************** */
unsigned long GL_pPortComSpeedLut_UL[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };


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
    WCFG_GET_COM_CONFIG,
    WCFG_ERROR_READING,
    WCFG_BAD_PARAM
};

static WCFG_STATE GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_IDLE;
static boolean GL_WConfigManagerEnabled_B = false;

static WCFG_STATUS GL_WConfigStatus_E = WCFG_STS_ERROR;

static unsigned char GL_pWConfigBuffer_UB[WCONFIG_PARAMETER_BUFFER_SIZE];


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void TransitionToErrorReading(void);
static void TransitionToBadParam(void);

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

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive digital I/O's configuration");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_IO, GL_pWConfigBuffer_UB, 8) == 8) {

            int i = 0;

            /* Configure Inputs */
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Configure Inputs:");
            for (i = 0; i < 4; i++) {
                if ((GL_pWConfigBuffer_UB[i] && 0x01) == 0x01) {

                    pinMode(GL_GlobalData_X.pGpioInputIndex_UB[i], INPUT);

                    DBG_PRINT(DEBUG_SEVERITY_INFO, "- Configure IN");
                    DBG_PRINTDATA(i);
                    DBG_PRINTDATA(": Enabled");
                    DBG_ENDSTR();

                    // TODO : IRQ-based configuration sould be added here
                }
            }

            /* Configure Outputs */
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Configure Outputs:");
            for (i = 4; i < 8; i++) {
                if ((GL_pWConfigBuffer_UB[i] && 0x01) == 0x01) {

                    pinMode(GL_GlobalData_X.pGpioOutputIndex_UB[i], OUTPUT);
                    digitalWrite(GL_GlobalData_X.pGpioOutputIndex_UB[i], LOW);

                    DBG_PRINT(DEBUG_SEVERITY_INFO, "- Configure OUT");
                    DBG_PRINTDATA(i-4);
                    DBG_PRINTDATA(": Enabled");
                    DBG_ENDSTR();
                }
            }

            /* Configure Blinking LED */  
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Configure Blinking LED");
            pinMode(GL_GlobalData_X.LedPin_UB, OUTPUT);
            digitalWrite(GL_GlobalData_X.LedPin_UB, HIGH);	// Turn-on by default


            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET COM CONFIG");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_COM_CONFIG;
        }
        else {
            TransitionToErrorReading();
        }

        break;


    /* GET COM CONFIG */
    /* > Retreive serial COM port configuration. */
    case WCFG_GET_COM_CONFIG:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive serial COM port configuration");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_COM, GL_pWConfigBuffer_UB, 12) == 12) {
        
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Configure COM Ports:");
            for (int i = 0; i < 4; i++) {

                GL_GlobalConfig_X.ComPort_X[i].Index_UL = i;
                DBG_PRINT(DEBUG_SEVERITY_INFO, "- Configure COM");
                DBG_PRINTDATA(i);
                DBG_PRINTDATA(": ");

                if ((GL_pWConfigBuffer_UB[i * 3] && 0x01) == 0x01) {                    

                    // Check configuration
                    if ((GL_pWConfigBuffer_UB[i * 3 + 1] != 0x06)) {
                        DBG_PRINTDATA("Bad parameter in Config Mode (0x");
                        DBG_PRINTDATABASE(GL_pWConfigBuffer_UB[i * 3 + 1], HEX);
                        DBG_PRINTDATA(")");
                        DBG_ENDSTR();
                        TransitionToBadParam();
                    }
                    else
                        GL_GlobalConfig_X.ComPort_X[i].Config_UB = SERIAL_8N1;

                    // Check baudrate
                    if ((GL_pWConfigBuffer_UB[i * 3 + 2] > 0x07)) {
                        DBG_PRINTDATA("Bad parameter in Baudrate (0x");
                        DBG_PRINTDATABASE(GL_pWConfigBuffer_UB[i * 3 + 2], HEX);
                        DBG_PRINTDATA(")");
                        DBG_ENDSTR();
                        TransitionToBadParam();
                    }
                    else
                        GL_GlobalConfig_X.ComPort_X[i].Baudrate_UL = GL_pPortComSpeedLut_UL[GL_pWConfigBuffer_UB[i * 3 + 2]];

                    // Check if debug
                    GL_GlobalConfig_X.ComPort_X[i].isDebug_B = ((GL_pWConfigBuffer_UB[i * 3] && 0x02) == 0x02) ? true : false;

                    // TODO : IRQ-based configuration sould be added here

                    // Configure actual COM port


                    // Print out configuration
                    GL_GlobalConfig_X.ComPort_X[i].isEnabled_B = true;
                    DBG_PRINTDATA(": Enabled (8N1, ");
                    DBG_PRINTDATA(GL_GlobalConfig_X.ComPort_X[i].Baudrate_UL);
                    DBG_PRINTDATA(")");
                    if (GL_GlobalConfig_X.ComPort_X[i].isDebug_B)
                        DBG_PRINTDATA(" -> Configured as Debug COM port");
                    DBG_ENDSTR();

                }
                else {
                    GL_GlobalConfig_X.ComPort_X[i].isEnabled_B = false;
                }

            }

            //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET COM CONFIG");
            //GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_COM_CONFIG;

        }
        else {
            TransitionToErrorReading();
        }
        
        break;


    /* ERROR READING */
    /* > Error while reading a configuration in a previous state. */
    case WCFG_ERROR_READING:

        GL_WConfigStatus_E = WCFG_STS_ERROR_READING;

        break;


    /* BAD PARAM */
    /* > Bad parameter(s) found from a configuration in a previous state. */
    case WCFG_BAD_PARAM:

        GL_WConfigStatus_E = WCFG_STS_BAD_PARAM;

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

void TransitionToBadParam(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Bad parameter(s) found in configuration");
    GL_WConfigStatus_E = WCFG_STS_BAD_PARAM;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To BAD PARAM");
    GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_BAD_PARAM;
}
