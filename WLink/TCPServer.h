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

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define TCP_SERVER_DEFAULT_PORT			23

#define TCP_SERVER_DEFAULT_MAC_ADDR0	0x02  // Unicast - Locally Administered
#define TCP_SERVER_DEFAULT_MAC_ADDR1	0x00  // 0x00
#define TCP_SERVER_DEFAULT_MAC_ADDR2	0x00  // 0x00
#define TCP_SERVER_DEFAULT_MAC_ADDR3	0x00  //  
#define TCP_SERVER_DEFAULT_MAC_ADDR4	0x00  // 0x000001 - For Default MAC Address
#define TCP_SERVER_DEFAULT_MAC_ADDR5	0x01  // 

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
	TCP_SERVER_IDLE,
	TCP_SERVER_AWAKE,
	TCP_SERVER_CONNECTED,
	TCP_SERVER_RUNNING
} TCP_SERVER_STATUS;

typedef struct {
	boolean IsDhcp_B;
	boolean IsInitialized_B;
	boolean IsConnected_B;
	unsigned char pMacAddr_UB[6];
	IPAddress IpAddr_X;
	unsigned int LocalPort_UI;
	EthernetServer Server_H = EthernetServer(TCP_SERVER_DEFAULT_PORT);
	EthernetClient Client_H;
	TCP_SERVER_STATUS Status_E;
} TCP_SERVER_PARAM;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class TCPServer {
public:
	// Constructor
	TCPServer();

	// Functions
	void init();
	void init(unsigned char pMacAddr_UB[6]);
	void init(unsigned char pMacAddr_UB[6], IPAddress IpAddr_X);
	void init(unsigned char pMacAddr_UB[6], IPAddress IpAddr_X, unsigned int LocalPort_UI);
	void init(unsigned char pMacAddr_UB[6], unsigned int LocalPort_UI);
	boolean isInitialized();

	void setMacAddr(unsigned char pMacAddr_UB[6]);
	void setIpAddr(IPAddress IpAddr_X);
	void setLocalPort(unsigned int LocalPort_UI);
	unsigned char * getMacAddr();
	IPAddress getIpAddr();
	unsigned int getLocalPort();

	void begin();
	void renew();
	boolean isEthernetLinked();

	TCP_SERVER_PARAM GL_TcpServerParam_X;
};

#endif // __TCP_SERVER_H__

