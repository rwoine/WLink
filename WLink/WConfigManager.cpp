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

#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Wire.h>
#include <SD.h>

#include "Utilz.h"

#include "WConfigManager.h"
#include "SerialHandler.h"

#include "WCommand.h"
#include "WCommandMedium.h"
#include "WCommandInterpreter.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define WCONFIG_PARAMETER_BUFFER_SIZE   48

#define WCONFIG_CONFIG_TAG0             0xAA
#define WCONFIG_CONFIG_TAG1             0x55

    
#define WCONFIG_ADDR_CONFIG_TAG         0x0000
#define WCONFIG_ADDR_BOARD_REV          0x0002
#define WCONFIG_ADDR_LANGUAGE           0x0004
#define WCONFIG_ADDR_GEN                0x0006
#define WCONFIG_ADDR_WCMD_MEDIUM        0x0007
#define WCONFIG_ADDR_IO                 0x0008
#define WCONFIG_ADDR_COM                0x0010
#define WCONFIG_ADDR_ETH                0x001C
#define WCONFIG_ADDR_TCP_SERVER         0x0034
#define WCONFIG_ADDR_UDP_SERVER         0x0038


/* ******************************************************************************** */
/* LUT
/* ******************************************************************************** */
unsigned long GL_pPortComSpeedLut_UL[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
static const String GL_pLanguageLut_str[] = { "EN", "FR", "NL" };
static const String GL_pWCmdMediumLut_str[] = { "None", "COM0", "COM1", "COM2", "COM3", "UDP Server", "TCP Server", "GSM Server" };

/* ******************************************************************************** */
/* LCD Special Char
/* ******************************************************************************** */
byte GL_LcdWLinkLogo[8] = {
    B11111,
    B10001,
    B01010,
    B10001,
    B10001,
    B10001,
    B11111,
    B00000
};

/* ******************************************************************************** */
/* General Configuration
/* ******************************************************************************** */
LiquidCrystal GL_LcdObject_X(PIN_LCD_RS, PIN_LCD_RW, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

char GL_ppFlatPanel_KeyConfig_UB[4][4] = {	{ 'A','1','2','3' },        // A = Dedicated function (F1)
											{ 'B','4','5','6' },        // B = Dedicated function (F2)
											{ 'C','7','8','9' },        // C = Dedicated function (F3)
											{ 'X','V','0','.' }         // X = Cancel - V = Validate
											};

byte GL_pFlatPanel_RowPin_UB[4] = { PIN_FP7, PIN_FP6, PIN_FP5, PIN_FP4 };
byte GL_pFlatPanel_ColPin_UB[4] = { PIN_FP0, PIN_FP1, PIN_FP2, PIN_FP3 };
Keypad GL_Keypad_X = Keypad(makeKeymap(GL_ppFlatPanel_KeyConfig_UB), GL_pFlatPanel_RowPin_UB, GL_pFlatPanel_ColPin_UB, sizeof(GL_pFlatPanel_RowPin_UB), sizeof(GL_pFlatPanel_ColPin_UB));


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
    WCFG_INIT_RTC,
    WCFG_CHECK_TAG,
    WCFG_GET_BOARD_REVISION,
    WCFG_GET_LANGUAGE,
    WCFG_GET_GEN_CONFIG,
    WCFG_GET_WCMD_MEDIUM,
    WCFG_GET_IO_CONFIG,
    WCFG_GET_COM_CONFIG,
    WCFG_GET_ETH_CONFIG,
    WCFG_GET_TCP_SERVER_CONFIG,
    WCFG_GET_UDP_SERVER_CONFIG,
    WCFG_CONFIG_DONE,
    WCFG_ERROR_READING,
    WCFG_BAD_PARAM,
    WCFG_ERROR_INIT
};

static WCFG_STATE GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_IDLE;
static boolean GL_WConfigManagerEnabled_B = false;

static WCFG_STATUS GL_WConfigStatus_E = WCFG_STS_ERROR;

static unsigned char GL_pWConfigBuffer_UB[WCONFIG_PARAMETER_BUFFER_SIZE];


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void TransitionToConfigDone(void);
static void TransitionToErrorReading(void);
static void TransitionToBadParam(void);
static void TransitionToErrorInit(void);


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
    GL_WConfigStatus_E = WCFG_STS_NOT_ENABLED;
    GL_WConfigManagerEnabled_B = false;
}

