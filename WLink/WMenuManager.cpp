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


#define WMENU_NAVBUTTON_UP                  0
#define WMENU_NAVBUTTON_DOWN                1
#define WMENU_NAVBUTTON_ENTER               2
#define WMENU_NAVBUTTON_BACK                3
    

/* ******************************************************************************** */
/* Local Variables
/* ******************************************************************************** */
enum WMENU_STATE {
    WMENU_IDLE,
    WMENU_MENU
};

static WMENU_STATE GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_IDLE;
static boolean GL_WMenuManagerEnabled_B = false;

static WMENU_ITEM_STRUCT GL_pWMenuItem_X[WMENU_ITEM_NUMBER];
static WMENU_ITEM_STRUCT * GL_pWMenuCurrentItem_X;

static boolean GL_pNavButtonPressed_B[] = { false, false, false, false };

static String GL_WMenuTextRevisionId_Str = "";

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void ProcessIdle(void);
static void ProcessMenu(void);

static void TransitionToIdle(void);
static void TransitionToMenu(void);


/* ******************************************************************************** */
/* Prototypes for Callbacks
/* ******************************************************************************** */
static void WMenuCallback_OnUp(char * pKey_UB);
static void WMenuCallback_OnDown(char * pKey_UB);
static void WMenuCallback_OnEnter(char * pKey_UB);
static void WMenuCallback_OnBack(char * pKey_UB);


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
        GL_pWMenuItem_X[i].pOnUpItem_X      = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].pOnDownItem_X    = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].pOnEnterItem_X   = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].pOnBackItem_X    = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
    }

    /* > Null Item */
    GL_pWMenuItem_X[WMENU_ITEM_NULL].Type_E = WMENU_ITEM_TYPE_NULL;
    GL_pWMenuItem_X[WMENU_ITEM_NULL].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_NULL][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_NULL].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_NULL][GL_GlobalConfig_X.Language_E].c_str();


    /* 0. Welcom Screen */
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].Type_E = WMENU_ITEM_TYPE_INFO;
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].pOnEnterItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_PARAM]);     // On ENTER -> Go to PARAMETERS Menu
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_WELCOME_SCREEN][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].ppText_UB[1] = GL_WMenuTextRevisionId_Str.c_str();

    /* 0.0. Settings */
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].Type_E = WMENU_ITEM_TYPE_MENU;
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].pOnDownItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_MENU1]);
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].pOnBackItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN]);
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].ppText_UB[0]  = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_PARAMETERS][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_PARAM].ppText_UB[1]  = GL_ppWMenuItemText_Str[WMENU_ITEM_TEXT_NULL][GL_GlobalConfig_X.Language_E].c_str();

    /* 0.1. Menu1 */
    GL_pWMenuItem_X[WMENU_ITEM_MENU1].Type_E = WMENU_ITEM_TYPE_MENU;
    GL_pWMenuItem_X[WMENU_ITEM_MENU1].pOnUpItem_X   = &(GL_pWMenuItem_X[WMENU_ITEM_PARAM]);
    GL_pWMenuItem_X[WMENU_ITEM_MENU1].pOnBackItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN]);
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

    case WMENU_MENU:
        ProcessMenu();
    }
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */

void ProcessIdle(void) {
    if (GL_WMenuManagerEnabled_B)
        TransitionToMenu();
}

void ProcessMenu(void) {

    if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER]) {
        GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER] = false;
        if (((WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->pOnEnterItem_X))->Type_E != WMENU_ITEM_TYPE_NULL) {
            GL_pWMenuCurrentItem_X = (WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->pOnEnterItem_X);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, GL_pWMenuCurrentItem_X->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, GL_pWMenuCurrentItem_X->ppText_UB[1]);
        }
    }
    else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK]) {
        GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK] = false;
        if (((WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->pOnBackItem_X))->Type_E != WMENU_ITEM_TYPE_NULL) {
            GL_pWMenuCurrentItem_X = (WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->pOnBackItem_X);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, GL_pWMenuCurrentItem_X->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, GL_pWMenuCurrentItem_X->ppText_UB[1]);
        }
    }
    else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_UP]) {
        GL_pNavButtonPressed_B[WMENU_NAVBUTTON_UP] = false;
        if (((WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->pOnUpItem_X))->Type_E != WMENU_ITEM_TYPE_NULL) {
            GL_pWMenuCurrentItem_X = (WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->pOnUpItem_X);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, GL_pWMenuCurrentItem_X->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, GL_pWMenuCurrentItem_X->ppText_UB[1]);
        }
    }
    else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_DOWN]) {
        GL_pNavButtonPressed_B[WMENU_NAVBUTTON_DOWN] = false;
        if (((WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->pOnDownItem_X))->Type_E != WMENU_ITEM_TYPE_NULL) {
            GL_pWMenuCurrentItem_X = (WMENU_ITEM_STRUCT *)(GL_pWMenuCurrentItem_X->pOnDownItem_X);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, GL_pWMenuCurrentItem_X->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, GL_pWMenuCurrentItem_X->ppText_UB[1]);
        }
    }
 
}


void TransitionToIdle(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_IDLE;
}

void TransitionToMenu(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To MENU");

    GL_pWMenuCurrentItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN]);
    GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, GL_pWMenuCurrentItem_X->ppText_UB[0]);
    GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, GL_pWMenuCurrentItem_X->ppText_UB[1]);

    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_2, WMenuCallback_OnUp);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_8, WMenuCallback_OnDown);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_4, WMenuCallback_OnBack);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_6, WMenuCallback_OnEnter);

    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_CLEAR, WMenuCallback_OnBack);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_VALIDATE, WMenuCallback_OnEnter);

    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_MENU;
}



/* ******************************************************************************** */
/* Callbacks
/* ******************************************************************************** */
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


