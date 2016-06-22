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

#define MODULE_NAME		"WCommandMedium"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "Arduino.h"
#include "WCommandMedium.h"
#include "WCommandInterpreter.h"

#include "UDPServer.h"
#include "TCPServer.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static WCMD_MEDIUM_ENUM GL_Medium_E;
static HardwareSerial * GL_pMediumSerial_H;
static UDPServer * GL_pMediumUdpServer_H;
static TCPServer * GL_pMediumTcpServer_H;

static const String GL_pMediumLut_cstr[] = {"Serial", "UDP", "TCP"};

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
			GL_pMediumUdpServer_H = (UDPServer *)pMedium_H;
			break;

		case WCMD_MEDIUM_TCP:
			GL_pMediumTcpServer_H = (TCPServer *)pMedium_H;
			break;
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "W-Command Medium Initialized -> ");
	DBG_PRINTDATA(GL_pMediumLut_cstr[WCmdMedium_E]);
	DBG_ENDSTR();
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
		RetVal_B = GL_pMediumUdpServer_H->isInitialized();
		break;

	case WCMD_MEDIUM_TCP:
		RetVal_B = GL_pMediumTcpServer_H->isInitialized();
		break;
	}

	return RetVal_B;
}

int WCmdMedium_DataAvailable(void) {
	int RetVal_SI = 0;

	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		RetVal_SI = GL_pMediumSerial_H->available();
		break;

	case WCMD_MEDIUM_UDP:
		RetVal_SI = GL_pMediumUdpServer_H->GL_UdpServerParam_X.Server_H.parsePacket();
		break;

	case WCMD_MEDIUM_TCP:
		RetVal_SI = GL_pMediumTcpServer_H->GL_TcpServerParam_X.Client_H.available();
		break;
	}

	return RetVal_SI;
}

unsigned char WCmdMedium_ReadByte(void) {
	unsigned char RetVal_UB = 0x00;

	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		RetVal_UB = GL_pMediumSerial_H->read();
		break;

	case WCMD_MEDIUM_UDP:
		RetVal_UB = GL_pMediumUdpServer_H->GL_UdpServerParam_X.Server_H.read();
		break;

	case WCMD_MEDIUM_TCP:
		RetVal_UB = GL_pMediumTcpServer_H->GL_TcpServerParam_X.Client_H.read();
		break;
	}

	return RetVal_UB;
}

void WCmdMedium_WriteByte(unsigned char Byte_UB) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		GL_pMediumSerial_H->write(Byte_UB);
		break;

	case WCMD_MEDIUM_UDP:
		GL_pMediumUdpServer_H->GL_UdpServerParam_X.Server_H.write(Byte_UB);
		break;

	case WCMD_MEDIUM_TCP:
		GL_pMediumTcpServer_H->GL_TcpServerParam_X.Client_H.write(Byte_UB);
		break;
	}
}

void WCmdMedium_Write(unsigned char * pBuffer_UB, unsigned long NbData_UL) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		GL_pMediumSerial_H->write(pBuffer_UB, NbData_UL);
		break;

	case WCMD_MEDIUM_UDP:
		GL_pMediumUdpServer_H->GL_UdpServerParam_X.Server_H.write(pBuffer_UB, NbData_UL);
		break;

	case WCMD_MEDIUM_TCP:
		GL_pMediumTcpServer_H->GL_TcpServerParam_X.Client_H.write(pBuffer_UB, NbData_UL);
		break;
	}
}

void WCmdMedium_Flush(void) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		GL_pMediumSerial_H->flush();
		break;

	case WCMD_MEDIUM_UDP:
		GL_pMediumUdpServer_H->GL_UdpServerParam_X.Server_H.flush();
		break;

	case WCMD_MEDIUM_TCP:
		GL_pMediumTcpServer_H->GL_TcpServerParam_X.Client_H.flush();
		break;
	}
}

void WCmdMedium_Stop(void) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:	// Do nothing for Serial
		break;

	case WCMD_MEDIUM_UDP:
		GL_pMediumUdpServer_H->GL_UdpServerParam_X.Server_H.stop();
		break;

	case WCMD_MEDIUM_TCP:
		GL_pMediumTcpServer_H->GL_TcpServerParam_X.Client_H.stop();
		break;
	}
}

void WCmdMedium_BeginPacket(void) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:	// Do nothing for Serial
		break;

	case WCMD_MEDIUM_UDP:
		GL_pMediumUdpServer_H->GL_UdpServerParam_X.Server_H.beginPacket(GL_pMediumUdpServer_H->GL_UdpServerParam_X.RemoteIp_X, GL_pMediumUdpServer_H->GL_UdpServerParam_X.RemotePort_UI);
			break;

	case WCMD_MEDIUM_TCP:		// Do nothing for TCP
		break;
	}
}

void WCmdMedium_EndPacket(void) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:	// Do nothing for Serial
		break;

	case WCMD_MEDIUM_UDP:
		GL_pMediumUdpServer_H->GL_UdpServerParam_X.Server_H.endPacket();
		break;

	case WCMD_MEDIUM_TCP:		// Do nothing for TCP
		break;
	}
}
