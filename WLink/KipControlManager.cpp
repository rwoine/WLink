/* ******************************************************************************** */
/*                                                                                  */
/* KipControlManager.cpp												            */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the KipControl application   			*/
/*                                                                                  */
/* History :  	17/04/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"KipControlManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "KipControlManager.h"
#include "KipControlMenuManager.h"
#include "EepromWire.h"

#include "EthernetClient.h"
#include "Utilz.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Extenral Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define KC_WORKING_AREA_OFFSET      0x0400
#define KC_GLOBAL_DATA_ADDR         (KC_WORKING_AREA_OFFSET + 0x0000)
#define KC_TOLERANCE_ADDR           (KC_WORKING_AREA_OFFSET + 0x0001)
#define KC_REFERENCE_DATA_ID_ADDR   (KC_WORKING_AREA_OFFSET + 0x0002)
#define KC_MAX_DATA_NB_ADDR         (KC_WORKING_AREA_OFFSET + 0x0003)
#define KC_START_IDX_ADDR           (KC_WORKING_AREA_OFFSET + 0x0004)
#define KC_START_DATE_ADDR          (KC_WORKING_AREA_OFFSET + 0x0005)
#define KC_CURRENT_IDX_ADDR         (KC_WORKING_AREA_OFFSET + 0x0008)
#define KC_TOTAL_VALUE_ADDR         (KC_WORKING_AREA_OFFSET + 0x0009)
#define KC_VALUE_NB_ADDR            (KC_WORKING_AREA_OFFSET + 0x000D)
#define KC_AVERAGE_TABLE_ADDR       (KC_WORKING_AREA_OFFSET + 0x0100)


/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum KC_STATE {
    KC_IDLE,
    KC_GET_CONFIG,
    KC_CONNECTING,
    KC_WAIT_INDICATOR,
    KC_SEND_PACKET,
    KC_GET_SERVER_RESPONSE
};

static KC_STATE GL_KipControlManager_CurrentState_E = KC_STATE::KC_IDLE;
static boolean GL_KipControlManagerEnabled_B = false;

unsigned char GL_pBuffer_UB[16];

KC_HANDLE_STRUCT KipControl_H;
EthernetClient KipClient_H;

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessGetConfig(void);
static void ProcessConnecting(void);
static void ProcessWaitIndicator(void);
static void ProcessSendPacket(void);
static void ProcessGetServerResponse(void);

static void TransitionToIdle(void);
static void TransitionToGetConfig(void);
static void TransitionToConnecting(void);
static void TransitionToWaitIndicator(void);
static void TransitionToSendPacket(void);
static void TransitionToGetServerResponse(void);

/* ******************************************************************************** */
/* Prototypes for Getters & Setters
/* ******************************************************************************** */
static boolean GetConfiguredFlag(void);
static boolean GetRunningFlag(void);
static unsigned char GetTolerance(void);
static unsigned char GetReferenceDataId(void);
static unsigned char GetMaxDataNb(void);
static unsigned char GetStartIdx(void);
static RTC_DATE_STRUCT GetStartDate(void);
static unsigned char GetCurrentIdx(void);
static unsigned long GetTotalValue(void);
static unsigned long GetValueNb(void);

static void SetRunningFlag(boolean Running_B);
static void SetReferenceDataId(unsigned char ReferenceDataId_UB);
static void SetMaxDataNb(unsigned char MaxDataNb_UB);
static void SetStartIdx(unsigned char StartIdx_UB);
static void SetStartDate(RTC_DATE_STRUCT StartDate_X);
static void SetCurrentIdx(unsigned char CurrentIdx_UB);
static void SetTotalValue(unsigned long TotalValue_UL);
static void SetValueNb(unsigned long ValueNb_UL);

static void IncCurrentIdx(void);
static void AppendTotalValue(unsigned int Value_UW);
static void IncValueNb(void);

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void KipControlManager_Init() {
    GL_KipControlManagerEnabled_B = false;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "KipControl Manager Initialized");
}

