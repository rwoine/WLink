/* ******************************************************************************** */
/*                                                                                  */
/* BadgeReader.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the badge reader utilities functions				                */
/*                                                                                  */
/* History :  	01/12/2014  (RW)	Creation of this file                           */
/*				12/01/2015  (RW)	Manage indicator with low-level functions       */
/*				06/06/2016	(RW)	Re-mastered version								*/
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"BadgeReader"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "BadgeReader.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
#define BADGE_READER_CHAR_STX		0x02
#define BADGE_READER_CHAR_ETX		0x03
#define BADGE_READER_CHAR_ACK		0x06
#define BADGE_READER_RX_BUFFER_SIZE	32

static unsigned long GL_BadgeReaderBufferIndex_UL = 0;
static unsigned char GL_pBadgeReaderBuffer_UB[BADGE_READER_RX_BUFFER_SIZE];

static HardwareSerial * GL_pBadgeReaderSerial_H;

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
BadgeReader::BadgeReader() {
	GL_BadgeReaderParam_X.IsInitialized_B = false;
	GL_BadgeReaderParam_X.PacketIdSize_UL = 0;
	GL_BadgeReaderParam_X.PacketIdCompleted_B = false;
}

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void BadgeReader::init(HardwareSerial * pSerial_H) {
	GL_pBadgeReaderSerial_H = pSerial_H;
	GL_pBadgeReaderSerial_H->begin(BADGE_READER_DEFAULT_BAUDRATE);
	GL_BadgeReaderParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Badge Reader Initialized");
}

void BadgeReader::init(HardwareSerial * pSerial_H, unsigned long BaudRate_UL) {
	GL_pBadgeReaderSerial_H = pSerial_H;
	GL_pBadgeReaderSerial_H->begin(BaudRate_UL);
	GL_BadgeReaderParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Badge Reader Initialized");
}

boolean BadgeReader::isInitialized(void) {
	return GL_BadgeReaderParam_X.IsInitialized_B;
}

boolean BadgeReader::isPacketIdCompleted(void) {
	return GL_BadgeReaderParam_X.PacketIdCompleted_B;
}

void BadgeReader::resetPacketIdCompleted(void) {
	GL_BadgeReaderParam_X.PacketIdCompleted_B = false;
}

void BadgeReader::sendAck(void) {
	GL_pBadgeReaderSerial_H->write(BADGE_READER_CHAR_ACK);
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Send ACK");
}

unsigned char BadgeReader::getPacketChar(unsigned long Index_UL) {
	return GL_pBadgeReaderBuffer_UB[Index_UL];
}

void BadgeReader::flushBadgeReader(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Flush Serial Buffer of Badge Reader");
	GL_pBadgeReaderSerial_H->flush();
}


void BadgeReader::commEvent(void) {
	while (GL_pBadgeReaderSerial_H->available()) {
		GL_pBadgeReaderBuffer_UB[GL_BadgeReaderBufferIndex_UL] = GL_pBadgeReaderSerial_H->read();
		if (GL_pBadgeReaderBuffer_UB[GL_BadgeReaderBufferIndex_UL] == 0x03) {
			GL_BadgeReaderBufferIndex_UL = 0;
			GL_BadgeReaderParam_X.PacketIdSize_UL = GL_BadgeReaderBufferIndex_UL;
			GL_BadgeReaderParam_X.PacketIdCompleted_B = BADGE_READER_CHAR_ETX;
		}
		else {
			GL_BadgeReaderBufferIndex_UL++;
		}
	}
}
