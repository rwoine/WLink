/* ******************************************************************************** */
/*                                                                                  */
/* LcdDisplay.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the LCD utilities functions										*/
/*                                                                                  */
/* History :  	13/06/2016  (RW)	Creation of this file                           */
/*				15/07/2016	(RW)	Resolve bug when writing all lines				*/
/*				01/01/2016	(RW)	Fix LCD write and read functions				*/
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

static unsigned char GL_ppLcdLineShadow_UB[LCD_DISPLAY_LINE_NUMBER][LCD_DISPLAY_COLUMN_NUMBER];

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
LcdDisplay::LcdDisplay() {
	GL_LcdDisplayParam_X.IsInitialized_B = false;
	GL_LcdDisplayParam_X.ExternalWriteEnabled_B = false;
	GL_LcdDisplayParam_X.ExternalWriteInitLineIdx_UL = 0;
	GL_LcdDisplayParam_X.ExternalWriteInitColIdx_UL = 0;
	GL_LcdDisplayParam_X.ExternalWriteLineIdx_UL = 0;
	GL_LcdDisplayParam_X.ExternalWriteColIdx_UL = 0;
}

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void EraseLineShadowContent(LCD_DISPLAY_LINE_ENUM LineIndex_E);
static void PrintLineShadowContent(LCD_DISPLAY_LINE_ENUM LineIndex_E);

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
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "LCD Display Module Initialized");
}

boolean LcdDisplay::isInitialized(void) {
	return GL_LcdDisplayParam_X.IsInitialized_B;
}


void LcdDisplay::clearDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E) {
	if (LineIndex_E == LCD_DISPLAY_ALL_LINE) {
		GL_pLcdDevice_H->clear();
	}
	else {
		GL_pLcdDevice_H->setCursor(0, LineIndex_E);
		for (int i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
			GL_pLcdDevice_H->write(' ');
	}	

	// Erase Shadow Content
	EraseLineShadowContent(LineIndex_E);
}

void LcdDisplay::writeDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, String TextStr_Str) {
	unsigned char pBuffer_UB[LCD_DISPLAY_COLUMN_NUMBER * LCD_DISPLAY_LINE_NUMBER + 1];
	unsigned long BufferSize_UL = (TextStr_Str.length() > (LCD_DISPLAY_COLUMN_NUMBER * LCD_DISPLAY_LINE_NUMBER)) ? (LCD_DISPLAY_COLUMN_NUMBER * LCD_DISPLAY_LINE_NUMBER) : TextStr_Str.length();

	// Convert String to Array of Char 
	TextStr_Str.toCharArray((char *)pBuffer_UB, BufferSize_UL+1);

	// Call inner function
	writeDisplay(LineIndex_E, pBuffer_UB, BufferSize_UL);
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
		GL_ppLcdLineShadow_UB[LineIdx_UL][Index_UL%LCD_DISPLAY_COLUMN_NUMBER] = pTextStr_UB[Index_UL];

		// Display on Device
		GL_pLcdDevice_H->write(pTextStr_UB[Index_UL]);
		Index_UL++;
	}
}

void LcdDisplay::appendDisplay(String TextStr_Str) {
	unsigned char pBuffer_UB[LCD_DISPLAY_COLUMN_NUMBER * LCD_DISPLAY_LINE_NUMBER + 1];
	unsigned long BufferSize_UL = (TextStr_Str.length() > (LCD_DISPLAY_COLUMN_NUMBER * LCD_DISPLAY_LINE_NUMBER)) ? (LCD_DISPLAY_COLUMN_NUMBER * LCD_DISPLAY_LINE_NUMBER) : TextStr_Str.length();

	// Convert String to Array of Char 
	TextStr_Str.toCharArray((char *)pBuffer_UB, BufferSize_UL + 1);

	// Call inner function
	appendDisplay(pBuffer_UB, BufferSize_UL);
}

void LcdDisplay::appendDisplay(unsigned char * pTextStr_UB, unsigned long ArraySize_UL) {

	// Copy the whole buffer or until the end of the line
	for (int i = 0; (i < ArraySize_UL) && (GL_LcdDisplayParam_X.ExternalWriteColIdx_UL < LCD_DISPLAY_COLUMN_NUMBER); i++) {
		// Copy Content in Shadow Line
		GL_ppLcdLineShadow_UB[GL_LcdDisplayParam_X.ExternalWriteLineIdx_UL][GL_LcdDisplayParam_X.ExternalWriteColIdx_UL++] = pTextStr_UB[i];

		// Display on Device
		GL_pLcdDevice_H->write(pTextStr_UB[i]);
	}

}

