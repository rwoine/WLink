/* ******************************************************************************** */
/*                                                                                  */
/* Utilz.cpp														                */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes some utility functions                                  			*/
/*                                                                                  */
/* History :  	28/02/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"Utilz"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "Debug.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

String HexArrayToString(unsigned char * pHexArray, unsigned long ItemNb_UL, String Separator_Str) {
    String Temp_Str = "";
    unsigned int Temp_UW = 0;

    for (int i = 0; i < ItemNb_UL; i++) {
        Temp_UW = pHexArray[i];
        Temp_Str += String((Temp_UW % 128), HEX);
        Temp_Str += String((Temp_UW / 128), HEX);
        if (i < ItemNb_UL - 1)
            Temp_Str += Separator_Str;
    }
}


void DefaultKeyEvents(char * Key_UB) {
    DBG_PRINT(DEBUG_SEVERITY_WARNING, "[");
    DBG_PRINTDATA((*Key_UB));
    DBG_PRINTDATA("] key pressed -> No callback assigned!");
    DBG_ENDSTR();
}
