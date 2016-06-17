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

//#include "UDPServer.h"
//#include "UDPServerManager.h"

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

#include "Debug.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
	//UDPServer UdpServer_H;
	TCPServer TcpServer_H;
} NETWORK_INTERFACE_STRUCT;

typedef struct {
	unsigned char pRevisionId_UB[8];
	unsigned char LedPin_UB;
	unsigned char pGpioInputIndex_UB[4];
	unsigned char pGpioOutputIndex_UB[4];
	LcdDisplay Lcd_H;
	FlatPanel FlatPanel_H;
	NETWORK_INTERFACE_STRUCT NetworkIf_X;
	Indicator Indicator_H;
	BadgeReader BadgeReader_H;
} GLOBAL_PARAM_STRUCT;

#endif // __WLINK_H__