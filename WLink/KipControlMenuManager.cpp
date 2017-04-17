/* ******************************************************************************** */
/*                                                                                  */
/* KipControlMenuManager.cpp											            */
/*                                                                                  */
/* Description :                                                                    */
/*		Describes the state machine to manage the menu of the KipControl            */ 
/*      application                                                                 */
/*                                                                                  */
/* History :  	16/03/2017  (RW)	Creation of this file                           */
/*                                                                                  */
/* ******************************************************************************** */

#define MODULE_NAME		"KipControlMenuManager"

/* ******************************************************************************** */
/* Include
/* ******************************************************************************** */

#include "KipControlMenuManager.h"
#include "KipControlMenuItemText.h"

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
enum KCMENU_STATE {
    KCMENU_IDLE,
    KCMENU_MENU,
    KCMENU_INFO,
    KCMENU_PARAM
};

static KCMENU_STATE GL_KipControlMenuManager_CurrentState_E = KCMENU_STATE::KCMENU_IDLE;
static boolean GL_KipControlMenuManagerEnabled_B = false;

static KCMENU_ITEM_STRUCT GL_pKipControlMenuItem_X[KCMENU_ITEM_NUMBER];
static KCMENU_ITEM_STRUCT * GL_pKipControlMenuCurrentItem_X;

static boolean GL_pNavButtonPressed_B[4] = { false, false, false, false };

static boolean GL_KeyPressed_B = false;
static char GL_Key_UB = 0x00;
static unsigned char GL_pParam_UB[16];
static unsigned long GL_ParamIndex_UL = 0;

static unsigned long GL_AbsoluteTime_UL = 0;

/* ******************************************************************************** */
/* Prototypes for Internal Functions
/* ******************************************************************************** */
static void InitMenuItem(void);

static void ProcessIdle(void);
static void ProcessMenu(void);
static void ProcessInfo(void);
static void ProcessParam(void);

static void TransitionToIdle(void);
static void TransitionToMenu(void);
static void TransitionToInfo(void);
static void TransitionToParam(void);


/* ******************************************************************************** */
/* Prototypes for Callbacks
/* ******************************************************************************** */
static void KipControlMenuCallback_OnUp(char * pKey_UB);
static void KipControlMenuCallback_OnDown(char * pKey_UB);
static void KipControlMenuCallback_OnEnter(char * pKey_UB);
static void KipControlMenuCallback_OnBack(char * pKey_UB);
static void KipControlMenuCallback_OnNumericKey(char * pKey_UB);
static void KipControlMenuCallback_ResetFlags(void);


static void KipControlMenu_AssignResetCallbacks(void);
static void KipControlMenu_AssignNumericKeyCallbacks(void);
static void KipControlMenu_AssignEnterBackCallbacks(void);
static void KipControlMenu_AssignNavigationCallbacks(void);


/* ******************************************************************************** */
/* Prototypes for Managing Items
/* ******************************************************************************** */
static void KipControlMenu_DisplayItem(KCMENU_ITEM_STRUCT * pMenuItem_X);


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void KipControlMenuManager_Init() {
    GL_KipControlMenuManagerEnabled_B = false;

    // Initialize Navigation Button Flags
    KipControlMenuCallback_ResetFlags();

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "KipControl Menu Manager Initialized");
}

void KipControlMenuManager_Enable() {
    GL_KipControlMenuManagerEnabled_B = true;
}

void KipControlMenuManager_Disable() {
    GL_KipControlMenuManagerEnabled_B = false;
}

