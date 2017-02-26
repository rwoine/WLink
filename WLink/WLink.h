/* ******************************************************************************** */
/*                                                                                  */
/* WLink.h																			*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for W-Link project												*/
/*		Gathers global variables and definitions for the application				*/
/*                                                                                  */
/* History :	14/05/2016	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __WLINK_H__
#define __WLINK_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include <Arduino.h>

#include "Hardware.h"

#include "NetworkAdapter.h"
#include "NetworkAdapterManager.h"

#include "TCPServer.h"
#include "TCPServerManager.h"

#include "UDPServer.h"
#include "UDPServerManager.h"

#include "WCommand.h"
#include "WCommandMedium.h"
#include "WCommandInterpreter.h"

#include "Indicator.h"
#include "IndicatorInterface.h"
#include "IndicatorManager.h"

#include "BadgeReader.h"
#include "BadgeReaderManager.h"

#include "LcdDisplay.h"

#include "FlatPanel.h"
#include "FlatPanelManager.h"

#include "EepromWire.h"

#include "RealTimeClock.h"

#include "MemoryCard.h"


#include "WLinkManager.h"


#include "Debug.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
	NETWORK_PROTOCOL_UDP,
	NETWORK_PROTOCOL_TCP
} NETWORK_PROTOCOL_ENUM;

typedef struct {
	NETWORK_PROTOCOL_ENUM NetworkProtocol_E;
	boolean isDhcp_B;
    boolean isAdvancedConfig_B;
	unsigned char pMacAddr_UB[6];
	IPAddress IpAddr_X;
	IPAddress SubnetMaskAddr_X;
	IPAddress GatewayAddr_X;
	IPAddress DnsIpAddr_X;
	unsigned int LocalPort_UI;
    NetworkAdapter Adapter_H;
	//UDPServer UdpServer_H;
	//TCPServer TcpServer_H;
} NETWORK_INTERFACE_STRUCT;

typedef struct {
	unsigned char pRevisionId_UB[8];
	unsigned char LedPin_UB;
	unsigned char pGpioInputIndex_UB[4];
	unsigned char pGpioOutputIndex_UB[4];
	LcdDisplay Lcd_H;
	FlatPanel FlatPanel_H;
	EepromWire Eeprom_H;
    RealTimeClock Rtc_H;
    MemoryCard MemCard_H;
	Indicator Indicator_H;
	BadgeReader BadgeReader_H;
    NETWORK_INTERFACE_STRUCT NetworkIf_X;
} GLOBAL_PARAM_STRUCT;


typedef struct {
    unsigned long Index_UL;
    boolean isEnabled_B;
    unsigned char Config_UB;
    unsigned long Baudrate_UL;
    boolean isDebug_B;
} COM_PORT_CONFIG_STRUCT;


typedef struct {
    unsigned char MajorRev_UB;
    unsigned char MinorRev_UB;
    COM_PORT_CONFIG_STRUCT ComPort_X[4];
} GLOBAL_CONFIG_STRUCT;

#endif // __WLINK_H__