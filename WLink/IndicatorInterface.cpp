/* ******************************************************************************** */
/*                                                                                  */
/* IndicatorInterface.cpp															*/
/*                                                                                  */
/* Description :                                                                    */
/*		Manages the interface through the several indicator			                */
/*                                                                                  */
/* History :  	07/06/2016  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"IndicatorInterface"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "IndicatorInterface.h"

#include "LD5218.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
INDICATOR_INTERFACE_STRUCT GL_pIndicatorInterface_X[INDICATOR_INTERFACE_DEVICES_NUM];

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void IndicatorInterface_Init(void) {

	// Fill-Up LD5218 Parameters and Function
	GL_pIndicatorInterface_X[INDICATOR_LD5218].FctHandler = LD5218_ProcessWeight;
	for (int i = 0; i < INDICATOR_INTERFACE_FRAME_NUM; i++) {
		GL_pIndicatorInterface_X[INDICATOR_LD5218].pFrame[i].Size_UB = GL_pLD5218Frames_X[i].Size_UB;
		GL_pIndicatorInterface_X[INDICATOR_LD5218].pFrame[i].RespSize_UB = GL_pLD5218Frames_X[i].RespSize_UB;
		for (int j = 0; j < GL_pIndicatorInterface_X[INDICATOR_LD5218].pFrame[i].Size_UB; j++)
			GL_pIndicatorInterface_X[INDICATOR_LD5218].pFrame[i].pWords_UB[j] = GL_pLD5218Frames_X[i].pWords_UB[j];
	}

	// Do the same for other indicators..

}
