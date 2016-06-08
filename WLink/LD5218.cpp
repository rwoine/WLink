/* ******************************************************************************** */
/*                                                                                  */
/* IndicatorInterface.cpp															*/
/*                                                                                  */
/* Description :                                                                    */
/*		Manages the interface through the several indicator			                */
/*                                                                                  */
/* History :  	07/06/2016  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"LD5218"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "LD5218.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void LD5218_ProcessWeight(unsigned char * pBuffer_UB, INDICATOR_INTERFACE_FRAME_ENUM Frame_E, INDICATOR_WEIGHT_STRUCT * pWeight_X) {
	switch(Frame_E) {

		case INDICATOR_INTERFACE_FRAME_ASK_WEIGHT:
			break;


		case INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_ALIBI:
		case INDICATOR_INTERFACE_FRAME_ASK_LAST_ALIBI:
			break;


		case INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_MSA:
			break;

		default:
			pWeight_X->Sign_E = INDICATOR_WEIGHT_SIGN_UNDEFINED;
			pWeight_X->Status_E = INDICATOR_WEIGHT_STATUS_UNDEFINED;
			pWeight_X->Alibi_UI = 0;
			pWeight_X->Value_UI = 0;
			break;
	}

	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Print Weight Data Analysis");

	if ((Frame_E == INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_ALIBI) | (Frame_E == INDICATOR_INTERFACE_FRAME_ASK_LAST_ALIBI)) {
		DBG_PRINT(DEBUG_SEVERITY_INFO, "Alibi : ");
		DBG_PRINTDATA(pWeight_X->Alibi_UI);
		DBG_ENDSTR();
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Status : ");
	DBG_PRINTDATA(pWeight_X->Status_E);
	DBG_ENDSTR();

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Sign : ");
	DBG_PRINTDATA(pWeight_X->Sign_E);
	DBG_ENDSTR();

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Weight : ");
	DBG_PRINTDATA(pWeight_X->Value_UI);
	DBG_ENDSTR();
}
