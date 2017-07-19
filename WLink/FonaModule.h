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

#define FONA_MODULE_DEFAULT_BAUDRATE    4800UL

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

typedef struct {
    boolean IsInitialized_B;
    unsigned char PinRst_UB;
    unsigned char PinKey_UB;
    unsigned char PinPower_UB;
    unsigned long ApnIndex_UL;
    char pPinCode_UB[4];
} FONA_MODULE_PARAM;

const String GL_pFonaModuleApn_Str[] = { "mworld.be" };


/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class FonaModule {
public:
    // Constructor
    FonaModule();

    // Functions
    void init(HardwareSerial * pSerial_H, unsigned long BaudRate_UL, boolean Begin_B, unsigned char PinRst_UB, unsigned char PinKey_UB, unsigned char PinPower_UB, char * pPinCode_UB, unsigned long ApnIdx_UL = 0);
    boolean isInitialized();


    void setPinRst(void);
    void setPinKey(void);
    void clearPinRst(void);
    void clearPinKey(void);
    boolean isPowered(void);


    void flush(void);
    unsigned long readLine(boolean DebugPrint_B = false);
    void sendAtCommand(char * pData_UB);
    void sendAtCommand(String Data_Str);
    void sendAtCommand(char * pData_UB, char * pSuffix_UB);
    void sendAtCommand(char * pData_UB, String Suffix_Str);
    void sendAtCommand(String Data_Str, String Suffix_Str);
    void sendAtCommand(String Data_Str, char * pSuffix_UB);
    boolean checkAtResponse(char * pData_UB);
    boolean checkAtResponse(String Data_Str);
    void begin(void);


    boolean enterPinCode(char * pPinCode_UB);
    boolean enterPinCode(String PinCode_Str);
    boolean enterPinCode(void);

    signed int getSignalStrength(void);

    boolean enableGprs(void);
    boolean disabeGprs(void);


    FONA_MODULE_PARAM GL_FonaModuleParam_X;
};

#endif // __FONA_MODULE_H__

