/* ******************************************************************************** */
/*                                                                                  */
/*                           W-LINK APPLICATIVE CODE                                */
/*                                                                                  */
/*                                                                                  */
/* Description :                                                                    */
/*                                                                                  */
/* History :	01/12/2014  (RW)	Creation of this file                           */
/*				14/05/2016	(RW)	Re-mastered version								*/
/*              27/02/2017  (RW)    Re-mastered version with WConfigManager         */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"Main"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <SD.h>

#include "WLink.h"
#include "Hardware.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Constant
/* ******************************************************************************** */
const String cGL_pWLinkRevisionId_Str = "17121901";	// YYMMDDVV - Year-Month-Day-Version

/* ******************************************************************************** */
/* Global
/* ******************************************************************************** */
GLOBAL_PARAM_STRUCT GL_GlobalData_X;
GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;

unsigned long GL_AbsoluteTime_UL;

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
void BlinkingLedManager_Process(void);

/* ******************************************************************************** */
/* Functions Mapping
/* ******************************************************************************** */
const WCMD_FCT_DESCR cGL_pFctDescr_X[] =
{
	{ WCMD_GET_REVISION_ID, WCmdProcess_GetRevisionId },

	{ WCMD_GPIO_READ, WCmdProcess_GpioRead },
	{ WCMD_GPIO_WRITE, WCmdProcess_GpioWrite },
	{ WCMD_GPIO_SET_BIT, WCmdProcess_GpioSetBit },
	{ WCMD_GPIO_CLR_BIT, WCmdProcess_GpioClrBit },

	{ WCMD_INDICATOR_GET_WEIGHT, WCmdProcess_IndicatorGetWeight },
	{ WCMD_INDICATOR_GET_WEIGHT_ALIBI, WCmdProcess_IndicatorGetWeightAlibi },
	{ WCMD_INDICATOR_SET_ZERO, WCmdProcess_IndicatorSetZero },
	{ WCMD_INDICATOR_GET_WEIGHT_ASCII, WCmdProcess_IndicatorGetWeightAscii },

	{ WCMD_BADGE_READER_GET_ID, WCmdProcess_BadgeReaderGetBadgeId },

	{ WCMD_LCD_WRITE, WCmdProcess_LcdWrite },
	{ WCMD_LCD_READ, WCmdProcess_LcdRead },
	{ WCMD_LCD_CLEAR, WCmdProcess_LcdClear },
	{ WCMD_LCD_SET_BACKLIGHT, WCmdProcess_LcdSetBacklight },
	{ WCMD_LCD_ENABLE_EXT_WRITE, WCmdProcess_LcdEnableExternalWrite },
	{ WCMD_LCD_DISABLE_EXT_WRITE, WCmdProcess_LcdDisableExternalWrite },
	{ WCMD_LCD_GET_EXT_WRITE_STATUS, WCmdProcess_LcdGetExternalWriteStatus },
	{ WCMD_LCD_GET_EXT_WRITE_DATA, WCmdProcess_LcdGetExternalWriteData },

    { WCMD_EEPROM_WRITE, WCmdProcess_EepromWrite },
    { WCMD_EEPROM_READ, WCmdProcess_EepromRead },

    { WCMD_RTC_SET_DATETIME, WCmdProcess_RtcSetDateTime },
    { WCMD_RTC_GET_DATETIME, WCmdProcess_RtcGetDateTime },

	{ WCMD_COMPORT_OPEN, WCmdProcess_ComPortOpen },
    { WCMD_COMPORT_CLOSE, WCmdProcess_ComPortClose },
    { WCMD_COMPORT_WRITE, WCmdProcess_ComPortWrite },
    { WCMD_COMPORT_ENABLE_TUNNEL, WCmdProcess_ComPortEnableTunnel },
    { WCMD_COMPORT_DISABLE_TUNNEL, WCmdProcess_ComPortDisableTunnel },

	{ WCMD_TEST_CMD, WCmdProcess_TestCommand }

};



