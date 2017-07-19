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
void FonaModule::init(HardwareSerial * pSerial_H, unsigned long BaudRate_UL, boolean Begin_B, unsigned char PinRst_UB, unsigned char PinKey_UB, unsigned char PinPower_UB, char * pPinCode_UB, unsigned long ApnIdx_UL) {
    GL_pFonaSerial_H = pSerial_H;
    if (Begin_B) GL_pFonaSerial_H->begin(BaudRate_UL);

    // Assign pins and define mode

    // Notes : 
    //      > Output pins are connected on the NO output of the relay.
    //      > The common connection is tied to the GND.
    //      > Setting the pin to HIGH closes the relay and force the output to the GND.
    //      > Pull-up resistors are already present on the FONA module to tie the signal to VCC when relay is not activated (W-Link output = LOW).
    //      > The strap connection between the PowerKey and the GND should be removed on the FONA module.

    GL_FonaModuleParam_X.PinRst_UB = PinRst_UB;
    pinMode(GL_FonaModuleParam_X.PinRst_UB, OUTPUT);
    digitalWrite(GL_FonaModuleParam_X.PinRst_UB, LOW);  // tie low for at least 100[ms] for clean reset

    GL_FonaModuleParam_X.PinKey_UB = PinKey_UB;
    pinMode(GL_FonaModuleParam_X.PinKey_UB, OUTPUT);
    digitalWrite(GL_FonaModuleParam_X.PinKey_UB, LOW);  // tie low for at least 2[s] to switch state between ON and OFF

    GL_FonaModuleParam_X.PinPower_UB = PinPower_UB;
    pinMode(GL_FonaModuleParam_X.PinPower_UB, INPUT);   // Low = OFF - High = ON

    GL_FonaModuleParam_X.ApnIndex_UL = ApnIdx_UL;
    DBG_PRINT(DEBUG_SEVERITY_INFO, "Network Access Point Name (APN) is : ");
    DBG_PRINTDATA(GL_pFonaModuleApn_Str[GL_FonaModuleParam_X.ApnIndex_UL]);
    DBG_ENDSTR();

    memccpy(GL_FonaModuleParam_X.pPinCode_UB, pPinCode_UB, NULL, 5);

    GL_FonaModuleParam_X.IsInitialized_B = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "FONA Module Initialized");
}

boolean FonaModule::isInitialized(void) {
    return GL_FonaModuleParam_X.IsInitialized_B;
}


void FonaModule::setPinRst(void) {
    digitalWrite(GL_FonaModuleParam_X.PinRst_UB, LOW);
}

void FonaModule::setPinKey(void) {
    digitalWrite(GL_FonaModuleParam_X.PinKey_UB, LOW);
}

void FonaModule::clearPinRst(void) {
    digitalWrite(GL_FonaModuleParam_X.PinRst_UB, HIGH);
}

void FonaModule::clearPinKey(void) {
    digitalWrite(GL_FonaModuleParam_X.PinKey_UB, HIGH);
}

boolean FonaModule::isPowered(void) {
    return ((digitalRead(GL_FonaModuleParam_X.PinPower_UB) == HIGH) ? true : false);
}


void FonaModule::flush(void) {
    // Flush output
    GL_pFonaSerial_H->flush();

    delay(500);
    
    // Flush input
    while (GL_pFonaSerial_H->available()) {
        GL_pFonaSerial_H->read();
    }
}


void FonaModule::sendAtCommand(char * pData_UB) {
    flush();
    delay(5);
    GL_pFonaSerial_H->println(pData_UB);
}

void FonaModule::sendAtCommand(String Data_Str) {
    sendAtCommand(Data_Str.c_str());
}

void FonaModule::sendAtCommand(char * pData_UB, char * pSuffix_UB) {
    flush();
    delay(5);
    GL_pFonaSerial_H->print(pData_UB);
    GL_pFonaSerial_H->println(pSuffix_UB);
}

void FonaModule::sendAtCommand(char * pData_UB, String Suffix_Str) {
    sendAtCommand(pData_UB, Suffix_Str.c_str());
}

void FonaModule::sendAtCommand(String Data_Str, String Suffix_Str) {
    sendAtCommand(Data_Str.c_str(), Suffix_Str.c_str());
}

void FonaModule::sendAtCommand(String Data_Str, char * pSuffix_UB) {
    sendAtCommand(Data_Str.c_str(), pSuffix_UB);
}


unsigned long FonaModule::readLine(boolean DebugPrint_B) {

    delay(500);

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

    if (DebugPrint_B) {
        //DBG_PRINT(DEBUG_SEVERITY_INFO, "Print out reply from FONA Module (");
        //DBG_PRINTDATA(Index_UL);
        //DBG_PRINTDATA(" chars) :");
        //DBG_ENDSTR();
        DBG_PRINTLN(DEBUG_SEVERITY_INFO, GL_pReceiveBuffer_UB);
    }

    return Index_UL;
}


boolean FonaModule::checkAtResponse(char * pData_UB) {
    return (strcmp(GL_pReceiveBuffer_UB, pData_UB) == 0);
}

boolean FonaModule::checkAtResponse(String Data_Str) {
    return checkAtResponse(Data_Str.c_str());
}


