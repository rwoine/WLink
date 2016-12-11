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

extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;

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
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Flat Panel Module Initialized");
}

boolean FlatPanel::isInitialized(void) {
	return GL_FlatPanelParam_X.IsInitialized_B;
}

unsigned char FlatPanel::getKey(void) {
	return GL_pKeypad_H->getKey();
}

void FlatPanel::attachEvent(void(*pEventHandler_H)(char)) {
	GL_pKeypad_H->addEventListener(pEventHandler_H);
}

void FlatPanel::manageKeytoLcd(char Key_UB) {

	DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Enter ManageKey function");

	// If a Key is pressed
	if (GL_pKeypad_H->getState() == PRESSED) {

		DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Key PRESSED");

		// Manage Action according to Key
		switch (Key_UB) {
			
			case FLAT_PANEL_KEY_VALIDATE :
				GL_GlobalData_X.Lcd_H.disableExternalWrite();
				break;

			case FLAT_PANEL_KEY_CLEAR :
				break;

			case FLAT_PANEL_KEY_F1 :
				break;

			case FLAT_PANEL_KEY_F2 :
				break;

			case FLAT_PANEL_KEY_F3 :
				break;

			default :
				GL_GlobalData_X.Lcd_H.appendDisplay((unsigned char *) &Key_UB, 1);
				break;

		}
	}

}