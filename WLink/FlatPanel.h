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
#include "Keypad.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
    FLAT_PANEL_KEY_0            = 0,
    FLAT_PANEL_KEY_1            = 1,
    FLAT_PANEL_KEY_2            = 2,    //  ^   -> up
    FLAT_PANEL_KEY_3            = 3,
    FLAT_PANEL_KEY_4            = 4,    //  <   -> left
    FLAT_PANEL_KEY_5            = 5,
    FLAT_PANEL_KEY_6            = 6,    //  >   -> right
    FLAT_PANEL_KEY_7            = 7,
    FLAT_PANEL_KEY_8            = 8,    //  v   -> down
    FLAT_PANEL_KEY_9            = 9,
    FLAT_PANEL_KEY_DOT          = 10, 
    FLAT_PANEL_KEY_VALIDATE     = 11,
    FLAT_PANEL_KEY_CLEAR        = 12,
    FLAT_PANEL_KEY_F1           = 13, 
    FLAT_PANEL_KEY_F2           = 14, 
    FLAT_PANEL_KEY_F3           = 15
} FLAT_PANEL_KEY_ENUM;

typedef struct {
	boolean IsInitialized_B;
    void(*pFct_OnKeyPressed[16])(char *);
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
    void assignOnKeyPressedEvent(FLAT_PANEL_KEY_ENUM Key_E, void(*pFct_OnKeyPressed)(char *));

	FLAT_PANEL_PARAM GL_FlatPanelParam_X;
};

#endif // __FLAT_PANEL_H__
