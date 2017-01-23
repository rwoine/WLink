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
	unsigned char pMacAddr_UB[6];
	IPAddress IpAddr_X;
	IPAddress SubnetMaskAddr_X;
	IPAddress GatewayAddr_X;
	IPAddress DnsIpAddr_X;
	unsigned int LocalPort_UI;
	UDPServer UdpServer_H;
	TCPServer TcpServer_H;
} NETWORK_INTERFACE_STRUCT;

typedef struct {
	unsigned char pRevisionId_UB[8];
	unsigned char LedPin_UB;
	unsigned char pGpioInputIndex_UB[4];
	unsigned char pGpioOutputIndex_UB[4];
	LcdDisplay Lcd_H;
	FlatPanel FlatPanel_H;
	EepromWire Eeprom_H;
	NETWORK_INTERFACE_STRUCT NetworkIf_X;
	Indicator Indicator_H;
	BadgeReader BadgeReader_H;
} GLOBAL_PARAM_STRUCT;

#endif // __WLINK_H__