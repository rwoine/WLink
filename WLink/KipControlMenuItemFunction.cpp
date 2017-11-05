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


/* ******************************************************************************** */
/* Set Batch Number
/* ******************************************************************************** */
// > Process
void KCMenuItem_SetBatchNumber_Process(void * Handler_H) {

	// Get start position
	unsigned long ColIdx_UL = GetIndexOfChar((((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pSenderItem_H->ppText_UB[1]), ':');
	ColIdx_UL += 2;

	// Manage cursor
	ColIdx_UL += ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL;
	GL_GlobalData_X.Lcd_H.enableCursor(LCD_DISPLAY_LINE2, ColIdx_UL);

	if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B) {
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B = false;

		// Validate Input
		// TODO : Check for validation further
		switch (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL) {
		case 0:     break;	//	100		-	10		-	1
		case 1:     break;	//	10		-	1		-	
		case 2:     break;	//	1		-			-
		}

		// Write on LCD and in Buffer
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, ColIdx_UL, (unsigned char *)(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)), 1);
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL++] = atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB));
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL] = 0xFF;	// help to end-up number

		// Rollback
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 3)
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;

	}
}

// > OnValidate
void KCMenuItem_SetBatchNumber_OnValidate(unsigned char * pParam_UB) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "KCMenuItem_SetBatchNumber_OnValidate");
	unsigned char BatchId = 0x00;

	if (pParam_UB[1] = 0xFF) {
		BatchId = pParam_UB[0];
	}
	else if (pParam_UB[2] = 0xFF) {
		BatchId = pParam_UB[0] * 10 + pParam_UB[1];
	}
	else {

	}
}


/* ******************************************************************************** */
/* Set Reference ID
/* ******************************************************************************** */
// > Process
void KCMenuItem_SetReferenceId_Process(void * Handler_H) {

	// Get start position
	unsigned long ColIdx_UL = GetIndexOfChar((((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pSenderItem_H->ppText_UB[1]), ':');
	ColIdx_UL += 2;

	// Manage cursor
	ColIdx_UL += ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL;
	GL_GlobalData_X.Lcd_H.enableCursor(LCD_DISPLAY_LINE2, ColIdx_UL);

	if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B) {
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B = false;

		// Validate Input
		// TODO : Check for validation further
		switch (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL) {
		case 0:     break;	//	10		-	1			
		case 1:     break;	//	1		-				
		}

		// Write on LCD and in Buffer
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, ColIdx_UL, (unsigned char *)(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)), 1);
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL++] = atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB));
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL] = 0xFF;	// help to end-up number

																															// Rollback
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 2)
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;

	}
}

// > OnValidate
void KCMenuItem_SetReferenceId_OnValidate(unsigned char * pParam_UB) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "KCMenuItem_SetReferenceId_OnValidate");

}


/* ******************************************************************************** */
/* Set Tolerance
/* ******************************************************************************** */
// > Process
void KCMenuItem_SetTolerance_Process(void * Handler_H) {

	// Get start position
	unsigned long ColIdx_UL = GetIndexOfChar((((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pSenderItem_H->ppText_UB[1]), ':');
	ColIdx_UL += 2;

	// Manage cursor
	ColIdx_UL += ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL;
	GL_GlobalData_X.Lcd_H.enableCursor(LCD_DISPLAY_LINE2, ColIdx_UL);

	if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B) {
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B = false;

		// Validate Input
		// TODO : Check for validation further
		switch (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL) {
		case 0:     break;	//	10		-	1			
		case 1:     break;	//	1		-				
		}

		// Write on LCD and in Buffer
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, ColIdx_UL, (unsigned char *)(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)), 1);
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL++] = atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB));
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL] = 0xFF;	// help to end-up number

																															// Rollback
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 2)
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;

	}
}

// > OnValidate
void KCMenuItem_SetTolerance_OnValidate(unsigned char * pParam_UB) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "KCMenuItem_SetTolerance_OnValidate");

}


/* ******************************************************************************** */
/* Set Minimum Weight
/* ******************************************************************************** */
// > Process
void KCMenuItem_SetMinWeight_Process(void * Handler_H) {

	// Get start position
	unsigned long ColIdx_UL = GetIndexOfChar((((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pSenderItem_H->ppText_UB[1]), ':');
	ColIdx_UL += 2;

	// Manage cursor
	ColIdx_UL += ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL;
	GL_GlobalData_X.Lcd_H.enableCursor(LCD_DISPLAY_LINE2, ColIdx_UL);

	if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B) {
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B = false;

		// Validate Input
		// TODO : Check for validation further
		switch (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL) {
		case 0:     break;	//	1000	-	100		-	10		-	1	
		case 1:     break;	//	100		-	10		-	1		-	
		case 2:     break;	//	10		-	1		-			-
		case 3:     break;	//	1		-			-			-		
		}

		// Write on LCD and in Buffer
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, ColIdx_UL, (unsigned char *)(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)), 1);
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL++] = atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB));
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL] = 0xFF;	// help to end-up number

																															// Rollback
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 4)
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;

	}
}

// > OnValidate
void KCMenuItem_SetMinWeight_OnValidate(unsigned char * pParam_UB) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "KCMenuItem_SetMinWeight_OnValidate");

}


/* ******************************************************************************** */
/* Set Start Day
/* ******************************************************************************** */
// > Process
void KCMenuItem_SetStartDay_Process(void * Handler_H) {

	// Get start position
	unsigned long ColIdx_UL = GetIndexOfChar((((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pSenderItem_H->ppText_UB[1]), ':');
	ColIdx_UL += 2;

	// Manage cursor
	ColIdx_UL += ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL;
	GL_GlobalData_X.Lcd_H.enableCursor(LCD_DISPLAY_LINE2, ColIdx_UL);

	if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B) {
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->KeyPressed_B = false;

		// Validate Input
		// TODO : Check for validation further
		switch (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL) {
		case 0:     break;	//	10		-	1			
		case 1:     break;	//	1		-				
		}

		// Write on LCD and in Buffer
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, ColIdx_UL, (unsigned char *)(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB)), 1);
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL++] = atoi(&(((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->Key_UB));
		((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pParam_UB[((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL] = 0xFF;	// help to end-up number

																															// Rollback
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 2)
			((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;

	}
}

// > OnValidate
void KCMenuItem_SetStartDay_OnValidate(unsigned char * pParam_UB) {
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "KCMenuItem_SetStartDay_OnValidate");

}

