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

#include "Debug.h"


/* ******************************************************************************** */
/* Extenral Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define WMENU_ITEM_NUMBER                   16

#define WMENU_ITEM_NULL                     0
#define WMENU_ITEM_WELCOME_SCREEN           1
#define WMENU_ITEM_PARAM                    2
#define WMENU_ITEM_PARAM_LCD                3
#define WMENU_ITEM_PARAM_LCD_BACKLIGHT      4
#define WMENU_ITEM_PARAM_DATETIME           5
#define WMENU_ITEM_PARAM_DATETIME_DATE      6
#define WMENU_ITEM_PARAM_DATETIME_TIME      7

#define WMENU_ITEM_MENU1                    8
 

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

static unsigned long GL_NavButtonPressed_UL = WMENU_NAVBUTTON_NONE;

static String GL_WMenuTextRevisionId_Str = "";

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
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


static void WMenu_AssignResetCallbacks(void);
static void WMenu_AssignNumericKeyCallbacks(void);
static void WMenu_AssignEnterBackCallbacks(void);
static void WMenu_AssignNavigationCallbacks(void);


/* ******************************************************************************** */
/* Prototypes for Managing Items
/* ******************************************************************************** */
static WMENU_ITEM_TYPE_ENUM WMenu_GetNextItem(WMENU_ITEM_STRUCT * pCurrentMenuItem_X);
static void WMenu_DisplayItem(WMENU_ITEM_STRUCT * pMenuItem_X);


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void WMenuManager_Init() {
    GL_WMenuManagerEnabled_B = false;
    GL_WMenuTextRevisionId_Str = "> " + GL_GlobalData_X.RevisionId_Str;

    /* ==================== */
    /* Initialize Menu Item */
    /* ==================== */
    for (int i = 0; i < WMENU_ITEM_NUMBER; i++) {
        GL_pWMenuItem_X[i].NavIndex_UL                          = 0;
        GL_pWMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_UP]    = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_DOWN]  = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_BACK]  = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
    }

    /* > Null Item */
    GL_pWMenuItem_X[WMENU_ITEM_NULL].Type_E = WMENU_ITEM_TYPE_NULL;
    GL_pWMenuItem_X[WMENU_ITEM_NULL].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_NULL][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_NULL].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_NULL][GL_GlobalConfig_X.Language_E].c_str();


    /* 0. Welcom Screen */
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].Type_E = WMENU_ITEM_TYPE_INFO;
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_PARAM]);     // On ENTER -> Go to PARAMETERS Menu
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_WELCOME_SCREEN][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].ppText_UB[1] = GL_WMenuTextRevisionId_Str.c_str();

    /* 0.0. Settings */
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].Type_E        = WMENU_ITEM_TYPE_MENU;
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].NavIndex_UL   = 0;
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].ppOnNavItem_X[WMENU_NAVBUTTON_DOWN] = &(GL_pWMenuItem_X[WMENU_ITEM_MENU1]);
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN]);
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].ppText_UB[0]  = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_PARAMETERS][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].ppText_UB[1]  = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_NULL][GL_GlobalConfig_X.Language_E].c_str();

    /* 0.1. Menu1 */
    GL_pWMenuItem_X[WMENU_ITEM_MENU1].Type_E        = WMENU_ITEM_TYPE_MENU;
    GL_pWMenuItem_X[WMENU_ITEM_MENU1].NavIndex_UL   = 1;
    GL_pWMenuItem_X[WMENU_ITEM_MENU1].ppOnNavItem_X[WMENU_NAVBUTTON_UP]     = &(GL_pWMenuItem_X[WMENU_ITEM_PARAM]);
    GL_pWMenuItem_X[WMENU_ITEM_MENU1].ppOnNavItem_X[WMENU_NAVBUTTON_BACK]   = &(GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN]);
    GL_pWMenuItem_X[WMENU_ITEM_MENU1].ppText_UB[0]  = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_MENU1][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_MENU1].ppText_UB[1]  = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_NULL][GL_GlobalConfig_X.Language_E].c_str();


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
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
    if (GL_WMenuManagerEnabled_B)
        TransitionToWelcomeScreen();
}

void ProcessWelcomeScreen(void) {
    if (GL_NavButtonPressed_UL == WMENU_NAVBUTTON_ENTER) {
        GL_NavButtonPressed_UL = WMENU_NAVBUTTON_NONE;
        if (((WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_ENTER]))->Type_E == WMENU_ITEM_TYPE_MENU) {
            GL_pWMenuCurrentItem_X = (WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_ENTER]);
            WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
            TransitionToMenu();
        }
    }
}

void ProcessMenu(void) {

    if (GL_NavButtonPressed_UL != WMENU_NAVBUTTON_NONE) {

        GL_pWMenuCurrentItem_X = (WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->ppOnNavItem_X[GL_NavButtonPressed_UL]);
        WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
        GL_NavButtonPressed_UL = WMENU_NAVBUTTON_NONE;

    }

    //if (WMenu_GetNextItem(GL_pWMenuCurrentItem_X) == WMENU_ITEM_TYPE_INFO) {
    //    WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
    //    TransitionToInfo();
    //}
    //else if (WMenu_GetNextItem(GL_pWMenuCurrentItem_X) == WMENU_ITEM_TYPE_PARAM) {
    //    WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
    //    TransitionToParam();
    //}
    //else if (WMenu_GetNextItem(GL_pWMenuCurrentItem_X) == WMENU_ITEM_TYPE_MENU) {
    //    WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
    //}

}

