/* ******************************************************************************** */
/*                                                                                  */
/* FlatPanel.cpp																	*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the Flat Panel utilities functions								*/
/*                                                                                  */
/* History :  	21/06/2016  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"FlatPanel"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WLink.h"
#include "FlatPanel.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
static Keypad * GL_pKeypad_H;
static const char GL_pKeyLut_UB[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 'V', 'X', 'A', 'B', 'C' };

extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
void OnKeyEvents(char Key_UB);

/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
FlatPanel::FlatPanel() {
	GL_FlatPanelParam_X.IsInitialized_B = false;
}

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void FlatPanel::init(Keypad * pKeypad_H) {
	GL_pKeypad_H = pKeypad_H;
    GL_pKeypad_H->addEventListener(OnKeyEvents);
	GL_FlatPanelParam_X.IsInitialized_B = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Flat Panel Module Initialized");
}

boolean FlatPanel::isInitialized(void) {
	return GL_FlatPanelParam_X.IsInitialized_B;
}

unsigned char FlatPanel::getKey(void) {
	return GL_pKeypad_H->getKey();
}

void FlatPanel::assignOnKeyPressedEvent(FLAT_PANEL_KEY_ENUM Key_E, void(*pFct_OnKeyPressed)(void)) {
    GL_FlatPanelParam_X.pFct_OnKeyPressed[Key_E] = pFct_OnKeyPressed;
}

/* ******************************************************************************** */
/* Local Functions
/* ******************************************************************************** */
void OnKeyEvents(char Key_UB) {

    // If a Key is pressed
    if (GL_pKeypad_H->getState() == PRESSED) {
        switch (Key_UB) {
        case '0':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_0]();          break;
        case '1':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_1]();          break;
        case '2':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_2]();          break;
        case '3':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_3]();          break;
        case '4':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_4]();          break;
        case '5':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_5]();          break;
        case '6':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_6]();          break;
        case '7':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_7]();          break;
        case '8':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_8]();          break;
        case '9':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_9]();          break;
        case '.':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_DOT]();        break;
        case 'V':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_VALIDATE]();   break;
        case 'X':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_CLEAR]();      break;
        case 'A':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_F1]();         break;
        case 'B':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_F2]();         break;
        case 'C':   GL_GlobalData_X.FlatPanel_H.GL_FlatPanelParam_X.pFct_OnKeyPressed[FLAT_PANEL_KEY_F3]();         break;
        }        
    }

}


//void FlatPanel::manageKeytoLcd(char Key_UB) {
//
//	// If a Key is pressed
//	if (GL_pKeypad_H->getState() == PRESSED) {
//
//		// Manage Action according to Key
//		switch (Key_UB) {
//			
//			case FLAT_PANEL_KEY_VALIDATE :
//				GL_GlobalData_X.Lcd_H.disableExternalWrite();
//				break;
//
//			case FLAT_PANEL_KEY_CLEAR :
//				GL_GlobalData_X.Lcd_H.backspaceDisplay(1);
//				break;
//
//			case FLAT_PANEL_KEY_F1 :
//				break;
//
//			case FLAT_PANEL_KEY_F2 :
//				break;
//
//			case FLAT_PANEL_KEY_F3 :
//				break;
//
//			default :
//				GL_GlobalData_X.Lcd_H.appendDisplay((unsigned char *) &Key_UB, 1);
//				break;
//
//		}
//	}
//
//}