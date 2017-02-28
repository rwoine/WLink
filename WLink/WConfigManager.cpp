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

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define WCONFIG_PARAMETER_BUFFER_SIZE   128

#define WCONFIG_CONFIG_TAG0             0xAA
#define WCONFIG_CONFIG_TAG1             0x55

    
#define WCONFIG_ADDR_CONFIG_TAG         0x0000
#define WCONFIG_ADDR_BOARD_REV          0x0002
#define WCONFIG_ADDR_GEN                0x0006
#define WCONFIG_ADDR_IO                 0x0008
#define WCONFIG_ADDR_COM                0x0010
#define WCONFIG_ADDR_ETH                0x001C


/* ******************************************************************************** */
/* LUT
/* ******************************************************************************** */
unsigned long GL_pPortComSpeedLut_UL[] = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
static const String GL_pLanguageLut_str[] = { "EN", "FR", "NL" };


/* ******************************************************************************** */
/* General Configuration
/* ******************************************************************************** */
LiquidCrystal GL_LcdObject_X(PIN_LCD_RS, PIN_LCD_RW, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

char GL_ppFlatPanel_KeyConfig_UB[4][4] = {	{ 'A','1','2','3' },        // A = Dedicated function (F1)
											{ 'B','4','5','6' },        // B = Dedicated function (F2)
											{ 'C','7','8','9' },        // C = Dedicated function (F3)
											{ 'X','V','0','.' }
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
    WCFG_GET_GEN_CONFIG,
    WCFG_GET_IO_CONFIG,
    WCFG_GET_COM_CONFIG,
    WCFG_GET_ETH_CONFIG,
    WCFG_CONFIG_DONE,
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
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To INIT RTC");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_INIT_RTC;
        }
        else {
            // TODO : Manage error if EEPROM not initialized
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "EEPROM cannot be initialized");
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
            // TODO : Manage error if RTC not initialized
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "RTC cannot be initialized");
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
                // TODO : Manage error if no or bad configuration tag
                DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Error in configuration tag (no tag or bad tag)");
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
        if (GL_GlobalData_X.Eeprom_H.read(WCONFIG_ADDR_BOARD_REV, GL_pWConfigBuffer_UB, 3) == 3) {

            GL_GlobalConfig_X.MajorRev_UB = GL_pWConfigBuffer_UB[0];
            GL_GlobalConfig_X.MinorRev_UB = GL_pWConfigBuffer_UB[1];
            DBG_PRINT(DEBUG_SEVERITY_INFO, "Board Revision = ");
            DBG_PRINTDATA(GL_GlobalConfig_X.MajorRev_UB);
            DBG_PRINTDATA(".");
            DBG_PRINTDATA(GL_GlobalConfig_X.MinorRev_UB);
            DBG_ENDSTR();

            if (GL_pWConfigBuffer_UB[2] < 3) {
                GL_GlobalConfig_X.Language_E = (WLINK_LANGUAGE_ENUM)GL_pWConfigBuffer_UB[2];
                DBG_PRINT(DEBUG_SEVERITY_INFO, "Language sets to  ");
                DBG_PRINTDATA(GL_pLanguageLut_str[GL_pWConfigBuffer_UB[2]]);
                DBG_ENDSTR();
            }
            else {
                GL_GlobalConfig_X.Language_E = WLINK_LANGUAGE_EN;   // Default language
                DBG_PRINTDATA("Bad parameter for language settings (0x");
                DBG_PRINTDATABASE(GL_pWConfigBuffer_UB[2], HEX);
                DBG_PRINTDATA(")");
                DBG_ENDSTR();
                TransitionToBadParam();
            }

            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET GEN CONFIG");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_GEN_CONFIG;
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
            if ((GL_pWConfigBuffer_UB[0] && 0x01) == 0x01) {
                GL_GlobalConfig_X.HasLcd_B = true;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- LCD");
            }
            else {
                GL_GlobalConfig_X.HasLcd_B = false;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- No LCD");
            }


            // Check if WLink has Flat-Panel
            if ((GL_pWConfigBuffer_UB[0] && 0x02) == 0x02) {
                GL_GlobalConfig_X.HasFlatPanel_B = true;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- Flat-Panel");
            }
            else {
                GL_GlobalConfig_X.HasFlatPanel_B = false;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "- No Flat-Panel");
            }


            // Check if WLink has MemoryCard
            if ((GL_pWConfigBuffer_UB[0] && 0x04) == 0x04) {
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
                GL_GlobalData_X.Lcd_H.setBacklight(255);	// Max value for Backlight by default

                if (!GL_GlobalData_X.Lcd_H.isInitialized()) {
                    // TODO : Manage error if LCD not initialized
                    DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "LCD cannot be initialized");
                }                    
            }

            /* Initialize FlatPanel Modules */
            if (GL_GlobalConfig_X.HasFlatPanel_B) {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize Flat-Panel Modules");
                GL_GlobalData_X.FlatPanel_H.init(&GL_Keypad_X);

                if (!GL_GlobalData_X.FlatPanel_H.isInitialized()) {
                    // TODO : Manage error if Flat-Panel not initialized
                    DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Flat-Panel cannot be initialized");
                }
            }

            /* Initialize Memory Card Modules */
            if (GL_GlobalConfig_X.HasMemoryCard_B) {
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize Memory Card Modules");
                GL_GlobalData_X.MemCard_H.init(PIN_SD_CS, PIN_SD_CD, PIN_SD_WP);

                if (!GL_GlobalData_X.MemCard_H.isInitialized()) {
                    // TODO : Manage error if Memory Card not initialized
                    DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Memory Card cannot be initialized");
                }
            }


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
                        GL_GlobalConfig_X.pComPortConfig_X[i].Config_UB = SERIAL_8N1;

                    // Check baudrate
                    if ((GL_pWConfigBuffer_UB[i * 3 + 2] > 0x07)) {
                        DBG_PRINTDATA("Bad parameter in Baudrate (0x");
                        DBG_PRINTDATABASE(GL_pWConfigBuffer_UB[i * 3 + 2], HEX);
                        DBG_PRINTDATA(")");
                        DBG_ENDSTR();
                        TransitionToBadParam();
                    }
                    else
                        GL_GlobalConfig_X.pComPortConfig_X[i].Baudrate_UL = GL_pPortComSpeedLut_UL[GL_pWConfigBuffer_UB[i * 3 + 2]];

                    // Check if debug
                    GL_GlobalConfig_X.pComPortConfig_X[i].isDebug_B = ((GL_pWConfigBuffer_UB[i * 3] && 0x02) == 0x02) ? true : false;

                    // TODO : IRQ-based configuration sould be added here

                    // Configure actual COM port (if not debug port)
                    if (!GL_GlobalConfig_X.pComPortConfig_X[i].isDebug_B)
                        GetSerialHandle(i)->begin(GL_GlobalConfig_X.pComPortConfig_X[i].Baudrate_UL);


                    // Print out configuration
                    GL_GlobalConfig_X.pComPortConfig_X[i].isEnabled_B = true;
                    DBG_PRINTDATA(": Enabled (8N1, ");
                    DBG_PRINTDATA(GL_GlobalConfig_X.pComPortConfig_X[i].Baudrate_UL);
                    DBG_PRINTDATA(" [baud])");
                    if (GL_GlobalConfig_X.pComPortConfig_X[i].isDebug_B)
                        DBG_PRINTDATA(" -> Configured as Debug COM port");
                    DBG_ENDSTR();

                }
                else {
                    GL_GlobalConfig_X.pComPortConfig_X[i].isEnabled_B = false;
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


            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of COM Ports configuration");


            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET ETH CONFIG");
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_GET_ETH_CONFIG;
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

            if ((GL_pWConfigBuffer_UB[0] && 0x01) == 0x01) {
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
                if ((GL_pWConfigBuffer_UB[0] && 0x02) == 0x02) {
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
                    if ((GL_pWConfigBuffer_UB[0] && 0x03) == 0x03) {
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

            }
            else {
                // Disable Network Adapter Manager
                NetworkAdapterManager_Disable();

                GL_GlobalConfig_X.EthConfig_X.isEnabled_B = false;
                DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Ethernet not enabled");
            }


            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of Ethernet configuration");


            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONFIG DONE");
            GL_WConfigStatus_E = WCFG_STS_OK;
            GL_WConfigManager_CurrentState_E = WCFG_STATE::WCFG_CONFIG_DONE;
        }
        else {
            TransitionToErrorReading();
        }

        break;


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