void FonaModule::begin(void) {

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "FONA Module correctly powered -> start begin sequence..");

    /*

    AT\r\n

    AT\r
    \r\n
    OK\r\n
    

    ATZ\r\n

    ATZ\r
    \r\n
    OK\r\n


    ATE0\r\n

    ATE0\r
    \r\n
    OK\r\n


    ATI\r\n

    \r\n
    SIM800 R13.08\r\n
    \r\n
    OK\r\n


    AT\r\n
    
    \r\n
    OK\r\n    
    
    */



    boolean NoError_B = false;   

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Open Communication with AT");
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
        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Reset Factory settings..");
        sendAtCommand("ATZ");
        readLine(); // Should reply ATZ
        readLine(); // Should reply OK
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
        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Turn off echo..");
        sendAtCommand("ATE0");
        readLine(); // Should reply ATE0
        readLine(); // Should reply OK
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
        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get module information..");
        sendAtCommand("ATI");
        readLine(true); // Should reply module information
        readLine(); // Should reply OK
        if (checkAtResponse("OK")) {
            NoError_B = true;
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Properly identified module");
        }
        else {
            NoError_B = false;
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Could not get module information");
        }
    }


    // Get IMEI
    if (NoError_B) {
        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get IMEI..");
        sendAtCommand("AT+GSN");
        readLine(true); // Should reply IMEI
        readLine();     // Should reply OK
        if (checkAtResponse("OK")) {
            NoError_B = true;
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Properly get IMEI");
        }
        else {
            NoError_B = false;
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Could not get IMEI");
        }
    }


    // Send last AT command to validate communication without Echo
    if (NoError_B) {
        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Send last 'AT'..");
        sendAtCommand("AT");
        readLine(); // Should reply OK

        if (checkAtResponse("OK")) {
            NoError_B = true;
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "FONA module ready for duty !");
        }
        else {
            NoError_B = false;
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "FONA module not ready -> reset needed");
        }
    }

}

boolean FonaModule::enterPinCode(char * pPinCode_UB) {

    boolean Status_B = false;

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get PIN requirement");
    sendAtCommand("AT+CPIN?");

    readLine(true); // Should reply requirement
    
    if (checkAtResponse("+CPIN: READY")) {

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "PIN code not requested");

    }
    else if (checkAtResponse("+CPIN: SIM PIN")) {

        DBG_PRINTLN(DEBUG_SEVERITY_INFO, "PIN code requested > Send PIN ****");

        sendAtCommand("AT+CPIN=", pPinCode_UB);
        readLine();
        if (checkAtResponse("OK")) {
            Status_B = true;
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "PIN code entered > SIM card unlocked");
        }
        else {
            Status_B = false;
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Wrong PIN > SIM card still locked");
        }
    }
    else {

        DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Problem with PIN > Card should be unlocked by other means..");

    }

    return Status_B;
}

boolean FonaModule::enterPinCode(String PinCode_Str) {
    enterPinCode(PinCode_Str.c_str());
}

boolean FonaModule::enterPinCode(void) {
    enterPinCode(GL_FonaModuleParam_X.pPinCode_UB);
}

signed int FonaModule::getSignalStrength(void) {

    char pCharValue_UB[4] = { 0 };
    int RawValue_SI = 0;
    int DbmValue_SI = 0;

    //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get Received Signal Strength Indication (RSSI)");
    //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Value from -155[dBm] to -52[dBm]");
    sendAtCommand("AT+CSQ");
    readLine();

    // Response should be : +CSQ: rssi,ber
    //              index = 0123456789
    for (int i = 0; i < 4; i++) {
        if (GL_pReceiveBuffer_UB[6 + i] == ',') {
            // End of RSSI value
            pCharValue_UB[i] = 0;   // NULL termination
            break;
        }
        else {
            pCharValue_UB[i] = GL_pReceiveBuffer_UB[6 + i];
        }    
    }

    RawValue_SI = atoi(pCharValue_UB);
    //DBG_PRINT(DEBUG_SEVERITY_INFO, "Raw value given by FONA module : ");
    //DBG_PRINTDATA(RawValue_SI);
    //DBG_ENDSTR();

    if (RawValue_SI == 0) {
        DbmValue_SI = -115;
    }
    else if (RawValue_SI == 1) {
        DbmValue_SI = -111;
    }
    else if ((RawValue_SI >= 2) && (RawValue_SI <= 30)) {
        DbmValue_SI = (RawValue_SI << 1) - 114;
    }
    else if (RawValue_SI == 31) {
        DbmValue_SI = -52;
    }
    else {
        DbmValue_SI = 0;    // Error
    }

    //DBG_PRINT(DEBUG_SEVERITY_INFO, "RSSI Value : ");
    //DBG_PRINTDATA(DbmValue_SI);
    //DBG_PRINTDATA("[dBm]");
    //DBG_ENDSTR();

    return DbmValue_SI;
}


boolean FonaModule::enableGprs(void) {
    return false;
}

boolean FonaModule::disabeGprs(void) {

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Disconnect all Sockets");

    sendAtCommand("AT+CIPSHUT");
    delay(20000);
    readLine(true);
    if (!checkAtResponse("SHUT OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Deactivate Bearer Proile");

    sendAtCommand("AT+SAPBR=0,1");
    delay(10000);
    readLine(true);
    if (!checkAtResponse("OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Detatch from GPRS Network");

    sendAtCommand("AT+CGATT=0");
    delay(10000);
    readLine(true);
    if (!checkAtResponse("OK"))
        return false;


    return true;

}
