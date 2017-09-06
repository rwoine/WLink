/* ******************************************************************************** */
/*                                                                                  */
/* WMenuItemFunction.cpp												            */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the specific functions for the W-Link Menu Items.       			*/
/*                                                                                  */
/* History :  	03/09/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"WMenuItemFunction"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WMenuItemFunction.h"
#include "WMenuManager.h"
#include "WMenuItemText.h"
#include "WConfigManager.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Extenral Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Welcome Screen
/* ******************************************************************************** */

void WMenuItem_WelcomeScreen_Transition(void * Hander_H) {
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 0, "> ");
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 2, GL_GlobalData_X.RevisionId_Str);
}

/* ******************************************************************************** */
/* Idle
/* ******************************************************************************** */
void WMenuItem_Idle_Process(void * Handler_H) {
	int SignalStrength_SI = 0;
	
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 19, (byte)0);
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 1, GL_GlobalData_X.Rtc_H.getDateTimeString());

	// Display Network Signal Strength if any
	if (GL_GlobalConfig_X.GsmConfig_X.isEnabled_B) {
		SignalStrength_SI = FonaModuleManager_GetCurrentSignalStrength();
		if ((SignalStrength_SI >= -111) && (SignalStrength_SI <= -97)) {
			GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 18, (byte)1);
		}
		else if ((SignalStrength_SI > -97) && (SignalStrength_SI <= -82)) {
			GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 18, (byte)2);
		}
		else if ((SignalStrength_SI > -82) && (SignalStrength_SI <= -67)) {
			GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 18, (byte)3);
		}
		else if ((SignalStrength_SI > -67) && (SignalStrength_SI <= -52)) {
			GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 18, (byte)4);
		}
		else {
			GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 18, (byte)5);
		} 
	}
	else {
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 18, " ");
	}
}

/* ******************************************************************************** */
/* Language Selection
/* ******************************************************************************** */
void WMenuItem_LanguageSelect_Process(void * Handler_H) {
	if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B) {
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B = false;

		switch (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB) {

		case '1':
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[0] = (unsigned char)WLINK_LANGUAGE_EN;
			GL_GlobalData_X.Lcd_H.enableCursor(1, 0);
			break;

		case '2':
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[0] = (unsigned char)WLINK_LANGUAGE_FR;
			GL_GlobalData_X.Lcd_H.enableCursor(1, 7);
			break;

		case '3':
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[0] = (unsigned char)WLINK_LANGUAGE_NL;
			GL_GlobalData_X.Lcd_H.enableCursor(1, 14);
			break;

		default:
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[0] = (unsigned char)WLINK_LANGUAGE_EN;
			GL_GlobalData_X.Lcd_H.enableCursor(1, 0);
			break;

		}
	}
}

/* ******************************************************************************** */
/* Date Set
/* ******************************************************************************** */
void WMenuItem_DateSet_Process(void * Handler_H) {
	unsigned long ColIdx_UL = 0;

	// Manage cursor
	switch (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL) {
	case 0:   ColIdx_UL = 2; break;  // -> D 10
	case 1:   ColIdx_UL = 3; break;  // -> D 1
	case 2:   ColIdx_UL = 5; break;  // -> M 10
	case 3:   ColIdx_UL = 6; break;  // -> M 1
	case 4:   ColIdx_UL = 8; break;  // -> Y 10
	case 5:   ColIdx_UL = 9; break;  // -> Y 1
	}

	GL_GlobalData_X.Lcd_H.enableCursor(LCD_DISPLAY_LINE2, ColIdx_UL);

	if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B) {
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B = false;

		// Validate Input
		switch (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL) {
		case 0:     if (atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)) > 3)    (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB) = '3';																		break;
		case 1:     if ((((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[0] == 3) && (atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)) > 1))  (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB) = '1';		break;
		case 2:     if (atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)) > 1)    (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB) = '1';																		break;
		case 3:     if ((((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[2] == 1) && (atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)) > 2))  (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB) = '2';		break;
		case 4:     if (atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)) > 3)    (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB) = '3';																		break;
		case 5:     break;
		}

		// Write on LCD and in Buffer
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, ColIdx_UL, (unsigned char *)(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)), 1);
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL++] = atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB));

		// Rollback
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 6)
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;

	}
}

/* ******************************************************************************** */
/* Time Set
/* ******************************************************************************** */
void WMenuItem_TimeSet_Process(void * Handler_H) {
	unsigned long ColIdx_UL = 0;

	// Manage cursor
	switch (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL) {
	case 0:   ColIdx_UL = 2; break;  // -> h 10
	case 1:   ColIdx_UL = 3; break;  // -> h 1
	case 2:   ColIdx_UL = 5; break;  // -> m 10
	case 3:   ColIdx_UL = 6; break;  // -> m 1
	case 4:   ColIdx_UL = 8; break;  // -> s 10
	case 5:   ColIdx_UL = 9; break;  // -> s 1
	}

	GL_GlobalData_X.Lcd_H.enableCursor(LCD_DISPLAY_LINE2, ColIdx_UL);

	if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B) {
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B = false;

		// Validate Input
		switch (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL) {
		case 0:     if (atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)) > 2)    (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB) = '2';																		break;
		case 1:     if ((((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[0] == 2) && (atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)) > 3))  (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB) = '3';		break;
		case 2:     if (atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)) > 5)    (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB) = '5';																		break;
		case 3:     break;
		case 4:     if (atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)) > 5)    (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB) = '5';																		break;
		case 5:     break;
		}

		// Write on LCD and in Buffer
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, ColIdx_UL, (unsigned char *)(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)), 1);
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL++] = atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB));

		// Rollback
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 6)
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;

	}
}

