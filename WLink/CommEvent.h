/* ******************************************************************************** */
/*                                                                                  */
/* CommEvent.h																		*/
/*                                                                                  */
/* Description :                                                                    */
/*		Gathers CommEvent functions to be mapped to SerialEvent interrupt.			*/
/*                                                                                  */
/* History :	11/06/2016	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */
#ifndef __COMM_EVENT_H__
#define __COMM_EVENT_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include "WLink.h"

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef struct {
	void(*EventHandler)(void);
} COM_EVENT_FCT_STRUCT;

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;

/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
static void CommEvent_BadgeReader(void) {
	return (GL_GlobalData_X.BadgeReader_H.commEvent());
}

static void CommEvent_Indicator(void) {
    GL_GlobalData_X.Indicator_H.setIrq();
}

#endif // __COMM_EVENT_H__
