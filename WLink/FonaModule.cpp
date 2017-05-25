/* ******************************************************************************** */
/*                                                                                  */
/* FonaModule.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the FONA module utilities functions   			                */
/*                                                                                  */
/* History :  	25/05/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"FonaModule"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "FonaModule.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static HardwareSerial * GL_pFonaSerial_H;

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
FonaModule::FonaModule() {
    GL_FonaModuleParam_X.IsInitialized_B = false;
}

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void FonaModule::init(HardwareSerial * pSerial_H, boolean Begin_B, unsigned char PinRst_UB, unsigned char PinKey_UB, unsigned char PinPower_UB) {
    init(pSerial_H, FONA_MODULE_DEFAULT_BAUDRATE, Begin_B, PinRst_UB, PinKey_UB, PinPower_UB);
}

void FonaModule::init(HardwareSerial * pSerial_H, unsigned long BaudRate_UL, boolean Begin_B, unsigned char PinRst_UB, unsigned char PinKey_UB, unsigned char PinPower_UB) {
    GL_pFonaSerial_H = pSerial_H;
    if (Begin_B) GL_pFonaSerial_H->begin(BaudRate_UL);
    GL_FonaModuleParam_X.IsInitialized_B = true;

    // Assign pins and define mode
    GL_FonaModuleParam_X.PinRst_UB = PinRst_UB;
    pinMode(GL_FonaModuleParam_X.PinRst_UB, OUTPUT);
    digitalWrite(GL_FonaModuleParam_X.PinRst_UB, HIGH); // tie low for at least 100[ms] for clean reset

    GL_FonaModuleParam_X.PinKey_UB = PinKey_UB;
    pinMode(GL_FonaModuleParam_X.PinKey_UB, OUTPUT);
    digitalWrite(GL_FonaModuleParam_X.PinKey_UB, HIGH); // tie low for at least 2[s] to switch state between ON and OFF

    GL_FonaModuleParam_X.PinPower_UB = PinPower_UB;
    pinMode(GL_FonaModuleParam_X.PinPower_UB, INPUT);   // Low = OFF - High = ON

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "FONA Module Initialized");
}

boolean FonaModule::isInitialized(void) {
    return GL_FonaModuleParam_X.IsInitialized_B;
}

void FonaModule::begin(void) {
    // add default param ATE ATZ ATI...
}
