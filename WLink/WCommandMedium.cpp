/* ******************************************************************************** */
/*                                                                                  */
/* WCommandMedium.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the functions related to the communication medium used for        */
/*		the commands channel														*/
/*                                                                                  */
/* History :  	26/05/2015  (RW)	Creation of this file                           */
/*              01/03/2017  (RW)    Add GSM as possible medium                      */
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
#include "UDPServerManager.h"
#include "TCPServer.h"
#include "TCPServerManager.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static WCMD_MEDIUM_ENUM GL_Medium_E;
static HardwareSerial * GL_pMediumSerial_H;
static UDPServer * GL_pMediumUdpServer_H;
static TCPServer * GL_pMediumTcpServer_H;

static const String GL_pMediumLut_cstr[] = {"Serial", "UDP", "TCP", "GSM"};

static boolean GL_IsMonoClient_B = false;

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void WCmdMedium_Init(WCMD_MEDIUM_ENUM WCmdMedium_E, void * pMedium_H, boolean isMonoClient_B) {
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

        case WCMD_MEDIUM_GSM:
            // TODO : not yet implemented
            break;
	}

    GL_IsMonoClient_B = isMonoClient_B;

	DBG_PRINT(DEBUG_SEVERITY_INFO, "W-Command Medium Assigned -> ");
	DBG_PRINTDATA(GL_pMediumLut_cstr[WCmdMedium_E]);
	DBG_ENDSTR();
}

/* ******************************************************************************** */
/* Functions - Exported
/* ******************************************************************************** */
boolean WCmdMedium_IsMonoClient(void) {
    return GL_IsMonoClient_B;
}

boolean WCmdMedium_IsRunning(void) {
	boolean RetVal_B = false;

	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		RetVal_B = true; // Serial always running
		break;

	case WCMD_MEDIUM_UDP:
		RetVal_B = UDPServerManager_IsRunning();
		break;

	case WCMD_MEDIUM_TCP:
		RetVal_B = TCPServerManager_IsRunning();
		break;

    case WCMD_MEDIUM_GSM:
        // TODO : not yet implemented
        break;
	}

	return RetVal_B;
}

boolean WCmdMedium_IsPacketReceived(void) {
    boolean RetVal_B = false;

    switch (GL_Medium_E) {
    case WCMD_MEDIUM_SERIAL:
        if (GL_pMediumSerial_H->available())
            RetVal_B = true;
        break;

    case WCMD_MEDIUM_UDP:
        if (GL_pMediumUdpServer_H->getServer()->parsePacket())
            RetVal_B = true;
        break;

    case WCMD_MEDIUM_TCP:
        if (GL_pMediumTcpServer_H->getClient()->available())
            RetVal_B = true;
        break;

    case WCMD_MEDIUM_GSM:
        // TODO : not yet implemented
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
        RetVal_SI = GL_pMediumUdpServer_H->getServer()->available();
		break;

	case WCMD_MEDIUM_TCP:
        RetVal_SI = GL_pMediumTcpServer_H->getClient()->available();
		break;

    case WCMD_MEDIUM_GSM:
        // TODO : not yet implemented
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
		RetVal_UB = GL_pMediumUdpServer_H->getServer()->read();
		break;

	case WCMD_MEDIUM_TCP:
        RetVal_UB = GL_pMediumTcpServer_H->getClient()->read();
		break;

    case WCMD_MEDIUM_GSM:
        // TODO : not yet implemented
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
		GL_pMediumUdpServer_H->getServer()->write(Byte_UB);
		break;

	case WCMD_MEDIUM_TCP:
        GL_pMediumTcpServer_H->getClient()->write(Byte_UB);
		break;

    case WCMD_MEDIUM_GSM:
        // TODO : not yet implemented
        break;
	}
}

void WCmdMedium_Write(unsigned char * pBuffer_UB, unsigned long NbData_UL) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		GL_pMediumSerial_H->write(pBuffer_UB, NbData_UL);
		break;

	case WCMD_MEDIUM_UDP:
		GL_pMediumUdpServer_H->getServer()->write(pBuffer_UB, NbData_UL);
		break;

	case WCMD_MEDIUM_TCP:
        GL_pMediumTcpServer_H->getClient()->write(pBuffer_UB, NbData_UL);
		break;

    case WCMD_MEDIUM_GSM:
        // TODO : not yet implemented
        break;
	}
}

void WCmdMedium_Flush(void) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:
		GL_pMediumSerial_H->flush();
		break;

	case WCMD_MEDIUM_UDP:
        GL_pMediumUdpServer_H->flushServer();
		break;

	case WCMD_MEDIUM_TCP:
        GL_pMediumTcpServer_H->flushClient();
		break;

    case WCMD_MEDIUM_GSM:
        // TODO : not yet implemented
        break;
	}
}

void WCmdMedium_Stop(void) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:	// Do nothing for Serial
		break;

	case WCMD_MEDIUM_UDP:
        GL_pMediumUdpServer_H->stopServer();
		break;

	case WCMD_MEDIUM_TCP:
        GL_pMediumTcpServer_H->stopClient();
		break;

    case WCMD_MEDIUM_GSM:
        // TODO : not yet implemented
        break;
	}
}

void WCmdMedium_BeginPacket(void) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:	// Do nothing for Serial
		break;

	case WCMD_MEDIUM_UDP:
		GL_pMediumUdpServer_H->getServer()->beginPacket(GL_pMediumUdpServer_H->getServer()->remoteIP(), GL_pMediumUdpServer_H->getServer()->remotePort());
		break;

	case WCMD_MEDIUM_TCP:		// Do nothing for TCP
		break;

    case WCMD_MEDIUM_GSM:
        // TODO : not yet implemented
        break;
	}
}

void WCmdMedium_EndPacket(void) {
	switch (GL_Medium_E) {
	case WCMD_MEDIUM_SERIAL:	// Do nothing for Serial
		break;

	case WCMD_MEDIUM_UDP:
		GL_pMediumUdpServer_H->getServer()->endPacket();
		break;

	case WCMD_MEDIUM_TCP:		// Do nothing for TCP
		break;

    case WCMD_MEDIUM_GSM:
        // TODO : not yet implemented
        break;
	}
}
