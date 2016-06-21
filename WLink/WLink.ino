/* ******************************************************************************** */
/*                                                                                  */
/*                           W-LINK APPLICATIVE CODE                                */
/*                                                                                  */
/*                                                                                  */
/* Description :                                                                    */
/*                                                                                  */
/* History :	01/12/2014  (RW)	Creation of this file                           */
/*				14/05/2016	(RW)	Re-mastered version								*/
/*                                                                                  */
/* ******************************************************************************** */
#define MODULE_NAME		"Main"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

#include "WLink.h"
#include "Hardware.h"
#include "CommEvent.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Constant
/* ******************************************************************************** */
const String cGL_pWLinkRevisionId_Str = "16062001";	// YYMMDDVV - Year-Month-Day-Version

/* ******************************************************************************** */
/* Global
/* ******************************************************************************** */
GLOBAL_PARAM_STRUCT GL_GlobalData_X;

/* ******************************************************************************** */
/* Flat Panel Configuration
/* ******************************************************************************** */
char GL_ppFlatPanel_KeyConfig_UB[4][4] = {	{ 'A','1','2','3' },
											{ 'B','4','5','6' },
											{ 'C','7','8','9' },
											{ 'X','V','0','.' }
											};

byte GL_pFlatPanel_RowPin_UB[4] = { PIN_FP7, PIN_FP6, PIN_FP5, PIN_FP4 };
byte GL_pFlatPanel_ColPin_UB[4] = { PIN_FP0, PIN_FP1, PIN_FP2, PIN_FP3 };

/* ******************************************************************************** */
/* Serial Related
/* ******************************************************************************** */
HardwareSerial * GL_PortComMap_X[] = { &Serial, &Serial1, &Serial2, &Serial3 };
COM_EVENT_FCT_STRUCT GL_PortComEventMap_X[] = { NULL, NULL, NULL, NULL };

/* ******************************************************************************** */
/* Pre-Built Objects
/* ******************************************************************************** */
Keypad GL_Keypad_X = Keypad(makeKeymap(GL_ppFlatPanel_KeyConfig_UB), GL_pFlatPanel_RowPin_UB, GL_pFlatPanel_ColPin_UB, sizeof(GL_pFlatPanel_RowPin_UB), sizeof(GL_pFlatPanel_ColPin_UB));
LiquidCrystal GL_LcdObject_X(PIN_LCD_RS, PIN_LCD_RW, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

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
	{ WCMD_LCD_SET_BACKLIGHT, WCmdProcess_LcdSetBacklight }

};

#define WCMD_FCT_DESCR_SIZE (sizeof(cGL_pFctDescr_X)/sizeof(WCMD_FCT_DESCR))


