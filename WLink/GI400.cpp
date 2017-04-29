/* ******************************************************************************** */
/*                                                                                  */
/* GI400.cpp																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		GI400 Indicator specific functions							                */
/*                                                                                  */
/* History :  	29/04/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"GI400"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "GI400.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
static INDICATOR_WEIGHT_STATUS_ENUM GI400_GetWeightStatus(unsigned char * pBuffer_UB) {
    INDICATOR_WEIGHT_STATUS_ENUM Status_E = INDICATOR_WEIGHT_STATUS_UNDEFINED;
    switch (pBuffer_UB[0]) {
    case '.':	Status_E = INDICATOR_WEIGHT_STATUS_STABLE;		break;
    default:	Status_E = INDICATOR_WEIGHT_STATUS_UNDEFINED;	break;
    }
    return Status_E;
}

static INDICATOR_WEIGHT_SIGN_ENUM GI400_GetWeightSign(unsigned char * pBuffer_UB) {
    INDICATOR_WEIGHT_SIGN_ENUM Sign_E = INDICATOR_WEIGHT_SIGN_UNDEFINED;
    switch (pBuffer_UB[0]) {
    case '+':	Sign_E = INDICATOR_WEIGHT_SIGN_POS;			break;
    case '-':	Sign_E = INDICATOR_WEIGHT_SIGN_NEG;			break;
    default:	Sign_E = INDICATOR_WEIGHT_SIGN_UNDEFINED;	break;
    }
    return Sign_E;
}

static unsigned int GI400_GetWeightValue(unsigned char * pBuffer_UB) {
    unsigned int Value_UI = 0;

    for (int i = 0; i < 6; i++)
        pBuffer_UB[i] = (pBuffer_UB[i] == 0x20) ? 0x30 : pBuffer_UB[i]; // Replace space by '0'
    
    Value_UI =  (pBuffer_UB[5] - 0x30) * 1 +
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
void GI400_ProcessFrame(unsigned char * pBuffer_UB, INDICATOR_INTERFACE_FRAME_ENUM Frame_E, INDICATOR_WEIGHT_STRUCT * pWeight_X) {
    switch (Frame_E) {

    case INDICATOR_INTERFACE_FRAME_ASK_WEIGHT:
        pWeight_X->Status_E = GI400_GetWeightStatus(&(pBuffer_UB[7]));
        pWeight_X->Sign_E = GI400_GetWeightSign(&(pBuffer_UB[0]));
        pWeight_X->Value_UI = GI400_GetWeightValue(&(pBuffer_UB[1]));
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
