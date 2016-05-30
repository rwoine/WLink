/* ******************************************************************************** */
/*                                                                                  */
/* WCommandMedium.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the functions related to the communication medium used for        */
/*		the commands channel														*/
/*                                                                                  */
/* History :  	26/05/2015  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "Arduino.h"
#include "WCommandMedium.h"
#include "WCommandInterpreter.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static WCMD_MEDIUM_ENUM GL_Medium_E;
static HardwareSerial * GL_pMediumSerial_H;

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void WCmdMedium_Init(WCMD_MEDIUM_ENUM WCmdMedium_E, void * pMedium_H) {
	GL_Medium_E = WCmdMedium_E;
	switch (GL_Medium_E) {
		case WCMD_MEDIUM_SERIAL:
			GL_pMediumSerial_H = (HardwareSerial *)pMedium_H;
			break;

		case WCMD_MEDIUM_UDP:
			break;	// TODO : UDP to be implemented

		case WCMD_MEDIUM_TCP:
			break;	// TODO : TCP to be implemented
	}
}

/* ******************************************************************************** */
/* Functions - Exported
/* ******************************************************************************** */
boolean WCmdMedium_IsConnected(void) {
	boolean RetVal_B = false;

	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		RetVal_B = true; // Serial always connected
		break;

	case WCMD_MEDIUM_UDP:
		break;	// TODO : UDP to be implemented

	case WCMD_MEDIUM_TCP:
		break;	// TODO : TCP to be implemented
	}

	return RetVal_B;
}

int WCmdMedium_DataAvailable(void) {
	int RetVal_SW = 0;

	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		RetVal_SW = GL_pMediumSerial_H->available();
		break;

	case WCMD_MEDIUM_UDP:
		break;	// TODO : UDP to be implemented

	case WCMD_MEDIUM_TCP:
		break;	// TODO : TCP to be implemented
	}

	return RetVal_SW;
}

unsigned char WCmdMedium_ReadByte(void) {
	unsigned char RetVal_UB = 0x00;

	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		RetVal_UB = GL_pMediumSerial_H->read();
		break;

	case WCMD_MEDIUM_UDP:
		break;	// TODO : UDP to be implemented

	case WCMD_MEDIUM_TCP:
		break;	// TODO : TCP to be implemented
	}

	return RetVal_UB;
}

void WCmdMedium_WriteByte(unsigned char Byte_UB) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		GL_pMediumSerial_H->write(Byte_UB);
		break;

	case WCMD_MEDIUM_UDP:
		break;	// TODO : UDP to be implemented

	case WCMD_MEDIUM_TCP:
		break;	// TODO : TCP to be implemented
	}
}

void WCmdMedium_Write(unsigned char * pBuffer_UB, unsigned long NbData_UL) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		GL_pMediumSerial_H->write(pBuffer_UB, NbData_UL);
		break;

	case WCMD_MEDIUM_UDP:
		break;	// TODO : UDP to be implemented

	case WCMD_MEDIUM_TCP:
		break;	// TODO : TCP to be implemented
	}
}

void WCmdMedium_Flush(void) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		GL_pMediumSerial_H->flush();
		break;

	case WCMD_MEDIUM_UDP:
		break;	// TODO : UDP to be implemented

	case WCMD_MEDIUM_TCP:
		break;	// TODO : TCP to be implemented
	}
}

void WCmdMedium_Stop(void) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:	// Do nothing for Serial
		break;

	case WCMD_MEDIUM_UDP:
		break;	// TODO : UDP to be implemented

	case WCMD_MEDIUM_TCP:
		break;	// TODO : TCP to be implemented
	}
}
