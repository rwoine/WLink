/* ******************************************************************************** */
/*                                                                                  */
/* TCPServer.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Defines the utility functions that manage the TCP Server object             */
/*                                                                                  */
/* History :  	31/05/2015  (RW)	Creation of this file                           */
/*				23/01/2017	(RW)	Add setup of subnet, gateway and DNS			*/
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"TCPServer"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "TCPServer.h"
#include "Hardware.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
TCPServer::TCPServer() {
	GL_TcpServerParam_X.IsInitialized_B = false;
	GL_TcpServerParam_X.IsConnected_B = false;
	GL_TcpServerParam_X.Status_E = TCP_SERVER_IDLE;
	pinMode(PIN_ETH_LINKED, INPUT);
}


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void TCPServer::init() {
	GL_TcpServerParam_X.pMacAddr_UB[0] = TCP_SERVER_DEFAULT_MAC_ADDR0;
	GL_TcpServerParam_X.pMacAddr_UB[1] = TCP_SERVER_DEFAULT_MAC_ADDR1;
	GL_TcpServerParam_X.pMacAddr_UB[2] = TCP_SERVER_DEFAULT_MAC_ADDR2;
	GL_TcpServerParam_X.pMacAddr_UB[3] = TCP_SERVER_DEFAULT_MAC_ADDR3;
	GL_TcpServerParam_X.pMacAddr_UB[4] = TCP_SERVER_DEFAULT_MAC_ADDR4;
	GL_TcpServerParam_X.pMacAddr_UB[5] = TCP_SERVER_DEFAULT_MAC_ADDR5;
	GL_TcpServerParam_X.LocalPort_UI = TCP_SERVER_DEFAULT_PORT;
	GL_TcpServerParam_X.AdvancedConfig_B = false;
	GL_TcpServerParam_X.IsDhcp_B = true;
	GL_TcpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server Initialized");
}

void TCPServer::init(unsigned char pMacAddr_UB[6]) {
	GL_TcpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_TcpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_TcpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_TcpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_TcpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_TcpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
	GL_TcpServerParam_X.LocalPort_UI = TCP_SERVER_DEFAULT_PORT;
	GL_TcpServerParam_X.AdvancedConfig_B = false;
	GL_TcpServerParam_X.IsDhcp_B = true;
	GL_TcpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server Initialized");
}

void TCPServer::init(unsigned char pMacAddr_UB[6], IPAddress IpAddr_X) {
	GL_TcpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_TcpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_TcpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_TcpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_TcpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_TcpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
	GL_TcpServerParam_X.IpAddr_X = IpAddr_X;
	GL_TcpServerParam_X.LocalPort_UI = TCP_SERVER_DEFAULT_PORT;
	GL_TcpServerParam_X.AdvancedConfig_B = false;
	GL_TcpServerParam_X.IsDhcp_B = false;
	GL_TcpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server Initialized");
}

void TCPServer::init(unsigned char pMacAddr_UB[6], IPAddress IpAddr_X, unsigned int LocalPort_UI) {
	GL_TcpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_TcpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_TcpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_TcpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_TcpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_TcpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
	GL_TcpServerParam_X.IpAddr_X = IpAddr_X;
	GL_TcpServerParam_X.LocalPort_UI = LocalPort_UI;
	GL_TcpServerParam_X.AdvancedConfig_B = false;
	GL_TcpServerParam_X.IsDhcp_B = false;
	GL_TcpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server Initialized");
}

void TCPServer::init(unsigned char pMacAddr_UB[6], IPAddress IpAddr_X, IPAddress SubnetMaskAddr_X, IPAddress GatewayAddr_X, IPAddress DnsIpAddr_X, unsigned int LocalPort_UI) {
	GL_TcpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_TcpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_TcpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_TcpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_TcpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_TcpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
	GL_TcpServerParam_X.IpAddr_X = IpAddr_X;
	GL_TcpServerParam_X.SubnetMaskAddr_X = SubnetMaskAddr_X;
	GL_TcpServerParam_X.GatewayAddr_X = GatewayAddr_X;
	GL_TcpServerParam_X.DnsIpAddr_X = DnsIpAddr_X;
	GL_TcpServerParam_X.LocalPort_UI = LocalPort_UI;
	GL_TcpServerParam_X.AdvancedConfig_B = true;
	GL_TcpServerParam_X.IsDhcp_B = false;
	GL_TcpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server Initialized");
}

