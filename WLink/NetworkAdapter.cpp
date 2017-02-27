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

#include "Debug.h"


/* ******************************************************************************** */
/* Internal Functions Prototypes
/* ******************************************************************************** */
String HexArrayToString(unsigned char * pHexArray, unsigned long ItemNb_UL, String Separator_Str);


/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
NetworkAdapter::NetworkAdapter() {
    GL_NetworkAdapterParam_X.IsInitialized_B = false;
    GL_NetworkAdapterParam_X.IsConnected_B = false;
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
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Default MAC Address = " + HexArrayToString(GL_NetworkAdapterParam_X.pMacAddr_UB, sizeof(GL_NetworkAdapterParam_X.pMacAddr_UB), ":"));
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "DHCP Configuration");
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
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "MAC Address = " + HexArrayToString(GL_NetworkAdapterParam_X.pMacAddr_UB, sizeof(GL_NetworkAdapterParam_X.pMacAddr_UB), ":"));
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "DHCP Configuration");
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
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "MAC Address = " + HexArrayToString(GL_NetworkAdapterParam_X.pMacAddr_UB, sizeof(GL_NetworkAdapterParam_X.pMacAddr_UB), ":"));
    DBG_PRINT(DEBUG_SEVERITY_INFO, "IP Address = ");
    DBG_PRINTDATA(IpAddr_X);
    DBG_ENDSTR();
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
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "MAC Address = " + HexArrayToString(GL_NetworkAdapterParam_X.pMacAddr_UB, sizeof(GL_NetworkAdapterParam_X.pMacAddr_UB), ":"));
    DBG_PRINT(DEBUG_SEVERITY_INFO, "IP Address = ");
    DBG_PRINTDATA(IpAddr_X);
    DBG_ENDSTR();
    DBG_PRINT(DEBUG_SEVERITY_INFO, "Subnet Mask = ");
    DBG_PRINTDATA(SubnetMaskAddr_X);
    DBG_ENDSTR();
    DBG_PRINT(DEBUG_SEVERITY_INFO, "Gateway Address = ");
    DBG_PRINTDATA(GatewayAddr_X);
    DBG_ENDSTR();
    DBG_PRINT(DEBUG_SEVERITY_INFO, "DNS Address = ");
    DBG_PRINTDATA(DnsIpAddr_X);
    DBG_ENDSTR();
}

boolean NetworkAdapter::isInitialized(void) {
    return GL_NetworkAdapterParam_X.IsInitialized_B;
}


void NetworkAdapter::setMacAddr(unsigned char pMacAddr_UB[6]) {
    for (int i = 0; i < 6; i++)
        GL_NetworkAdapterParam_X.pMacAddr_UB[i] = pMacAddr_UB[i];
}

void NetworkAdapter::setIpAddr(IPAddress IpAddr_X) {
    GL_NetworkAdapterParam_X.IsDhcp_B = false;
    GL_NetworkAdapterParam_X.IpAddr_X = IpAddr_X;
}

void NetworkAdapter::setSubnetMaskAddr(IPAddress SubnetMaskAddr_X) {
    GL_NetworkAdapterParam_X.SubnetMaskAddr_X = SubnetMaskAddr_X;
}

void NetworkAdapter::setGatewayAddr(IPAddress GatewayAddr_X) {
    GL_NetworkAdapterParam_X.GatewayAddr_X = GatewayAddr_X;
}

void NetworkAdapter::setDnsIpAddr(IPAddress DnsIpAddr_X) {
    GL_NetworkAdapterParam_X.DnsIpAddr_X = DnsIpAddr_X;
}

unsigned char * NetworkAdapter::getMacAddr(void) {
    return GL_NetworkAdapterParam_X.pMacAddr_UB;
}

IPAddress NetworkAdapter::getIpAddr(void) {
    return GL_NetworkAdapterParam_X.IpAddr_X;
}

IPAddress NetworkAdapter::getSubnetMaskAddr(void) {
    return GL_NetworkAdapterParam_X.SubnetMaskAddr_X;
}

IPAddress NetworkAdapter::getGatewayAddr(void) {
    return GL_NetworkAdapterParam_X.GatewayAddr_X;
}

IPAddress NetworkAdapter::getDnsIpAddr(void) {
    return GL_NetworkAdapterParam_X.DnsIpAddr_X;
}


void NetworkAdapter::enableDhcp(void) {
    GL_NetworkAdapterParam_X.IsDhcp_B = true;
}


void NetworkAdapter::begin(void) {
    if (GL_NetworkAdapterParam_X.AdvancedConfig_B) {
        Ethernet.begin(GL_NetworkAdapterParam_X.pMacAddr_UB, GL_NetworkAdapterParam_X.IpAddr_X, GL_NetworkAdapterParam_X.DnsIpAddr_X, GL_NetworkAdapterParam_X.GatewayAddr_X, GL_NetworkAdapterParam_X.SubnetMaskAddr_X);
    }
    else if (GL_NetworkAdapterParam_X.IsDhcp_B) {
        Ethernet.begin(GL_NetworkAdapterParam_X.pMacAddr_UB);
        GL_NetworkAdapterParam_X.IpAddr_X = Ethernet.localIP();
        DBG_PRINT(DEBUG_SEVERITY_INFO, "IP Address = ");
        DBG_PRINTDATA(GL_NetworkAdapterParam_X.IpAddr_X);
        DBG_ENDSTR();
    }
    else {
        Ethernet.begin(GL_NetworkAdapterParam_X.pMacAddr_UB, GL_NetworkAdapterParam_X.IpAddr_X);
    }
    GL_NetworkAdapterParam_X.IsConnected_B = true;
}

void NetworkAdapter::renew(void) {
    GL_NetworkAdapterParam_X.IsConnected_B = false;
    flush();
    begin();
}

void NetworkAdapter::flush(void) {
    // TODO : Call stop/flush functions. Callback ?
}

boolean NetworkAdapter::isConnected() {
    return (GL_NetworkAdapterParam_X.IsConnected_B);
}

boolean NetworkAdapter::isEthernetLinked() {
    return ((digitalRead(GL_NetworkAdapterParam_X.PintEthLink_UB) == LOW) ? true : false);
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
String HexArrayToString(unsigned char * pHexArray, unsigned long ItemNb_UL, String Separator_Str) {
    String Temp_Str = "";
    unsigned int Temp_UW = 0;

    for (int i = 0; i < ItemNb_UL; i++) {
        Temp_UW = pHexArray[i];
        Temp_Str += String((Temp_UW % 128), HEX);
        Temp_Str += String((Temp_UW / 128), HEX);
        if (i < ItemNb_UL-1)
            Temp_Str += Separator_Str;
    }
}