void KipControlMenuManager_Process() {
    switch (GL_KipControlMenuManager_CurrentState_E) {
    case KCMENU_IDLE:
        ProcessIdle();
        break;

    case KCMENU_MENU:
        ProcessMenu();
        break;

    case KCMENU_INFO:
        ProcessInfo();
        break;

    case KCMENU_PARAM:
        ProcessParam();
        break;
    }

    // Reset Flags to avoid unwanted jumps
    KipControlMenuCallback_ResetFlags();
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
void InitMenuItem(void) {

    /* Default Values */
    for (int i = 0; i < KCMENU_ITEM_NUMBER; i++) {
        GL_pKipControlMenuItem_X[i].NavIndex_UL = 0;
        GL_pKipControlMenuItem_X[i].ppOnNavItem[KCMENU_NAVBUTTON_DOWN] = &(GL_pKipControlMenuItem_X[KCMENU_ITEM_NULL]);
        GL_pKipControlMenuItem_X[i].ppOnNavItem[KCMENU_NAVBUTTON_UP] = &(GL_pKipControlMenuItem_X[KCMENU_ITEM_NULL]);
        GL_pKipControlMenuItem_X[i].ppOnNavItem[KCMENU_NAVBUTTON_ENTER] = &(GL_pKipControlMenuItem_X[KCMENU_ITEM_NULL]);
        GL_pKipControlMenuItem_X[i].ppOnNavItem[KCMENU_NAVBUTTON_BACK] = &(GL_pKipControlMenuItem_X[KCMENU_ITEM_NULL]);
        GL_pKipControlMenuItem_X[i].AdditionalParam_UL = 0;
        GL_pKipControlMenuItem_X[i].pFct_OnValidateParam = DefaultOnValidateFct;
    }

    /* > Null Item */
    GL_pKipControlMenuItem_X[KCMENU_ITEM_NULL].Type_E = KCMENU_ITEM_TYPE_NULL;
    GL_pKipControlMenuItem_X[KCMENU_ITEM_NULL].ppText_UB[0] = GL_ppKipControlMenuItemText_Str[KCMENU_ITEM_NULL][GL_GlobalConfig_X.Language_E].c_str();
    GL_pKipControlMenuItem_X[KCMENU_ITEM_NULL].ppText_UB[1] = GL_ppKipControlMenuItemText_Str[KCMENU_ITEM_NULL][GL_GlobalConfig_X.Language_E].c_str();

}


void ProcessIdle(void) {
    if (GL_KipControlMenuManagerEnabled_B) {
        InitMenuItem();
        GL_AbsoluteTime_UL = millis();
        //TransitionToWelcomeScreen();
    }
}

//void ProcessWelcomeScreen(void) {
//
//    if (((millis() - GL_AbsoluteTime_UL) >= 5000) || (GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_ENTER])) {
//
//        // Enter Idle Screen after Welcome Screen
//        GL_pKipControlMenuCurrentItem_X = GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_ENTER];
//        KipControlMenu_DisplayItem(GL_pKipControlMenuCurrentItem_X);
//        TransitionToInfo();
//
//    }
//}

void ProcessMenu(void) {

    // > DOWN : Go to Next Menu
    if (GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_DOWN]) {

        if (GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_DOWN]->Type_E == KCMENU_ITEM_TYPE_MENU) {
            GL_pKipControlMenuCurrentItem_X = GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_DOWN];
            KipControlMenu_DisplayItem(GL_pKipControlMenuCurrentItem_X);
        }

    } // > UP : Go to Previous Menu
    else if (GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_UP]) {

        if (GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_UP]->Type_E == KCMENU_ITEM_TYPE_MENU) {
            GL_pKipControlMenuCurrentItem_X = GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_UP];
            KipControlMenu_DisplayItem(GL_pKipControlMenuCurrentItem_X);
        }

    }  // > ENTER : Go inside Menu
    else if (GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_ENTER]) {

        if (GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_ENTER]->Type_E != KCMENU_ITEM_TYPE_NULL) {
            GL_pKipControlMenuCurrentItem_X = GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_ENTER];
            KipControlMenu_DisplayItem(GL_pKipControlMenuCurrentItem_X);

            // Change state if no Menu anymore
            if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_INFO)
                TransitionToInfo();
            else if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_PARAM)
                TransitionToParam();

        }

    } // > BACK : Go to Previous Screen
    else if (GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_BACK]) {

        if (GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_BACK]->Type_E != KCMENU_ITEM_TYPE_NULL) {
            GL_pKipControlMenuCurrentItem_X = GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_BACK];
            KipControlMenu_DisplayItem(GL_pKipControlMenuCurrentItem_X);

            // Change state if no Menu anymore
            if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_INFO)
                TransitionToInfo();
            else if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_PARAM)
                TransitionToParam();

        }

    }

}


