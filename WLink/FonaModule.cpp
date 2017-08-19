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


void FonaModule::sendAtCommand(char * pData_UB, boolean Completed_B) {
    flush();
    delay(5);
    GL_pFonaSerial_H->print(pData_UB);

    if (Completed_B)
        GL_pFonaSerial_H->println();
}

void FonaModule::sendAtCommand(String Data_Str, boolean Completed_B) {
    flush();
    delay(5);
    GL_pFonaSerial_H->print(Data_Str);

    if (Completed_B)
        GL_pFonaSerial_H->println();
}

void FonaModule::sendAtCommand(char * pData_UB, char * pSuffix_UB, boolean Quoted_B, boolean Completed_B) {
    flush();
    delay(5);
    GL_pFonaSerial_H->print(pData_UB);

    if (!Quoted_B) {
        GL_pFonaSerial_H->print(pSuffix_UB);
    }
    else {
        GL_pFonaSerial_H->print('"');
        GL_pFonaSerial_H->print(pSuffix_UB);
        GL_pFonaSerial_H->print('"');
    }

    if (Completed_B)
        GL_pFonaSerial_H->println();
}

void FonaModule::sendAtCommand(char * pData_UB, String Suffix_Str, boolean Quoted_B, boolean Completed_B) {
    flush();
    delay(5);
    GL_pFonaSerial_H->print(pData_UB);

    if (!Quoted_B) {
        GL_pFonaSerial_H->print(Suffix_Str);
    }
    else {
        GL_pFonaSerial_H->print('"');
        GL_pFonaSerial_H->print(Suffix_Str);
        GL_pFonaSerial_H->print('"');
    }

    if (Completed_B)
        GL_pFonaSerial_H->println();
}

void FonaModule::sendAtCommand(String Data_Str, String Suffix_Str, boolean Quoted_B, boolean Completed_B) {
    flush();
    delay(5);
    GL_pFonaSerial_H->print(Data_Str);    
    
    if (!Quoted_B) {
        GL_pFonaSerial_H->print(Suffix_Str);
    }
    else {
        GL_pFonaSerial_H->print('"');
        GL_pFonaSerial_H->print(Suffix_Str);
        GL_pFonaSerial_H->print('"');
    }

    if (Completed_B)
        GL_pFonaSerial_H->println();
}

void FonaModule::sendAtCommand(String Data_Str, char * pSuffix_UB, boolean Quoted_B, boolean Completed_B) {
    flush();
    delay(5);
    GL_pFonaSerial_H->print(Data_Str);

    if (!Quoted_B) {
        GL_pFonaSerial_H->print(pSuffix_UB);
    }
    else {
        GL_pFonaSerial_H->print('"');
        GL_pFonaSerial_H->print(pSuffix_UB);
        GL_pFonaSerial_H->print('"');
    }

    if (Completed_B)
        GL_pFonaSerial_H->println();
}

void FonaModule::addAtData(int Data_SI, boolean Quoted_B, boolean Completed_B) {
    if (!Quoted_B) {
        GL_pFonaSerial_H->print(Data_SI);
    }
    else {
        GL_pFonaSerial_H->print('"');
        GL_pFonaSerial_H->print(Data_SI);
        GL_pFonaSerial_H->print('"');
    }

    if (Completed_B)
        GL_pFonaSerial_H->println();
}

void FonaModule::addAtData(char Data_UB, boolean Quoted_B, boolean Completed_B) {
    if (!Quoted_B) {
        GL_pFonaSerial_H->print(Data_UB);
    }
    else {
        GL_pFonaSerial_H->print('"');
        GL_pFonaSerial_H->print(Data_UB);
        GL_pFonaSerial_H->print('"');
    }

    if (Completed_B)
        GL_pFonaSerial_H->println();
}

void FonaModule::addAtData(unsigned long Data_UL, boolean Quoted_B, boolean Completed_B) {
	if (!Quoted_B) {
		GL_pFonaSerial_H->print(Data_UL);
	}
	else {
		GL_pFonaSerial_H->print('"');
		GL_pFonaSerial_H->print(Data_UL);
		GL_pFonaSerial_H->print('"');
	}

	if (Completed_B)
		GL_pFonaSerial_H->println();
}

void FonaModule::addAtData(char * pData_UB, boolean Quoted_B, boolean Completed_B) {
    if (!Quoted_B) {
        GL_pFonaSerial_H->print(pData_UB);
    }
    else {
        GL_pFonaSerial_H->print('"');
        GL_pFonaSerial_H->print(pData_UB);
        GL_pFonaSerial_H->print('"');
    }

    if (Completed_B)
        GL_pFonaSerial_H->println();
}

