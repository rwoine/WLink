/* ******************************************************************************** */
/*                                                                                  */
/* WMenuManager.cpp														            */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the menu of the W-Link       			*/
/*                                                                                  */
/* History :  	16/03/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"WMenuManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "WMenuManager.h"
#include "WMenuItemText.h"
#include "WConfigManager.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Extenral Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
 

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum WMENU_STATE {
    WMENU_IDLE,
    WMENU_WELCOME_SCREEN,
    WMENU_MENU,
    WMENU_INFO,
    WMENU_PARAM
};

static WMENU_STATE GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_IDLE;
static boolean GL_WMenuManagerEnabled_B = false;

static WMENU_ITEM_STRUCT GL_pWMenuItem_X[WMENU_ITEM_NUMBER];
static WMENU_ITEM_STRUCT * GL_pWMenuCurrentItem_X;

static boolean GL_pNavButtonPressed_B[4] = {false, false, false, false};

static boolean GL_KeyPressed_B = false;
static char GL_Key_UB = 0x00;
static unsigned char GL_pParam_UB[16];

static String GL_WMenuTextRevisionId_Str = "";

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void InitMenuItem(void);

static void ProcessIdle(void);
static void ProcessWelcomeScreen(void);
static void ProcessMenu(void);
static void ProcessInfo(void);
static void ProcessParam(void);

static void TransitionToIdle(void);
static void TransitionToWelcomeScreen(void);
static void TransitionToMenu(void);
static void TransitionToInfo(void);
static void TransitionToParam(void);


/* ******************************************************************************** */
/* Prototypes for Callbacks
/* ******************************************************************************** */
static void WMenuCallback_OnUp(char * pKey_UB);
static void WMenuCallback_OnDown(char * pKey_UB);
static void WMenuCallback_OnEnter(char * pKey_UB);
static void WMenuCallback_OnBack(char * pKey_UB);
static void WMenuCallback_OnNumericKey(char * pKey_UB);
static void WMenuCallback_ResetFlags(void);


static void WMenu_AssignResetCallbacks(void);
static void WMenu_AssignNumericKeyCallbacks(void);
static void WMenu_AssignEnterBackCallbacks(void);
static void WMenu_AssignNavigationCallbacks(void);


/* ******************************************************************************** */
/* Prototypes for Managing Items
/* ******************************************************************************** */
static void WMenu_DisplayItem(WMENU_ITEM_STRUCT * pMenuItem_X);


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void WMenuManager_Init() {
    GL_WMenuManagerEnabled_B = false;
    GL_WMenuTextRevisionId_Str = "> " + GL_GlobalData_X.RevisionId_Str;

    // Initialize Navigation Button Flags
    WMenuCallback_ResetFlags();

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "W-Link Manager Initialized");
}

void WMenuManager_Enable() {
    GL_WMenuManagerEnabled_B = true;
}

void WMenuManager_Disable() {
    GL_WMenuManagerEnabled_B = false;
}

