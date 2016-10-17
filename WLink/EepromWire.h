/* ******************************************************************************** */
/*                                                                                  */
/* EepromWire.h			                                                            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for Eeprom.cpp													*/
/*                                                                                  */
/* History :  	17/10/2016  (RW)	Creation of this file							*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __EEPROM_WIRE_H__
#define __EEPROM_WIRE_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <Arduino.h>
#include <Wire.h>

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
	boolean IsInitialized_B;
} EEPROM_WIRE_PARAM;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class EepromWire {
public:
	// Constructor
	EepromWire();

	// Functions
	void init(TwoWire* pWire_X, unsigned char EepromAddr_UB);
	boolean isInitialized(void);

	void write(unsigned int Addr_UW, unsigned char * pData_UB, unsigned long Size_UL);
	unsigned long read(unsigned int Addr_UW, unsigned char * pData_UB, unsigned long Size_UL);

	EEPROM_WIRE_PARAM GL_EepromWireParam_X;
};

#endif // __EEPROM_WIRE_H__
