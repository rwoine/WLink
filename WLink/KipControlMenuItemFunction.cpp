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
#include "KipControlMenuItemText.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Extenral Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;

extern WMENU_ITEM_STRUCT GL_pKCMenuItem_X[KCMENU_ITEM_NUMBER];


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
static unsigned long GL_ItemAbsoluteTime_UL = 0;

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Welcome Screen
/* ******************************************************************************** */
// > Get Condition
boolean KCMenuItem_WelcomeScreen_GetCondition(void * Handler_H) {
    if (KipControlManager_IsError()) {
        ((WMENU_ITEM_STRUCT *)Handler_H)->pOnConditionNavItem_X = ((WMENU_ITEM_STRUCT *)Handler_H)->pErrorItem_X;
        return true;
    }
    else {
        return (KipControlManager_IsReady());
    }
}

/* ******************************************************************************** */
/* Continue Recording
/* ******************************************************************************** */
// > On Transition
void KCMenuItem_ContinueRecording_OnTransition(void * Handler_H) {
    //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Prevent statuses polling of FONA module");
    //FonaModuleManager_EnableStatusPolling(false);
}

// > On Enter
void KCMenuItem_ContinueRecording_OnEnter(void * Handler_H) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Set Configured Flag");
    KipControlManager_SetConfiguredFlag();
}

// On Timer Elapsed
void KCMenuItem_ContinueRecording_OnTimerElapsed(void * Handler_H) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Set Configured Flag");
    KipControlManager_SetConfiguredFlag();
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

    KipControlManager_SetConfiguredFlag();  // Allow recording to be processed
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

        if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B) {
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B = false;
            for (int i = ColIdx_UL; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
                GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, i, " ");
        }

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
        if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 3) {
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B = true;
        }

	}
}

// > OnValidate
void KCMenuItem_SetBatchNumber_OnValidate(unsigned char * pParam_UB) {
	unsigned char BatchId_UB = 0x00;

	if (pParam_UB[1] == 0xFF) {
		BatchId_UB = pParam_UB[0];
	}
	else if (pParam_UB[2] == 0xFF) {
		BatchId_UB = pParam_UB[0] * 10 + pParam_UB[1];
	}
	else if (pParam_UB[3] == 0xFF) {
		BatchId_UB = pParam_UB[0] * 100 + pParam_UB[1] * 10 + pParam_UB[2];
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Set new value for Batch ID : ");
	DBG_PRINTDATA(BatchId_UB);
	DBG_ENDSTR();

	GL_GlobalData_X.KipControl_H.setBatchId(BatchId_UB);
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

        if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B) {
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B = false;
            for (int i = ColIdx_UL; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
                GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, i, " ");
        }

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
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 2) {
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B = true;
        }

	}
}

// > OnValidate
void KCMenuItem_SetReferenceId_OnValidate(unsigned char * pParam_UB) {
	unsigned char ReferenceId_UB = 0x00;

	if (pParam_UB[1] == 0xFF) {
		ReferenceId_UB = pParam_UB[0];
	}
	else if (pParam_UB[2] == 0xFF) {
		ReferenceId_UB = pParam_UB[0] * 10 + pParam_UB[1];
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Set new value for Reference ID : ");
	DBG_PRINTDATA(ReferenceId_UB);
	DBG_ENDSTR();

	GL_GlobalData_X.KipControl_H.setReferenceDataId(ReferenceId_UB);
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

        if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B) {
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B = false;
            for (int i = ColIdx_UL; i < LCD_DISPLAY_COLUMN_NUMBER-1; i++)
                GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, i, " ");
        }

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
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 2) {
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B = true;
        }

	}
}

