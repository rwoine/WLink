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

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define UDP_SERVER_DEFAULT_PORT			8888

#define UDP_SERVER_DEFAULT_MAC_ADDR0	0x02  // Unicast - Locally Administered
#define UDP_SERVER_DEFAULT_MAC_ADDR1	0x00  // 0x00
#define UDP_SERVER_DEFAULT_MAC_ADDR2	0x00  // 0x00
#define UDP_SERVER_DEFAULT_MAC_ADDR3	0x00  //  
#define UDP_SERVER_DEFAULT_MAC_ADDR4	0x00  // 0x000001 - For Default MAC Address
#define UDP_SERVER_DEFAULT_MAC_ADDR5	0x01  // 

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
	UDP_SERVER_IDLE,
	UDP_SERVER_AWAKE,
	UDP_SERVER_CONNECTED,
	UDP_SERVER_RUNNING
} UDP_SERVER_STATUS;

typedef struct {
	boolean IsDhcp_B;
	boolean IsInitialized_B;
	boolean IsConnected_B;
	unsigned char pMacAddr_UB[6];
	IPAddress IpAddr_X;
	unsigned int LocalPort_UI;
	EthernetUDP Server_H;
	UDP_SERVER_STATUS Status_E;
} UDP_SERVER_PARAM;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class UDPServer {
public:
	// Constructor
	UDPServer();

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

	UDP_SERVER_PARAM GL_UdpServerParam_X;
};

#endif // __UDP_SERVER_H__

