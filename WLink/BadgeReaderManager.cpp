/* ******************************************************************************** */
/*                                                                                  */
/* BadgeReaderManager.cpp															*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the Badge Reader object				*/
/*                                                                                  */
/* History :  	11/06/2015  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"BadgeReaderManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "BadgeReaderManager.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */



/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum BADGE_READER_MANAGER_STATE {
	BADGE_READER_MANAGER_IDLE
};

static BADGE_READER_MANAGER_STATE GL_BadgeReaderManager_CurrentState_E = BADGE_READER_MANAGER_STATE::BADGE_READER_MANAGER_IDLE;

static BadgeReader * GL_pBadgeReader_H;




/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */



/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void BadgeReaderManager_Init(BadgeReader * pBadgeReader_H) {

}

void BadgeReaderManager_Enable() {

}

void BadgeReaderManager_Disable() {

}

void BadgeReaderManager_Process() {

}


boolean BadgeReaderManager_IsBadgeAvailable(unsigned long * pPacketIdSize_UL) {

}

unsigned char BadgeReaderManager_GetBadgeChar(unsigned long Index_UL) {

}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