void WMenuManager_Process() {
    switch (GL_WMenuManager_CurrentState_E) {
    case WMENU_IDLE:
        ProcessIdle();
        break;

    case WMENU_WELCOME_SCREEN:
        ProcessWelcomeScreen();
        break;

    case WMENU_MENU:
        ProcessMenu();
        break;

    case WMENU_INFO:
        ProcessInfo();
        break;

    case WMENU_PARAM:
        ProcessParam();
        break;
    }

    // Reset Flags to avoid unwanted jumps
    WMenuCallback_ResetFlags();
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
void InitMenuItem(void) {

    /* Default Values */
    for (int i = 0; i < WMENU_ITEM_NUMBER; i++) {
        GL_pWMenuItem_X[i].NavIndex_UL = 0;
        GL_pWMenuItem_X[i].ppOnNavItem[WMENU_NAVBUTTON_DOWN] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].ppOnNavItem[WMENU_NAVBUTTON_UP] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].ppOnNavItem[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].ppOnNavItem[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].AdditionalParam_UL = 0;
        GL_pWMenuItem_X[i].pFct_OnValidateParam = DefaultOnValidateFct;
    }

    /* > Null Item */
    GL_pWMenuItem_X[WMENU_ITEM_NULL].Type_E = WMENU_ITEM_TYPE_NULL;
    GL_pWMenuItem_X[WMENU_ITEM_NULL].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_NULL][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_NULL].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_NULL][GL_GlobalConfig_X.Language_E].c_str();


    /* 0. Welcom Screen */
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].Type_E = WMENU_ITEM_TYPE_INFO;
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].ppOnNavItem[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS]);
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_WELCOME_SCREEN][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].ppText_UB[1] = GL_WMenuTextRevisionId_Str.c_str();

    /* 0.0. Settings */
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].Type_E = WMENU_ITEM_TYPE_MENU;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].NavIndex_UL = 0;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].ppOnNavItem[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].ppOnNavItem[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_NULL][GL_GlobalConfig_X.Language_E].c_str();

    /* 0.0.0. Settings > Languages */
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].Type_E = WMENU_ITEM_TYPE_MENU;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].NavIndex_UL = 0;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].ppOnNavItem[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].ppOnNavItem[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].ppOnNavItem[WMENU_NAVBUTTON_DOWN] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_LANGUAGE][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_NULL][GL_GlobalConfig_X.Language_E].c_str();

    /* 0.0.0.0. Settings > Languages > Selection */
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].Type_E = WMENU_ITEM_TYPE_PARAM;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].NavIndex_UL = 0;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].ppOnNavItem[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].ppOnNavItem[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_NULL][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].AdditionalParam_UL = WMENU_ITEM_SETTINGS_LANGUAGE_SELECT;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].pFct_OnValidateParam = WConfig_SetLanguage;

    /* 0.0.1. Settings > Date & Time */
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].Type_E = WMENU_ITEM_TYPE_MENU;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].NavIndex_UL = 1;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].ppOnNavItem[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].ppOnNavItem[WMENU_NAVBUTTON_UP] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].ppOnNavItem[WMENU_NAVBUTTON_DOWN] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_DATETIME][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_NULL][GL_GlobalConfig_X.Language_E].c_str();

    /* 0.0.2. Settings > LCD */
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].Type_E = WMENU_ITEM_TYPE_MENU;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].NavIndex_UL = 2;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].ppOnNavItem[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].ppOnNavItem[WMENU_NAVBUTTON_UP] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_LCD][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_NULL][GL_GlobalConfig_X.Language_E].c_str();
}


void ProcessIdle(void) {
    if (GL_WMenuManagerEnabled_B) {
        InitMenuItem();
        TransitionToWelcomeScreen();
    }
}

void ProcessWelcomeScreen(void) {
    if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER]) {

        // Enter Menu after Welcome Screen
        GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_ENTER];
        WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
        TransitionToMenu();

    }
}

void ProcessMenu(void) {

    // > DOWN : Go to Next Menu
    if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_DOWN]) {

        if (GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_DOWN]->Type_E == WMENU_ITEM_TYPE_MENU) {
            GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_DOWN];
            WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
        }

    } // > UP : Go to Previous Menu
    else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_UP]) {

        if (GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_UP]->Type_E == WMENU_ITEM_TYPE_MENU) {
            GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_UP];
            WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
        }

    }  // > ENTER : Go inside Menu
    else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER]) {
        
        if (GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_ENTER]->Type_E != WMENU_ITEM_TYPE_NULL) {
            GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_ENTER];
            WMenu_DisplayItem(GL_pWMenuCurrentItem_X);

            // Change state if no Menu anymore
            if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
                TransitionToInfo();
            else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
                TransitionToParam();

        }

    } // > BACK : Go to Previous Screen
    else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK]) {

        if (GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_BACK]->Type_E != WMENU_ITEM_TYPE_NULL) {
            GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_BACK];
            WMenu_DisplayItem(GL_pWMenuCurrentItem_X);

            // Change state if no Menu anymore
            if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
                TransitionToInfo();
            else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
                TransitionToParam();

        }

    }

}