WCFG_STATUS WConfigManager_Process() { 

    boolean ErrorInit_B = false;
    boolean BadParam_B = false;

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
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To INIT RTC");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_INIT_RTC;
        }
        else {
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "EEPROM cannot be initialized");
            TransitionToErrorInit();
        }

        break;


    /* INIT RTC */
    /* > Initialize RTC. */
    case WCFG_INIT_RTC:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize RTC Modules");
        GL_GlobalData_X.Rtc_H.init(&Wire, PIN_RTC_SQUARE_OUT);

        if (GL_GlobalData_X.Rtc_H.isInitialized()) {
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CHECK TAG");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_CHECK_TAG;
        }
        else {
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "RTC cannot be initialized");
            TransitionToErrorInit();
        }

        break;


    /* CHECK TAG */
    /* > Check if EEPROM has been configured or not thanks to the configuration tag. */
    case WCFG_CHECK_TAG:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive configuration tag");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_CONFIG_TAG, GL_pWConfigBuffer_UB, 2) == 2) {
            if ((GL_pWConfigBuffer_UB[0] == WCONFIG_CONFIG_TAG0) && (GL_pWConfigBuffer_UB[1] == WCONFIG_CONFIG_TAG1)) {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Configuration tag ok!");
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET BOARD REVISION");
                GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_BOARD_REVISION;
            }
            else {
                DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Error in configuration tag (no tag or bad tag)");
                TransitionToBadParam();
            }
        }
        else {
            TransitionToErrorReading();
        }

        break;


    /* GET BOARD REVISION */
    /* > Retreive board revision and language settings. */
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

            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET LANGUAGE");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_LANGUAGE;
        }
        else {
            TransitionToErrorReading();
        }

        break;


    /* GET LANGUAGE */
    /* > Retreive language. */
    case WCFG_GET_LANGUAGE:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive language settings");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_LANGUAGE, GL_pWConfigBuffer_UB, 1) == 1) {

            if ((GL_pWConfigBuffer_UB[0] & 0x0F) < 3) {
                GL_GlobalConfig_X.Language_E = (WLINK_LANGUAGE_ENUM)GL_pWConfigBuffer_UB[0];
                DBG_PRINT(DEBUG_SEVERITY_INFO, "Language sets to  ");
                DBG_PRINTDATA(GL_pLanguageLut_str[GL_pWConfigBuffer_UB[0]]);
                DBG_ENDSTR();

                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET GEN CONFIG");
                GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_GEN_CONFIG;
            }
            else {
                GL_GlobalConfig_X.Language_E = WLINK_LANGUAGE_EN;   // Default language
                DBG_PRINT(DEBUG_SEVERITY_ERROR, "Bad parameter for language settings (0x");
                DBG_PRINTDATABASE((GL_pWConfigBuffer_UB[0] & 0x0F), HEX);
                DBG_PRINTDATA(")");
                DBG_ENDSTR();
                DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Language sets to " + GL_pLanguageLut_str[GL_GlobalConfig_X.Language_E] + " (default)");
                TransitionToBadParam();
            }
        }
        else {
            TransitionToErrorReading();
        }


        break;


    /* GET GEN CONFIG */
    /* > Retreive general configuration. */
    case WCFG_GET_GEN_CONFIG:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive general configuration");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_GEN, GL_pWConfigBuffer_UB, 1) == 1) {

            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "General configuration :");

            // Check if WLink has LCD
            if ((GL_pWConfigBuffer_UB[0] & 0x01) == 0x01) {
                GL_GlobalConfig_X.HasLcd_B = true;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- LCD");
            }
            else {
                GL_GlobalConfig_X.HasLcd_B = false;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- No LCD");
            }


            // Check if WLink has Flat-Panel
            if ((GL_pWConfigBuffer_UB[0] & 0x02) == 0x02) {
                GL_GlobalConfig_X.HasFlatPanel_B = true;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- Flat-Panel");
            }
            else {
                GL_GlobalConfig_X.HasFlatPanel_B = false;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- No Flat-Panel");
            }


            // Check if WLink has MemoryCard
            if ((GL_pWConfigBuffer_UB[0] & 0x04) == 0x04) {
                GL_GlobalConfig_X.HasMemoryCard_B = true;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- Memory Card");
            }
            else {
                GL_GlobalConfig_X.HasMemoryCard_B = false;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- No Memory Card");
            }


            /* Initialize LCD Modules */
            if (GL_GlobalConfig_X.HasLcd_B) {                
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize LCD Modules");
                GL_GlobalData_X.Lcd_H.init(&GL_LcdObject_X, PIN_LCD_BACKLIGHT);
                GL_GlobalData_X.Lcd_H.clearDisplay();
                GL_GlobalData_X.Lcd_H.setBacklight(255);	// Max value for Backlight by default
                GL_GlobalData_X.Lcd_H.createChar(0, GL_LcdWLinkLogo);

                if (!GL_GlobalData_X.Lcd_H.isInitialized()) {
                    DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "LCD cannot be initialized");
                    ErrorInit_B = true;
                }                    
            }

            /* Initialize FlatPanel Modules */
            if (GL_GlobalConfig_X.HasFlatPanel_B) {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize Flat-Panel Modules");
                GL_GlobalData_X.FlatPanel_H.init(&GL_Keypad_X);

                if (!GL_GlobalData_X.FlatPanel_H.isInitialized()) {
                    FlatPanelManager_Disable();
                    DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Flat-Panel cannot be initialized");
                    ErrorInit_B = true;
                }
                else {
                    /* Initialize & Enable Interface Manager */
                    FlatPanelManager_Init(&(GL_GlobalData_X.FlatPanel_H));
                    FlatPanelManager_Enable();

                    /* Initialize default callbacks for each key */
                    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Assign default callback for each key");
                    for (int i = 0; i < 16; i++)
                        GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent((FLAT_PANEL_KEY_ENUM)i, DefaultKeyEvents);
                }
            }

            /* Initialize Memory Card Modules */
            if (GL_GlobalConfig_X.HasMemoryCard_B) {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize Memory Card Modules");
                GL_GlobalData_X.MemCard_H.init(PIN_SD_CS, PIN_SD_CD, PIN_SD_WP);

                if (!GL_GlobalData_X.MemCard_H.isInitialized()) {
                    DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Memory Card cannot be initialized");
                    ErrorInit_B = true;
                }
            }

            if (ErrorInit_B) {
                TransitionToErrorInit();
            }
            else {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of general configuration");

                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET WCMD MEDIUM");
                GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_WCMD_MEDIUM;
            }
        }
        else {
            TransitionToErrorReading();
        }

        break;


    /* GET WCMD MEDIUM */
    /* Retreive WCommand Medium. */
    case WCFG_GET_WCMD_MEDIUM:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive WCommand Medium");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_WCMD_MEDIUM, GL_pWConfigBuffer_UB, 1) == 1) {

            if ((GL_pWConfigBuffer_UB[0] & 0x0F) < 8) {
                GL_GlobalConfig_X.WCmdConfig_X.Medium_E = (WLINK_WCMD_MEDIUM_ENUM)(GL_pWConfigBuffer_UB[0] & 0x0F);
                DBG_PRINT(DEBUG_SEVERITY_INFO, "WCommand Medium sets to ");
                DBG_PRINTDATA(GL_pWCmdMediumLut_str[(GL_pWConfigBuffer_UB[0] & 0x0F)]);
                DBG_ENDSTR();

                if ((GL_pWConfigBuffer_UB[0] & 0x10) == 0x10) {
                    GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B = true;
                    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCommand Medium is mono-client");
                }
                else { 
                    GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B = false;
                    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "WCommand Medium is not mono-client -> close connection after each command");
                }

                /* Initialize W-Link Command Medium Modules */
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize W-Link Command Management Modules");

                switch (GL_GlobalConfig_X.WCmdConfig_X.Medium_E) {
                case WLINK_WCMD_MEDIUM_COM0:        WCmdMedium_Init(WCMD_MEDIUM_SERIAL, GetSerialHandle(0), GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B);                    break;
                case WLINK_WCMD_MEDIUM_COM1:        WCmdMedium_Init(WCMD_MEDIUM_SERIAL, GetSerialHandle(1), GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B);                    break;
                case WLINK_WCMD_MEDIUM_COM2:        WCmdMedium_Init(WCMD_MEDIUM_SERIAL, GetSerialHandle(2), GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B);                    break;
                case WLINK_WCMD_MEDIUM_COM3:        WCmdMedium_Init(WCMD_MEDIUM_SERIAL, GetSerialHandle(3), GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B);                    break;
                case WLINK_WCMD_MEDIUM_UDP_SERVER:  WCmdMedium_Init(WCMD_MEDIUM_UDP, &(GL_GlobalData_X.EthAP_X.UdpServer_H), GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B);   break;
                case WLINK_WCMD_MEDIUM_TCP_SERVER:  WCmdMedium_Init(WCMD_MEDIUM_TCP, &(GL_GlobalData_X.EthAP_X.TcpServer_H), GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B);   break;
                case WLINK_WCMD_MEDIUM_GSM_SERVER:  WCmdMedium_Init(WCMD_MEDIUM_GSM, &(GL_GlobalData_X.EthAP_X.GsmServer_H), GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B);   break;      // TODO : to modify once the GSMServer Object will be created
                }       

                WCommandInterpreter_Init(GL_GlobalConfig_X.WCmdConfig_X.pFctDescr_X, GL_GlobalConfig_X.WCmdConfig_X.NbFct_UL);

                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of WCommand Medium configuration");

                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET IO CONFIG");
                GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_IO_CONFIG;
            }
            else {

                DBG_PRINT(DEBUG_SEVERITY_ERROR, "Bad parameter for WCommand Medium settings (0x");
                DBG_PRINTDATABASE((GL_pWConfigBuffer_UB[0] & 0x0F), HEX);
                DBG_PRINTDATA(")");
                DBG_ENDSTR();

                WConfigManager_BuildSerialGateway();
                TransitionToBadParam();
            }           

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
                if ((GL_pWConfigBuffer_UB[i] & 0x01) == 0x01) {

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
                if ((GL_pWConfigBuffer_UB[i] & 0x01) == 0x01) {

                    pinMode(GL_GlobalData_X.pGpioOutputIndex_UB[i-4], OUTPUT);
                    digitalWrite(GL_GlobalData_X.pGpioOutputIndex_UB[i-4], LOW);

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


            /* Add Output Management for Bug in SPI ¨*/
            pinMode(PIN_SPI_CS, OUTPUT);


            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of I/O's configuration");

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

                GL_GlobalConfig_X.pComPortConfig_X[i].Index_UL = i;
                DBG_PRINT(DEBUG_SEVERITY_INFO, "- Configure COM");
                DBG_PRINTDATA(i);
                DBG_PRINTDATA(": ");

                if ((GL_pWConfigBuffer_UB[i * 3] & 0x01) == 0x01) {                    

                    // Check configuration
                    if ((GL_pWConfigBuffer_UB[i * 3 + 1] != 0x06)) {
                        DBG_PRINTDATA("Bad parameter in Config Mode (0x");
                        DBG_PRINTDATABASE(GL_pWConfigBuffer_UB[i * 3 + 1], HEX);
                        DBG_PRINTDATA(")");
                        DBG_ENDSTR();
                        BadParam_B = true;
                    }
                    else
                        GL_GlobalConfig_X.pComPortConfig_X[i].Config_UB = SERIAL_8N1;

                    // Check baudrate
                    if ((GL_pWConfigBuffer_UB[i * 3 + 2] > 0x07)) {
                        DBG_PRINTDATA("Bad parameter in Baudrate (0x");
                        DBG_PRINTDATABASE(GL_pWConfigBuffer_UB[i * 3 + 2], HEX);
                        DBG_PRINTDATA(")");
                        DBG_ENDSTR();
                        BadParam_B = true;
                    }
                    else
                        GL_GlobalConfig_X.pComPortConfig_X[i].Baudrate_UL = GL_pPortComSpeedLut_UL[GL_pWConfigBuffer_UB[i * 3 + 2]];

                    // Check if debug
                    GL_GlobalConfig_X.pComPortConfig_X[i].isDebug_B = ((GL_pWConfigBuffer_UB[i * 3] & 0x02) == 0x02) ? true : false;

                    // TODO : IRQ-based configuration sould be added here

                    // Configure actual COM port (if not debug port)
                    if (!GL_GlobalConfig_X.pComPortConfig_X[i].isDebug_B)
                        GetSerialHandle(i)->begin(GL_GlobalConfig_X.pComPortConfig_X[i].Baudrate_UL);


                    // Print out configuration
                    GL_GlobalConfig_X.pComPortConfig_X[i].isEnabled_B = true;
                    DBG_PRINTDATA(": Enabled (8N1, ");
                    DBG_PRINTDATA(GL_GlobalConfig_X.pComPortConfig_X[i].Baudrate_UL);
                    DBG_PRINTDATA("[baud])");
                    if (GL_GlobalConfig_X.pComPortConfig_X[i].isDebug_B)
                        DBG_PRINTDATA(" -> Configured as Debug COM port");
                    DBG_ENDSTR();

                }
                else {
                    GL_GlobalConfig_X.pComPortConfig_X[i].isEnabled_B = false;
                    DBG_PRINTDATA(": Not enabled");
                    DBG_ENDSTR();
                }

            }

            // Configure Debug COM port in last
            for (int i = 0; i < 4; i++) {
                if ((GL_GlobalConfig_X.pComPortConfig_X[i].isEnabled_B) && (GL_GlobalConfig_X.pComPortConfig_X[i].isDebug_B)) {
                    if (i == WLINK_DEBUG_DEFAULT_COM_PORT) {
                        if (GL_GlobalConfig_X.pComPortConfig_X[i].Baudrate_UL != WLINK_DEBUG_DEFAULT_SPEED) {
                            DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Change Debug output speed");
                            delay(10);
                            Debug_Init(GetSerialHandle(i), GL_GlobalConfig_X.pComPortConfig_X[i].Baudrate_UL);
                        }
                        else {
                            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "No change in Debug COM port default configuration");
                        }
                    }
                    else {
                        DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Change Debug COM port");
                        delay(10);
                        Debug_Init(GetSerialHandle(i), GL_GlobalConfig_X.pComPortConfig_X[i].Baudrate_UL);
                    }
                }
            }


            if (BadParam_B) {
                TransitionToBadParam();
            }
            else {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of COM Ports configuration");

                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET ETH CONFIG");
                GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_ETH_CONFIG;
            }
        }
        else {
            TransitionToErrorReading();
        }
        
        break;


    /* GET ETH CONFIG */
    /* > Retreive Ethernet configuration. */
    case WCFG_GET_ETH_CONFIG:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive Ethernet configuration");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_ETH, GL_pWConfigBuffer_UB, 24) == 24) {

            if ((GL_pWConfigBuffer_UB[0] & 0x01) == 0x01) {
                GL_GlobalConfig_X.EthConfig_X.isEnabled_B = true;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Ethernet enabled");

                // Get MAC Address
                for (int i = 0; i < sizeof(GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB); i++) {
                    GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB[i] = GL_pWConfigBuffer_UB[i + 2];
                }
                DBG_PRINT(DEBUG_SEVERITY_INFO, "- MAC Address = [");
                DBG_PRINTDATA(HexArrayToString(GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB, sizeof(GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB), ":"));
                DBG_PRINTDATA("]");
                DBG_ENDSTR();

                // Check if DHCP
                if ((GL_pWConfigBuffer_UB[0] & 0x02) == 0x02) {
                    GL_GlobalConfig_X.EthConfig_X.isDhcp_B = true;
                    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- DHCP enabled");
                }
                else {
                    GL_GlobalConfig_X.EthConfig_X.isDhcp_B = false;
                    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- DHCP not enabled");

                    GL_GlobalConfig_X.EthConfig_X.IpAddr_X = IPAddress(GL_pWConfigBuffer_UB[8], GL_pWConfigBuffer_UB[9], GL_pWConfigBuffer_UB[10], GL_pWConfigBuffer_UB[11]);
                    DBG_PRINT(DEBUG_SEVERITY_INFO, "- IP Address = ");
                    DBG_PRINTDATA(GL_GlobalConfig_X.EthConfig_X.IpAddr_X);
                    DBG_ENDSTR();

                    // Check if Advanced Configuration
                    if ((GL_pWConfigBuffer_UB[0] & 0x03) == 0x03) {
                        GL_GlobalConfig_X.EthConfig_X.isAdvancedConfig_B = true;
                        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- Advanced configuration");

                        GL_GlobalConfig_X.EthConfig_X.SubnetMaskAddr_X = IPAddress(GL_pWConfigBuffer_UB[8], GL_pWConfigBuffer_UB[9], GL_pWConfigBuffer_UB[10], GL_pWConfigBuffer_UB[11]);
                        DBG_PRINT(DEBUG_SEVERITY_INFO, "- Subnet Mask = ");
                        DBG_PRINTDATA(GL_GlobalConfig_X.EthConfig_X.SubnetMaskAddr_X);
                        DBG_ENDSTR();

                        GL_GlobalConfig_X.EthConfig_X.GatewayAddr_X = IPAddress(GL_pWConfigBuffer_UB[8], GL_pWConfigBuffer_UB[9], GL_pWConfigBuffer_UB[10], GL_pWConfigBuffer_UB[11]);
                        DBG_PRINT(DEBUG_SEVERITY_INFO, "- Gateway Address = ");
                        DBG_PRINTDATA(GL_GlobalConfig_X.EthConfig_X.GatewayAddr_X);
                        DBG_ENDSTR();

                        GL_GlobalConfig_X.EthConfig_X.DnsIpAddr_X = IPAddress(GL_pWConfigBuffer_UB[8], GL_pWConfigBuffer_UB[9], GL_pWConfigBuffer_UB[10], GL_pWConfigBuffer_UB[11]);
                        DBG_PRINT(DEBUG_SEVERITY_INFO, "- DNS Address = ");
                        DBG_PRINTDATA(GL_GlobalConfig_X.EthConfig_X.DnsIpAddr_X);
                        DBG_ENDSTR();
                    }
                    else {
                        GL_GlobalConfig_X.EthConfig_X.isAdvancedConfig_B = false;
                        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- No advanced configuration");
                    }
                }
                
                // Initialize Network Adapter
                if (GL_GlobalConfig_X.EthConfig_X.isDhcp_B) {
                    GL_GlobalData_X.Network_H.init(PIN_ETH_LINKED, GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB);
                }
                else {
                    if (GL_GlobalConfig_X.EthConfig_X.isAdvancedConfig_B) {
                        GL_GlobalData_X.Network_H.init(PIN_ETH_LINKED, GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB, GL_GlobalConfig_X.EthConfig_X.IpAddr_X, GL_GlobalConfig_X.EthConfig_X.SubnetMaskAddr_X, GL_GlobalConfig_X.EthConfig_X.GatewayAddr_X, GL_GlobalConfig_X.EthConfig_X.DnsIpAddr_X);
                    }
                    else {
                        GL_GlobalData_X.Network_H.init(PIN_ETH_LINKED, GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB, GL_GlobalConfig_X.EthConfig_X.IpAddr_X);
                    }
                }

                // Enable Network Adapter Manager
                NetworkAdapterManager_Init(&(GL_GlobalData_X.Network_H));
                NetworkAdapterManager_Enable(); // GL_GlobalData_X.Network_H.begin() is called in NetworkAdapterManager_Process() when cable is conneted

                
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of Ethernet configuration");
            }
            else {
                // Disable Network Adapter Manager
                NetworkAdapterManager_Disable();

                GL_GlobalConfig_X.EthConfig_X.isEnabled_B = false;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Ethernet not enabled");
            }


            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET TCP SERVER CONFIG");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_TCP_SERVER_CONFIG;

        }
        else {
            TransitionToErrorReading();
        }

        break;


    /* GET TCP SERVER CONFIG */
    /* > Retreive TCP Server configuration. */
    case WCFG_GET_TCP_SERVER_CONFIG:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive TCP Server configuration");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_TCP_SERVER, GL_pWConfigBuffer_UB, 4) == 4) {
        
            if ((GL_pWConfigBuffer_UB[0] & 0x01) == 0x01) {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server enabled");

                GL_GlobalConfig_X.EthConfig_X.TcpServerConfig_X.LocalPort_UI = (unsigned int)((GL_pWConfigBuffer_UB[2] << 8) + GL_pWConfigBuffer_UB[1]);
                DBG_PRINT(DEBUG_SEVERITY_INFO, "Local port = ");
                DBG_PRINTDATA(GL_GlobalConfig_X.EthConfig_X.TcpServerConfig_X.LocalPort_UI);
                DBG_ENDSTR();

                /* Initialize TCP Server Modules */
                GL_GlobalData_X.EthAP_X.TcpServer_H.init(&(GL_GlobalData_X.Network_H), GL_GlobalConfig_X.EthConfig_X.TcpServerConfig_X.LocalPort_UI);
                if (GL_GlobalData_X.EthAP_X.TcpServer_H.isInitialized()) {
                    TCPServerManager_Init(&(GL_GlobalData_X.Network_H), &(GL_GlobalData_X.EthAP_X.TcpServer_H));
                    TCPServerManager_Enable();
                    GL_GlobalConfig_X.EthConfig_X.TcpServerConfig_X.isEnabled_B = true;
                }

                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of TCP Server configuration");

            }
            else {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server not enabled");
                GL_GlobalConfig_X.EthConfig_X.TcpServerConfig_X.isEnabled_B = false;
            }

            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET UDP SERVER CONFIG");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_UDP_SERVER_CONFIG;        
        }
        else {
            TransitionToErrorReading();        
        }

        break;



    /* GET UDP SERVER CONFIG */
    /* > Retreive UDP Server configuration. */
    case WCFG_GET_UDP_SERVER_CONFIG:

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive UDP Server configuration");
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_UDP_SERVER, GL_pWConfigBuffer_UB, 4) == 4) {

            if ((GL_pWConfigBuffer_UB[0] & 0x01) == 0x01) {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "UDP Server enabled");
                GL_GlobalConfig_X.EthConfig_X.UdpServerConfig_X.isEnabled_B = true;

                GL_GlobalConfig_X.EthConfig_X.UdpServerConfig_X.LocalPort_UI = (unsigned int)((GL_pWConfigBuffer_UB[2] << 8) + GL_pWConfigBuffer_UB[1]);
                DBG_PRINT(DEBUG_SEVERITY_INFO, "Local port = ");
                DBG_PRINTDATA(GL_GlobalConfig_X.EthConfig_X.UdpServerConfig_X.LocalPort_UI);
                DBG_ENDSTR();

                /* Initialize UDP Server Modules */
                GL_GlobalData_X.EthAP_X.UdpServer_H.init(&(GL_GlobalData_X.Network_H), GL_GlobalConfig_X.EthConfig_X.UdpServerConfig_X.LocalPort_UI);
                if (GL_GlobalData_X.EthAP_X.UdpServer_H.isInitialized()) {
                    UDPServerManager_Init(&(GL_GlobalData_X.Network_H), &(GL_GlobalData_X.EthAP_X.UdpServer_H));
                    UDPServerManager_Enable();
                    GL_GlobalConfig_X.EthConfig_X.UdpServerConfig_X.isEnabled_B = true;
                }

                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of UDP Server configuration");
            }
            else {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "UDP Server not enabled");
                GL_GlobalConfig_X.EthConfig_X.UdpServerConfig_X.isEnabled_B = false;
            }

            TransitionToConfigDone();
        }
        else {
            TransitionToErrorReading();
        }

        break;


    ///* GET XXX CONFIG */
    ///* > Retreive XXX configuration. */
    //case WCFG_STATE:

        //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive XXX configuration");
        //if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_XXX, GL_pWConfigBuffer_UB, XXX) == XXX) {



        //      TransitionToConfigDone();


        //}
        //else {
        //    TransitionToErrorReading();
        //}

    //    break;


    /* CONFIG DONE */
    /* > End of configuration phase. State machine idling until renew of configuration. */
    case WCFG_CONFIG_DONE:

        GL_WConfigStatus_E = WCFG_STS_OK;

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


    /* ERROR INIT */
    /* > Error while trying to initialize an object in a previous state. */
    case WCFG_ERROR_INIT:

        GL_WConfigStatus_E = WCFG_STS_ERROR_INIT;

        break;


    }

    return GL_WConfigStatus_E;
}


