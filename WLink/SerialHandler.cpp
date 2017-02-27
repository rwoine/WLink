/* ******************************************************************************** */
/*                                                                                  */
/* SerialHandler.cpp													            */
/*                                                                                  */
/* Description :                                                                    */
/*		Provides the necessary functions to manage the Serial object(s)    			*/
/*                                                                                  */
/* History :  	27/02/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"SerialHandler"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "SerialHandler.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */


/* ******************************************************************************** */
/* Variables
/* ******************************************************************************** */
HardwareSerial * GL_pPortComMap_X[] = { &Serial, &Serial1, &Serial2, &Serial3 };

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
HardwareSerial * GetSerialHandle(unsigned long ComPortIndex_UL) {
    return (GL_pPortComMap_X[ComPortIndex_UL]);
}

/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
