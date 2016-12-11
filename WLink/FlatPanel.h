/* ******************************************************************************** */
/*                                                                                  */
/* FlatPanel.h			                                                            */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for FlatPanel.cpp												*/
/*                                                                                  */
/* History :  	16/06/2016  (RW)	Creation of this file							*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __FLAT_PANEL_H__
#define __FLAT_PANEL_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <Arduino.h>
#include <Keypad.h>

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

#define FLAT_PANEL_KEY_VALIDATE		'V'
#define FLAT_PANEL_KEY_CLEAR		'X'
#define FLAT_PANEL_KEY_F1			'A'
#define FLAT_PANEL_KEY_F2			'B'
#define FLAT_PANEL_KEY_F3			'C'

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
	boolean IsInitialized_B;
} FLAT_PANEL_PARAM;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class FlatPanel {
public:
	// Constructor
	FlatPanel();

	// Functions
	void init(Keypad * pKeypad_H);
	boolean isInitialized(void);

	unsigned char getKey(void);
	void attachEvent(void(*EventHandler)(char));

	void manageKeytoLcd(char Key_UB);

	FLAT_PANEL_PARAM GL_FlatPanelParam_X;
};

#endif // __FLAT_PANEL_H__
