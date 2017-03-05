/* ******************************************************************************** */
/*                                                                                  */
/* TCPServer.h																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for TCPServer.cpp												*/
/*		This class creates and manages a TCPServer object to dialog with a TCP		*/
/*		Ethernet-based communication through the external SPI chip					*/
/*                                                                                  */
/* History :	31/05/2016	(RW)	Creation of this file                           */
/*              04/03/2017  (RW)    Re-mastered version with Network Adapter        */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include <Arduino.h>
#include "SPI.h"
#include "Ethernet.h"
#include "NetworkAdapter.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define TCP_SERVER_DEFAULT_PORT			23


/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
    boolean IsInitialized_B;
    boolean IsConnected_B;
	unsigned int LocalPort_UI;
	EthernetServer Server_H = EthernetServer(TCP_SERVER_DEFAULT_PORT);
	EthernetClient Client_H;
} TCP_SERVER_PARAM;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class TCPServer {
public:
	// Constructor
	TCPServer();

	// Functions
	void init(NetworkAdapter * pNetworkAdapter_H);
	void init(NetworkAdapter * pNetworkAdapter_H, unsigned int LocalPort_UI);
	boolean isInitialized();

	void setLocalPort(unsigned int LocalPort_UI);
	unsigned int getLocalPort();

	void begin();
	void renew();
    void flushClient(void);
    void stopClient(void);
    boolean isConnected(void);
    boolean isClientAvailable(void);
    boolean isClientConnected(void);

    EthernetServer * getServer(void);
    EthernetClient * getClient(void);

	TCP_SERVER_PARAM GL_TcpServerParam_X;
};

#endif // __TCP_SERVER_H__

