/* ******************************************************************************** */
/*                                                                                  */
/* UDPServer.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Defines the utility functions that manage the UDP Server object             */
/*                                                                                  */
/* History :  	21/06/2016  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"UDPServer"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "UDPServer.h"
#include "Hardware.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
UDPServer::UDPServer() {
	GL_UdpServerParam_X.IsInitialized_B = false;
	GL_UdpServerParam_X.IsConnected_B = false;
	GL_UdpServerParam_X.Status_E = UDP_SERVER_IDLE;
	pinMode(PIN_ETH_LINKED, INPUT);
}


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void UDPServer::init() {
	GL_UdpServerParam_X.pMacAddr_UB[0] = UDP_SERVER_DEFAULT_MAC_ADDR0;
	GL_UdpServerParam_X.pMacAddr_UB[1] = UDP_SERVER_DEFAULT_MAC_ADDR1;
	GL_UdpServerParam_X.pMacAddr_UB[2] = UDP_SERVER_DEFAULT_MAC_ADDR2;
	GL_UdpServerParam_X.pMacAddr_UB[3] = UDP_SERVER_DEFAULT_MAC_ADDR3;
	GL_UdpServerParam_X.pMacAddr_UB[4] = UDP_SERVER_DEFAULT_MAC_ADDR4;
	GL_UdpServerParam_X.pMacAddr_UB[5] = UDP_SERVER_DEFAULT_MAC_ADDR5;
	GL_UdpServerParam_X.LocalPort_UI = UDP_SERVER_DEFAULT_PORT;
	GL_UdpServerParam_X.IsDhcp_B = true;
	GL_UdpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "UDP Server Initialized");
}

void UDPServer::init(unsigned char pMacAddr_UB[6]) {
	GL_UdpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_UdpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_UdpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_UdpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_UdpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_UdpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
	GL_UdpServerParam_X.LocalPort_UI = UDP_SERVER_DEFAULT_PORT;
	GL_UdpServerParam_X.IsDhcp_B = true;
	GL_UdpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "UDP Server Initialized");
}

void UDPServer::init(unsigned char pMacAddr_UB[6], IPAddress IpAddr_X) {
	GL_UdpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_UdpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_UdpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_UdpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_UdpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_UdpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
	GL_UdpServerParam_X.IpAddr_X = IpAddr_X;
	GL_UdpServerParam_X.LocalPort_UI = UDP_SERVER_DEFAULT_PORT;
	GL_UdpServerParam_X.IsDhcp_B = false;
	GL_UdpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "UDP Server Initialized");
}

void UDPServer::init(unsigned char pMacAddr_UB[6], IPAddress IpAddr_X, unsigned int LocalPort_UI) {
	GL_UdpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_UdpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_UdpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_UdpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_UdpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_UdpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
	GL_UdpServerParam_X.IpAddr_X = IpAddr_X;
	GL_UdpServerParam_X.LocalPort_UI = LocalPort_UI;
	GL_UdpServerParam_X.IsDhcp_B = false;
	GL_UdpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "UDP Server Initialized");
}

void UDPServer::init(unsigned char pMacAddr_UB[6], unsigned int LocalPort_UI) {
	GL_UdpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_UdpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_UdpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_UdpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_UdpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_UdpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
	GL_UdpServerParam_X.LocalPort_UI = LocalPort_UI;
	GL_UdpServerParam_X.IsDhcp_B = true;
	GL_UdpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "UDP Server Initialized");
}

boolean UDPServer::isInitialized() {
	return GL_UdpServerParam_X.IsInitialized_B;
}


void UDPServer::setMacAddr(unsigned char pMacAddr_UB[6]) {
	GL_UdpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_UdpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_UdpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_UdpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_UdpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_UdpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
}

void UDPServer::setIpAddr(IPAddress IpAddr_X) {
	GL_UdpServerParam_X.IsDhcp_B = false;
	GL_UdpServerParam_X.IpAddr_X = IpAddr_X;
}

void UDPServer::setLocalPort(unsigned int LocalPort_UI) {
	GL_UdpServerParam_X.LocalPort_UI = LocalPort_UI;
}

unsigned char * UDPServer::getMacAddr() {
	return GL_UdpServerParam_X.pMacAddr_UB;
}

IPAddress UDPServer::getIpAddr() {
	return GL_UdpServerParam_X.IpAddr_X;
}

unsigned int UDPServer::getLocalPort() {
	return GL_UdpServerParam_X.LocalPort_UI;
}


void UDPServer::begin() {
	if (GL_UdpServerParam_X.IsDhcp_B) {
		Ethernet.begin(GL_UdpServerParam_X.pMacAddr_UB);
		GL_UdpServerParam_X.IpAddr_X = Ethernet.localIP();
	}
	else {
		Ethernet.begin(GL_UdpServerParam_X.pMacAddr_UB, GL_UdpServerParam_X.IpAddr_X);
	}

	GL_UdpServerParam_X.Server_H.begin(GL_UdpServerParam_X.LocalPort_UI);
	GL_UdpServerParam_X.IsConnected_B = true;

	DBG_PRINT(DEBUG_SEVERITY_INFO, "UDP Server Started @ ");
	DBG_PRINTDATA(GL_UdpServerParam_X.IpAddr_X);
	DBG_PRINTDATA(":");
	DBG_PRINTDATA(GL_UdpServerParam_X.LocalPort_UI);
	DBG_ENDSTR();
}

void UDPServer::renew() {
	GL_UdpServerParam_X.IsConnected_B = false;
	GL_UdpServerParam_X.Server_H.flush();	// Flush buffer
	GL_UdpServerParam_X.Server_H.stop(); 	// Close connection if any
	begin();
}

boolean UDPServer::isEthernetLinked() {
	return ((digitalRead(PIN_ETH_LINKED) == LOW) ? true : false);
}
