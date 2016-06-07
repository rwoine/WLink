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
#include "Indicator.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define INDICATOR_INTERFACE_MAX_FRAME_SIZE	10

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
	INDICATOR_LD5218,
	INDICATOR_INTERFACE_DEVICES_NUM
} INDICATOR_INTERFACE_DEVICES_ENUM;

typedef enum {
	INDICATOR_INTERFACE_FRAME_ASK_WEIGHT,
	INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_ALIBI,
	INDICATOR_INTERFACE_FRAME_ASK_LAST_ALIBI,
	INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_MSA,
	INDICATOR_INTERFACE_FRAME_SET_ZERO,
	INDICATOR_INTERFACE_FRAME_NUM
} INDICATOR_INTERFACE_FRAME_ENUM;

typedef struct {
	char Size_UB;
	char pWords_UB[INDICATOR_INTERFACE_MAX_FRAME_SIZE];
} INDICATOR_INTERFACE_FRAME_STRUCT;

typedef struct {
	INDICATOR_INTERFACE_FRAME_STRUCT pFrame[INDICATOR_INTERFACE_FRAME_NUM];
	void(*FctHandler)(unsigned char *, INDICATOR_WEIGHT_STRUCT *, boolean, boolean);
} INDICATOR_INTERFACE_STRUCT;

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void IndicatorInterface_Init(void);

#endif // __INDICATOR_INTERFACE_H__