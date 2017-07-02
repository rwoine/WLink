/* ******************************************************************************** */
/*                                                                                  */
/* SerialManager.cpp																*/
/*                                                                                  */
/* Description :                                                                    */
/*		Implements the specific functions for the Serial communication.				*/
/*                                                                                  */
/* History :  	04/06/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"SerialManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "SerialHandler.h"
#include "SerialManager.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum SERIAL_MANAGER_STATE {
    SERIAL_MANAGER_IDLE,
    SERIAL_MANAGER_PROCESS_TUNNEL
};

static SERIAL_MANAGER_STATE GL_SerialManager_CurrentState_E = SERIAL_MANAGER_STATE::SERIAL_MANAGER_IDLE;

typedef struct {
    boolean IsTunnelEnabled_B;
    HardwareSerial * pSerialA_H;
    HardwareSerial * pSerialB_H;
} SERIAL_MANAGER_PARAM;

static SERIAL_MANAGER_PARAM GL_SerialManagerParam_X;

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessTunnel(void);

static void TransitionToIdle(void);
static void TransitionToProcessTunnel(void);

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void SerialManager_Init() {
    GL_SerialManager_CurrentState_E = SERIAL_MANAGER_STATE::SERIAL_MANAGER_IDLE;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Flat Panel Manager Initialized");
}



void SerialManager_SetTunnel(unsigned long ComPortA_UL, unsigned long ComPortB_UL) {

    // Assign COM port
    GL_SerialManagerParam_X.pSerialA_H = GetSerialHandle(ComPortA_UL);
    GL_SerialManagerParam_X.pSerialB_H = GetSerialHandle(ComPortB_UL);

    // Set Flag
    GL_SerialManagerParam_X.IsTunnelEnabled_B = true;
}

void SerialManager_StopTunnel(void) {
    GL_SerialManagerParam_X.IsTunnelEnabled_B = false;
}


void SerialManager_Process() {

    /* State Machine */
    switch (GL_SerialManager_CurrentState_E) {
    case SERIAL_MANAGER_IDLE:
        ProcessIdle();
        break;

    case SERIAL_MANAGER_PROCESS_TUNNEL:
        ProcessTunnel();
        break;

    }
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void) {
    if (GL_SerialManagerParam_X.IsTunnelEnabled_B)
        TransitionToProcessTunnel();
}

static void ProcessTunnel(void) {
    if (GL_SerialManagerParam_X.IsTunnelEnabled_B) {

        // Forward A to B
        while (GL_SerialManagerParam_X.pSerialA_H->available()) {
            //DBG_PRINTDATA("A = ");
            //DBG_PRINTDATA((char)GL_SerialManagerParam_X.pSerialA_H->read());
            GL_SerialManagerParam_X.pSerialB_H->write(GL_SerialManagerParam_X.pSerialA_H->read());
        }

        // Forward B to A
        while (GL_SerialManagerParam_X.pSerialB_H->available()) {
            //DBG_PRINTDATA("B = ");
            //DBG_PRINTDATA((char)GL_SerialManagerParam_X.pSerialB_H->read());
            GL_SerialManagerParam_X.pSerialA_H->write(GL_SerialManagerParam_X.pSerialB_H->read());
        }

    }
    else {
        // Flush A
        GL_SerialManagerParam_X.pSerialA_H->flush();
        while (GL_SerialManagerParam_X.pSerialA_H->available())
            GL_SerialManagerParam_X.pSerialA_H->read();

        // Flush B
        GL_SerialManagerParam_X.pSerialB_H->flush();
        while (GL_SerialManagerParam_X.pSerialB_H->available())
            GL_SerialManagerParam_X.pSerialB_H->read();

        // Go to IDLE
        TransitionToIdle();
    }
}

void TransitionToIdle() {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_SerialManager_CurrentState_E = SERIAL_MANAGER_STATE::SERIAL_MANAGER_IDLE;
}


static void TransitionToProcessTunnel() {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To PROCESS TUNNEL");
    GL_SerialManager_CurrentState_E = SERIAL_MANAGER_STATE::SERIAL_MANAGER_PROCESS_TUNNEL;
}