// > OnValidate
void KCMenuItem_SetTolerance_OnValidate(unsigned char * pParam_UB) {
	unsigned char Tolerance_UB = 0x00;

	if (pParam_UB[1] == 0xFF) {
		Tolerance_UB = pParam_UB[0];
	}
	else if (pParam_UB[2] == 0xFF) {
		Tolerance_UB = pParam_UB[0] * 10 + pParam_UB[1];
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Set new value for Tolerance : ");
	DBG_PRINTDATA(Tolerance_UB);
	DBG_PRINTDATA(" [%]");
	DBG_ENDSTR();

	GL_GlobalData_X.KipControl_H.setTolerance(Tolerance_UB);
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

        if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B) {
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B = false;
            for (int i = ColIdx_UL; i < LCD_DISPLAY_COLUMN_NUMBER-1; i++)
                GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, i, " ");
        }

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
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 4) {
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B = true;
        }

	}
}

// > OnValidate
void KCMenuItem_SetMinWeight_OnValidate(unsigned char * pParam_UB) {
	signed int MinWeight_SI = 0;

	if (pParam_UB[1] == 0xFF) {
		MinWeight_SI = pParam_UB[0];
	}
	else if (pParam_UB[2] == 0xFF) {
		MinWeight_SI = pParam_UB[0] * 10 + pParam_UB[1];
	}
	else if (pParam_UB[3] == 0xFF) {
		MinWeight_SI = pParam_UB[0] * 100 + pParam_UB[1] * 10 + pParam_UB[2];
	}
	else if (pParam_UB[4] == 0xFF) {
		MinWeight_SI = pParam_UB[0] * 1000 + pParam_UB[1] * 100 + pParam_UB[2] * 10 + pParam_UB[3];
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Set new value for Minimum Weight : ");
	DBG_PRINTDATA(MinWeight_SI);
	DBG_PRINTDATA(" [g]");
	DBG_ENDSTR();

	GL_GlobalData_X.KipControl_H.setWeightMin(MinWeight_SI);
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

        if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B) {
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B = false;
            for (int i = ColIdx_UL; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
                GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, i, " ");
        }

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
		if (((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL == 2) {
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->ParamIndex_UL = 0;
            ((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->HasRollbacked_B = true;
        }

	}
}

// > OnValidate
void KCMenuItem_SetStartDay_OnValidate(unsigned char * pParam_UB) {
	unsigned char StartDay_UB = 0x00;
	RTC_DATE_STRUCT CurrentDate_X = GL_GlobalData_X.Rtc_H.getDate();

	if (pParam_UB[1] == 0xFF) {
		StartDay_UB = pParam_UB[0];
	}
	else if (pParam_UB[2] == 0xFF) {
		StartDay_UB = pParam_UB[0] * 10 + pParam_UB[1];
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Set new value for Start Day : ");
	DBG_PRINTDATA(StartDay_UB);
	DBG_ENDSTR();
	DBG_PRINT(DEBUG_SEVERITY_INFO, "-> Start Index : ");
	DBG_PRINTDATA(StartDay_UB - 1);
	DBG_ENDSTR();

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Set Start Date to today : ");
	DBG_PRINTDATA(dateToString(CurrentDate_X));
	DBG_ENDSTR();

	GL_GlobalData_X.KipControl_H.setStartIdx(StartDay_UB - 1);
	GL_GlobalData_X.KipControl_H.setStartDate(CurrentDate_X);	

    KipControlManager_SetConfiguredFlag();  // Enable configuration
}

/* ******************************************************************************** */
/* New Recording
/* ******************************************************************************** */
// > OnEnter
void KCMenuItem_NewRecording_OnEnter(void * Handler_H) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Reset running flag -> New Recording");

    GL_GlobalData_X.KipControl_H.setRunningFlag(false);
    KipControlManager_ResetConfiguredFlag();    // Disable configuration
    KipControlManager_RelaunchProcess();        // Ensure new beginning of state machine
}

/* ******************************************************************************** */
/* Actual Recording
/* ******************************************************************************** */
// > Process
void KCMenuItem_ActualRecording_Process(void * Handler_H) {
	int SignalStrength_SI = 0;
	unsigned char CurrentDay_UB = 0;
	unsigned long ColIdx_UL = 0;
	float Average_f = 0.0;
    String CurrentReference_str = "";
	String Average_str = "";

	// --> Line 1
	// ----------

    // Display Date/Time
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 0, GL_GlobalData_X.Rtc_H.getDateShort());

    // Display Theoritical Weight
    CurrentReference_str = String(KipControlManager_GetCurrentReferenceData()) + " g";
    GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 17 - CurrentReference_str.length(), CurrentReference_str);

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


	// --> Line 2
	// ----------
	CurrentDay_UB = GL_GlobalData_X.KipControl_H.getCurrentIdx() + 1;
	if (CurrentDay_UB < 10) {
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 1, '0');
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 2, String(CurrentDay_UB));
	}
	else {
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 1, String(CurrentDay_UB));
	}

	ColIdx_UL = GetIndexOfChar((((WMENU_ITEM_PARAM_STRUCT *)Handler_H)->pSenderItem_H->ppText_UB[1]), '.');
	ColIdx_UL += 2;

	if (GL_GlobalData_X.KipControl_H.getValueNb() != 0)
		Average_f = ((float)(GL_GlobalData_X.KipControl_H.getTotalValue())) / ((float)(GL_GlobalData_X.KipControl_H.getValueNb()));
	else
		Average_f = 0.0;
	Average_str = String(Average_f, 2);
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, ColIdx_UL, Average_str);
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, ColIdx_UL + Average_str.length(), " g");

    // Erase end of line
    for (int i = ColIdx_UL + Average_str.length() + 2; i < LCD_DISPLAY_COLUMN_NUMBER; i++)
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, i, " ");
}

