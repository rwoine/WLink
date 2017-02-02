/* ******************************************************************************** */
/*                                                                                  */
/* MemoryCard.cpp																    */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the SD Card utilities functions									*/
/*                                                                                  */
/* History :  	25/01/2017  (RW)	Creation of this file                           */
/*																					*/
/* ******************************************************************************** */

#define MODULE_NAME		"MemoryCard"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "MemoryCard.h"

#include "Debug.h"

/* ******************************************************************************** */
/* Local Structures
/* ******************************************************************************** */
typedef struct{
    char * pFileName_UB;
    File file_H;
    boolean isOpened;
} FILE_HANDLING_DATA_STRUCT;

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */

static SDClass * GL_pCard_H;

static unsigned char GL_PinChipSelect_UB;
static unsigned char GL_PinCardDetect_UB;
static unsigned char GL_PinWriteProtect_UB;

static FILE_HANDLING_DATA_STRUCT GL_FileData_X;


/* ******************************************************************************** */
/* Constructor
/* ******************************************************************************** */
MemoryCard::MemoryCard() {
    GL_MemoryCardParam_X.IsInitialized_B = false;
}

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */
void MemoryCard::init(unsigned char PinChipSelect_UB, unsigned char PinCardDetect_UB, unsigned char PinWriteProtect_UB) {
    GL_pCard_H = &SD;
    GL_PinChipSelect_UB = PinChipSelect_UB;
    GL_PinCardDetect_UB = PinCardDetect_UB;
    GL_PinWriteProtect_UB = PinWriteProtect_UB;

    pinMode(GL_PinChipSelect_UB, OUTPUT);
    pinMode(GL_PinCardDetect_UB, INPUT);
    pinMode(GL_PinWriteProtect_UB, INPUT);

    // Check if Card is present before initialize handle
    if (isCardInserted()) {
        if (GL_pCard_H->begin(GL_PinChipSelect_UB)) {
            GL_MemoryCardParam_X.IsInitialized_B = true;
            DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Memory Card Module Initialized");
        } else {
            GL_MemoryCardParam_X.IsInitialized_B = false;
            DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Memory Card Module NOT Initialized - Error in low-level function !");
        }
    } else {
        GL_MemoryCardParam_X.IsInitialized_B = false;
        DBG_PRINTLN(DEBUG_SEVERITY_ERROR, "Memory Card Module NOT Initialized - Card not detected !");
    }
}

boolean MemoryCard::isInitialized(void) {
    return GL_MemoryCardParam_X.IsInitialized_B;
}

boolean MemoryCard::isCardInserted(void) {
    return (digitalRead(GL_PinCardDetect_UB) == HIGH);
}


boolean MemoryCard::doesFileExist(char * pFileName_UB) {
    return (GL_pCard_H->exists(pFileName_UB));
}

boolean MemoryCard::doesFileExist(String FileName_Str) {
    return (doesFileExist(FileName_Str.c_str()));
}

FILE_HANDLING_STS_ENUM MemoryCard::createFile(char * pFileName_UB) {

    if (doesFileExist(pFileName_UB))
        return FILE_HANDLING_STS_ALREADY_EXISTS;

    // Open() function auto-create a file
    GL_FileData_X.pFileName_UB = pFileName_UB;
    GL_FileData_X.file_H = GL_pCard_H->open(pFileName_UB, FILE_WRITE); 

    if (GL_FileData_X.file_H) {

        GL_FileData_X.isOpened = true;

        // Close the file
        GL_FileData_X.file_H.close();
        GL_FileData_X.file_H = File();  // empty constructor
        GL_FileData_X.isOpened = false;
    }
    else {
        return FILE_HANDLING_STS_CANNOT_CREATE;
    }

    return FILE_HANDLING_STS_OK;
}

FILE_HANDLING_STS_ENUM MemoryCard::createFile(String FileName_Str) { return (createFile(FileName_Str.c_str())); }

FILE_HANDLING_STS_ENUM MemoryCard::openFile(File * pFile_H, char * pFileName_UB, FILE_HANDLING_OPEN_MODE_ENUM Mode_E) {

    // Check if file already opened
    if (GL_FileData_X.isOpened) {
        if (strcmp(pFileName_UB, pFileName_UB))
            return FILE_HANDLING_STS_CANNOT_OPEN;       // another file is opened
        else
            return FILE_HANDLING_STS_ALREADY_OPENED;    // the file is already opened
    }

    // Open file
    GL_FileData_X.pFileName_UB = pFileName_UB;
    GL_FileData_X.file_H = (Mode_E == FILE_HANDLING_OPEN_MODE_READ) ? GL_pCard_H->open(pFileName_UB, FILE_READ) : GL_pCard_H->open(pFileName_UB, FILE_WRITE);
    GL_FileData_X.isOpened = true;

    pFile_H = &(GL_FileData_X.file_H);

    return FILE_HANDLING_STS_OK;
}

FILE_HANDLING_STS_ENUM MemoryCard::openFile(File * pFile_H, String FileName_Str, FILE_HANDLING_OPEN_MODE_ENUM Mode_E) { return (openFile(pFile_H, FileName_Str.c_str(), Mode_E)); }

FILE_HANDLING_STS_ENUM MemoryCard::closeFile(File * pFile_H) {

    pFile_H->close();
    GL_FileData_X.file_H = File();  // empty constructor
    GL_FileData_X.isOpened = false;

    return FILE_HANDLING_STS_OK;
}

FILE_HANDLING_STS_ENUM MemoryCard::writeFile(File * pFile_H, char * pWriteDate_UB) {

    if (!(GL_FileData_X.isOpened))
        return FILE_HANDLING_STS_IS_NOT_OPENED;

    pFile_H->write(pWriteDate_UB);

    return FILE_HANDLING_STS_OK;
}

FILE_HANDLING_STS_ENUM MemoryCard::writeFile(File * pFile_H, String pWriteDate_Str) { return (writeFile(pFile_H, pWriteDate_Str.c_str())); }


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