void ProcessInfo(void) {

    if (WMenu_GetNextItem(GL_pWMenuCurrentItem_X) == WMENU_ITEM_TYPE_INFO) {
        WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
    }
    else if (WMenu_GetNextItem(GL_pWMenuCurrentItem_X) == WMENU_ITEM_TYPE_PARAM) {
        WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
        TransitionToParam();
    }
    else if (WMenu_GetNextItem(GL_pWMenuCurrentItem_X) == WMENU_ITEM_TYPE_MENU) {
        WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
    }

}

void ProcessParam(void) {

    if (WMenu_GetNextItem(GL_pWMenuCurrentItem_X) == WMENU_ITEM_TYPE_INFO) {
        WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
        TransitionToInfo();
    }
    else if (WMenu_GetNextItem(GL_pWMenuCurrentItem_X) == WMENU_ITEM_TYPE_PARAM) {
        WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
    }
    else if (WMenu_GetNextItem(GL_pWMenuCurrentItem_X) == WMENU_ITEM_TYPE_MENU) {
        WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
        TransitionToMenu();
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

    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_MENU;
}

void TransitionToParam(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To PARAM");

    // Numeric Key
    WMenu_AssignNumericKeyCallbacks();

    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_MENU;
}


/* ******************************************************************************** */
/* Manage Items
/* ******************************************************************************** */
WMENU_ITEM_TYPE_ENUM WMenu_GetNextItem(WMENU_ITEM_STRUCT * pCurrentMenuItem_X) {
    WMENU_ITEM_TYPE_ENUM ReturnType_E = WMENU_ITEM_TYPE_NULL;

    // If Button is Pressed
        // Then 1st get Type of next Item
        // Then assign new current Item

    //if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER]) {
    //    GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER] = false;
    //    ReturnType_E = ((WMENU_ITEM_STRUCT *)(pCurrentMenuItem_X->pOnEnterItem_X))->Type_E;
    //    pCurrentMenuItem_X = (WMENU_ITEM_STRUCT *)(pCurrentMenuItem_X->pOnEnterItem_X);
    //}
    //else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK]) {
    //    GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK] = false;
    //    ReturnType_E = ((WMENU_ITEM_STRUCT *)(pCurrentMenuItem_X->pOnBackItem_X))->Type_E;
    //    pCurrentMenuItem_X = (WMENU_ITEM_STRUCT *)(pCurrentMenuItem_X->pOnBackItem_X);
    //}
    //else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_UP]) {
    //    GL_pNavButtonPressed_B[WMENU_NAVBUTTON_UP] = false;
    //    ReturnType_E = ((WMENU_ITEM_STRUCT *)(pCurrentMenuItem_X->pOnUpItem_X))->Type_E;
    //    pCurrentMenuItem_X = (WMENU_ITEM_STRUCT *)(pCurrentMenuItem_X->pOnUpItem_X);
    //}
    //else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_DOWN]) {
    //    GL_pNavButtonPressed_B[WMENU_NAVBUTTON_DOWN] = false;
    //    ReturnType_E = ((WMENU_ITEM_STRUCT *)(pCurrentMenuItem_X->pOnDownItem_X))->Type_E;
    //    pCurrentMenuItem_X = (WMENU_ITEM_STRUCT *)(pCurrentMenuItem_X->pOnDownItem_X);
    //}

    return ReturnType_E;
}


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
    // > Display the first line of text and clear the second one
    case WMENU_ITEM_TYPE_PARAM:
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppText_UB[0]);
        GL_GlobalData_X.Lcd_H.clearDisplay(LCD_DISPLAY_LINE2);
        break;

    // MENU
    // > Display the first line of text as the Menu text
    // > Display the second line of text as the Next Menu text
    case WMENU_ITEM_TYPE_MENU:
        if (((pMenuItem_X->NavIndex_UL) % 2) == 0) {
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 0, ">");
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, ((WMENU_ITEM_STRUCT *)pMenuItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_DOWN])->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 0, " ");
        }
        else {
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, ((WMENU_ITEM_STRUCT *)pMenuItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_UP])->ppText_UB[0]);
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
void WMenuCallback_OnUp(char * pKey_UB) {
    GL_NavButtonPressed_UL = WMENU_NAVBUTTON_UP;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button UP Pressed");
}

void WMenuCallback_OnDown(char * pKey_UB) {
    GL_NavButtonPressed_UL = WMENU_NAVBUTTON_DOWN;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button DOWN Pressed");
}

void WMenuCallback_OnEnter(char * pKey_UB) {
    GL_NavButtonPressed_UL = WMENU_NAVBUTTON_ENTER;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button ENTER/RIGHT");
}

void WMenuCallback_OnBack(char * pKey_UB) {
    GL_NavButtonPressed_UL = WMENU_NAVBUTTON_BACK;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button BACK/LEFT");
}

void WMenuCallback_OnNumericKey(char * pKey_UB) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Numeric key pressed");
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
