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
	void init();
	boolean isInitialized(void);


};

#endif // __FLAT_PANEL_H__
