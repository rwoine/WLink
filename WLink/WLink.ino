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

#define APP_USE_DEBUG	// Comment line to disbale Debug functionalitiy

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <SPI.h>
#include <Ethernet.h>

#include "WLink.h"
#include "Hardware.h"
#include "CommEvent.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Constant
/* ******************************************************************************** */
const unsigned char cGL_pWLinkRevisionId_UB[] = "16061301";	// YYMMDDVV - Year-Month-Day-Version

/* ******************************************************************************** */
/* Global
/* ******************************************************************************** */
GLOBAL_PARAM_STRUCT GL_GlobalData_X;

HardwareSerial * GL_PortComMap_X[] = { &Serial, &Serial1, &Serial2, &Serial3 };
COM_EVENT_FCT_STRUCT GL_PortComEventMap_X[] = { NULL, NULL, NULL, NULL };

/* ******************************************************************************** */
/* Functions Mapping
/* ******************************************************************************** */
const WCMD_FCT_DESCR cGL_pFctDescr_X[] =
{
	{ WCMD_GET_REVISION_ID, WCmdProcess_GetRevisionId },
	{ WCMD_GPIO_READ, WCmdProcess_GpioRead },
	{ WCMD_GPIO_WRITE, WCmdProcess_GpioWrite },
	{ WCMD_GPIO_SET_BIT, WCmdProcess_GpioSetBit },
	{ WCMD_GPIO_CLR_BIT, WCmdProcess_GpioClrBit }
};

#define WCMD_FCT_DESCR_SIZE (sizeof(cGL_pFctDescr_X)/sizeof(WCMD_FCT_DESCR))


/* ******************************************************************************** */
/* Setup
/* ******************************************************************************** */
void setup() {

	/* Enable All UARTs by Default */
	pinMode(PIN_EN_SERIAL01, OUTPUT);
	digitalWrite(PIN_EN_SERIAL01, LOW);
	pinMode(PIN_EN_SERIAL23, OUTPUT);
	digitalWrite(PIN_EN_SERIAL23, LOW);

	/* Initialize Debug Module */
	Debug_Init(GL_PortComMap_X[PORT_COM0]);

	/* Assign Revision ID */
	for (int i = 0; i < sizeof(cGL_pWLinkRevisionId_UB); i++)
		GL_GlobalData_X.pRevisionId_UB[i] = cGL_pWLinkRevisionId_UB[i];

	/* Initialize LED */
	pinMode(GL_GlobalData_X.LedPin_UB, OUTPUT);
	//digitalWrite(GL_GlobalData_X.LedPin_UB, LOW);	// Turn-off by default
	analogWrite(GL_GlobalData_X.LedPin_UB, 128);	// 50% duty-cycle

	/* Initialize GPIO */
	for (int i = 0; i < 4; i++) {
		pinMode(GL_GlobalData_X.pGpioInputIndex_UB[i], INPUT);
		pinMode(GL_GlobalData_X.pGpioOutputIndex_UB[i], OUTPUT);
		digitalWrite(GL_GlobalData_X.pGpioOutputIndex_UB[i], LOW);
	}

	/* Initialize TCP Server Modules */
	GL_GlobalData_X.NetworkIf_X.TcpServer_H.init();
	TCPServerManager_Init(&(GL_GlobalData_X.NetworkIf_X.TcpServer_H));

	/* Initialiaze W-Link Command Management Modules */
	//WCmdMedium_Init(WCMD_MEDIUM_SERIAL, GL_PortComMap_X[PORT_COM3]);	// Medium mapped on COM3
	WCmdMedium_Init(WCMD_MEDIUM_TCP, &(GL_GlobalData_X.NetworkIf_X.TcpServer_H));	// Medium mapped on TCP Server
	WCommandInterpreter_Init(cGL_pFctDescr_X, WCMD_FCT_DESCR_SIZE);	

	/* Initialize Indicator Modules */
	GL_GlobalData_X.Indicator_H.init(GL_PortComMap_X[PORT_COM2], 2400);
	IndicatorInterface_Init();
	IndicatorManager_Init(&(GL_GlobalData_X.Indicator_H));
	IndicatorManager_Enable();	// Normal frame by default
	GL_GlobalData_X.Indicator_H.attachEcho(GL_PortComMap_X[PORT_COM3], 9600);

	/* Initialize Badge Reader Modules */
	GL_GlobalData_X.BadgeReader_H.init(GL_PortComMap_X[PORT_COM1], 9600);
	BadgeReaderManager_Init(&(GL_GlobalData_X.BadgeReader_H));
	BadgeReaderManager_Enable();


	// TODO : Add Output Management for Bug in SPI (additional output to maintain low ?)




	/* Map Specific Functions on CommEvent Handler */
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
