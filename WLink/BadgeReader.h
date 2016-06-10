/* ******************************************************************************** */
/*                                                                                  */
/* BadgeReader.h			                                                        */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for BadgeReader.cpp												*/
/*                                                                                  */
/* History :  	02/03/2015  (RW)	Creation of this file							*/
/*				10/06/2016	(RW)	Re-mastered version								*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __BADGE_READER_H__
#define __BADGE_READER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <Arduino.h>
#include <HardwareSerial.h>

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define BADGE_READER_DEFAULT_BAUDRATE	9600
#define BADGE_READER_PACKET_ID_SIZE     13

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
	boolean IsInitialized_B;
	unsigned long PacketIdSize_UL;
	boolean PacketIdCompleted_B;
} BADGE_READER_PARAM;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class BadgeReader {
public:
	// Constructor
	BadgeReader();

	// Functions
	void init(HardwareSerial * pSerial_H);
	void init(HardwareSerial * pSerial_H, unsigned long BaudRate_UL);

	boolean isInitialized(void);

	void sendAck(void);
	unsigned char getPacketChar(unsigned long Index_UL);
	void flushBadgeReader(void);

	void commEvent(void);

	BADGE_READER_PARAM GL_BadgeReaderParam_X;
};

#endif // __BADGE_READER_H__