void KipControlManager_Enable() {
    GL_KipControlManagerEnabled_B = true;
}

void KipControlManager_Disable() {
    GL_KipControlManagerEnabled_B = false;
}

void KipControlManager_Process() {
    switch (GL_KipControlManager_CurrentState_E) {
    case KC_IDLE:
        ProcessIdle();
        break;

    case KC_GET_CONFIG:
        ProcessGetConfig();
        break;

    case KC_CONNECTING:
        ProcessConnecting();
        break;

    case KC_WAIT_INDICATOR:
        ProcessWaitIndicator();
        break;

    case KC_SEND_PACKET:
        ProcessSendPacket();
        break;

    case KC_GET_SERVER_RESPONSE:
        ProcessGetServerResponse();
        break;
    }
}

boolean KipControlManager_IsEnabled() {
    return (GL_KipControlManagerEnabled_B);
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
    if (GL_KipControlManagerEnabled_B && GetConfiguredFlag())
        TransitionToGetConfig();
}

void ProcessGetConfig(void) {
    KipControl_H.IsConfigured_B = GetConfiguredFlag();
    KipControl_H.IsRunning_B = GetRunningFlag();
    KipControl_H.Tolerance_UB = GetTolerance();
    KipControl_H.ReferenceDataId_UB = GetReferenceDataId();
    KipControl_H.MaxDataNb_UB = GetMaxDataNb();
    KipControl_H.StartIdx_UB = GetStartIdx();
    KipControl_H.StartDate_X = GetStartDate();
    KipControl_H.CurrentIdx_UB = GetCurrentIdx();
    KipControl_H.TotalValue_UL = GetTotalValue();
    KipControl_H.ValueNb_UL = GetValueNb();

    TransitionToConnecting();
}

void ProcessConnecting(void) {
    if (GL_GlobalData_X.Network_H.isConnected())
        TransitionToWaitIndicator();
}

void ProcessWaitIndicator(void) {
    if (!(GL_GlobalData_X.Indicator_H.isFifoEmpty())) {
        TransitionToSendPacket();
    }
}

void ProcessSendPacket(void) {

    signed int Weigh_SI = 0;
    String MacAddr_Str = "";
    String TimeStamp_Str = "";

    MacAddr_Str.reserve(20);
    TimeStamp_Str.reserve(80);


    Weigh_SI = GL_GlobalData_X.Indicator_H.fifoPop();
    MacAddr_Str = HexArrayToString(GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB, sizeof(GL_GlobalConfig_X.EthConfig_X.pMacAddr_UB), ":");
    TimeStamp_Str = GL_GlobalData_X.Rtc_H.getTimestamp();

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Retreive information before sending:");
    DBG_PRINT(DEBUG_SEVERITY_INFO, "- ");
    DBG_PRINTDATA(Weigh_SI);
    DBG_ENDSTR();
    DBG_PRINT(DEBUG_SEVERITY_INFO, "- ");
    DBG_PRINTDATA(MacAddr_Str);
    DBG_ENDSTR();
    DBG_PRINT(DEBUG_SEVERITY_INFO, "- ");
    DBG_PRINTDATA(TimeStamp_Str);
    DBG_ENDSTR();

    if (KipClient_H.connect("www.balthinet.be", 80)) {

        KipClient_H.print("GET /kipcontrol/import?");
        KipClient_H.print("data[0][Weight]=");
        KipClient_H.print(Weigh_SI);
        KipClient_H.print("&data[0][Name]=");
        //KipClient_H.print(MacAddr_Str);
        KipClient_H.print("serial");
        KipClient_H.print("&data[0][DateTime]=");
        KipClient_H.print(TimeStamp_Str);
        KipClient_H.println("&submitted=1&action=validate HTTP/1.1");
        KipClient_H.println("Host: www.balthinet.be");
        KipClient_H.println("Connection: close");
        KipClient_H.println();
    }
    else {
        DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Cannot connect to portail..");
    }

    TransitionToGetServerResponse();
}

