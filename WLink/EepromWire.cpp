/* ******************************************************************************** */
/*                                                                                  */
/* EepromWire.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the EEPROM utilities functions over I2C communication.			*/
/*                                                                                  */
/* History :  	17/10/2016  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"EepromWire"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "EepromWire.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
static TwoWire * GL_pEepromWire_H;
static unsigned char GL_EepromDeviceAddr_UB = 0x00;

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
EepromWire::EepromWire() {
	GL_EepromWireParam_X.IsInitialized_B = false;
}

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void EepromWire::init(TwoWire * pWire_H, unsigned char EepromAddr_UB) {
	GL_pEepromWire_H = pWire_H;
	GL_EepromDeviceAddr_UB = EepromAddr_UB;
	GL_pEepromWire_H->begin();	// Begin as a Master
	GL_EepromWireParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "EEPROM Wire Module Initialized");
}

boolean EepromWire::isInitialized(void) {
	return GL_EepromWireParam_X.IsInitialized_B;
}

void EepromWire::write(unsigned int Addr_UW, unsigned char * pData_UB, unsigned long Size_UL) {

	// 1 Page = 64 Bytes --> manage pages crossing
	// Use of 16 Bytes Write du to Small Buffer in Wire Library

	// Space Available in First Page
	unsigned long PageSpace_UL = (unsigned long)((((Addr_UW / 64) + 1) * 64) - Addr_UW);

	// First Write Size
	unsigned long FirstWriteSize_UL = 0;
	if (PageSpace_UL > 16) {
		FirstWriteSize_UL = PageSpace_UL - ((PageSpace_UL / 16) * 16);
		if (FirstWriteSize_UL == 0)
			FirstWriteSize_UL = 16;
	}
	else {
		FirstWriteSize_UL = PageSpace_UL;
	}

	// Last Write Size
	unsigned long LastWriteSize_UL = 0;
	if (Size_UL > FirstWriteSize_UL)
		LastWriteSize_UL = (Size_UL - FirstWriteSize_UL) % 16;

	// Write Number
	unsigned long WriteNbr_UL = 0;
	if (Size_UL > FirstWriteSize_UL)
		WriteNbr_UL = ((Size_UL - FirstWriteSize_UL) / 16) + 2;
	else
		WriteNbr_UL = 1;

	// Address
	unsigned int LoopAddr_UW = Addr_UW;

	unsigned long TotalSize_UL = 0;
	unsigned long WriteSize_UL = 0;

	// Loop for each transfer
	for (unsigned long WriteCnt_UL = 0; WriteCnt_UL < WriteNbr_UL; WriteCnt_UL++) {

		// Write Size for each Transfer
		if (WriteCnt_UL == 0)
			WriteSize_UL = (Size_UL < FirstWriteSize_UL) ? Size_UL : FirstWriteSize_UL;
		else if (WriteCnt_UL == (WriteNbr_UL - 1))
			WriteSize_UL = LastWriteSize_UL;
		else
			WriteSize_UL = 16;

		if (WriteSize_UL) {
			GL_pEepromWire_H->beginTransmission((int)GL_EepromDeviceAddr_UB);
			GL_pEepromWire_H->write((int)(LoopAddr_UW >> 8));   // Address MSB
			GL_pEepromWire_H->write((int)(LoopAddr_UW & 0xFF)); // Address LSB

			for (int i = 0; i < WriteSize_UL; i++)
				GL_pEepromWire_H->write(pData_UB[TotalSize_UL++]);
			GL_pEepromWire_H->endTransmission();

			LoopAddr_UW += WriteSize_UL;

			delay(5); // 5[ms] for Page Write   
		}

	}
}

unsigned long EepromWire::read(unsigned int Addr_UW, unsigned char * pData_UB, unsigned long Size_UL) {

	unsigned long TotalSize_UL = 0;
	unsigned long LoopSize_UL = 0;
	unsigned int LoopAddr_UW = 0;

	unsigned long LoopNbr_UL = (Size_UL - Size_UL % 16) / 16; // Use of small buffer in Wire Library.
	if ((Size_UL >= 16) && ((Size_UL % 16) == 0))
		LoopNbr_UL--; // Manage boundaries
	LoopNbr_UL++; // For the loop

	for (unsigned long LoopCnt_UL = 0; LoopCnt_UL < LoopNbr_UL; LoopCnt_UL++) {

		LoopAddr_UW = Addr_UW + 16 * LoopCnt_UL;

		GL_pEepromWire_H->beginTransmission((int)GL_EepromDeviceAddr_UB);
		GL_pEepromWire_H->write((int)(LoopAddr_UW >> 8));   // Address MSB
		GL_pEepromWire_H->write((int)(LoopAddr_UW & 0xFF)); // Address LSB
		GL_pEepromWire_H->endTransmission();

		if (LoopCnt_UL == 0)
			LoopSize_UL = (Size_UL <= 16) ? Size_UL : 16; // first request
		else if (LoopCnt_UL == LoopNbr_UL - 1)
			LoopSize_UL = Size_UL - 16 * LoopCnt_UL;  // last request
		else
			LoopSize_UL = 16;                       // requests of 16

		GL_pEepromWire_H->requestFrom(GL_EepromDeviceAddr_UB, LoopSize_UL);

		int i = 0;
		while (GL_pEepromWire_H->available() || i < LoopSize_UL) {
			pData_UB[TotalSize_UL++] = GL_pEepromWire_H->read();
			i++;
		}

	}

	return ((unsigned long)TotalSize_UL);
}

