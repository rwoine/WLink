/* ******************************************************************************** */
/*                                                                                  */
/* UDPServer.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Defines the utility functions that manage the UDP Server object             */
/*                                                                                  */
/* History :  	21/06/2016  (RW)	Creation of this file                           */
/*              06/03/2017  (RW)    Re-mastered version with Network Adapter        */
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
static NetworkAdapter * GL_pNetworkAdapter_H;

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
UDPServer::UDPServer() {
	GL_UdpServerParam_X.IsInitialized_B = false;
	GL_UdpServerParam_X.IsConnected_B = false;
}


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void UDPServer::init(NetworkAdapter * pNetworkAdapter_H) {
    init(pNetworkAdapter_H, UDP_SERVER_DEFAULT_PORT);
}

void UDPServer::init(NetworkAdapter * pNetworkAdapter_H, unsigned int LocalPort_UI) {
    GL_pNetworkAdapter_H = pNetworkAdapter_H;
    GL_UdpServerParam_X.LocalPort_UI = LocalPort_UI;
    GL_UdpServerParam_X.IsInitialized_B = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "UDP Server Initialized");
}

boolean UDPServer::isInitialized() {
	return GL_UdpServerParam_X.IsInitialized_B;
}


void UDPServer::setLocalPort(unsigned int LocalPort_UI) {
	GL_UdpServerParam_X.LocalPort_UI = LocalPort_UI;
}

unsigned int UDPServer::getLocalPort() {
	return GL_UdpServerParam_X.LocalPort_UI;
}


void UDPServer::begin() {
	GL_UdpServerParam_X.Server_H.begin(GL_UdpServerParam_X.LocalPort_UI);
	GL_UdpServerParam_X.IsConnected_B = true;

	DBG_PRINT(DEBUG_SEVERITY_INFO, "UDP Server Started @ ");
    DBG_PRINTDATA(GL_pNetworkAdapter_H->getIpAddr());
	DBG_PRINTDATA(":");
	DBG_PRINTDATA(GL_UdpServerParam_X.LocalPort_UI);
	DBG_ENDSTR();
}

void UDPServer::renew() {
	GL_UdpServerParam_X.IsConnected_B = false;
    flushServer();
    stopServer();
	begin();
}

void UDPServer::flushServer(void) {
    GL_UdpServerParam_X.Server_H.flush();   // Flush buffer
}

void UDPServer::stopServer(void) {
    GL_UdpServerParam_X.Server_H.stop();    // Close connection if any
}

boolean UDPServer::isConnected(void) {
    return (GL_UdpServerParam_X.IsConnected_B);
}


EthernetUDP * UDPServer::getServer(void) {
    return (&(GL_UdpServerParam_X.Server_H));
}