void ProcessInfo(void) {

    /* =================================== */
    /* Define Displaying according to Info */
    /* =================================== */
    switch (GL_pKipControlMenuCurrentItem_X->AdditionalParam_UL) {


    default:
        break;

    }

    // > BACK : Quit Screen
    if (GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_BACK]) {

        if (GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_BACK]->Type_E != KCMENU_ITEM_TYPE_NULL) {
            GL_pKipControlMenuCurrentItem_X = GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_BACK];
            KipControlMenu_DisplayItem(GL_pKipControlMenuCurrentItem_X);

            // Change state if no Info anymore
            if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_MENU)
                TransitionToMenu();
            else if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_PARAM)
                TransitionToParam();

        }

    } // > ENTER : Quit Screen
    else if (GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_ENTER]) {

        if (GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_ENTER]->Type_E != KCMENU_ITEM_TYPE_NULL) {
            GL_pKipControlMenuCurrentItem_X = GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_ENTER];
            KipControlMenu_DisplayItem(GL_pKipControlMenuCurrentItem_X);

            // Change state if no Info anymore
            if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_MENU)
                TransitionToMenu();
            else if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_PARAM)
                TransitionToParam();

        }

    }

}

void ProcessParam(void) {

    unsigned long ColIdx_UL = 0;

    /* ===================================== */
    /* Define Action according to Parameters */
    /* ===================================== */
    switch (GL_pKipControlMenuCurrentItem_X->AdditionalParam_UL) {


        // Default
        // -------
    default:
        GL_KeyPressed_B = false;
        break;
    }




    // > BACK : Quit Screen
    if (GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_BACK]) {

        // Disable Cursor on LCD and Reset Parameters
        GL_GlobalData_X.Lcd_H.disableCursor();
        GL_ParamIndex_UL = 0;

        if (GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_BACK]->Type_E != KCMENU_ITEM_TYPE_NULL) {
            GL_pKipControlMenuCurrentItem_X = GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_BACK];
            KipControlMenu_DisplayItem(GL_pKipControlMenuCurrentItem_X);

            // Change state if no Param anymore
            if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_MENU)
                TransitionToMenu();
            else if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_INFO)
                TransitionToInfo();

        }

    } // > ENTER : Quit Screen
    else if (GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_ENTER]) {

        // Disable Cursor on LCD and Reset Parameters
        GL_GlobalData_X.Lcd_H.disableCursor();
        GL_ParamIndex_UL = 0;

        if (GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_ENTER]->Type_E != KCMENU_ITEM_TYPE_NULL) {

            // Call OnValidate Function
            GL_pKipControlMenuCurrentItem_X->pFct_OnValidateParam(GL_pParam_UB);

            // Change Screen
            GL_pKipControlMenuCurrentItem_X = GL_pKipControlMenuCurrentItem_X->ppOnNavItem[KCMENU_NAVBUTTON_ENTER];
            KipControlMenu_DisplayItem(GL_pKipControlMenuCurrentItem_X);

            // Change state if no Param anymore
            if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_MENU)
                TransitionToMenu();
            else if (GL_pKipControlMenuCurrentItem_X->Type_E == KCMENU_ITEM_TYPE_INFO)
                TransitionToInfo();

        }

    }

}


void TransitionToIdle(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_KipControlMenuManager_CurrentState_E = KCMENU_STATE::KCMENU_IDLE;
}

//void TransitionToWelcomeScreen(void) {
//    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WELCOME SCREEN");
//
//    GL_pKipControlMenuCurrentItem_X = &(GL_pKipControlMenuItem_X[KCMENU_ITEM_WELCOME_SCREEN]);
//    KipControlMenu_DisplayItem(GL_pKipControlMenuCurrentItem_X);
//    KipControlMenu_AssignEnterBackCallbacks();
//
//    GL_KipControlMenuManager_CurrentState_E = KCMENU_STATE::KCMENU_WELCOME_SCREEN;
//}

void TransitionToMenu(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To MENU");

    // Navigation
    KipControlMenu_AssignNavigationCallbacks();

    GL_KipControlMenuManager_CurrentState_E = KCMENU_STATE::KCMENU_MENU;
}

void TransitionToInfo(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To INFO");

    // Only ENTER and BACK
    KipControlMenu_AssignEnterBackCallbacks();

    GL_KipControlMenuManager_CurrentState_E = KCMENU_STATE::KCMENU_INFO;
}

void TransitionToParam(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To PARAM");

    // Numeric Key
    KipControlMenu_AssignNumericKeyCallbacks();

    GL_KipControlMenuManager_CurrentState_E = KCMENU_STATE::KCMENU_PARAM;
}