void ProcessInfo(void) {

    // > BACK : Quit Screen
    if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK]) {

        if (GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_BACK]->Type_E != WMENU_ITEM_TYPE_NULL) {
            GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_BACK];
            WMenu_DisplayItem(GL_pWMenuCurrentItem_X);

            // Change state if no Info anymore
            if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_MENU)
                TransitionToMenu();
            else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
                TransitionToParam();

        }

    } // > ENTER : Quit Screen
    else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER]) {

        if (GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_ENTER]->Type_E != WMENU_ITEM_TYPE_NULL) {
            GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_ENTER];
            WMenu_DisplayItem(GL_pWMenuCurrentItem_X);

            // Change state if no Info anymore
            if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_MENU)
                TransitionToMenu();
            else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
                TransitionToParam();

        }

    }

}

void ProcessParam(void) {

    /* ===================================== */
    /* Define Action according to Parameters */
    /* ===================================== */
    switch (GL_pWMenuCurrentItem_X->AdditionalParam_UL) {


    // Language Selection
    // ------------------
    case WMENU_ITEM_SETTINGS_LANGUAGE_SELECT:

        if (GL_KeyPressed_B) {
            GL_KeyPressed_B = false;

            switch (GL_Key_UB) {

            case '1':   
                GL_pParam_UB[0] = (unsigned char)WLINK_LANGUAGE_EN;  
                GL_GlobalData_X.Lcd_H.enableCursor(1, 0);
                break;

            case '2':   
                GL_pParam_UB[0] = (unsigned char)WLINK_LANGUAGE_FR;
                GL_GlobalData_X.Lcd_H.enableCursor(1, 7);
                break;

            case '3':   
                GL_pParam_UB[0] = (unsigned char)WLINK_LANGUAGE_NL;
                GL_GlobalData_X.Lcd_H.enableCursor(1, 14);
                break;

            default:    
                GL_pParam_UB[0] = (unsigned char)WLINK_LANGUAGE_EN;
                GL_GlobalData_X.Lcd_H.enableCursor(1, 0);
                break;

            }
        }

        break;


    // Default
    // -------
    default:
        GL_KeyPressed_B = false;
        break;
    }




    // > BACK : Quit Screen
    if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK]) {

        // Disable Cursor on LCD
        GL_GlobalData_X.Lcd_H.disableCursor();

        if (GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_BACK]->Type_E != WMENU_ITEM_TYPE_NULL) {
            GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_BACK];
            WMenu_DisplayItem(GL_pWMenuCurrentItem_X);

            // Change state if no Param anymore
            if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_MENU)
                TransitionToMenu();
            else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
                TransitionToInfo();

        }

    } // > ENTER : Quit Screen
    else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER]) {

        // Disable Cursor on LCD
        GL_GlobalData_X.Lcd_H.disableCursor();

        if (GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_ENTER]->Type_E != WMENU_ITEM_TYPE_NULL) {

            // Call OnValidate Function
            GL_pWMenuCurrentItem_X->pFct_OnValidateParam(GL_pParam_UB);
            if(GL_pWMenuCurrentItem_X->AdditionalParam_UL == WMENU_ITEM_SETTINGS_LANGUAGE_SELECT)   InitMenuItem();     // Re-Init Menu if Language has changed

            // Change Screen
            GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem[WMENU_NAVBUTTON_ENTER];
            WMenu_DisplayItem(GL_pWMenuCurrentItem_X);

            // Change state if no Param anymore
            if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_MENU)
                TransitionToMenu();
            else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
                TransitionToInfo();

        }

    }

}


void TransitionToIdle(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_IDLE;
}

void TransitionToWelcomeScreen(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WELCOME SCREEN");

    GL_pWMenuCurrentItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN]);
    WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
    WMenu_AssignEnterBackCallbacks();

    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_WELCOME_SCREEN;
}

void TransitionToMenu(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To MENU");

    // Navigation
    WMenu_AssignNavigationCallbacks();

    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_MENU;
}

void TransitionToInfo(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To INFO");

    // Only ENTER and BACK
    WMenu_AssignEnterBackCallbacks();

    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_INFO;
}

void TransitionToParam(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To PARAM");

    // Numeric Key
    WMenu_AssignNumericKeyCallbacks();

    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_PARAM;
}


