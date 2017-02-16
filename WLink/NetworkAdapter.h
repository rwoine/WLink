/* ******************************************************************************** */
/*                                                                                  */
/* NewtorkAdapter.h																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for NewtorkAdapter.cpp											*/
/*		This class creates and manages the low-level Ethernet communication through */
/*      the external SPI chip 	                                                    */
/*                                                                                  */
/* History :	15/02/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __NETWORK_ADAPTER_H__
#define __NETWORK_ADAPTER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include <Arduino.h>
#include "SPI.h"
#include "Ethernet.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

#define NETWORK_ADAPTER_DEFAULT_MAC_ADDR0	0x02  // Unicast - Locally Administered
#define NETWORK_ADAPTER_DEFAULT_MAC_ADDR1	0x00  // 0x00
#define NETWORK_ADAPTER_DEFAULT_MAC_ADDR2	0x00  // 0x00
#define NETWORK_ADAPTER_DEFAULT_MAC_ADDR3	0x00  //  
#define NETWORK_ADAPTER_DEFAULT_MAC_ADDR4	0x00  // 0x000001 - For Default MAC Address
#define NETWORK_ADAPTER_DEFAULT_MAC_ADDR5	0x01  // 

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
    NETWORK_ADAPTER_IDLE,
    NETWORK_ADAPTER_AWAKE,
    NETWORK_ADAPTER_CONNECTED,
    NETWORK_ADAPTER_RUNNING
} NETWORK_ADAPTER_STATUS;

typedef struct {
    boolean IsInitialized_B;
    boolean IsConnected_B;
    boolean IsDhcp_B;
    boolean AdvancedConfig_B;
    unsigned char PintEthLink_UB;
    unsigned char pMacAddr_UB[6];
    IPAddress IpAddr_X;
    IPAddress SubnetMaskAddr_X;
    IPAddress GatewayAddr_X;
    IPAddress DnsIpAddr_X;
    NETWORK_ADAPTER_STATUS Status_E;
} NETWORK_ADAPTER_PARAM;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class NetworkAdapter {
public:
    // Constructor
    NetworkAdapter();

    // Functions
    void init(unsigned char PinEthLink_UB);
    void init(unsigned char PinEthLink_UB, unsigned char pMacAddr_UB[6]);    // DHCP
    void init(unsigned char PinEthLink_UB, unsigned char pMacAddr_UB[6], IPAddress IpAddr_X);
    void init(unsigned char PinEthLink_UB, unsigned char pMacAddr_UB[6], IPAddress IpAddr_X, IPAddress SubnetMaskAddr_X, IPAddress GatewayAddr_X, IPAddress DnsIpAddr_X);    // Advanced Configuration
    boolean isInitialized();

    void setMacAddr(unsigned char pMacAddr_UB[6]);
    void setIpAddr(IPAddress IpAddr_X);
    void setSubnetMaskAddr(IPAddress SubnetMaskAddr_X);
    void setGatewayAddr(IPAddress GatewayAddr_X);
    void setDnsIpAddr(IPAddress DnsIpAddr_X);
    unsigned char * getMacAddr();
    IPAddress getIpAddr();
    IPAddress getSubnetMaskAddr();
    IPAddress getGatewayAddr();
    IPAddress getDnsIpAddr();

    void begin();
    void renew();
    boolean isEthernetLinked();

    NETWORK_ADAPTER_PARAM GL_NetworkAdapterParam_X;
};

#endif // __NETWORK_ADAPTER_H__

