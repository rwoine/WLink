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
#include "Utilz.h"

#include "SerialHandler.h"
#include "EepromWire.h"
#include "RealTimeClock.h"
#include "LcdDisplay.h"
#include "FlatPanel.h"
#include "MemoryCard.h"

#include "Indicator.h"
#include "IndicatorManager.h"
#include "BadgeReader.h"
#include "BadgeReaderManager.h"

#include "WCommand.h"

#include "NetworkAdapter.h"
#include "NetworkAdapterManager.h"

#include "WLinkManager.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

#define WLINK_DEBUG_DEFAULT_COM_PORT        PORT_COM0
#define WLINK_DEBUG_DEFAULT_SPEED           115200


/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

// Language Enumeration
typedef enum {
    WLINK_LANGUAGE_EN = 0,
    WLINK_LANGUAGE_FR = 1,
    WLINK_LANGUAGE_NL = 2
} WLINK_LANGUAGE_ENUM;

// Global Parameters Strucure
typedef struct {
	unsigned char pRevisionId_UB[8];
	unsigned char LedPin_UB;
	unsigned char pGpioInputIndex_UB[4];
	unsigned char pGpioOutputIndex_UB[4];
    EepromWire Eeprom_H;
    RealTimeClock Rtc_H;
    LcdDisplay Lcd_H;
    FlatPanel FlatPanel_H;
    MemoryCard MemCard_H;
    NetworkAdapter Network_H;
    Indicator Indicator_H;          // Not yet managed
    BadgeReader BadgeReader_H;      // Not yet managed
} GLOBAL_PARAM_STRUCT;

// > Dedicated Strucure for COM Port Configuration
typedef struct {
    unsigned long Index_UL;
    boolean isEnabled_B;
    boolean isDebug_B;
    unsigned char Config_UB;
    unsigned long Baudrate_UL;
} COM_PORT_CONFIG_STRUCT;

// > Dedicated Strucure for Ethernet Configuration
typedef struct {
    boolean isEnabled_B;
    boolean isDhcp_B;
    boolean isAdvancedConfig_B;
    unsigned char pMacAddr_UB[6];
    IPAddress IpAddr_X;
    IPAddress SubnetMaskAddr_X;
    IPAddress GatewayAddr_X;
    IPAddress DnsIpAddr_X;
} ETHERNET_CONFIG_STRUCT;

// Global Configuration Structure
typedef struct {
    unsigned char MajorRev_UB;
    unsigned char MinorRev_UB;
    WLINK_LANGUAGE_ENUM Language_E;
    boolean HasLcd_B;
    boolean HasFlatPanel_B;
    boolean HasMemoryCard_B;
    COM_PORT_CONFIG_STRUCT pComPortConfig_X[4];
    ETHERNET_CONFIG_STRUCT EthConfig_X;
} GLOBAL_CONFIG_STRUCT;

#endif // __WLINK_H__
