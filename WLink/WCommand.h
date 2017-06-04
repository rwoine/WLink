/* ******************************************************************************** */
/*                                                                                  */
/* WCommand.h                                                                       */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for WCommand.cpp												*/
/*		Gathers the several functions to dialog with the W-Link						*/
/*                                                                                  */
/* History :	14/05/2016	(RW)	Creation of this file                           */
/*				08/10/2016	(RW)	Update WCMD_FCT_STS enumeration					*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __WCOMMAND_H__
#define __WCOMMAND_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

#define WCMD_UNKNOWN						0x7F
#define WCMD_GET_REVISION_ID				0x00
#define WCMD_GPIO_READ						0x01
#define WCMD_GPIO_WRITE						0x02
#define WCMD_GPIO_SET_BIT					0x03
#define WCMD_GPIO_CLR_BIT					0x04
#define WCMD_INDICATOR_GET_WEIGHT			0x11
#define WCMD_INDICATOR_GET_WEIGHT_ALIBI		0x12
#define WCMD_INDICATOR_SET_ZERO				0x13
#define WCMD_INDICATOR_GET_WEIGHT_ASCII		0x14
#define WCMD_BADGE_READER_GET_ID			0x21
#define WCMD_LCD_WRITE						0x30
#define WCMD_LCD_READ						0x31
#define WCMD_LCD_CLEAR						0x32
#define WCMD_LCD_SET_BACKLIGHT				0x33
#define WCMD_LCD_ENABLE_EXT_WRITE			0x34
#define WCMD_LCD_DISABLE_EXT_WRITE			0x35
#define WCMD_LCD_GET_EXT_WRITE_STATUS		0x36
#define WCMD_LCD_GET_EXT_WRITE_DATA			0x37
#define WCMD_EEPROM_WRITE					0x40
#define WCMD_EEPROM_READ					0x41
#define WCMD_RTC_SET_DATETIME               0x48
#define WCMD_RTC_GET_DATETIME               0x49
#define WCMD_COMPORT_OPEN					0x50
#define WCMD_COMPORT_CLOSE					0x51
#define WCMD_COMPORT_WRITE					0x52
#define WCMD_COMPORT_ENABLE_TUNNEL          0x55
#define WCMD_COMPORT_DISABLE_TUNNEL         0x56
#define WCMD_TEST_CMD						0x70

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

typedef enum {
	WCMD_FCT_STS_OK = 0x00,
	WCMD_FCT_STS_ERROR = 0x01,
	WCMD_FCT_STS_BAD_DATA = 0xFC,
	WCMD_FCT_STS_BAD_PACKET = 0xFD,
	WCMD_FCT_STS_BAD_PARAM_NB = 0xFE,
	WCMD_FCT_STS_UNKNOWN = 0xFF
} WCMD_FCT_STS;

typedef struct {
	unsigned char CmdID_UB;
	WCMD_FCT_STS(*FctHandler)(const unsigned char*, unsigned long, unsigned char*, unsigned long*);
} WCMD_FCT_DESCR;


/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */

WCMD_FCT_STS WCmdProcess_GetRevisionId(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);

WCMD_FCT_STS WCmdProcess_GpioRead(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_GpioWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_GpioSetBit(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_GpioClrBit(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);

WCMD_FCT_STS WCmdProcess_IndicatorGetWeight(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_IndicatorGetWeightAlibi(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_IndicatorSetZero(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_IndicatorGetWeightAscii(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);

WCMD_FCT_STS WCmdProcess_BadgeReaderGetBadgeId(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);

WCMD_FCT_STS WCmdProcess_LcdWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_LcdRead(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_LcdClear(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_LcdSetBacklight(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_LcdEnableExternalWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_LcdDisableExternalWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_LcdGetExternalWriteStatus(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_LcdGetExternalWriteData(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);

WCMD_FCT_STS WCmdProcess_EepromWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_EepromRead(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);

WCMD_FCT_STS WCmdProcess_RtcSetDateTime(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_RtcGetDateTime(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);

WCMD_FCT_STS WCmdProcess_ComPortOpen(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_ComPortClose(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_ComPortWrite(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_ComPortEnableTunnel(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);
WCMD_FCT_STS WCmdProcess_ComPortDisableTunnel(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);

WCMD_FCT_STS WCmdProcess_TestCommand(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);


#endif // __WCOMMAND_H__