/* ******************************************************************************** */
/*                                                                                  */
/* LcdDisplay.h			                                                            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for LcdDisplay.cpp												*/
/*                                                                                  */
/* History :  	13/06/2016  (RW)	Creation of this file							*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __LCD_DISPLAY_H__
#define __LCD_DISPLAY_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <Arduino.h>
#include <LiquidCrystal.h>

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

#define LCD_DISPLAY_COLUMN_NUMBER	20
#define LCD_DISPLAY_LINE_NUMBER		2

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
	boolean IsInitialized_B;
	boolean ExternalWriteEnabled_B;
	unsigned long ExternalWriteInitLineIdx_UL;
	unsigned long ExternalWriteInitColIdx_UL;
	unsigned long ExternalWriteLineIdx_UL;
	unsigned long ExternalWriteColIdx_UL;
	String ExternalWriteData_Str;
} LCD_DISPLAY_PARAM;

typedef enum {
	LCD_DISPLAY_LINE1 = 0,
	LCD_DISPLAY_LINE2,
	LCD_DISPLAY_ALL_LINE
} LCD_DISPLAY_LINE_ENUM;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class LcdDisplay {
public:
	// Constructor
	LcdDisplay();

	// Functions
	void init(LiquidCrystal * pLcd_H, unsigned char PinBacklight_UB);
	boolean isInitialized(void);

    void createChar(unsigned char CharNb_UB, unsigned char * pChar_UB);

	void clearDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E = LCD_DISPLAY_ALL_LINE);
	void writeDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, String TextStr_Str);
    void writeDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, unsigned char * pTextStr_UB, unsigned long ArraySize_UL);
    void writeDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, unsigned long ColIdx_UL, String TextStr_Str);
    void writeDisplay(LCD_DISPLAY_LINE_ENUM LineIndex_E, unsigned long ColIdx_UL, unsigned char * pTextStr_UB, unsigned long ArraySize_UL);

	void appendDisplay(String TextStr_Str);
	void appendDisplay(unsigned char * pTextStr_UB, unsigned long ArraySize_UL);

	void backspaceDisplay(unsigned long BackspaceNb_UL);
	void readDisplayShadowContent(LCD_DISPLAY_LINE_ENUM LineIndex_E, unsigned char * pTextStr_UB, unsigned long * pArraySize_UL);

	void setBacklight(unsigned char Value_UB);
    void enableCursor(unsigned long LineIdx_UL, unsigned long ColIdx_UL);
    void disableCursor(void);

	void enableExternalWrite(unsigned long LineIdx_UL, unsigned long ColIdx_UL);
	void disableExternalWrite(void);

	boolean isExternalWriteEnabled(void);
	String getExternalWriteData(void);

	LCD_DISPLAY_PARAM GL_LcdDisplayParam_X;
};

#endif // __LCD_DISPLAY_H__
