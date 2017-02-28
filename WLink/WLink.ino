/* ******************************************************************************** */
/*                                                                                  */
/*                           W-LINK APPLICATIVE CODE                                */
/*                                                                                  */
/*                                                                                  */
/* Description :                                                                    */
/*                                                                                  */
/* History :	01/12/2014  (RW)	Creation of this file                           */
/*				14/05/2016	(RW)	Re-mastered version								*/
/*              27/02/2017  (RW°    Re-mastered version with WConfigManager         */
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
#include <Keypad.h>
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
const String cGL_pWLinkRevisionId_Str = "17022701";	// YYMMDDVV - Year-Month-Day-Version

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
//void ManageKeyToLcd(char Key_UB);

/* ******************************************************************************** */
/* Serial Related
/* ******************************************************************************** */
//COM_EVENT_FCT_STRUCT GL_pPortComEventMap_X[] = { NULL, NULL, NULL, NULL };

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

	{ WCMD_TEST_CMD, WCmdProcess_TestCommand }

};

#define WCMD_FCT_DESCR_SIZE (sizeof(cGL_pFctDescr_X)/sizeof(WCMD_FCT_DESCR))

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



    /* Print out Global Header */ 
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "-------------------- W-LINK --------------------");

    /* Assign Revision ID */
    DBG_PRINT(DEBUG_SEVERITY_INFO, "Revision ID = ");
    for (int i = 0; i < 8; i++)
        GL_GlobalData_X.pRevisionId_UB[i] = cGL_pWLinkRevisionId_Str.charAt(i);
    DBG_PRINTDATA(cGL_pWLinkRevisionId_Str);
    DBG_ENDSTR();


    /* W-Link Manager Initialization */
    WLinkManager_Init();
    WLinkManager_Enable();









	///* WCommand Interface Selection */
	//GL_GlobalWCmdMedium_E = WCMD_MEDIUM_TCP;

	///* Network Configuration */
	//GL_GlobalData_X.NetworkIf_X.NetworkProtocol_E = NETWORK_PROTOCOL_TCP;
	//GL_GlobalData_X.NetworkIf_X.isDhcp_B = false;								// DHCP can not be set when giving SubnetMask, Gateway and DNS
 //   GL_GlobalData_X.NetworkIf_X.isAdvancedConfig_B = true;                      // Advanced configuration when giving SubnetMask, Gateway and DNS
	//GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[0] = 0x02;
	//GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[1] = 0x00;
	//GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[2] = 0x00;
	//GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[3] = 0x01;
	//GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[4] = 0x00;
	//GL_GlobalData_X.NetworkIf_X.pMacAddr_UB[5] = 0x0C;
	//GL_GlobalData_X.NetworkIf_X.IpAddr_X = IPAddress(192, 168, 1, 16);
	//GL_GlobalData_X.NetworkIf_X.SubnetMaskAddr_X = IPAddress(255, 255, 255, 0);	// Default value = 255.255.255.0
	//GL_GlobalData_X.NetworkIf_X.GatewayAddr_X = IPAddress(192, 168, 1, 1);		// Default value = IP Address with last octet set to 1
	//GL_GlobalData_X.NetworkIf_X.DnsIpAddr_X = IPAddress(192, 168, 1, 1);		// Default value = IP Address with last octet set to 1
	//GL_GlobalData_X.NetworkIf_X.LocalPort_UI = (GL_GlobalData_X.NetworkIf_X.NetworkProtocol_E == NETWORK_PROTOCOL_TCP)?TCP_SERVER_DEFAULT_PORT:UDP_SERVER_DEFAULT_PORT;



	///* Initialize TCP Server Modules */
	//DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize TCP Server Modules");
	//if(GL_GlobalData_X.NetworkIf_X.isDhcp_B)
	//	GL_GlobalData_X.NetworkIf_X.TcpServer_H.init(GL_GlobalData_X.NetworkIf_X.pMacAddr_UB, GL_GlobalData_X.NetworkIf_X.LocalPort_UI);
	//else {
	//	//GL_GlobalData_X.NetworkIf_X.TcpServer_H.init(GL_GlobalData_X.NetworkIf_X.pMacAddr_UB, GL_GlobalData_X.NetworkIf_X.IpAddr_X, GL_GlobalData_X.NetworkIf_X.LocalPort_UI);
	//	GL_GlobalData_X.NetworkIf_X.TcpServer_H.init(GL_GlobalData_X.NetworkIf_X.pMacAddr_UB, GL_GlobalData_X.NetworkIf_X.IpAddr_X, GL_GlobalData_X.NetworkIf_X.SubnetMaskAddr_X, GL_GlobalData_X.NetworkIf_X.GatewayAddr_X, GL_GlobalData_X.NetworkIf_X.DnsIpAddr_X, GL_GlobalData_X.NetworkIf_X.LocalPort_UI);
	//}
	//TCPServerManager_Init(&(GL_GlobalData_X.NetworkIf_X.TcpServer_H));

	///* Initialize UDP Server Modules */
	//DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize UDP Server Modules");
	//if (GL_GlobalData_X.NetworkIf_X.isDhcp_B)
	//	GL_GlobalData_X.NetworkIf_X.UdpServer_H.init(GL_GlobalData_X.NetworkIf_X.pMacAddr_UB, GL_GlobalData_X.NetworkIf_X.LocalPort_UI);
	//else
	//	GL_GlobalData_X.NetworkIf_X.UdpServer_H.init(GL_GlobalData_X.NetworkIf_X.pMacAddr_UB, GL_GlobalData_X.NetworkIf_X.IpAddr_X, GL_GlobalData_X.NetworkIf_X.LocalPort_UI);
	//UDPServerManager_Init(&(GL_GlobalData_X.NetworkIf_X.UdpServer_H));

	///* Enable Proper Server */
	//if (GL_GlobalData_X.NetworkIf_X.NetworkProtocol_E == NETWORK_PROTOCOL_TCP) {
	//	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Enable TCP Server");
	//	TCPServerManager_Enable();
	//}
	//if (GL_GlobalData_X.NetworkIf_X.NetworkProtocol_E == NETWORK_PROTOCOL_UDP) {
	//	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Enable UDP Server");
	//	UDPServerManager_Enable();
	//}


    ///* Initialize Network Adapter Modules */
    //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize Network Adapter Modules");
    //if (GL_GlobalData_X.NetworkIf_X.isDhcp_B)
    //    GL_GlobalData_X.NetworkIf_X.Adapter_H.init(PIN_ETH_LINKED, GL_GlobalData_X.NetworkIf_X.pMacAddr_UB);
    //else if (!GL_GlobalData_X.NetworkIf_X.isAdvancedConfig_B)
    //    GL_GlobalData_X.NetworkIf_X.Adapter_H.init(PIN_ETH_LINKED, GL_GlobalData_X.NetworkIf_X.pMacAddr_UB, GL_GlobalData_X.NetworkIf_X.IpAddr_X);
    //else
    //    GL_GlobalData_X.NetworkIf_X.Adapter_H.init(PIN_ETH_LINKED, GL_GlobalData_X.NetworkIf_X.pMacAddr_UB, GL_GlobalData_X.NetworkIf_X.IpAddr_X, GL_GlobalData_X.NetworkIf_X.SubnetMaskAddr_X, GL_GlobalData_X.NetworkIf_X.GatewayAddr_X, GL_GlobalData_X.NetworkIf_X.DnsIpAddr_X);
    //NetworkAdapterManager_Init(&(GL_GlobalData_X.NetworkIf_X.Adapter_H));
    //NetworkAdapterManager_Enable();
	
	///* Initialiaze W-Link Command Management Modules */
	//DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize W-Link Command Management Modules");
	//switch (GL_GlobalWCmdMedium_E) {
	//	case WCMD_MEDIUM_SERIAL:	WCmdMedium_Init(WCMD_MEDIUM_SERIAL, GL_PortComMap_X[PORT_COM3]);				break;	// Medium mapped on COM3
	//	case WCMD_MEDIUM_UDP:		WCmdMedium_Init(WCMD_MEDIUM_UDP, &(GL_GlobalData_X.NetworkIf_X.UdpServer_H));	break;	// Medium mapped on UDP Server	
	//	case WCMD_MEDIUM_TCP:		WCmdMedium_Init(WCMD_MEDIUM_TCP, &(GL_GlobalData_X.NetworkIf_X.TcpServer_H));	break;	// Medium mapped on TCP Server
	//}
	//WCommandInterpreter_Init(cGL_pFctDescr_X, WCMD_FCT_DESCR_SIZE);	

	///* Initialize Indicator Modules */
	//DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize Indicator Modules");
	//GL_GlobalData_X.Indicator_H.init(GL_PortComMap_X[PORT_COM2], 2400);
	//GL_GlobalData_X.Indicator_H.setIndicatorDevice(INDICATOR_LD5218);
	//IndicatorInterface_Init();
	//IndicatorManager_Init(&(GL_GlobalData_X.Indicator_H));
	//IndicatorManager_Enable();	// Normal frame by default
	////IndicatorManager_Disable();
	////GL_GlobalData_X.Indicator_H.attachEcho(GL_PortComMap_X[PORT_COM3], 9600);

	///* Initialize Badge Reader Modules */
	//DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize Badge Reader Modules");
	//GL_GlobalData_X.BadgeReader_H.init(GL_PortComMap_X[PORT_COM1], 9600);
	//BadgeReaderManager_Init(&(GL_GlobalData_X.BadgeReader_H));
	//BadgeReaderManager_Enable();




	//GL_GlobalData_X.FlatPanel_H.attachEvent(ManageKeyToLcd);
	//FlatPanelManager_Init(&(GL_GlobalData_X.FlatPanel_H));
	//FlatPanelManager_Enable();





 //   
	///* Map Specific Functions on CommEvent Handler */
	//DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Map Specific CommEvent Handler");
	//GL_pPortComEventMap_X[PORT_COM1].EventHandler = CommEvent_BadgeReader;

	///* Add Output Management for Bug in SPI ¨*/
	//pinMode(PIN_SPI_CS, OUTPUT);


	///* End of Initialization */
	//DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of Initialization!");
	//DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Enter Process Management in Loop Code..");


	///* Display Welcome Message */
	//GL_GlobalData_X.Lcd_H.clearDisplay();
	//GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, "-----  W-Link  -----");
	//GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, "> " + cGL_pWLinkRevisionId_Str);


	///* Record Absolute Time */
	//GL_AbsoluteTime_UL = millis();

}

/* ******************************************************************************** */
/* Loop
/* ******************************************************************************** */
void loop() {

    BlinkingLedManager_Process();
    WLinkManager_Process();



	//UDPServerManager_Process();
	//TCPServerManager_Process();
	//IndicatorManager_Process();
	//BadgeReaderManager_Process();
	//FlatPanelManager_Process();

 //   NetworkAdapterManager_Process();

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
//void serialEvent() { GL_PortComEventMap_X[PORT_COM0].EventHandler(); }
//void serialEvent1() { GL_pPortComEventMap_X[PORT_COM1].EventHandler(); }
//void serialEvent2() { GL_PortComEventMap_X[PORT_COM2].EventHandler(); }
//void serialEvent3() { GL_PortComEventMap_X[PORT_COM3].EventHandler(); }

/* Keypad Event */
//void ManageKeyToLcd(char Key_UB) { GL_GlobalData_X.FlatPanel_H.manageKeytoLcd(Key_UB); }
