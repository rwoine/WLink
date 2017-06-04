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
static char GL_pReceiveBuffer_UB[256];

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


void FonaModule::setPinRst(void) {
    digitalWrite(GL_FonaModuleParam_X.PinRst_UB, HIGH);
}

void FonaModule::setPinKey(void) {
    digitalWrite(GL_FonaModuleParam_X.PinKey_UB, HIGH);
}

void FonaModule::resetPinRst(void) {
    digitalWrite(GL_FonaModuleParam_X.PinRst_UB, LOW);
}

void FonaModule::resetPinKey(void) {
    digitalWrite(GL_FonaModuleParam_X.PinKey_UB, LOW);
}

boolean FonaModule::isPowered(void) {
    return ((digitalRead(GL_FonaModuleParam_X.PinPower_UB) == HIGH) ? true : false);
}


void FonaModule::flush(void) {
    // Flush output
    GL_pFonaSerial_H->flush(); 
    
    // Flush input
    while (GL_pFonaSerial_H->available()) {
        GL_pFonaSerial_H->read();
    }
}


void FonaModule::sendAtCommand(char * pData_UB) {
    flush();
    delay(1);
    GL_pFonaSerial_H->println(pData_UB);
}

void FonaModule::sendAtCommand(String Data_Str) {
    sendAtCommand(Data_Str.c_str());
}


unsigned long FonaModule::readLine(void) {

    char c;
    unsigned long Index_UL = 0;

    while (GL_pFonaSerial_H->available()) {

        c = GL_pFonaSerial_H->read();

        if (c == 0x0D)
            continue;

        if (c == 0x0A) {
            if (Index_UL == 0)
                continue;   // Ignore first 0x0A
            else
                break;      // Break on 0x0A - Assume end of response
        }

        GL_pReceiveBuffer_UB[Index_UL++] = c;
        delay(1);
    }

    GL_pReceiveBuffer_UB[Index_UL] = 0; // NULL termination

    return Index_UL;
}


boolean FonaModule::checkAtResponse(char * pData_UB) {
    return (strcmp(GL_pReceiveBuffer_UB, pData_UB) == 0);
}

boolean FonaModule::checkAtResponse(String Data_Str) {
    return checkAtResponse(Data_Str.c_str());
}


void FonaModule::begin(void) {

    boolean NoError_B = false;   

    // Open Communication with AT
    sendAtCommand("AT");
    flush();
    sendAtCommand("AT");
    readLine();

    if (checkAtResponse("AT")) {
        NoError_B = true;
        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Communication opened with FONA module");
    }
    else {
        NoError_B = false;
        DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Could not open communication with FONA module !");
    }


    // Restore factory settings
    if (NoError_B) {
        sendAtCommand("ATZ");
        readLine();
        if (checkAtResponse("OK")) {
            NoError_B = true;
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Factory settings restored");
        }
        else {
            NoError_B = false;
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Could not restore factory settings");
        }
    }


    // Turn-off echo
    if (NoError_B) {
        sendAtCommand("ATE0");
        readLine();
        if (checkAtResponse("OK")) {
            NoError_B = true;
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Echo turned off");
        }
        else {
            NoError_B = false;
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Could not turn off echo");
        }
    }


    // Get module information
    if (NoError_B) {
        sendAtCommand("ATI");
        readLine();
        if (checkAtResponse("OK")) {
            NoError_B = true;

        }
        else {
            NoError_B = false;

        }
    }


}