/* ******************************************************************************** */
/* Setup
/* ******************************************************************************** */
void setup() {

	/* Network Configuration */
	GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[0] = 0x02;
	GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[1] = 0x00;
	GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[2] = 0x00;
	GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[3] = 0x01;
	GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[4] = 0x00;
	GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[5] = 0x0C;
	GL_GlobalData_X.NetworkIf_X.IpAddr_X = IPAddress(192, 168, 1, 16);
	GL_GlobalData_X.NetworkIf_X.LocalPort_UI = 23;

	/* Enable All UARTs by Default */
	pinMode(PIN_EN_SERIAL01, OUTPUT);
	digitalWrite(PIN_EN_SERIAL01, LOW);
	pinMode(PIN_EN_SERIAL23, OUTPUT);
	digitalWrite(PIN_EN_SERIAL23, LOW);

	/* Initialize Debug Module */
	Debug_Init(GL_PortComMap_X[PORT_COM0], 115200);

	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "---------- W-LINK ----------");

	/* Assign Revision ID */
	DBG_PRINT(DEBUG_SEVERITY_INFO, "Revision ID = ");
	for (int i = 0; i < 8; i++)
		GL_GlobalData_X.pRevisionId_UB[i] = cGL_pWLinkRevisionId_Str.charAt(i);
	DBG_PRINTDATA(cGL_pWLinkRevisionId_Str);
	DBG_ENDSTR();

	/* Initialize LED */
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize LED");
	GL_GlobalData_X.LedPin_UB = PIN_BLINK_LED;
	pinMode(GL_GlobalData_X.LedPin_UB, OUTPUT);
	//digitalWrite(GL_GlobalData_X.LedPin_UB, HIGH);	// Turn-on by default
	//digitalWrite(GL_GlobalData_X.LedPin_UB, LOW);	// Turn-off by default
	analogWrite(GL_GlobalData_X.LedPin_UB, 128);	// 50% duty-cycle

	/* Initialize GPIO */
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize GPIOs");
	GL_GlobalData_X.pGpioInputIndex_UB[0] = PIN_GPIO_INPUT0;
	GL_GlobalData_X.pGpioInputIndex_UB[1] = PIN_GPIO_INPUT1;
	GL_GlobalData_X.pGpioInputIndex_UB[2] = PIN_GPIO_INPUT2;
	GL_GlobalData_X.pGpioInputIndex_UB[3] = PIN_GPIO_INPUT3;
	GL_GlobalData_X.pGpioOutputIndex_UB[0] = PIN_GPIO_OUTPUT0;
	GL_GlobalData_X.pGpioOutputIndex_UB[1] = PIN_GPIO_OUTPUT1;
	GL_GlobalData_X.pGpioOutputIndex_UB[2] = PIN_GPIO_OUTPUT2;
	GL_GlobalData_X.pGpioOutputIndex_UB[3] = PIN_GPIO_OUTPUT3;
	for (int i = 0; i < 4; i++) {
		pinMode(GL_GlobalData_X.pGpioInputIndex_UB[i], INPUT);
		pinMode(GL_GlobalData_X.pGpioOutputIndex_UB[i], OUTPUT);
		digitalWrite(GL_GlobalData_X.pGpioOutputIndex_UB[i], LOW);
	}

	/* Initialize TCP Server Modules */
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize TCP Server Modules");
	GL_GlobalData_X.NetworkIf_X.TcpServer_H.init(GL_GlobalData_X.NetworkIf_X.pMacAddr_UB, GL_GlobalData_X.NetworkIf_X.IpAddr_X, GL_GlobalData_X.NetworkIf_X.LocalPort_UI);
	TCPServerManager_Init(&(GL_GlobalData_X.NetworkIf_X.TcpServer_H));
	
	/* Initialiaze W-Link Command Management Modules */
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize W-Link Command Management Modules");
	//WCmdMedium_Init(WCMD_MEDIUM_SERIAL, GL_PortComMap_X[PORT_COM3]);	// Medium mapped on COM3
	WCmdMedium_Init(WCMD_MEDIUM_TCP, &(GL_GlobalData_X.NetworkIf_X.TcpServer_H));	// Medium mapped on TCP Server
	WCommandInterpreter_Init(cGL_pFctDescr_X, WCMD_FCT_DESCR_SIZE);	

	/* Initialize Indicator Modules */
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize Indicator Modules");
	GL_GlobalData_X.Indicator_H.init(GL_PortComMap_X[PORT_COM2], 2400);
	IndicatorInterface_Init();
	IndicatorManager_Init(&(GL_GlobalData_X.Indicator_H));
	//IndicatorManager_Enable();	// Normal frame by default
	GL_GlobalData_X.Indicator_H.attachEcho(GL_PortComMap_X[PORT_COM3], 9600);

	/* Initialize Badge Reader Modules */
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize Badge Reader Modules");
	GL_GlobalData_X.BadgeReader_H.init(GL_PortComMap_X[PORT_COM1], 9600);
	BadgeReaderManager_Init(&(GL_GlobalData_X.BadgeReader_H));
	BadgeReaderManager_Enable();

	/* Initialize LCD Modules */
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize LCD Modules");
	GL_GlobalData_X.Lcd_H.init(&GL_LcdObject_X, PIN_LCD_BACKLIGHT);
	GL_GlobalData_X.Lcd_H.setBacklight(255);	// Max value for Backlight by default
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, "-----  W-Link  -----");
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, "-  Pesage Warnier  -");

	/* Initialize FlatPanel Modules*/
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize Flat Panel Modules");
	GL_GlobalData_X.FlatPanel_H.init(&GL_Keypad_X);


	// TODO : Add Output Management for Bug in SPI (additional output to maintain low ?)

	

	/* Map Specific Functions on CommEvent Handler */
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Map Specific CommEvent Handler");
	GL_PortComEventMap_X[PORT_COM1].EventHandler = CommEvent_BadgeReader;

}

/* ******************************************************************************** */
/* Loop
/* ******************************************************************************** */
void loop() {

	TCPServerManager_Process();
	IndicatorManager_Process();
	BadgeReaderManager_Process();

} 

/* ******************************************************************************** */
/* Events
/* ******************************************************************************** */
void serialEvent() { GL_PortComEventMap_X[PORT_COM0].EventHandler(); }
void serial1Event() { GL_PortComEventMap_X[PORT_COM1].EventHandler(); }
void serial2Event() { GL_PortComEventMap_X[PORT_COM2].EventHandler(); }
void serial3Event() { GL_PortComEventMap_X[PORT_COM3].EventHandler(); }
