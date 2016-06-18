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


void LcdDisplay::clearDisplay(void) {
	GL_pLcdDevice_H->clear();
}

void LcdDisplay::writeDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, String TextStr_Str) {
	if (LineIndex_E == LCD_DISPLAY_LINE2)
		GL_pLcdDevice_H->setCursor(0, 1); // start at line 2
	else
		GL_pLcdDevice_H->setCursor(0, 0); // start at line 1

	GL_pLcdDevice_H->print(TextStr_Str);
}

void LcdDisplay::writeDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, unsigned char * pTextStr_UB, unsigned long ArraySize_UL) {
	unsigned long Index_UL = 0;

	if (LineIndex_E == LCD_DISPLAY_LINE2)
		GL_pLcdDevice_H->setCursor(0, 1); // start at line 2
	else
		GL_pLcdDevice_H->setCursor(0, 0); // start at line 1

	while (Index_UL < ArraySize_UL) {
		if (Index_UL == 20) {
			if (LineIndex_E == LCD_DISPLAY_LINE2)
				break;
			else
				GL_pLcdDevice_H->setCursor(0, 1);
		}

		if (Index_UL == 40)
			break;

		GL_pLcdDevice_H->write(pTextStr_UB[Index_UL]);
		Index_UL++;
	}
}

void LcdDisplay::setBacklight(unsigned char Value_UB) {
	analogWrite(GL_PinBacklight_UB, Value_UB);
}