void TCPServer::init(unsigned char pMacAddr_UB[6], unsigned int LocalPort_UI) {
	GL_TcpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_TcpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_TcpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_TcpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_TcpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_TcpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
	GL_TcpServerParam_X.LocalPort_UI = LocalPort_UI;
	GL_TcpServerParam_X.AdvancedConfig_B = false;
	GL_TcpServerParam_X.IsDhcp_B = true;
	GL_TcpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server Initialized");
}

boolean TCPServer::isInitialized() {
	return GL_TcpServerParam_X.IsInitialized_B;
}


void TCPServer::setMacAddr(unsigned char pMacAddr_UB[6]) {
	GL_TcpServerParam_X.pMacAddr_UB[0] = pMacAddr_UB[0];
	GL_TcpServerParam_X.pMacAddr_UB[1] = pMacAddr_UB[1];
	GL_TcpServerParam_X.pMacAddr_UB[2] = pMacAddr_UB[2];
	GL_TcpServerParam_X.pMacAddr_UB[3] = pMacAddr_UB[3];
	GL_TcpServerParam_X.pMacAddr_UB[4] = pMacAddr_UB[4];
	GL_TcpServerParam_X.pMacAddr_UB[5] = pMacAddr_UB[5];
}

void TCPServer::setIpAddr(IPAddress IpAddr_X) {
	GL_TcpServerParam_X.IsDhcp_B = false;
	GL_TcpServerParam_X.IpAddr_X = IpAddr_X;
}

void TCPServer::setLocalPort(unsigned int LocalPort_UI) {
	GL_TcpServerParam_X.LocalPort_UI = LocalPort_UI;
}

unsigned char * TCPServer::getMacAddr() {
	return GL_TcpServerParam_X.pMacAddr_UB;
}

IPAddress TCPServer::getIpAddr() {
	return GL_TcpServerParam_X.IpAddr_X;
}

unsigned int TCPServer::getLocalPort() {
	return GL_TcpServerParam_X.LocalPort_UI;
}


void TCPServer::begin() {
	if (GL_TcpServerParam_X.AdvancedConfig_B) {
		Ethernet.begin(GL_TcpServerParam_X.pMacAddr_UB, GL_TcpServerParam_X.IpAddr_X, GL_TcpServerParam_X.DnsIpAddr_X, GL_TcpServerParam_X.GatewayAddr_X, GL_TcpServerParam_X.SubnetMaskAddr_X);
	}
	else if (GL_TcpServerParam_X.IsDhcp_B) {
		Ethernet.begin(GL_TcpServerParam_X.pMacAddr_UB);
		GL_TcpServerParam_X.IpAddr_X = Ethernet.localIP();
	}
	else {
		Ethernet.begin(GL_TcpServerParam_X.pMacAddr_UB, GL_TcpServerParam_X.IpAddr_X);
	}

	GL_TcpServerParam_X.Server_H = EthernetServer(GL_TcpServerParam_X.LocalPort_UI);
	GL_TcpServerParam_X.Server_H.begin();
	GL_TcpServerParam_X.IsConnected_B = true;

	DBG_PRINT(DEBUG_SEVERITY_INFO, "TCP Server Started @ ");
	DBG_PRINTDATA(GL_TcpServerParam_X.IpAddr_X);
	DBG_PRINTDATA(":");
	DBG_PRINTDATA(GL_TcpServerParam_X.LocalPort_UI);
	DBG_ENDSTR();
}

void TCPServer::renew() {
	GL_TcpServerParam_X.IsConnected_B = false;
	GL_TcpServerParam_X.Client_H.flush(); 	// Flush buffer
	GL_TcpServerParam_X.Client_H.stop(); 	// Close connection if any
	GL_TcpServerParam_X.Status_E = TCP_SERVER_IDLE;
	begin();
}

boolean TCPServer::isEthernetLinked() {
	return ((digitalRead(PIN_ETH_LINKED) == LOW) ? true : false);
}
