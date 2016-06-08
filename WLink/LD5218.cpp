/* ******************************************************************************** */
/*                                                                                  */
/* LD5218.cpp																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		LD5218 Indicator specific functions							                */
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
/* Internal Functions
/* ******************************************************************************** */
static INDICATOR_WEIGHT_STATUS_ENUM LD5218_GetWeightStatus(unsigned char * pBuffer_UB) {
	INDICATOR_WEIGHT_STATUS_ENUM Status_E = INDICATOR_WEIGHT_STATUS_UNDEFINED;
	switch (pBuffer_UB[0]) {
		case 'P':
		case 'p':
		case 't':	Status_E = INDICATOR_WEIGHT_STATUS_STABLE;		break;
		case 0x40:
		case 0x60:  Status_E = INDICATOR_WEIGHT_STATUS_UNSTABLE;	break;
		case 0x48:  Status_E = INDICATOR_WEIGHT_STATUS_OVERRANGE;	break;
		default:	Status_E = INDICATOR_WEIGHT_STATUS_UNDEFINED;	break;
	}
	return Status_E;
}

static INDICATOR_WEIGHT_SIGN_ENUM LD5218_GetWeightSign(unsigned char * pBuffer_UB) {
	INDICATOR_WEIGHT_SIGN_ENUM Sign_E = INDICATOR_WEIGHT_SIGN_UNDEFINED;
	switch (pBuffer_UB[0]) {
		case '+':	Sign_E = INDICATOR_WEIGHT_SIGN_POS;			break;
		case '-':	Sign_E = INDICATOR_WEIGHT_SIGN_NEG;			break;
		default:	Sign_E = INDICATOR_WEIGHT_SIGN_UNDEFINED;	break;
	}
	return Sign_E;
}

static unsigned int LD5218_GetWeightValue(unsigned char * pBuffer_UB) {
	unsigned int Value_UI = 0;
	Value_UI =	(pBuffer_UB[5] - 0x30) * 1 +
				(pBuffer_UB[4] - 0x30) * 10 +
				(pBuffer_UB[3] - 0x30) * 100 +
				(pBuffer_UB[2] - 0x30) * 1000 +
				(pBuffer_UB[1] - 0x30) * 10000 +
				(pBuffer_UB[0] - 0x30) * 100000;
	return Value_UI;
}

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void LD5218_ProcessFrame(unsigned char * pBuffer_UB, INDICATOR_INTERFACE_FRAME_ENUM Frame_E, INDICATOR_WEIGHT_STRUCT * pWeight_X) {
	switch(Frame_E) {

		case INDICATOR_INTERFACE_FRAME_ASK_WEIGHT:
			pWeight_X->Status_E = LD5218_GetWeightStatus(&(pBuffer_UB[0]));
			pWeight_X->Sign_E = LD5218_GetWeightSign(&(pBuffer_UB[1]));
			pWeight_X->Value_UI = LD5218_GetWeightValue(&(pBuffer_UB[2]));
			break;


		case INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_ALIBI:
		case INDICATOR_INTERFACE_FRAME_ASK_LAST_ALIBI:
			pWeight_X->Value_UI = LD5218_GetWeightValue(&(pBuffer_UB[6]));
			pWeight_X->Alibi_UI =	(pBuffer_UB[3] - 0x30) * 1 +
									(pBuffer_UB[2] - 0x30) * 10 +
									(pBuffer_UB[1] - 0x30) * 100 +
									(pBuffer_UB[0] - 0x30) * 1000;
			break;


		case INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_MSA:
			pWeight_X->Status_E = LD5218_GetWeightStatus(&(pBuffer_UB[4]));
			pWeight_X->Sign_E = LD5218_GetWeightSign(&(pBuffer_UB[5]));
			pWeight_X->Value_UI = LD5218_GetWeightValue(&(pBuffer_UB[21]));
			break;

		default:
			pWeight_X->Sign_E = INDICATOR_WEIGHT_SIGN_UNDEFINED;
			pWeight_X->Status_E = INDICATOR_WEIGHT_STATUS_UNDEFINED;
			pWeight_X->Alibi_UI = 0;
			pWeight_X->Value_UI = 0;
			break;
	}

	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Print Weight Data Analysis");

	// Print Alibi only if needed
	if ((Frame_E == INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_ALIBI) || (Frame_E == INDICATOR_INTERFACE_FRAME_ASK_LAST_ALIBI)) {
		DBG_PRINT(DEBUG_SEVERITY_INFO, "Alibi : ");
		DBG_PRINTDATA(pWeight_X->Alibi_UI);
		DBG_ENDSTR();
	}

	// Do not print Status and Sign when in Alibi
	if ((Frame_E != INDICATOR_INTERFACE_FRAME_ASK_WEIGHT_ALIBI) && (Frame_E != INDICATOR_INTERFACE_FRAME_ASK_LAST_ALIBI)) {
		DBG_PRINT(DEBUG_SEVERITY_INFO, "Status : ");
		DBG_PRINTDATA(pWeight_X->Status_E);
		DBG_ENDSTR();

		DBG_PRINT(DEBUG_SEVERITY_INFO, "Sign : ");
		DBG_PRINTDATA(pWeight_X->Sign_E);
		DBG_ENDSTR();
	}

	DBG_PRINT(DEBUG_SEVERITY_INFO, "Weight : ");
	DBG_PRINTDATA(pWeight_X->Value_UI);
	DBG_ENDSTR();
}
