/* ******************************************************************************** */
/*                                                                                  */
/* WCommandIntepreter.h                                                             */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for WCommandIntepreter.cpp										*/
/*		State machine to manage the command processing                              */
/*                                                                                  */
/* History :  	16/02/2015  (RW)	Creation of this file							*/
/*				14/05/2016	(RW)	Re-mastered version	(medium independant)		*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __WCOMMAND_INTERPRETER_H__
#define __WCOMMAND_INTERPRETER_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WCommand.h"

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
	WCMD_INTERPRETER_STATE_IDLE,
	WCMD_INTERPRETER_STATE_CHECK_CMD,
	WCMD_INTERPRETER_STATE_PROCESS_CMD,
	WCMD_INTERPRETER_STATE_SEND_RESP,
	WCMD_INTERPRETER_STATE_SEND_NACK
} WCMD_INTERPRETER_STATE;


/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void WCommandInterpreter_Init(const WCMD_FCT_DESCR *pFctDescr_X, unsigned long NbFct_UL);
void WCommandInterpreter_Process();
void WCommandInterpreter_Restart();


#endif // __WCOMMAND_INTERPRETER_H__