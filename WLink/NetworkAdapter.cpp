/* ******************************************************************************** */
/*                                                                                  */
/* NetworkAdapter.cpp														        */
/*                                                                                  */
/* Description :                                                                    */
/*		Defines the utility functions that manage the Network Adapter object        */
/*                                                                                  */
/* History :  	16/02/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"NetworkAdapter"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "NetworkAdapter.h"
#include "Hardware.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */



/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
NetworkAdapter::NetworkAdapter() {
    GL_NetworkAdapterParam_X.IsInitialized_B = false;
    GL_NetworkAdapterParam_X.IsConnected_B = false;
    GL_NetworkAdapterParam_X.Status_E = NETWORK_ADAPTER_IDLE;
}


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void NetworkAdapter::init(unsigned char PinEthLink_UB) {
    GL_NetworkAdapterParam_X.PintEthLink_UB = PinEthLink_UB;
    pinMode(GL_NetworkAdapterParam_X.PintEthLink_UB, INPUT);

    GL_NetworkAdapterParam_X.pMacAddr_UB[0] = NETWORK_ADAPTER_DEFAULT_MAC_ADDR0;
    GL_NetworkAdapterParam_X.pMacAddr_UB[1] = NETWORK_ADAPTER_DEFAULT_MAC_ADDR1;
    GL_NetworkAdapterParam_X.pMacAddr_UB[2] = NETWORK_ADAPTER_DEFAULT_MAC_ADDR2;
    GL_NetworkAdapterParam_X.pMacAddr_UB[3] = NETWORK_ADAPTER_DEFAULT_MAC_ADDR3;
    GL_NetworkAdapterParam_X.pMacAddr_UB[4] = NETWORK_ADAPTER_DEFAULT_MAC_ADDR4;
    GL_NetworkAdapterParam_X.pMacAddr_UB[5] = NETWORK_ADAPTER_DEFAULT_MAC_ADDR5;

    GL_NetworkAdapterParam_X.IsDhcp_B = true;
    GL_NetworkAdapterParam_X.AdvancedConfig_B = false;

    GL_NetworkAdapterParam_X.IsInitialized_B = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Network Adapter Initialized");
}

void NetworkAdapter::init(unsigned char PinEthLink_UB, unsigned char pMacAddr_UB[6]) {
    GL_NetworkAdapterParam_X.PintEthLink_UB = PinEthLink_UB;
    pinMode(GL_NetworkAdapterParam_X.PintEthLink_UB, INPUT);

    for (int i = 0; i < 6; i++)
        GL_NetworkAdapterParam_X.pMacAddr_UB[i] = pMacAddr_UB[i];

    GL_NetworkAdapterParam_X.IsDhcp_B = true;
    GL_NetworkAdapterParam_X.AdvancedConfig_B = false;

    GL_NetworkAdapterParam_X.IsInitialized_B = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Network Adapter Initialized");
}

void NetworkAdapter::init(unsigned char PinEthLink_UB, unsigned char pMacAddr_UB[6], IPAddress IpAddr_X) {
    GL_NetworkAdapterParam_X.PintEthLink_UB = PinEthLink_UB;
    pinMode(GL_NetworkAdapterParam_X.PintEthLink_UB, INPUT);

    for (int i = 0; i < 6; i++)
        GL_NetworkAdapterParam_X.pMacAddr_UB[i] = pMacAddr_UB[i];

    GL_NetworkAdapterParam_X.IpAddr_X = IpAddr_X;

    GL_NetworkAdapterParam_X.IsDhcp_B = false;
    GL_NetworkAdapterParam_X.AdvancedConfig_B = false;

    GL_NetworkAdapterParam_X.IsInitialized_B = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Network Adapter Initialized");
}

void NetworkAdapter::init(unsigned char PinEthLink_UB, unsigned char pMacAddr_UB[6], IPAddress IpAddr_X, IPAddress SubnetMaskAddr_X, IPAddress GatewayAddr_X, IPAddress DnsIpAddr_X) {
    GL_NetworkAdapterParam_X.PintEthLink_UB = PinEthLink_UB;
    pinMode(GL_NetworkAdapterParam_X.PintEthLink_UB, INPUT);

    for (int i = 0; i < 6; i++)
        GL_NetworkAdapterParam_X.pMacAddr_UB[i] = pMacAddr_UB[i];

    GL_NetworkAdapterParam_X.IpAddr_X = IpAddr_X;
    GL_NetworkAdapterParam_X.SubnetMaskAddr_X = SubnetMaskAddr_X;
    GL_NetworkAdapterParam_X.GatewayAddr_X = GatewayAddr_X;
    GL_NetworkAdapterParam_X.DnsIpAddr_X = DnsIpAddr_X;

    GL_NetworkAdapterParam_X.IsDhcp_B = false;
    GL_NetworkAdapterParam_X.AdvancedConfig_B = true;

    GL_NetworkAdapterParam_X.IsInitialized_B = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Network Adapter Initialized");
}

boolean NetworkAdapter::isInitialized() {
    return GL_NetworkAdapterParam_X.IsInitialized_B;
}