void ProcessGetServerResponse(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Print out Server respone :");

    DBG_PRINT(DEBUG_SEVERITY_INFO, "");
    while (KipClient_H.available())
        DBG_PRINTDATA((char)(KipClient_H.read()));
    DBG_ENDSTR();

    DBG_ENDSTR();
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "End of Server response");

    TransitionToWaitIndicator();
}


void TransitionToIdle(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_IDLE;
}

void TransitionToGetConfig(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET CONFIG");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_GET_CONFIG;
}

void TransitionToConnecting(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To CONNECTING");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_CONNECTING;
}

void TransitionToWaitIndicator(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WAIT INDICATOR");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_WAIT_INDICATOR;
}

void TransitionToSendPacket(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To SEND PACKET");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_SEND_PACKET;
}

void TransitionToGetServerResponse(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To GET SERVER RESPONSE");
    GL_KipControlManager_CurrentState_E = KC_STATE::KC_GET_SERVER_RESPONSE;
}


/* ******************************************************************************** */
/* Getters & Setters
/* ******************************************************************************** */

boolean GetConfiguredFlag(void) {
    if (GL_GlobalData_X.Eeprom_H.read(KC_GLOBAL_DATA_ADDR, GL_pBuffer_UB, 1) == 1)
        return ((GL_pBuffer_UB[0] && 0x01) == 0x01);
    else
        return false;
}

boolean GetRunningFlag(void) {
    if (GL_GlobalData_X.Eeprom_H.read(KC_GLOBAL_DATA_ADDR, GL_pBuffer_UB, 1) == 1)
        return ((GL_pBuffer_UB[0] && 0x02) == 0x02);
    else
        return false;
}

unsigned char GetTolerance(void) {
    if (GL_GlobalData_X.Eeprom_H.read(KC_TOLERANCE_ADDR, GL_pBuffer_UB, 1) == 1)
        return GL_pBuffer_UB[0];
    else
        return 0;
}

unsigned char GetReferenceDataId(void) {
    if (GL_GlobalData_X.Eeprom_H.read(KC_REFERENCE_DATA_ID_ADDR, GL_pBuffer_UB, 1) == 1)
        return GL_pBuffer_UB[0];
    else
        return 0;
}

unsigned char GetMaxDataNb(void) {
    if (GL_GlobalData_X.Eeprom_H.read(KC_MAX_DATA_NB_ADDR, GL_pBuffer_UB, 1) == 1)
        return GL_pBuffer_UB[0];
    else
        return 0;
}

unsigned char GetStartIdx(void) {
    if (GL_GlobalData_X.Eeprom_H.read(KC_START_IDX_ADDR, GL_pBuffer_UB, 1) == 1)
        return GL_pBuffer_UB[0];
    else
        return 0;
}

RTC_DATE_STRUCT GetStartDate(void) {
    RTC_DATE_STRUCT Date_X;
    
    if (GL_GlobalData_X.Eeprom_H.read(KC_START_DATE_ADDR, GL_pBuffer_UB, 3) == 3) {
        Date_X.Day_UB = GL_pBuffer_UB[0];
        Date_X.Month_UB = GL_pBuffer_UB[1];
        Date_X.Year_UB = GL_pBuffer_UB[2];
    }
    else {
        Date_X.Day_UB = 0;
        Date_X.Month_UB = 0;
        Date_X.Year_UB = 0;
    }

    return Date_X;
}

unsigned char GetCurrentIdx(void) {
    if (GL_GlobalData_X.Eeprom_H.read(KC_CURRENT_IDX_ADDR, GL_pBuffer_UB, 1) == 1)
        return GL_pBuffer_UB[0];
    else
        return 0;
}

unsigned long GetTotalValue(void) {
    if (GL_GlobalData_X.Eeprom_H.read(KC_TOTAL_VALUE_ADDR, GL_pBuffer_UB, 4) == 4)
        return ((GL_pBuffer_UB[3] << 24) + (GL_pBuffer_UB[2] << 16) + (GL_pBuffer_UB[1] << 8) + GL_pBuffer_UB[0]);
    else
        return 0;
}

