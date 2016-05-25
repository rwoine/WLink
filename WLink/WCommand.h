/* ******************************************************************************** */
/*                                                                                  */
/* WCommand.h                                                                       */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for WCommand.cpp												*/
/*		Gathers the several functions to dialog with the W-Link						*/
/*                                                                                  */
/* History :	14/05/2016	(RW)	Creation of this file                           */
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
#define WCMD_EEPROM_WRITE					0x40
#define WCMD_EEPROM_READ					0x41

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

typedef enum
{
	WCMD_FCT_STS_OK,
	WCMD_FCT_STS_UNKNOWN,
	WCMD_FCT_STS_BAD_PARAM_NB,
	WCMD_FCT_STS_ERROR
} WCMD_FCT_STS;


/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */

WCMD_FCT_STS WCmdProcess_GetRevisionId(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL);



/* ******************************************************************************** */
/* Functions Mapping
/* ******************************************************************************** */

typedef struct {
	unsigned char CmdID_UB;
	WCMD_FCT_STS(*FctHandler)(const unsigned char*, unsigned long, unsigned char*, unsigned long*);
} WCMD_FCT_DESCR;

const WCMD_FCT_DESCR GL_pFctDescr[] =
{
	{ WCMD_GET_REVISION_ID, WCmdProcess_GetRevisionId }
};

#define WCMD_FCT_DESCR_SIZE (sizeof(GL_pFctDescr)/sizeof(WCMD_FCT_DESCR))


#endif // __WCOMMAND_H__