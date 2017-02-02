/* ******************************************************************************** */
/*                                                                                  */
/* MemoryCard.h		                                                                */
/*                                                                                  */
/* Description :                                                                    */
/*		Header file for MemoryCard.cpp											    */
/*                                                                                  */
/* History :  	25/01/2017  (RW)	Creation of this file							*/
/*                                                                                  */
/* ******************************************************************************** */

#ifndef __MEMORY_CARD_H__
#define __MEMORY_CARD_H__

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */
#include <Arduino.h>

#include <SPI.h>
#include <SD.h>

/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */

/* ******************************************************************************** */
/* Structure & Enumeration
/* ******************************************************************************** */
typedef enum {
    FILE_HANDLING_STS_OK,
    FILE_HANDLING_STS_ALREADY_EXISTS,
    FILE_HANDLING_STS_ALREADY_OPENED,
    FILE_HANDLING_STS_IS_NOT_OPENED,
    FILE_HANDLING_STS_CANNOT_CREATE,
    FILE_HANDLING_STS_CANNOT_OPEN,
    FILE_HANDLING_STS_CANNOT_WRITE,
    FILE_HANDLING_STS_ERROR
} FILE_HANDLING_STS_ENUM;

typedef enum {
    FILE_HANDLING_OPEN_MODE_READ,
    FILE_HANDLING_OPEN_MODE_WRITE
} FILE_HANDLING_OPEN_MODE_ENUM;


typedef struct {
    boolean IsInitialized_B;
} MEMORY_CARD_PARAM;

/* ******************************************************************************** */
/* Class
/* ******************************************************************************** */
class MemoryCard {
public:
    // Constructor
    MemoryCard();

    // Functions
    void init(unsigned char PinChipSelect_UB, unsigned char PinCardDetect_UB, unsigned char PinWriteProtect_UB);
    boolean isInitialized(void);
    boolean isCardInserted(void);

    boolean doesFileExist(char * pFileName_UB);
    boolean doesFileExist(String FileName_Str);
    FILE_HANDLING_STS_ENUM createFile(char * pFileName_UB);
    FILE_HANDLING_STS_ENUM createFile(String FileName_Str);
    FILE_HANDLING_STS_ENUM openFile(File * pFile_H, char * pFileName_UB, FILE_HANDLING_OPEN_MODE_ENUM Mode_E = FILE_HANDLING_OPEN_MODE_READ);
    FILE_HANDLING_STS_ENUM openFile(File * pFile_H, String FileName_Str, FILE_HANDLING_OPEN_MODE_ENUM Mode_E = FILE_HANDLING_OPEN_MODE_READ);
    FILE_HANDLING_STS_ENUM closeFile(File * pFile_H);

    FILE_HANDLING_STS_ENUM writeFile(File * pFile_H, char * pWriteDate_UB);
    FILE_HANDLING_STS_ENUM writeFile(File * pFile_H, String pWriteDate_Str);

    MEMORY_CARD_PARAM GL_MemoryCardParam_X;
};

#endif // __MEMORY_CARD_H__
