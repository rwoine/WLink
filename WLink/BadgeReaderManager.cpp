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
	BADGE_READER_MANAGER_IDLE,
	BADGE_READER_MANAGER_WAIT_BADGE
};

static BADGE_READER_MANAGER_STATE GL_BadgeReaderManager_CurrentState_E = BADGE_READER_MANAGER_STATE::BADGE_READER_MANAGER_IDLE;

static BadgeReader * GL_pBadgeReader_H;

typedef struct {
	boolean IsEnabled_B;
	boolean IsPacketAvaible_B;
	unsigned long Timer_UL;
	unsigned long ResidenceTime_UL;
	String CurrentPacketId_Str = "";
	unsigned char pCurrentPacketId_UB[BADGE_READER_PACKET_ID_SIZE] = "";
} BADGE_READER_MANAGER_PARAM;

static BADGE_READER_MANAGER_PARAM GL_BadgeReaderManagerParam_X;


/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessWaitBadge(void);

static void TransitionToIdle(void);
static void TransitionToWaitBadge(void);

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void BadgeReaderManager_Init(BadgeReader * pBadgeReader_H) {
	GL_pBadgeReader_H = pBadgeReader_H;
	GL_BadgeReaderManager_CurrentState_E = BADGE_READER_MANAGER_STATE::BADGE_READER_MANAGER_IDLE;
	GL_BadgeReaderManagerParam_X.IsEnabled_B = false;
	GL_BadgeReaderManagerParam_X.IsPacketAvaible_B = false;
	GL_BadgeReaderManagerParam_X.ResidenceTime_UL = BADGE_READER_MANAGER_DEFAULT_RESIDENCE_TIME;
	GL_BadgeReaderManagerParam_X.CurrentPacketId_Str.reserve(BADGE_READER_PACKET_ID_SIZE);
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Badge Reader Manager Initialized");
}

void BadgeReaderManager_Enable() {
	GL_BadgeReaderManagerParam_X.IsEnabled_B = true;
}

void BadgeReaderManager_Disable() {
	GL_BadgeReaderManagerParam_X.IsEnabled_B = false;
}

void BadgeReaderManager_Process() {
	switch (GL_BadgeReaderManager_CurrentState_E) {
	case BADGE_READER_MANAGER_IDLE:
		ProcessIdle();
		break;
	case BADGE_READER_MANAGER_WAIT_BADGE:
		ProcessWaitBadge();
		break;
	}
}


boolean BadgeReaderManager_IsBadgeAvailable(void) {
	return GL_BadgeReaderManagerParam_X.IsPacketAvaible_B;
}

unsigned char BadgeReaderManager_GetBadgeChar(unsigned long Index_UL) {
	return GL_BadgeReaderManagerParam_X.pCurrentPacketId_UB[Index_UL];
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void) {
	if (GL_pBadgeReader_H->isInitialized() && GL_BadgeReaderManagerParam_X.IsEnabled_B)
		TransitionToWaitBadge();
}

static void ProcessWaitBadge(void) {
	String Temp_Str = "";
	unsigned char pTemp_UB[BADGE_READER_PACKET_ID_SIZE] = "";

	// Wait for Packet ID Completed
	if (GL_pBadgeReader_H->isPacketIdCompleted()) {
		GL_pBadgeReader_H->flushBadgeReader();
		GL_pBadgeReader_H->sendAck();
		GL_pBadgeReader_H->resetPacketIdCompleted();
		GL_BadgeReaderManagerParam_X.Timer_UL = millis();	// Save timestamp
		Temp_Str = "";
		for (int i = 0; i < BADGE_READER_PACKET_ID_SIZE - 3; i++) {  // Skip STX, ETX and CR characters
			pTemp_UB[i] = GL_pBadgeReader_H->getPacketChar(i + 1);
			Temp_Str += pTemp_UB[i];
		}

		// Compare if new badge ..
		if (!(Temp_Str.equals(GL_BadgeReaderManagerParam_X.CurrentPacketId_Str))) {
			GL_BadgeReaderManagerParam_X.IsPacketAvaible_B = true;
			DBG_PRINT(DEBUG_SEVERITY_INFO, "New Badge ID = ");
			DBG_PRINTDATA(Temp_Str);
			DBG_ENDSTR();
			// Copy to Parameters
			GL_BadgeReaderManagerParam_X.CurrentPacketId_Str = Temp_Str;
			for (int j = 0; j < BADGE_READER_PACKET_ID_SIZE - 3; j++)
				GL_BadgeReaderManagerParam_X.pCurrentPacketId_UB[j] = pTemp_UB[j];
		}
	}

	// Reset Current Packet ID after Residence Time has elapsed
	if ((millis() - GL_BadgeReaderManagerParam_X.Timer_UL) >= GL_BadgeReaderManagerParam_X.ResidenceTime_UL) {
		GL_BadgeReaderManagerParam_X.IsPacketAvaible_B = false;
		GL_BadgeReaderManagerParam_X.CurrentPacketId_Str = "";
	}

	if (!GL_BadgeReaderManagerParam_X.IsEnabled_B)
		TransitionToIdle();
}


void TransitionToIdle() {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
	GL_BadgeReaderManager_CurrentState_E = BADGE_READER_MANAGER_STATE::BADGE_READER_MANAGER_IDLE;
}

void TransitionToWaitBadge() {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT BADGE");
	GL_BadgeReaderManager_CurrentState_E = BADGE_READER_MANAGER_STATE::BADGE_READER_MANAGER_WAIT_BADGE;
}
