/* ******************************************************************************** */
/*                                                                                  */
/* WCommand.cpp																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the functions related to each module                              */
/*                                                                                  */
/* History :  	16/02/2015  (RW)	Creation of this file                           */
/*				14/05/2016	(RW)	Re-mastered version								*/
/*                                                                                  */
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WLink.h"
#include "WCommand.h"

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

/* Global ************************************************************************* */
/* ******************************************************************************** */

WCMD_FCT_STS WCmdProcess_GetRevisionId(const unsigned char * pParam_UB, unsigned long ParamNb_UL, unsigned char * pAns_UB, unsigned long * pAnsNb_UL) {
	//DBG_PRINT("TcpCmdProcess_GetRevisionId");
	*pAnsNb_UL = 8;

	for (int i = 0; i < 8; i++)
		pAns_UB[i] = GL_GlobalData_X.pRevisionId_UB[i];

	return WCMD_FCT_STS_OK;
}