unsigned long GetValueNb(void) {
    if (GL_GlobalData_X.Eeprom_H.read(KC_VALUE_NB_ADDR, GL_pBuffer_UB, 4) == 4)
        return ((GL_pBuffer_UB[3] << 24) + (GL_pBuffer_UB[2] << 16) + (GL_pBuffer_UB[1] << 8) + GL_pBuffer_UB[0]);
    else
        return 0;
}


void SetRunningFlag(boolean Running_B) {
    if (GL_GlobalData_X.Eeprom_H.read(KC_GLOBAL_DATA_ADDR, GL_pBuffer_UB, 1) == 1) {
        if (Running_B)
            GL_pBuffer_UB[0] |= 0x02;
        else
            GL_pBuffer_UB[0] &= ~0x02;
        GL_GlobalData_X.Eeprom_H.write(KC_GLOBAL_DATA_ADDR, GL_pBuffer_UB, 1);
    }
}

void SetReferenceDataId(unsigned char ReferenceDataId_UB) {
    GL_GlobalData_X.Eeprom_H.write(KC_REFERENCE_DATA_ID_ADDR, &ReferenceDataId_UB, 1);
}

void SetMaxDataNb(unsigned char MaxDataNb_UB) {
    GL_GlobalData_X.Eeprom_H.write(KC_MAX_DATA_NB_ADDR, &MaxDataNb_UB, 1);
}

void SetStartIdx(unsigned char StartIdx_UB) {
    GL_GlobalData_X.Eeprom_H.write(KC_START_IDX_ADDR, &StartIdx_UB, 1);
}

void SetStartDate(RTC_DATE_STRUCT StartDate_X) {
    GL_pBuffer_UB[0] = StartDate_X.Day_UB;
    GL_pBuffer_UB[1] = StartDate_X.Month_UB;
    GL_pBuffer_UB[2] = StartDate_X.Year_UB;
    GL_GlobalData_X.Eeprom_H.write(KC_START_DATE_ADDR, GL_pBuffer_UB, 3);
}

void SetCurrentIdx(unsigned char CurrentIdx_UB) {
    GL_GlobalData_X.Eeprom_H.write(KC_CURRENT_IDX_ADDR, &CurrentIdx_UB, 1);
}

void SetTotalValue(unsigned long TotalValue_UL) {
    GL_pBuffer_UB[0] = (unsigned char)(TotalValue_UL % 256);
    GL_pBuffer_UB[1] = (unsigned char)((TotalValue_UL >> 8) % 256);
    GL_pBuffer_UB[2] = (unsigned char)((TotalValue_UL >> 16) % 256);
    GL_pBuffer_UB[3] = (unsigned char)((TotalValue_UL >> 24) % 256);
    GL_GlobalData_X.Eeprom_H.write(KC_TOTAL_VALUE_ADDR, GL_pBuffer_UB, 4);
}

void SetValueNb(unsigned long ValueNb_UL) {
    GL_pBuffer_UB[0] = (unsigned char)(ValueNb_UL % 256);
    GL_pBuffer_UB[1] = (unsigned char)((ValueNb_UL >> 8) % 256);
    GL_pBuffer_UB[2] = (unsigned char)((ValueNb_UL >> 16) % 256);
    GL_pBuffer_UB[3] = (unsigned char)((ValueNb_UL >> 24) % 256);
    GL_GlobalData_X.Eeprom_H.write(KC_VALUE_NB_ADDR, GL_pBuffer_UB, 4);
}


void IncCurrentIdx(void) {
    SetCurrentIdx(GetCurrentIdx() + 1);
}

void AppendTotalValue(unsigned int Value_UW) {
    SetTotalValue(GetTotalValue() + Value_UW);
}

void IncValueNb(void) {
    SetValueNb(GetValueNb() + 1);
}

