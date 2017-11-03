/* ******************************************************************************** */
/*                                                                                  */
/* KipControlMenuItemFunction.cpp												    */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the specific functions for the KipControl Menu Items.       		*/
/*                                                                                  */
/* History :  	03/09/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"KipControlMenuItemFunction"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "KipControlMenuItemFunction.h"
#include "KipControl.h"
#include "KipControlManager.h"

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
// > Get Condition
boolean KCMenuItem_WelcomeScreen_GetCondition(void * Hander_H) {
	return (KipControlManager_IsRunning() || KipControlManager_IsEnded());
}

/* ******************************************************************************** */
/* Get Batch Number
/* ******************************************************************************** */
// > Transition
void KCMenuItem_GetBatchNumber_Transition(void * Handler_H) {
	int Index_SI = GetIndexOfChar((((WMENU_ITEM_STRUCT *)Handler_H)->ppText_UB[1]), ':');
	Index_SI += 2;
	
	String Text_Str = String(GL_GlobalData_X.KipControl_H.getBatchId());
	
	while ((Index_SI + Text_Str.length()) < (LCD_DISPLAY_COLUMN_NUMBER - 1))
		Text_Str += ' ';

	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, Index_SI, Text_Str);
}

/* ******************************************************************************** */
/* Get Reference ID
/* ******************************************************************************** */
// > Transition
void KCMenuItem_GetReferenceId_Transition(void * Handler_H) {
	int Index_SI = GetIndexOfChar((((WMENU_ITEM_STRUCT *)Handler_H)->ppText_UB[1]), ':');
	Index_SI += 2;

	String Text_Str = String(GL_GlobalData_X.KipControl_H.getReferenceDataId());

	while ((Index_SI + Text_Str.length()) < (LCD_DISPLAY_COLUMN_NUMBER - 1))
		Text_Str += ' ';

	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, Index_SI, Text_Str);
}

/* ******************************************************************************** */
/* Get Tolerance
/* ******************************************************************************** */
// > Transition
void KCMenuItem_GetTolerance_Transition(void * Handler_H) {
	int Index_SI = GetIndexOfChar((((WMENU_ITEM_STRUCT *)Handler_H)->ppText_UB[1]), ':');
	Index_SI += 2;

	String Text_Str = String(GL_GlobalData_X.KipControl_H.getTolerance());
	Text_Str += " %";

	while ((Index_SI + Text_Str.length()) < (LCD_DISPLAY_COLUMN_NUMBER - 1))
		Text_Str += ' ';

	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, Index_SI, Text_Str);
}

/* ******************************************************************************** */
/* Get Minimum Weight
/* ******************************************************************************** */
// > Transition
void KCMenuItem_GetMinWeight_Transition(void * Handler_H) {
	int Index_SI = GetIndexOfChar((((WMENU_ITEM_STRUCT *)Handler_H)->ppText_UB[1]), ':');
	Index_SI += 2;

	String Text_Str = String(GL_GlobalData_X.KipControl_H.getWeightMin());
	Text_Str += " g";

	while ((Index_SI + Text_Str.length()) < (LCD_DISPLAY_COLUMN_NUMBER - 1))
		Text_Str += ' ';

	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, Index_SI, Text_Str);
}

/* ******************************************************************************** */
/* Get Current Day
/* ******************************************************************************** */
// > Transition
void KCMenuItem_GetCurrentDay_Transition(void * Handler_H) {
	int Index_SI = GetIndexOfChar((((WMENU_ITEM_STRUCT *)Handler_H)->ppText_UB[1]), ':');
	Index_SI += 2;

	String Text_Str = String(GL_GlobalData_X.KipControl_H.getCurrentIdx() + 1);

	while ((Index_SI + Text_Str.length()) < (LCD_DISPLAY_COLUMN_NUMBER - 1))
		Text_Str += ' ';

	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, Index_SI, Text_Str);
}
