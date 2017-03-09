/* ******************************************************************************** */
/*                                                                                  */
/* UDPServer.h																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for UDPServer.cpp												*/
/*		This class creates and manages a UDPServer object to dialog with a UDP		*/
/*		Ethernet-based communication through the external SPI chip					*/
/*                                                                                  */
/* History :	21/06/2016	(RW)	Creation of this file                           */
/*              06/03/2017  (RW)    Re-mastered version with Network Adapter        */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include <Arduino.h>
#include "SPI.h"
#include "Ethernet.h"
#include "EthernetUdp.h"
#include "NetworkAdapter.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define UDP_SERVER_DEFAULT_PORT			8888


/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
	boolean IsInitialized_B;
	boolean IsConnected_B;
	unsigned int LocalPort_UI;
	EthernetUDP Server_H;
} UDP_SERVER_PARAM;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class UDPServer {
public:
	// Constructor
	UDPServer();

	// Functions
    void init(NetworkAdapter * pNetworkAdapter_H);
    void init(NetworkAdapter * pNetworkAdapter_H, unsigned int LocalPort_UI);
	boolean isInitialized();

	void setLocalPort(unsigned int LocalPort_UI);
	unsigned int getLocalPort();

	void begin();
	void renew();
    void flushServer(void);
    void stopServer(void);
    boolean isConnected(void);

    EthernetUDP * getServer(void);

	UDP_SERVER_PARAM GL_UdpServerParam_X;
};

#endif // __UDP_SERVER_H__

