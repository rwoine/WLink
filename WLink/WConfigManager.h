/* ******************************************************************************** */
/*                                                                                  */
/* WConfigManager.h														            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for WConfigManager.cpp							            	*/
/*		Process functions to manage the configuration of the W-Link                 */
/*                                                                                  */
/* History :	25/02/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __WCONFIG_MANAGER_H__
#define __WCONFIG_MANAGER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "WLink.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
static const String GL_cPortalServerName_str = "www.balthinet.be";

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
    WCFG_STS_BUSY           = -1,
    WCFG_STS_OK             = 0,
    WCFG_STS_NOT_ENABLED    = 1,
    WCFG_STS_ERROR          = 100,
    WCFG_STS_ERROR_READING  = 101,
    WCFG_STS_BAD_PARAM      = 102,
    WCFG_STS_ERROR_INIT     = 103
} WCFG_STATUS;

typedef enum {
	WCFG_APP_DEFAULT		= 0x0,
	WCFG_APP_KIP_CONTROL	= 0x1, 
	WCFG_APP_COW_WEIGHT		= 0x2
} WCFG_APP_ENUM;

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void WConfigManager_Init();
void WConfigManager_Enable();
void WConfigManager_Disable();
WCFG_STATUS WConfigManager_Process();

void WConfigManager_BuildSerialGateway();

void WConfig_SetLanguage(unsigned char * pLanguage_UB);
void WConfig_SetDate(unsigned char * pDate_UB);
void WConfig_SetTime(unsigned char * pTime_UB);


#endif // __WCONFIG_MANAGER_H__

