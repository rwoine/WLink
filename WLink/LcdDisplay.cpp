/* ******************************************************************************** */
/*                                                                                  */
/* LcdDisplay.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the LCD utilities functions										*/
/*                                                                                  */
/* History :  	13/06/2016  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"LcdDisplay"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "LcdDisplay.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static LiquidCrystal * GL_pLcdDevice_H;
static unsigned char GL_PinBacklight_UB;

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
LcdDisplay::LcdDisplay() {
	GL_LcdDisplayParam_X.IsInitialized_B = false;
}

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void LcdDisplay::init(LiquidCrystal * pLcd_H, unsigned char PinBacklight_UB) {
	GL_pLcdDevice_H = pLcd_H;
	GL_pLcdDevice_H->begin(20, 2);
	GL_PinBacklight_UB = PinBacklight_UB;
	pinMode(GL_PinBacklight_UB, OUTPUT);
	digitalWrite(GL_PinBacklight_UB, LOW);
	GL_LcdDisplayParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "LCD Display Module Initialized");
}

boolean LcdDisplay::isInitialized(void) {
	return GL_LcdDisplayParam_X.IsInitialized_B;
}