/* ******************************************************************************** */
/* Setup
/* ******************************************************************************** */
void setup() {

    /* Assign Static Pinout */
    GL_GlobalData_X.pGpioInputIndex_UB[0] = PIN_GPIO_INPUT0;
    GL_GlobalData_X.pGpioInputIndex_UB[1] = PIN_GPIO_INPUT1;
    GL_GlobalData_X.pGpioInputIndex_UB[2] = PIN_GPIO_INPUT2;
    GL_GlobalData_X.pGpioInputIndex_UB[3] = PIN_GPIO_INPUT3;
    GL_GlobalData_X.pGpioOutputIndex_UB[0] = PIN_GPIO_OUTPUT0;
    GL_GlobalData_X.pGpioOutputIndex_UB[1] = PIN_GPIO_OUTPUT1;
    GL_GlobalData_X.pGpioOutputIndex_UB[2] = PIN_GPIO_OUTPUT2;
    GL_GlobalData_X.pGpioOutputIndex_UB[3] = PIN_GPIO_OUTPUT3;

    GL_GlobalData_X.LedPin_UB = PIN_BLINK_LED;



    /* Enable All UARTs by Default */
    pinMode(PIN_EN_SERIAL01, OUTPUT);
    digitalWrite(PIN_EN_SERIAL01, LOW);
    pinMode(PIN_EN_SERIAL23, OUTPUT);
    digitalWrite(PIN_EN_SERIAL23, LOW);

    /* Initialize Debug Module with Default Parameters */
    Debug_Init(GetSerialHandle(WLINK_DEBUG_DEFAULT_COM_PORT), WLINK_DEBUG_DEFAULT_SPEED);



    /* Assign W-Link Commands in Configuration Structure */
    GL_GlobalConfig_X.WCmdConfig_X.pFctDescr_X = cGL_pFctDescr_X;
    GL_GlobalConfig_X.WCmdConfig_X.NbFct_UL = (sizeof(cGL_pFctDescr_X) / sizeof(WCMD_FCT_DESCR));



    /* Print out Global Header */ 
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "-------------------- W-LINK --------------------");

    /* Assign Revision ID */
    DBG_PRINT(DEBUG_SEVERITY_INFO, "Revision ID = " + cGL_pWLinkRevisionId_Str);
    DBG_ENDSTR();
    for (int i = 0; i < 8; i++)
        GL_GlobalData_X.pRevisionId_UB[i] = cGL_pWLinkRevisionId_Str.charAt(i);
    GL_GlobalData_X.RevisionId_Str = cGL_pWLinkRevisionId_Str;



    ///* Configure Blinking LED */
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Configure Blinking LED");
    pinMode(GL_GlobalData_X.LedPin_UB, OUTPUT);
    digitalWrite(GL_GlobalData_X.LedPin_UB, HIGH);	// Turn-on by default


    /* W-Link Manager Initialization */
    WLinkManager_Init();
    WLinkManager_Enable();


	/* Record Absolute Time */
	GL_AbsoluteTime_UL = millis();
}

/* ******************************************************************************** */
/* Loop
/* ******************************************************************************** */
void loop() {

    BlinkingLedManager_Process();
    WLinkManager_Process();

}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
void BlinkingLedManager_Process(void) {
	if ((millis() - GL_AbsoluteTime_UL) >= 500) {
		if(digitalRead(GL_GlobalData_X.LedPin_UB) == LOW)
			digitalWrite(GL_GlobalData_X.LedPin_UB, HIGH);
		else
			digitalWrite(GL_GlobalData_X.LedPin_UB, LOW);
		GL_AbsoluteTime_UL = millis();
	}
}


/* ******************************************************************************** */
/* Events
/* ******************************************************************************** */

/* Serial Event */
void serialEvent() { GL_GlobalConfig_X.pComPortConfig_X[PORT_COM0].pFctCommEvent(); }
void serialEvent1() { GL_GlobalConfig_X.pComPortConfig_X[PORT_COM1].pFctCommEvent(); }
void serialEvent2() { GL_GlobalConfig_X.pComPortConfig_X[PORT_COM2].pFctCommEvent(); }
void serialEvent3() { GL_GlobalConfig_X.pComPortConfig_X[PORT_COM3].pFctCommEvent(); }
