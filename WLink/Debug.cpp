/* ******************************************************************************** */
/*                                                                                  */
/* Debug.cpp																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the functions for debugging management                            */
/*                                                                                  */
/* History :  	31/05/2015  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
HardwareSerial * GL_pSerial_H;

static const String pSeverityLut_str[] = {"[-] INFO", "[#] WARNING", "[!] ERROR"};

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void Debug_Init(HardwareSerial * pSerial_H) {
	GL_pSerial_H = pSerial_H;
	GL_pSerial_H->begin(DEBUG_DEFAULT_BAUDRATE);
}

void Debug_Init(HardwareSerial * pSerial_H, unsigned long Baudrate_UL) {
	GL_pSerial_H = pSerial_H;
	GL_pSerial_H->begin(Baudrate_UL);
}

HardwareSerial * Debug_GetHandle(void) {
	return GL_pSerial_H;
}

HardwareSerial * Debug_Print(DEBUG_SEVERITY_ENUM Severity_E, const String pModuleName_cstr, unsigned long LineNb_UL, const String pFunctionName_cstr) {
	GL_pSerial_H->print(pSeverityLut_str[Severity_E]);
	GL_pSerial_H->print(" > ");
	GL_pSerial_H->print(pModuleName_cstr);
	GL_pSerial_H->print(":");
	GL_pSerial_H->print(pFunctionName_cstr);
	GL_pSerial_H->print(":");
	GL_pSerial_H->print(LineNb_UL);
	GL_pSerial_H->print(" > ");
	return GL_pSerial_H;
}
