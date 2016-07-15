/* ******************************************************************************** */
/*                                                                                  */
/* TCPServerManager.h																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for TCPServerManager.cpp										*/
/*		Process functions to manage the TCP Server object							*/
/*                                                                                  */
/* History :	02/06/2016	(RW)	Creation of this file                           */
/*				15/07/2016	(RW)	Add Enable and Disable functions				*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __TCP_SERVER_MANAGER_H__
#define __TCP_SERVER_MANAGER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "TCPServer.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void TCPServerManager_Init(TCPServer * pTCPServer_H);
void TCPServerManager_Enable();
void TCPServerManager_Disable();
void TCPServerManager_Process();


#endif // __TCP_SERVER_MANAGER_H__

