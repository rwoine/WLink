/* ******************************************************************************** */
/*                                                                                  */
/* Hardware.h                                                                       */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for hardware related definition                                 */
/*                                                                                  */
/* History :	14/05/2016	(RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __HARDWARE_H__
#define __HARDWARE_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Pin Defintion
/* ******************************************************************************** */

/* LED */
#define PIN_BLINK_LED		13

/* Global Purpose Inputs/Outputs */
#define PIN_GPIO_INPUT0		28
#define PIN_GPIO_INPUT1     27
#define PIN_GPIO_INPUT2     26
#define PIN_GPIO_INPUT3     25
#define PIN_GPIO_OUTPUT0    32
#define PIN_GPIO_OUTPUT1    30
#define PIN_GPIO_OUTPUT2    12
#define PIN_GPIO_OUTPUT3    11

/* Liquid Crystal Display */
#define PIN_LCD_RS          45
#define PIN_LCD_RW          44
#define PIN_LCD_EN          3
#define PIN_LCD_D4          9
#define PIN_LCD_D5          8
#define PIN_LCD_D6          7
#define PIN_LCD_D7          6
#define PIN_LCD_BACKLIGHT   5

/* Flat Panel */
#define PIN_FP0				46
#define PIN_FP1				47
#define PIN_FP2				48
#define PIN_FP3				49
#define PIN_FP4				50
#define PIN_FP5				51
#define PIN_FP6				41
#define PIN_FP7				40
#define PIN_FP8				39
#define PIN_FP9				38

/* Secure Digital Card */
#define PIN_SD_WP           42
#define PIN_SD_CD           43
#define PIN_SD_CS           4

/* Ethernet */
#define PIN_ETH_LINKED		35

/* Serial COM Ports */
#define PORT_COM0			0
#define PORT_COM1			1
#define PORT_COM2			2
#define PORT_COM3			3

#define PIN_EN_SERIAL01		33
#define PIN_EN_SERIAL23		34


#endif // __HARDWARE_H__