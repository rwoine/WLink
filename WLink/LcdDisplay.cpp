/* ******************************************************************************** */
/*                                                                                  */
/* LcdDisplay.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the LCD utilities functions										*/
/*                                                                                  */
/* History :  	13/06/2016  (RW)	Creation of this file                           */
/*				15/07/2016	(RW)	Resolve bug when writing all lines				*/
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

static unsigned long GL_pLcdLindeIndex_UL[LCD_DISPLAY_LINE_NUMBER];
static unsigned char GL_ppLcdLineShadow_UB[LCD_DISPLAY_LINE_NUMBER][LCD_DISPLAY_COLUMN_NUMBER];

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
LcdDisplay::LcdDisplay() {
	GL_LcdDisplayParam_X.IsInitialized_B = false;
}

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void EraseLineShadowContent(LCD_DISPLAY_LINE_ENUM LineIndex_E);

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void LcdDisplay::init(LiquidCrystal * pLcd_H, unsigned char PinBacklight_UB) {
	GL_pLcdDevice_H = pLcd_H;
	GL_pLcdDevice_H->begin(LCD_DISPLAY_COLUMN_NUMBER, LCD_DISPLAY_LINE_NUMBER);
	GL_PinBacklight_UB = PinBacklight_UB;
	pinMode(GL_PinBacklight_UB, OUTPUT);
	digitalWrite(GL_PinBacklight_UB, LOW);
	GL_LcdDisplayParam_X.IsInitialized_B = true;
	for (int i = 0; i < LCD_DISPLAY_LINE_NUMBER; i++)
		GL_pLcdLindeIndex_UL[i] = 0;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "LCD Display Module Initialized");
}

boolean LcdDisplay::isInitialized(void) {
	return GL_LcdDisplayParam_X.IsInitialized_B;
}


void LcdDisplay::clearDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E) {
	if (LineIndex_E == LCD_DISPLAY_ALL_LINE) {
		GL_pLcdDevice_H->clear();
		for (int i = 0; i < LCD_DISPLAY_LINE_NUMBER; i++)
			GL_pLcdLindeIndex_UL[i] = 0;
	}
	else {
		GL_pLcdLindeIndex_UL[LineIndex_E] = 0;
		GL_pLcdDevice_H->setCursor(0, LineIndex_E);
		for (int i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
			GL_pLcdDevice_H->write(' ');
	}	
}

void LcdDisplay::writeDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, String TextStr_Str) {
	unsigned long LineIdx_UL = (unsigned long)LineIndex_E;

	if (LineIndex_E == LCD_DISPLAY_ALL_LINE)
		LineIdx_UL = 0;	// Force to start at Zero if all lines selected

	// Erase Content of Shadow Line
	EraseLineShadowContent(LineIndex_E);

	// Set Cursor Properly
	GL_pLcdDevice_H->setCursor(0, LineIdx_UL);

	// Copy Content in Shadow Line
	for (int i = 0; (i < TextStr_Str.length()) && (i < LCD_DISPLAY_COLUMN_NUMBER) ; i++)
		GL_ppLcdLineShadow_UB[LineIdx_UL][i] = TextStr_Str.charAt(i);

	// Display on Device
	GL_pLcdDevice_H->print(TextStr_Str);
}

void LcdDisplay::writeDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, unsigned char * pTextStr_UB, unsigned long ArraySize_UL) {
	unsigned long Index_UL = 0;
	unsigned long LineIdx_UL = (unsigned long)LineIndex_E;

	if (LineIndex_E == LCD_DISPLAY_ALL_LINE)
		LineIdx_UL = 0;	// Force to start at Zero if all lines selected

	// Erase Content of Shadow Linde
	EraseLineShadowContent(LineIndex_E);

	// Set Cursor Properly
	GL_pLcdDevice_H->setCursor(0, LineIdx_UL);

	while (Index_UL < ArraySize_UL) {
		if ((Index_UL > 0) && ((Index_UL%LCD_DISPLAY_COLUMN_NUMBER) == 0)) {
			if (LineIndex_E == (LCD_DISPLAY_LINE_NUMBER-1))		// Reach End of LCD
				break;
			else
				GL_pLcdDevice_H->setCursor(0, ++LineIdx_UL);	// Change Line
		}

		// Copy Content in Shadow Line
		GL_ppLcdLineShadow_UB[LineIdx_UL][Index_UL - (LCD_DISPLAY_COLUMN_NUMBER*LineIdx_UL)] = pTextStr_UB[Index_UL];

		// Display on Device
		GL_pLcdDevice_H->write(pTextStr_UB[Index_UL]);
		Index_UL++;
	}
}

