/* ******************************************************************************** */
/*                                                                                  */
/* UDPServerManager.h																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for UDPServerManager.cpp										*/
/*		Process functions to manage the UDP Server object							*/
/*                                                                                  */
/* History :	21/06/2016	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __UDP_SERVER_MANAGER_H__
#define __UDP_SERVER_MANAGER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "UDPServer.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void UDPServerManager_Init(NetworkAdapter * pNetworkAdapter_H, UDPServer * pUDPServer_H);
void UDPServerManager_Enable();
void UDPServerManager_Disable();
void UDPServerManager_Process();

boolean UDPServerManager_IsRunning();


#endif // __UDP_SERVER_MANAGER_H__