void LcdDisplay::backspaceDisplay(unsigned long BackspaceNb_UL) {

	for (int i = 0; (i < BackspaceNb_UL) && (GL_LcdDisplayParam_X.ExternalWriteColIdx_UL > GL_LcdDisplayParam_X.ExternalWriteInitColIdx_UL); i++) {

		// Erase in Shadow Line
		GL_ppLcdLineShadow_UB[GL_LcdDisplayParam_X.ExternalWriteLineIdx_UL][--GL_LcdDisplayParam_X.ExternalWriteColIdx_UL] = 0x20;

		// Erase on Device
		GL_pLcdDevice_H->setCursor(GL_LcdDisplayParam_X.ExternalWriteColIdx_UL, GL_LcdDisplayParam_X.ExternalWriteLineIdx_UL);
		GL_pLcdDevice_H->write(' ');
		GL_pLcdDevice_H->setCursor(GL_LcdDisplayParam_X.ExternalWriteColIdx_UL, GL_LcdDisplayParam_X.ExternalWriteLineIdx_UL);
	}

}

void LcdDisplay::readDisplayShadowContent(LCD_DISPLAY_LINE_ENUM LineIndex_E, unsigned char * pTextStr_UB, unsigned long * pArraySize_UL) {
	int i = 0;
	unsigned long Offset_UL = 0;
	unsigned long LineIdx_UL = (unsigned long)LineIndex_E;

	*pArraySize_UL = 0;

	if (LineIndex_E == LCD_DISPLAY_ALL_LINE) {
		for (LineIdx_UL = 0; LineIdx_UL < LCD_DISPLAY_LINE_NUMBER; LineIdx_UL++) {
			// Copy content of one line
			for (i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++) {
//				if (GL_ppLcdLineShadow_UB[LineIdx_UL][i] == 0x00) {
//					break;
//				}
//				else {
					pTextStr_UB[i + (LCD_DISPLAY_COLUMN_NUMBER*LineIdx_UL) + Offset_UL] = GL_ppLcdLineShadow_UB[LineIdx_UL][i];
					(*pArraySize_UL)++;
//				}
			}
			// Add Carriage Return + Line Feed between two lines
			if (LineIdx_UL < LCD_DISPLAY_LINE_NUMBER - 1) {
				pTextStr_UB[i + (LCD_DISPLAY_COLUMN_NUMBER*LineIdx_UL) + Offset_UL++] = 0x0D;
				pTextStr_UB[i + (LCD_DISPLAY_COLUMN_NUMBER*LineIdx_UL) + Offset_UL++] = 0x0A;
				(*pArraySize_UL) += 2;
			}
		}
	}
	else {
		for (i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++) {
//			if (GL_ppLcdLineShadow_UB[LineIdx_UL][i] == 0x00) {
//				break;
//			}
//			else {
				pTextStr_UB[i] = GL_ppLcdLineShadow_UB[LineIdx_UL][i];
				(*pArraySize_UL)++;
//			}
		}
	}

}

void LcdDisplay::setBacklight(unsigned char Value_UB) {
	analogWrite(GL_PinBacklight_UB, Value_UB);
}

void LcdDisplay::enableExternalWrite(unsigned long LineIdx_UL, unsigned long ColIdx_UL) {
	// Set Cursor Properly and Display it
	GL_pLcdDevice_H->setCursor(ColIdx_UL, LineIdx_UL);
	GL_pLcdDevice_H->cursor();

	// Save coordinates value
	GL_LcdDisplayParam_X.ExternalWriteInitLineIdx_UL = LineIdx_UL;
	GL_LcdDisplayParam_X.ExternalWriteInitColIdx_UL = ColIdx_UL;
	GL_LcdDisplayParam_X.ExternalWriteLineIdx_UL = LineIdx_UL;
	GL_LcdDisplayParam_X.ExternalWriteColIdx_UL = ColIdx_UL;

	// Set boolean flag
	GL_LcdDisplayParam_X.ExternalWriteEnabled_B = true;
}

void LcdDisplay::disableExternalWrite(void) {
	// Reset boolean flag
	GL_LcdDisplayParam_X.ExternalWriteEnabled_B = false;

	// Hide cursor
	GL_pLcdDevice_H->noCursor();
}

boolean LcdDisplay::isExternalWriteEnabled(void) {
	return GL_LcdDisplayParam_X.ExternalWriteEnabled_B;
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
				GL_ppLcdLineShadow_UB[LineIdx_UL][i] = 0x20;
		}
	}
	else {
		for (i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
			GL_ppLcdLineShadow_UB[LineIdx_UL][i] = 0x20;
	}
}

void PrintLineShadowContent(LCD_DISPLAY_LINE_ENUM LineIndex_E) {
	int i = 0;
	unsigned long LineIdx_UL = (unsigned long)LineIndex_E;

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Print LCD Shadow Content = ");
	if (LineIndex_E == LCD_DISPLAY_ALL_LINE) {
		for (LineIdx_UL = 0; LineIdx_UL < LCD_DISPLAY_LINE_NUMBER; LineIdx_UL++) {
			for (i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
				DBG_PRINTDATABASE(GL_ppLcdLineShadow_UB[LineIdx_UL][i], HEX);
		}
	}
	else {
		for (i = 0; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
			DBG_PRINTDATABASE(GL_ppLcdLineShadow_UB[LineIdx_UL][i], HEX);
	}
	DBG_ENDSTR();
}
