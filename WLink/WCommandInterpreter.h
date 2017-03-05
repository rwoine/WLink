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

#define WCMD_STX            0x02
#define WCMD_ETX            0x03
#define WCMD_ACK            0x06
#define WCMD_NACK           0x15

#define WCMD_PARAM_BIT_MASK	0x80
#define WCMD_CMD_ID_MASK    0x7F

#define WCMD_MAX_PARAM_NB	256
#define WCMD_MAX_ANS_NB		256

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void WCommandInterpreter_Init(const WCMD_FCT_DESCR *pFctDescr_X, unsigned long NbFct_UL);
void WCommandInterpreter_Process();


#endif // __WCOMMAND_INTERPRETER_H__