void FonaModule::addAtData(String Data_Str, boolean Quoted_B, boolean Completed_B) {
    if (!Quoted_B) {
        GL_pFonaSerial_H->print(Data_Str);
    }
    else {
        GL_pFonaSerial_H->print('"');
        GL_pFonaSerial_H->print(Data_Str);
        GL_pFonaSerial_H->print('"');
    }

    if (Completed_B)
        GL_pFonaSerial_H->println();
}

void FonaModule::endAtData(void) {
	GL_pFonaSerial_H->println();
}


unsigned long FonaModule::readLine(boolean DebugPrint_B, unsigned long TimeoutMs_UL) {

    char c;
    unsigned long Index_UL = 0;

    while (TimeoutMs_UL--) {

        // Check for Data in Serial Buffer
        while (GL_pFonaSerial_H->available()) {

            c = GL_pFonaSerial_H->read();

            if (c == 0x0D)
                continue;

            if (c == 0x0A) {
                if (Index_UL == 0)
                    continue;           // Ignore first 0x0A
                else {
                    TimeoutMs_UL = 0;   // Reset timeout value to leave 1st while loop
                    break;              // Break on 0x0A - Assume end of response
                }
            }

            GL_pReceiveBuffer_UB[Index_UL++] = c;
            delay(1);
        }

        delay(1);
        
        if (TimeoutMs_UL == 0) {
            if (Index_UL == 0)
                DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Timeout while waiting for reply!");
            break;
        }

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


boolean FonaModule::parseResponse(char * pBuffer_UB, char * pPrefix_UB, int * pValue_SI, char Token_UB, unsigned int Index_UI) {

    char * p = strstr(pBuffer_UB, pPrefix_UB);  // Get pointer to the Prefix

    if (p == 0)
        return false;

    p += strlen(pPrefix_UB);    // Move pointer after the Prefix

    // Get pointer to the right index, regarding the token
    for (int i = 0; i < Index_UI; i++) {

        p = strchr(p, Token_UB);    // get pointer to token
        if (!p) return false;       // return if no more token
        p++;                        // get char right after token
    }

    // Convert char array to int
    *pValue_SI = atoi(p);

    return true;
}

boolean FonaModule::parseResponse(char * pBuffer_UB, String Prefix_Str, int * pValue_SI, char Token_UB, unsigned int Index_UI) {
    return parseResponse(pBuffer_UB, Prefix_Str.c_str(), pValue_SI, Token_UB, Index_UI);
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


unsigned int FonaModule::getBatteryLevel(void) {

    int Level_SI = 0;

    //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get Battery remaining Capacity");
    sendAtCommand("AT+CBC");
    readLine();

    // Response should be : +CBC: 0,64,3916

    if (!parseResponse(GL_pReceiveBuffer_UB, "+CBC: ", &Level_SI, ',', 1))
        return 0;

    return Level_SI;
}


boolean FonaModule::enableGprs(void) {

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Disconnect all Sockets");

    sendAtCommand("AT+CIPSHUT");
    readLine(true, 20000);
    if (!checkAtResponse("SHUT OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Attach to GPRS Service");

    sendAtCommand("AT+CGATT=1");    // 1 = Attach
    readLine(true, 10000);
    if (!checkAtResponse("OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Configure Bearer Profile - Type GPRS");

    sendAtCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");  // 3 = Configure Bearer, 1 = Bearer Profile Identifier, Set Connection Type to GPRS
    readLine(true, 10000);
    if (!checkAtResponse("OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Configure Bearer Profile - APN");

    sendAtCommand("AT+SAPBR=3,1,\"APN\",", GL_pFonaModuleApn_Str[GL_FonaModuleParam_X.ApnIndex_UL], true);  // 3 = Configure Bearer, 1 = Bearer Profile Identifier, Set APN
    readLine(true, 10000);
    if (!checkAtResponse("OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Start Task and Set APN");

    sendAtCommand("AT+CSTT=", GL_pFonaModuleApn_Str[GL_FonaModuleParam_X.ApnIndex_UL], true);     // Start Task with configured APN 
    readLine(true, 10000);
    if (!checkAtResponse("OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Open Bearer Profile");

    sendAtCommand("AT+SAPBR=1,1");  // 1 = Open Bearer, 1 = Bearer Profile Identifier
    readLine(true, 30000);
    if (!checkAtResponse("OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Check Bearer Status");

    sendAtCommand("AT+SAPBR=2,1");  // 1 = Open Bearer, 1 = Bearer Profile Identifier
    readLine(true, 30000);          // Should reply information
    readLine(true);
    if (!checkAtResponse("OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Bring Up Wireless Connection with GPRS");

    sendAtCommand("AT+CIICR"); 
    readLine(true, 10000);
    if (!checkAtResponse("OK"))
        return false;


    return true;
}

boolean FonaModule::disabeGprs(void) {

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Disconnect all Sockets");

    sendAtCommand("AT+CIPSHUT");
    readLine(true, 20000);
    if (!checkAtResponse("SHUT OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Deactivate Bearer Profile");

    sendAtCommand("AT+SAPBR=0,1");  // 0 = Close Bearer, 1 = Bearer Profile Identifier
    readLine(true, 10000);
    if (!checkAtResponse("OK"))
        return false;


    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Detatch from GPRS Service");

    sendAtCommand("AT+CGATT=0");    // 0 = Detach
    readLine(true, 10000);
    if (!checkAtResponse("OK"))
        return false;


    return true;

}

boolean FonaModule::isGprsEnabled(void) {

    //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get GPRS Service State");
    sendAtCommand("AT+CGATT?");
    readLine();

    // Response should be : +CGATT: state
    //              index = 012345678

    if (GL_pReceiveBuffer_UB[8] == '1')
        return true;
    else
        return false;
}

boolean FonaModule::getNetworkStatus(int * pStatus_SI) {

    //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Get Network Status");
    sendAtCommand("AT+CREG?");
    readLine();

    if (!parseResponse(GL_pReceiveBuffer_UB, "+CREG: ", pStatus_SI, ',', 1))
        return false;

    DBG_PRINT(DEBUG_SEVERITY_INFO, "Network Status = ");
    DBG_PRINTDATA(GL_pFonaModuleNetworkStatus_Str[*pStatus_SI]);
    DBG_ENDSTR();

    return true;
}


boolean FonaModule::httpInit(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Initialize HTTP Service");
    sendAtCommand("AT+HTTPINIT");
    readLine(true);
    return checkAtResponse("OK");
}

boolean FonaModule::httpTerm(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Terminate HTTP Service");
    sendAtCommand("AT+HTTPTERM");
    readLine(true);
    return checkAtResponse("OK");
}


boolean FonaModule::httpParam(FONA_MODULE_HTTP_PARAM_ENUM Param_E, int Value_SI) {
    DBG_PRINT(DEBUG_SEVERITY_INFO, "Set HTTP Parameters Value (int) : ");
    DBG_PRINTDATA(GL_pFonaModuleHttpParam_Str[Param_E]);
    DBG_PRINTDATA(",");
    DBG_PRINTDATA(Value_SI);
    DBG_ENDSTR();

    sendAtCommand("AT+HTTPPARA=", GL_pFonaModuleHttpParam_Str[Param_E], true, false);   // Quoted Param Identifier
    addAtData(",", false, false);
    addAtData(Value_SI, false, true);  // Non-Quoted Value of Param
    readLine(true);
    return checkAtResponse("OK");
}

boolean FonaModule::httpParam(FONA_MODULE_HTTP_PARAM_ENUM Param_E, char * pParamValue_UB) {
	DBG_PRINT(DEBUG_SEVERITY_INFO, "Set HTTP Parameters Value (char array) : ");
	DBG_PRINTDATA(GL_pFonaModuleHttpParam_Str[Param_E]);
	DBG_PRINTDATA(",");
	DBG_PRINTDATA(pParamValue_UB);
	DBG_ENDSTR();

	sendAtCommand("AT+HTTPPARA=", GL_pFonaModuleHttpParam_Str[Param_E], true, false);   // Quoted Param Identifier
	addAtData(",", false, false);
	addAtData(pParamValue_UB, true, true);  // Quoted Value of Param

	readLine(true);
	return checkAtResponse("OK");
}

boolean FonaModule::httpParam(FONA_MODULE_HTTP_PARAM_ENUM Param_E, String ParamValue_Str) {
    return httpParam(Param_E, ParamValue_Str.c_str());
}

boolean FonaModule::httpParamStart(FONA_MODULE_HTTP_PARAM_ENUM Param_E) {
	DBG_PRINT(DEBUG_SEVERITY_INFO, "Set HTTP Parameters Value (char array) : ");
	DBG_PRINTDATA(GL_pFonaModuleHttpParam_Str[Param_E]);
	DBG_ENDSTR();

	sendAtCommand("AT+HTTPPARA=", GL_pFonaModuleHttpParam_Str[Param_E], true, false);   // Quoted Param Identifier
	addAtData(",", false, false);
	return true;
}

boolean FonaModule::httpParamAdd(char ParamValue_UB) {
	DBG_PRINT(DEBUG_SEVERITY_INFO, "Add HTTP Data to Param : ");
	DBG_PRINTDATA(ParamValue_UB);
	DBG_ENDSTR();
	addAtData(ParamValue_UB, false, false);
}

boolean FonaModule::httpParamAdd(int ParamValue_SI) {
	DBG_PRINT(DEBUG_SEVERITY_INFO, "Add HTTP Data to Param : ");
	DBG_PRINTDATA(ParamValue_SI);
	DBG_ENDSTR();
	addAtData(ParamValue_SI, false, false);
}

boolean FonaModule::httpParamAdd(unsigned long ParamValue_UL) {
	DBG_PRINT(DEBUG_SEVERITY_INFO, "Add HTTP Data to Param : ");
	DBG_PRINTDATA(ParamValue_UL);
	DBG_ENDSTR();
	addAtData(ParamValue_UL, false, false);
}

boolean FonaModule::httpParamAdd(char * pParamValue_UB) {
	DBG_PRINT(DEBUG_SEVERITY_INFO, "Add HTTP Data to Param : ");
	DBG_PRINTDATA(pParamValue_UB);
	DBG_ENDSTR();
	addAtData(pParamValue_UB, false, false);
}

boolean FonaModule::httpParamAdd(String ParamValue_Str) {
	return httpParamAdd(ParamValue_Str.c_str());
}

boolean FonaModule::httpParamEnd(void) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of HTTP Data to Param.");
	endAtData();
	readLine(true);
	return checkAtResponse("OK");
}


boolean FonaModule::httpAction(FONA_MODULE_HTTP_ACTION_ENUM Action_E, int * pServerResponse_SI, int * pDataSize_SI) {

    DBG_PRINT(DEBUG_SEVERITY_INFO, "Set HTTP Mehod Action : ");
    DBG_PRINTDATA(GL_pFonaModuleHttpAction_Str[Action_E]);
    DBG_ENDSTR();

    sendAtCommand("AT+HTTPACTION=", (boolean)(false));
    addAtData((int)(Action_E), false, true);            // Non-Quoted Value of Param


    readLine(true, 10000);
    if (!checkAtResponse("OK"))
        return false;


    readLine(true, 30000); // Should reply +HTTPACTION: 0,302,14
    if (!parseResponse(GL_pReceiveBuffer_UB, "+HTTPACTION: ", pServerResponse_SI, ',', 1))
        return false;
    if (!parseResponse(GL_pReceiveBuffer_UB, "+HTTPACTION: ", pDataSize_SI, ',', 2))
        return false;

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Print out Server information : ");
    DBG_PRINT(DEBUG_SEVERITY_INFO, "- Response = ");
    DBG_PRINTDATA(*pServerResponse_SI);
    DBG_ENDSTR();
    DBG_PRINT(DEBUG_SEVERITY_INFO, "- Data Size = ");
    DBG_PRINTDATA(*pDataSize_SI);
    DBG_ENDSTR();

    return true;
}

boolean FonaModule::httpRead(char * pData_UB) {
	int DataLength_SI = 0;

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Read the HTTP Server Response (all data present)");

    sendAtCommand("AT+HTTPREAD", (boolean)true);

    readLine(true); // Response with data length
	if (!parseResponse(GL_pReceiveBuffer_UB, "+HTTPREAD: ", &DataLength_SI, ',', 0))
		return false;
    
	readLine(true); // data
	for (int i = 0; i < DataLength_SI; i++)	pData_UB[i] = GL_pReceiveBuffer_UB[i];

    readLine(true); // OK

    return checkAtResponse("OK");
}

boolean FonaModule::httpRead(char * pData_UB, unsigned long StartAddr_UL, unsigned long DataLength_UL) {
	int DataLength_SI = 0;

    DBG_PRINT(DEBUG_SEVERITY_INFO, "Read the HTTP Server Response : ");
    DBG_PRINTDATA(DataLength_UL);
    DBG_PRINTDATA("[bytes], from offset : ");
    DBG_PRINTDATA(StartAddr_UL);
    DBG_PRINTDATA("[bytes]");
    DBG_ENDSTR();

    sendAtCommand("AT+HTTPREAD=", (boolean)(false));
    addAtData((int)(StartAddr_UL), false, false);
    addAtData(",", false, false);
    addAtData((int)(DataLength_UL), false, true);

    readLine(true); // Response with data length
	if (!parseResponse(GL_pReceiveBuffer_UB, "+HTTPREAD: ", &DataLength_SI, ',', 0))
		return false;

    readLine(true); // data
	for (int i = 0; i < DataLength_SI; i++)	pData_UB[i] = GL_pReceiveBuffer_UB[i];

    readLine(true); // OK

    return checkAtResponse("OK");
}
