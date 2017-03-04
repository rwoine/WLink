/* ******************************************************************************** */
/*                                                                                  */
/* TCPServer.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Defines the utility functions that manage the TCP Server object             */
/*                                                                                  */
/* History :  	31/05/2015  (RW)	Creation of this file                           */
/*				23/01/2017	(RW)	Add setup of subnet, gateway and DNS			*/
/*              04/03/2017  (RW)    Re-mastered version with Network Adapter        */
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
static NetworkAdapter * GL_pNetworkAdapter_H;


/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
TCPServer::TCPServer() {
	GL_TcpServerParam_X.IsInitialized_B = false;
	GL_TcpServerParam_X.IsConnected_B = false;
}


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void TCPServer::init(NetworkAdapter * pNetworkAdapter_H) {
    GL_pNetworkAdapter_H = pNetworkAdapter_H;
    GL_TcpServerParam_X.LocalPort_UI = TCP_SERVER_DEFAULT_PORT;
	GL_TcpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server Initialized");
}

void TCPServer::init(NetworkAdapter * pNetworkAdapter_H, unsigned int LocalPort_UI) {
    GL_pNetworkAdapter_H = pNetworkAdapter_H;    
	GL_TcpServerParam_X.LocalPort_UI = LocalPort_UI;
	GL_TcpServerParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "TCP Server Initialized");
}


boolean TCPServer::isInitialized() {
	return GL_TcpServerParam_X.IsInitialized_B;
}


void TCPServer::setLocalPort(unsigned int LocalPort_UI) {
	GL_TcpServerParam_X.LocalPort_UI = LocalPort_UI;
}

unsigned int TCPServer::getLocalPort() {
	return GL_TcpServerParam_X.LocalPort_UI;
}


void TCPServer::begin() {
	GL_TcpServerParam_X.Server_H = EthernetServer(GL_TcpServerParam_X.LocalPort_UI);
	GL_TcpServerParam_X.Server_H.begin();
	GL_TcpServerParam_X.IsConnected_B = true;

	DBG_PRINT(DEBUG_SEVERITY_INFO, "TCP Server Started @ ");
	DBG_PRINTDATA(GL_pNetworkAdapter_H->getIpAddr());
	DBG_PRINTDATA(":");
	DBG_PRINTDATA(GL_TcpServerParam_X.LocalPort_UI);
	DBG_ENDSTR();
}

void TCPServer::renew() {
	GL_TcpServerParam_X.IsConnected_B = false;
	GL_TcpServerParam_X.Client_H.flush(); 	// Flush buffer
	GL_TcpServerParam_X.Client_H.stop(); 	// Close connection if any
	begin();
}

boolean TCPServer::isConnected(void) {
    return (GL_TcpServerParam_X.IsConnected_B);
}