/* ******************************************************************************** */
/* Manage Items
/* ******************************************************************************** */
void WMenu_DisplayItem(WMENU_ITEM_STRUCT * pMenuItem_X) {

    // Clear Display
    GL_GlobalData_X.Lcd_H.clearDisplay();

    // Display depends on the Item Type
    switch (pMenuItem_X->Type_E) {

    // NULL
    // > Do nothing
    case WMENU_ITEM_TYPE_NULL:
        break;
        
    // INFO
    // > Display the two lines of text
    case WMENU_ITEM_TYPE_INFO:
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppText_UB[0]);
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, pMenuItem_X->ppText_UB[1]);
        break;

    // PARAM
    // > Display the first line of text as the Menu text
    // > Display the second line of text as the Param text
    case WMENU_ITEM_TYPE_PARAM:
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppOnNavItem[WMENU_NAVBUTTON_BACK]->ppText_UB[0]);
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, pMenuItem_X->ppText_UB[0]);
        break;

    // MENU
    // > Display the first line of text as the Menu text
    // > Display the second line of text as the Next Menu text
    case WMENU_ITEM_TYPE_MENU:
        if (((pMenuItem_X->NavIndex_UL) % 2) == 0) {
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 0, ">");
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, pMenuItem_X->ppOnNavItem[WMENU_NAVBUTTON_DOWN]->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 0, " ");
        }
        else {
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppOnNavItem[WMENU_NAVBUTTON_UP]->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 0, " ");
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, pMenuItem_X->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 0, ">");
        }
        break;
    }
}


/* ******************************************************************************** */
/* Callbacks
/* ******************************************************************************** */
void WMenuCallback_ResetFlags(void) {
    for (int i = 0; i < 4; i++)
        GL_pNavButtonPressed_B[i] = false;
}


void WMenuCallback_OnUp(char * pKey_UB) {
    GL_pNavButtonPressed_B[WMENU_NAVBUTTON_UP] = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button UP Pressed");
}

void WMenuCallback_OnDown(char * pKey_UB) {
    GL_pNavButtonPressed_B[WMENU_NAVBUTTON_DOWN] = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button DOWN Pressed");
}

void WMenuCallback_OnEnter(char * pKey_UB) {
    GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER] = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button ENTER/RIGHT");
}

void WMenuCallback_OnBack(char * pKey_UB) {
    GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK] = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button BACK/LEFT");
}

void WMenuCallback_OnNumericKey(char * pKey_UB) {
    GL_KeyPressed_B = true;
    GL_Key_UB = *pKey_UB;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Numeric key pressed : " + String(GL_Key_UB));
}


void WMenu_AssignResetCallbacks(void) {
    for (int i = 0; i < 16; i++)
        GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent((FLAT_PANEL_KEY_ENUM)i, DefaultKeyEvents);
}

void WMenu_AssignNumericKeyCallbacks(void) {
    // Reset callbacks
    WMenu_AssignResetCallbacks();

    // Assign numeric keys
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_0, WMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_1, WMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_2, WMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_3, WMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_4, WMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_5, WMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_6, WMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_7, WMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_8, WMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_9, WMenuCallback_OnNumericKey);

    // Assign navigation Enter and Back keys
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_CLEAR, WMenuCallback_OnBack);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_VALIDATE, WMenuCallback_OnEnter);
}

void WMenu_AssignEnterBackCallbacks(void) {
    // Reset callbacks
    WMenu_AssignResetCallbacks();

    // Assign navigation Enter and Back keys
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_CLEAR, WMenuCallback_OnBack);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_VALIDATE, WMenuCallback_OnEnter);
}

void WMenu_AssignNavigationCallbacks(void) {
    // Reset callbacks
    WMenu_AssignResetCallbacks();

    // Assign navigation with numeric keys
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_2, WMenuCallback_OnUp);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_8, WMenuCallback_OnDown);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_4, WMenuCallback_OnBack);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_6, WMenuCallback_OnEnter);

    // Assign navigation Enter and Back keys
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_CLEAR, WMenuCallback_OnBack);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_VALIDATE, WMenuCallback_OnEnter);
}
