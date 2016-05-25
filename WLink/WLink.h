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


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

typedef struct {
	unsigned char pRevisionId_UB[8];
	const unsigned char pGpioInputIndex_UB[4] = { PIN_GPIO_INPUT0, PIN_GPIO_INPUT1, PIN_GPIO_INPUT2, PIN_GPIO_INPUT3 };
	const unsigned char pGpioOutputIndex_UB[4] = { PIN_GPIO_OUTPUT0, PIN_GPIO_OUTPUT1, PIN_GPIO_OUTPUT2, PIN_GPIO_OUTPUT3 };
} GLOBAL_PARAM_STRUCT;


#endif // __WLINK_H__