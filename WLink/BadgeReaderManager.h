/* ******************************************************************************** */
/*                                                                                  */
/* BadgeReaderManager.h																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for BadgeReaderManager.cpp										*/
/*		Process functions to manager to Badge Reader object							*/
/*                                                                                  */
/* History :	11/06/2016	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __BADGE_READER_MANAGER_H__
#define __BADGE_READER_MANAGER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "BadgeReader.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define BADGE_READER_MANAGER_DEFAULT_RESIDENCE_TIME	    4000

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void BadgeReaderManager_Init(BadgeReader * pBadgeReader_H);
void BadgeReaderManager_Enable();
void BadgeReaderManager_Disable();
void BadgeReaderManager_Process();

boolean BadgeReaderManager_IsBadgeAvailable(unsigned long * pPacketIdSize_UL);
unsigned char BadgeReaderManager_GetBadgeChar(unsigned long Index_UL);


#endif // __BADGE_READER_MANAGER_H__

