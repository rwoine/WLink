/* ******************************************************************************** */
/*                                                                                  */
/* NetworkAdapterManager.h															*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for NetworkAdapterManager.cpp									*/
/*		Process functions to manage the Network Adapter object						*/
/*                                                                                  */
/* History :	22/02/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __NETWORK_ADAPTER_MANAGER_H__
#define __NETWORK_ADAPTER_MANAGER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "NetworkAdapter.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void NetworkAdapterManager_Init(NetworkAdapter * pNetworkAdapter_H);
void NetworkAdapterManager_Enable();
void NetworkAdapterManager_Disable();
void NetworkAdapterManager_Process();

boolean NetworkAdapterManager_IsRunning();


#endif // __NETWORK_ADAPTER_MANAGER_H__

