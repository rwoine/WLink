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

void WMenuItem_WelcomeScreen_Transition(void * Hander_H) {
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 0, "> ");
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 2, GL_GlobalData_X.RevisionId_Str);
}

void WMenuItem_Idle_Process(void * Handler_H) {
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 19, (byte)0, 1);
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 1, GL_GlobalData_X.Rtc_H.getDateTimeString());
}