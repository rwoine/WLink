/* ******************************************************************************** */
/*                                                                                  */
/* IndicatorInterface.h		                                                        */
/*                                                                                  */
/* Description :                                                                    */
/*		TODO																		*/
/*                                                                                  */
/* History :  	07/06/2015  (RW)	Creation of this file							*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __INDICATOR_INTERFACE_H__
#define __INDICATOR_INTERFACE_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <Arduino.h>

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define INDICATOR_INTERFACE_MAX_FRAME_SIZE	10
#define INDICATOR_INTERFACE_MAX_RESP_SIZE	32

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
	INDICATOR_WEIGHT_STATUS_STABLE,
	INDICATOR_WEIGHT_STATUS_UNSTABLE,
	INDICATOR_WEIGHT_STATUS_OVERRANGE,
	INDICATOR_WEIGHT_STATUS_UNDEFINED
} INDICATOR_WEIGHT_STATUS_ENUM;

typedef enum {
	INDICATOR_WEIGHT_SIGN_POS,
	INDICATOR_WEIGHT_SIGN_NEG,
	INDICATOR_WEIGHT_SIGN_UNDEFINED
} INDICATOR_WEIGHT_SIGN_ENUM;

typedef struct {
	INDICATOR_WEIGHT_STATUS_ENUM Status_E;
	INDICATOR_WEIGHT_SIGN_ENUM Sign_E;
	unsigned int Value_UI;
	unsigned int Alibi_UI;
} INDICATOR_WEIGHT_STRUCT;

typedef struct {
	boolean IsInitialized_B;
	boolean HasEcho_B;
	boolean IsAlibi_B;
	boolean IsMsa_B;
    boolean IrqReceived_B;
	INDICATOR_WEIGHT_STRUCT Weight_X;
} INDICATOR_PARAM;


typedef enum {
	INDICATOR_LD5218,
    INDICATOR_GI400,
	INDICATOR_INTERFACE_DEVICES_NUM
} INDICATOR_INTERFACE_DEVICES_ENUM;

const String pIndicatorInterfaceDeviceLut_Str[INDICATOR_INTERFACE_DEVICES_NUM] = {"LD5218", "GI400"};

typedef enum {
	INDICATOR_INTERFACE_FRAME_ASK_WEIGHT,
	INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_ALIBI,
	INDICATOR_INTERFACE_FRAME_ASK_LAST_ALIBI,
	INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_MSA,
	INDICATOR_INTERFACE_FRAME_SET_ZERO,
	INDICATOR_INTERFACE_FRAME_NUM
} INDICATOR_INTERFACE_FRAME_ENUM;

const String pIndicatorInterfaceFrameLut_Str[INDICATOR_INTERFACE_FRAME_NUM] = {"Ask Weight", "Ask Weight with Alibi", "Ask Last Alibi", "Ask Weight MS/A", "Set Weight to Zero"};


typedef struct {
	char Size_UB;
	char pWords_UB[INDICATOR_INTERFACE_MAX_FRAME_SIZE];
	char RespSize_UB;
} INDICATOR_INTERFACE_FRAME_STRUCT;

typedef struct {
	INDICATOR_INTERFACE_FRAME_STRUCT pFrame[INDICATOR_INTERFACE_FRAME_NUM];
	void(*FctHandler)(unsigned char *, INDICATOR_INTERFACE_FRAME_ENUM, INDICATOR_WEIGHT_STRUCT *);
} INDICATOR_INTERFACE_STRUCT;

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void IndicatorInterface_Init(void);

#endif // __INDICATOR_INTERFACE_H__