void WConfigManager_BuildSerialGateway(void) {

    DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Build Serial Gateway");

    GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B = true;
    DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "WCommand Medium is mono-client");
    GL_GlobalConfig_X.WCmdConfig_X.Medium_E = WLINK_WCMD_MEDIUM_COM0;   // Default Medium = Default debug port
    DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "WCommand Medium sets to " + GL_pWCmdMediumLut_str[GL_GlobalConfig_X.WCmdConfig_X.Medium_E] + " (default)");

    WCmdMedium_Init(WCMD_MEDIUM_SERIAL, GetSerialHandle(0), GL_GlobalConfig_X.WCmdConfig_X.isMonoClient_B);
    WCommandInterpreter_Init(GL_GlobalConfig_X.WCmdConfig_X.pFctDescr_X, GL_GlobalConfig_X.WCmdConfig_X.NbFct_UL);

}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void TransitionToConfigDone(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of Configuration from EEPROM..");
    GL_WConfigStatus_E = WCFG_STS_OK;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONFIG DONE");
    GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_CONFIG_DONE;
}

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

void TransitionToErrorInit(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Error while trying to initialize an object");
    GL_WConfigStatus_E = WCFG_STS_ERROR_INIT;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To ERROR INIT");
    GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_ERROR_INIT;
}