void LcdDisplay::appendDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, String TextStr_Str) {
	//unsigned long LineIdx_UL = (LineIndex_E == LCD_DISPLAY_LINE2) ? 1 : 0;

	//// Erase Content of Shadow Linde
	//EraseLineShadowContent(LineIndex_E);

	//// Set Cursor Properly
	//GL_pLcdDevice_H->setCursor(0, LineIdx_UL);

	//// Copy Content in Shadow Line
	//for (int i = 0; (i < TextStr_Str.length()) && (i < LCD_DISPLAY_COLUMN_NUMBER); i++)
	//	GL_ppLcdLineShadow_UB[LineIdx_UL][i] = TextStr_Str.charAt(i);

	//// Display on Device
	//GL_pLcdDevice_H->print(TextStr_Str);
}

void LcdDisplay::appendDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, unsigned char * pTextStr_UB, unsigned long ArraySize_UL) {
	//unsigned long Index_UL = 0;
	//unsigned long LineIdx_UL = (LineIndex_E == LCD_DISPLAY_LINE2) ? 1 : 0;

	//// Erase Content of Shadow Linde
	//EraseLineShadowContent(LineIndex_E);

	//// Set Cursor Properly
	//GL_pLcdDevice_H->setCursor(0, LineIdx_UL);

	//while (Index_UL < ArraySize_UL) {
	//	if (Index_UL == LCD_DISPLAY_COLUMN_NUMBER) {
	//		if (LineIndex_E == LCD_DISPLAY_LINE2)
	//			break;
	//		else
	//			GL_pLcdDevice_H->setCursor(0, LineIdx_UL++);
	//	}

	//	if (Index_UL == 2 * LCD_DISPLAY_COLUMN_NUMBER)
	//		break;

	//	// Copy Content in Shadow Line
	//	GL_ppLcdLineShadow_UB[LineIdx_UL][Index_UL - (LCD_DISPLAY_COLUMN_NUMBER*LineIdx_UL)] = pTextStr_UB[Index_UL];

	//	// Display on Device
	//	GL_pLcdDevice_H->write(pTextStr_UB[Index_UL]);
	//	Index_UL++;
	//}
}

void LcdDisplay::readDisplayShadowContent(LCD_DISPLAY_LINE_ENUM LineIndex_E, unsigned char * pTextStr_UB, unsigned long * pArraySize_UL) {
	int i = 0;
	unsigned long LineIdx_UL = (unsigned long)LineIndex_E;

	*pArraySize_UL = 0;

	if (LineIndex_E == LCD_DISPLAY_ALL_LINE) {
		for (LineIdx_UL = 0; LineIdx_UL < LCD_DISPLAY_LINE_NUMBER; LineIdx_UL++) {
			for (i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++) {
				pTextStr_UB[i + (LCD_DISPLAY_COLUMN_NUMBER*LineIdx_UL)] = GL_ppLcdLineShadow_UB[LineIdx_UL][i];
				*pArraySize_UL++;
			}
		}
	}
	else {
		for (i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++) {
			pTextStr_UB[i] = GL_ppLcdLineShadow_UB[LineIdx_UL][i];
			*pArraySize_UL++;
		}
	}
}

void LcdDisplay::setBacklight(unsigned char Value_UB) {
	analogWrite(GL_PinBacklight_UB, Value_UB);
}

void LcdDisplay::enableExternalWrite(unsigned long LineNb_UL, unsigned long ColNb_UL) {
	// TODO : Manage Coordinates
	GL_LcdDisplayParam_X.ExternalWriteEnabled_B = true;
}

void LcdDisplay::disableExternalWrite(void) {
	GL_LcdDisplayParam_X.ExternalWriteEnabled_B = false;
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void EraseLineShadowContent(LCD_DISPLAY_LINE_ENUM LineIndex_E) {
	int i = 0;
	unsigned long LineIdx_UL = (unsigned long)LineIndex_E;

	if (LineIndex_E == LCD_DISPLAY_ALL_LINE) {
		for (LineIdx_UL = 0; LineIdx_UL < LCD_DISPLAY_LINE_NUMBER; LineIdx_UL++) {
			for (i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
				GL_ppLcdLineShadow_UB[LineIdx_UL][i] = 0x00;
		}
	}
	else {
		for (i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
			GL_ppLcdLineShadow_UB[LineIdx_UL][i] = 0x00;
	}
}
