/* ******************************************************************************** */
/*                                                                                  */
/* TCPServer.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Defines the utility functions that manage the TCP Server object             */
/*                                                                                  */
/* History :  	31/05/2015  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"TCPServer"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "TCPServer.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
TCPServer::TCPServer() {

}


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void TCPServer::init() {

}

void TCPServer::init(unsigned char pMacAddr_UB[6]) {

}

void TCPServer::init(unsigned char pMacAddr_UB[6], IPAddress IpAddr_X) {

}

void TCPServer::init(unsigned char pMacAddr_UB[6], IPAddress IpAddr_X, unsigned int LocalPort_UI) {

}

void TCPServer::init(unsigned char pMacAddr_UB[6], unsigned int LocalPort_UI) {

}

boolean TCPServer::isInitialized() {

}


void TCPServer::setMacAddr(unsigned char pMacAddr_UB[6]) {

}

void TCPServer::setIpAddr(IPAddress IpAddr_X) {

}

void TCPServer::setLocalPort(unsigned int LocalPort_UI) {

}

unsigned char * TCPServer::getMacAddr() {

}

IPAddress TCPServer::getIpAddr() {

}

unsigned int TCPServer::getLocalPort() {

}


void TCPServer::begin() {

}

void TCPServer::renew() {

}

boolean TCPServer::isEthernetLinked() {

}
