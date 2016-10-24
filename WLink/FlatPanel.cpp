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

#include "FlatPanel.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
static Keypad * GL_pKeypad_H;

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