/* ******************************************************************************** */
/* Manage Items
/* ******************************************************************************** */
void KipControlMenu_DisplayItem(KCMENU_ITEM_STRUCT * pMenuItem_X) {

    // Clear Display
    GL_GlobalData_X.Lcd_H.clearDisplay();

    // Display depends on the Item Type
    switch (pMenuItem_X->Type_E) {

        // NULL
        // > Do nothing
    case KCMENU_ITEM_TYPE_NULL:
        break;

        // INFO
        // > Display the two lines of text
    case KCMENU_ITEM_TYPE_INFO:
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppText_UB[0]);
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, pMenuItem_X->ppText_UB[1]);
        break;

        // PARAM
        // > Display the first line of text as the Menu text
        // > Display the second line of text as the Param text
    case KCMENU_ITEM_TYPE_PARAM:
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppOnNavItem[KCMENU_NAVBUTTON_BACK]->ppText_UB[0]);
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, pMenuItem_X->ppText_UB[0]);
        break;

        // MENU
        // > Display the first line of text as the Menu text
        // > Display the second line of text as the Next Menu text
    case KCMENU_ITEM_TYPE_MENU:
        if (((pMenuItem_X->NavIndex_UL) % 2) == 0) {
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 0, ">");
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, pMenuItem_X->ppOnNavItem[KCMENU_NAVBUTTON_DOWN]->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 0, " ");
        }
        else {
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppOnNavItem[KCMENU_NAVBUTTON_UP]->ppText_UB[0]);
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
void KipControlMenuCallback_ResetFlags(void) {
    for (int i = 0; i < 4; i++)
        GL_pNavButtonPressed_B[i] = false;
}


void KipControlMenuCallback_OnUp(char * pKey_UB) {
    GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_UP] = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button UP Pressed");
}

void KipControlMenuCallback_OnDown(char * pKey_UB) {
    GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_DOWN] = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button DOWN Pressed");
}

void KipControlMenuCallback_OnEnter(char * pKey_UB) {
    GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_ENTER] = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button ENTER/RIGHT");
}

void KipControlMenuCallback_OnBack(char * pKey_UB) {
    GL_pNavButtonPressed_B[KCMENU_NAVBUTTON_BACK] = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button BACK/LEFT");
}

void KipControlMenuCallback_OnNumericKey(char * pKey_UB) {
    GL_KeyPressed_B = true;
    GL_Key_UB = *pKey_UB;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Numeric key pressed : " + String(GL_Key_UB));
}


void KipControlMenu_AssignResetCallbacks(void) {
    for (int i = 0; i < 16; i++)
        GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent((FLAT_PANEL_KEY_ENUM)i, DefaultKeyEvents);
}

void KipControlMenu_AssignNumericKeyCallbacks(void) {
    // Reset callbacks
    KipControlMenu_AssignResetCallbacks();

    // Assign numeric keys
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_0, KipControlMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_1, KipControlMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_2, KipControlMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_3, KipControlMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_4, KipControlMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_5, KipControlMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_6, KipControlMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_7, KipControlMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_8, KipControlMenuCallback_OnNumericKey);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_9, KipControlMenuCallback_OnNumericKey);

    // Assign navigation Enter and Back keys
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_CLEAR, KipControlMenuCallback_OnBack);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_VALIDATE, KipControlMenuCallback_OnEnter);
}

void KipControlMenu_AssignEnterBackCallbacks(void) {
    // Reset callbacks
    KipControlMenu_AssignResetCallbacks();

    // Assign navigation Enter and Back keys
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_CLEAR, KipControlMenuCallback_OnBack);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_VALIDATE, KipControlMenuCallback_OnEnter);
}

void KipControlMenu_AssignNavigationCallbacks(void) {
    // Reset callbacks
    KipControlMenu_AssignResetCallbacks();

    // Assign navigation with numeric keys
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_2, KipControlMenuCallback_OnUp);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_8, KipControlMenuCallback_OnDown);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_4, KipControlMenuCallback_OnBack);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_6, KipControlMenuCallback_OnEnter);

    // Assign navigation Enter and Back keys
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_CLEAR, KipControlMenuCallback_OnBack);
    GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_VALIDATE, KipControlMenuCallback_OnEnter);
}