/* ******************************************************************************** */
/* Configuration Functions
/* ******************************************************************************** */
void WConfig_SetLanguage(unsigned char * pLanguage_UB) {

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Set language");
    if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_LANGUAGE, GL_pWConfigBuffer_UB, 1) == 1) {

        // Get Byte and change it
        GL_pWConfigBuffer_UB[0] &= 0xF0;
        GL_pWConfigBuffer_UB[0] |= (*pLanguage_UB);

        // Write in EEPROM
        GL_GlobalData_X.Eeprom_H.write(WCONFIG_ADDR_LANGUAGE, GL_pWConfigBuffer_UB, 1);

        // Assign Global Config Data
        GL_GlobalConfig_X.Language_E = *((WLINK_LANGUAGE_ENUM *)(pLanguage_UB));
        DBG_PRINT(DEBUG_SEVERITY_INFO, "Language sets to  ");
        DBG_PRINTDATA(GL_pLanguageLut_str[*pLanguage_UB]);
        DBG_ENDSTR();

    }

}


void WConfig_SetDate(unsigned char * pDate_UB) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Set date");

    RTC_DATE_STRUCT Date_X;
    Date_X.Day_UB = pDate_UB[0] * 10 + pDate_UB[1];
    Date_X.Month_UB = pDate_UB[2] * 10 + pDate_UB[3];
    Date_X.Year_UB = pDate_UB[4] * 10 + pDate_UB[5];

    DBG_PRINT(DEBUG_SEVERITY_INFO, "Date sets to : ");
    DBG_PRINTDATA(Date_X.Day_UB);
    DBG_PRINTDATA("/");
    DBG_PRINTDATA(Date_X.Month_UB);
    DBG_PRINTDATA("/");
    DBG_PRINTDATA(Date_X.Year_UB);
    DBG_ENDSTR();

    // Call low-level function
    GL_GlobalData_X.Rtc_H.setDate(Date_X);
}


void WConfig_SetTime(unsigned char * pTime_UB) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Set time");

    RTC_TIME_STRUCT Time_X;
    Time_X.Hour_UB = pTime_UB[0] * 10 + pTime_UB[1];
    Time_X.Min_UB = pTime_UB[2] * 10 + pTime_UB[3];
    Time_X.Sec_UB = pTime_UB[4] * 10 + pTime_UB[5];;

    DBG_PRINT(DEBUG_SEVERITY_INFO, "Time sets to : ");
    DBG_PRINTDATA(Time_X.Hour_UB);
    DBG_PRINTDATA(":");
    DBG_PRINTDATA(Time_X.Min_UB);
    DBG_PRINTDATA(":");
    DBG_PRINTDATA(Time_X.Sec_UB);
    DBG_ENDSTR();

    // Call low-level function
    GL_GlobalData_X.Rtc_H.setTime(Time_X);
}