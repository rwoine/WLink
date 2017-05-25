/* ******************************************************************************** */
/*                                                                                  */
/* FonaModule.h																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for FonaModule.cpp												*/
/*		This class creates and manages a FonaModule object to dialog with an 		*/
/*		external GSM module through Serial communication.       					*/
/*                                                                                  */
/* History :	25/05/2017	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __FONA_MODULE_H__
#define __FONA_MODULE_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include <Arduino.h>
#include "Adafruit_FONA.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

#define FONA_MODULE_DEFAULT_BAUDRATE    4800

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

typedef struct {
    boolean IsInitialized_B;
    unsigned char PinRst_UB;
    unsigned char PinKey_UB;
    unsigned char PinPower_UB;
} FONA_MODULE_PARAM;


/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class FonaModule {
public:
    // Constructor
    FonaModule();

    // Functions
    void init(HardwareSerial * pSerial_H, boolean Begin_B, unsigned char PinRst_UB, unsigned char PinKey_UB, unsigned char PinPower_UB);
    void init(HardwareSerial * pSerial_H, unsigned long BaudRate_UL, boolean Begin_B, unsigned char PinRst_UB, unsigned char PinKey_UB, unsigned char PinPower_UB);
    boolean isInitialized();

    void begin(void);

    // add get/set for pin


    FONA_MODULE_PARAM GL_FonaModuleParam_X;
};

#endif // __FONA_MODULE_H__