// > Transition
void KCMenuItem_ActualRecording_Transition(void * Handler_H) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Enable recording to portal and average management");
    KipControlManager_EnableRecording(true);

    //DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Allow FONA Module statuses polling");
    //FonaModuleManager_EnableStatusPolling(true);
}

// > Get Condition
boolean KCMenuItem_ActualRecording_GetCondition(void * Hander_H) {
    if (KipControlManager_IsError()) {
        ((WMENU_ITEM_STRUCT *)Hander_H)->pOnConditionNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_ERROR_SCREEN]);
        return true;
    }
    else if (KipControlManager_IsProcessingWeight()) {
        ((WMENU_ITEM_STRUCT *)Hander_H)->pOnConditionNavItem_X = &(GL_pKCMenuItem_X[KCMENU_ITEM_CURRENT_RECORD]);
        return true;
    }
    //else if (KipControlManager_IsEnd()) {
    //      TODO - to add
    //    return true;
    //}
    else {
        return false;
    }
}


/* ******************************************************************************** */
/* Current Record
/* ******************************************************************************** */
// > Process
void KCMenuItem_CurrentRecord_Process(void * Handler_H) {   // TODO : on Transition ?
	String Weight_str = String(KipControlManager_GetCurrentWeight());
	GL_GlobalData_X.Lcd_H.clearDisplay(LCD_DISPLAY_LINE2);
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 1, Weight_str);
	GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 1 + Weight_str.length() + 1, "g");
}


/* ******************************************************************************** */
/* Current Weight
/* ******************************************************************************** */
// > Process
void KCMenuItem_CurrentWeight_Process(void * Handler_H) {
    if ((millis() - GL_ItemAbsoluteTime_UL) >= 800) {
        String Weight_str = String(KipControlManager_GetCurrentWeight());
        GL_GlobalData_X.Lcd_H.clearDisplay(LCD_DISPLAY_LINE2);
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 1, Weight_str);
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 1 + Weight_str.length() + 1, "g");
        GL_ItemAbsoluteTime_UL = millis();
    }
}

// > Transition
void KCMenuItem_CurrentWeight_Transition(void * Handler_H) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Disable recording to display current weight");
    KipControlManager_EnableRecording(false);
    GL_ItemAbsoluteTime_UL = millis();
}


/* ******************************************************************************** */
/* Reset Weight
/* ******************************************************************************** */
// > On Enter
void KCMenuItem_ResetWeight_OnEnter(void * Handler_H) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Set flag to zero indicator");
    KipControlManager_SetZeroIndicator();
}