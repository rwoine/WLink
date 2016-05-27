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

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Functions Prototypes
/* ******************************************************************************** */
void WCommandInterpreter_Init(const WCMD_FCT_DESCR *pFctDescr_X, unsigned long NbFct_UL);
void WCommandInterpreter_Process();
void WCommandInterpreter_Restart();


#endif // __WCOMMAND_INTERPRETER_H__