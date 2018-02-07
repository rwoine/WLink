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
#include "WMenuItemFunction.h"

#include "Debug.h"


/* ******************************************************************************** */
/* Extenral Variables
/* ******************************************************************************** */
extern GLOBAL_PARAM_STRUCT GL_GlobalData_X;
extern GLOBAL_CONFIG_STRUCT GL_GlobalConfig_X;


/* ******************************************************************************** */
/* Define
/* ******************************************************************************** */
#define WMENU_JUMP_FROM_APP_SEQUENCE_TIMEOUT_MS		3500
#define WMENU_JUMP_TO_APP_TIMER_MS					10000
 

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
static boolean GL_pFunctionButtonPressed_B[3] = {false, false, false};

WMENU_ITEM_PARAM_STRUCT GL_ItemParam_X;
static unsigned char GL_pParam_UB[16];

static unsigned long GL_AbsoluteTime_UL = 0;
static unsigned long GL_ExAppTime_UL = 0;

static const char GL_pWMenuPassword_UB[] = {'1', '5', '9'};

typedef struct {
	unsigned long Step_UL;
	unsigned long TimeOut_UL;
	char pCharArray_UB[16];
} WMENU_FROM_APP_STRUCT;
WMENU_FROM_APP_STRUCT GL_WMenuFromApp_X;


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

static void WMenuCallback_OnFunction1(char * pKey_UB);
static void WMenuCallback_OnFunction2(char * pKey_UB);
static void WMenuCallback_OnFunction3(char * pKey_UB);

static void WMenuCallback_OnNumericKey(char * pKey_UB);
static void WMenuCallback_ResetFlags(void);


static void WMenu_AssignResetCallbacks(void);
static void WMenu_AssignNumericKeyCallbacks(void);
static void WMenu_AssignEnterBackCallbacks(void);
static void WMenu_AssignNavigationCallbacks(void);


/* ******************************************************************************** */
/* Prototypes for Managing Items
/* ******************************************************************************** */
static void WMenu_ManageJumpToApp(void);
static void WMenu_ManageJumpFromApp(void);
static void WMenu_DisplayItem(WMENU_ITEM_STRUCT * pMenuItem_X);


/* ******************************************************************************** */
/* Functions
/* ******************************************************************************** */

void WMenuManager_Init() {
    GL_WMenuManagerEnabled_B = false;

    // Initialize Navigation Button Flags
    WMenuCallback_ResetFlags();

	// Initialize Parameters Structure
	GL_ItemParam_X.pSenderItem_H = NULL;
	GL_ItemParam_X.KeyPressed_B = false;
	GL_ItemParam_X.Key_UB = 0x00;
	GL_ItemParam_X.ParamIndex_UL = 0;
	GL_ItemParam_X.pParam_UB = GL_pParam_UB;
	for (int i = 0; i < sizeof(GL_pParam_UB); i++)
		GL_pParam_UB[i] = 0x00;

    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "W-Link Menu Manager Initialized");
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

	if ((!(GL_pWMenuCurrentItem_X->IsFromApp_B)) && (GL_GlobalConfig_X.App_X.hasApplication_B))
		WMenu_ManageJumpToApp();

    // Reset Flags to avoid unwanted jumps
    WMenuCallback_ResetFlags();
}


/* ******************************************************************************** */
/* Internal Functions
/* ******************************************************************************** */
void InitMenuItem(void) {

	/* -------------- */
    /* Default Values */
	/* -------------- */
    for (int i = 0; i < WMENU_ITEM_NUMBER; i++) {

		GL_pWMenuItem_X[i].Type_E = WMENU_ITEM_TYPE_NULL;
		GL_pWMenuItem_X[i].Id_UL = WMENU_ITEM_NULL;
        GL_pWMenuItem_X[i].NavIndex_UL = 0;
		GL_pWMenuItem_X[i].IsFromApp_B = false;
		GL_pWMenuItem_X[i].TimerValue_UL = 0;

		GL_pWMenuItem_X[i].ppText_UB[0] = "";
		GL_pWMenuItem_X[i].ppText_UB[1] = "";

        GL_pWMenuItem_X[i].pErrorItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);

		GL_pWMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_UP] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
		GL_pWMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_DOWN] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
        GL_pWMenuItem_X[i].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);

		GL_pWMenuItem_X[i].ppOnFctItem_X[WMENU_FCTBUTTON_F1] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
		GL_pWMenuItem_X[i].ppOnFctItem_X[WMENU_FCTBUTTON_F2] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
		GL_pWMenuItem_X[i].ppOnFctItem_X[WMENU_FCTBUTTON_F3] = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);

		GL_pWMenuItem_X[i].pOnConditionNavItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);
		GL_pWMenuItem_X[i].pOnTimerNavItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_NULL]);

		GL_pWMenuItem_X[i].pFct_GetCondition = DefaultGetCondition;
		GL_pWMenuItem_X[i].pFct_OnTransition = DefaultOnTransitionFct;
        GL_pWMenuItem_X[i].pFct_OnProcess = DefaultOnProcessFct;
        GL_pWMenuItem_X[i].pFct_OnEnter = DefaultOnProcessFct;
        GL_pWMenuItem_X[i].pFct_OnTimerElapsed = DefaultOnProcessFct;
		GL_pWMenuItem_X[i].pFct_OnValidateParam = DefaultOnValidateFct;
    }


	/* -------------------- */
	/* Initialize all items */
	/* -------------------- */

	/* 0. Welcom Screen */
	GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].Type_E = WMENU_ITEM_TYPE_INFO;
	GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].Id_UL = WMENU_ITEM_WELCOME_SCREEN;
	GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].NavIndex_UL = 0;

	GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_WELCOME_SCREEN][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_IDLE_SCREEN]);

	GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN].pFct_OnTransition = WMenuItem_WelcomeScreen_Transition;
	


	/* 0. Idle */
	GL_pWMenuItem_X[WMENU_ITEM_IDLE_SCREEN].Type_E = WMENU_ITEM_TYPE_INFO;
	GL_pWMenuItem_X[WMENU_ITEM_IDLE_SCREEN].Id_UL = WMENU_ITEM_IDLE_SCREEN;
	GL_pWMenuItem_X[WMENU_ITEM_IDLE_SCREEN].NavIndex_UL = 0;

	GL_pWMenuItem_X[WMENU_ITEM_IDLE_SCREEN].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_IDLE_SCREEN][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_IDLE_SCREEN].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS]);

	GL_pWMenuItem_X[WMENU_ITEM_IDLE_SCREEN].pFct_OnProcess = WMenuItem_Idle_Process;



	/* 0.0. Settings */
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].Type_E = WMENU_ITEM_TYPE_MENU;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].Id_UL = WMENU_ITEM_SETTINGS;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].NavIndex_UL = 0;

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_IDLE_SCREEN]);



	/* 0.0.0. Languages */
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].Type_E = WMENU_ITEM_TYPE_MENU;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].Id_UL = WMENU_ITEM_SETTINGS_LANGUAGE;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].NavIndex_UL = 0;

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_LANGUAGE][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE].ppOnNavItem_X[WMENU_NAVBUTTON_DOWN] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME]);



	/* 0.0.0.0. Language Selection */
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].Type_E = WMENU_ITEM_TYPE_PARAM;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].Id_UL = WMENU_ITEM_SETTINGS_LANGUAGE_SELECT;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].NavIndex_UL = 0;

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_LANGUAGE][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE]);

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].pFct_OnProcess = WMenuItem_LanguageSelect_Process;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE_SELECT].pFct_OnValidateParam = WConfig_SetLanguage;


	/* 0.0.1. Date & Time */
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].Type_E = WMENU_ITEM_TYPE_MENU;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].Id_UL = WMENU_ITEM_SETTINGS_DATETIME;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].NavIndex_UL = 1;

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_DATETIME][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].ppOnNavItem_X[WMENU_NAVBUTTON_UP] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LANGUAGE]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME].ppOnNavItem_X[WMENU_NAVBUTTON_DOWN] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD]);



	/* 0.0.1.0. Date */
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE].Type_E = WMENU_ITEM_TYPE_MENU;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE].Id_UL = WMENU_ITEM_SETTINGS_DATETIME_DATE;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE].NavIndex_UL = 0;

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_DATETIME_DATE][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE].ppOnNavItem_X[WMENU_NAVBUTTON_DOWN] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME]);



	/* 0.0.1.0.0. Set Date */
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET].Type_E = WMENU_ITEM_TYPE_PARAM;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET].Id_UL = WMENU_ITEM_SETTINGS_DATETIME_DATE_SET;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET].NavIndex_UL = 0;

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_DATETIME_DATE][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE]);

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET].pFct_OnProcess = WMenuItem_DateSet_Process;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE_SET].pFct_OnValidateParam = WConfig_SetDate;



	/* 0.0.1.1. Time */
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME].Type_E = WMENU_ITEM_TYPE_MENU;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME].Id_UL = WMENU_ITEM_SETTINGS_DATETIME_TIME;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME].NavIndex_UL = 1;

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_DATETIME_TIME][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME].ppOnNavItem_X[WMENU_NAVBUTTON_UP] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_DATE]);



	/* 0.0.1.1.0. Set Time */
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET].Type_E = WMENU_ITEM_TYPE_PARAM;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET].Id_UL = WMENU_ITEM_SETTINGS_DATETIME_TIME_SET;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET].NavIndex_UL = 0;

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_DATETIME_TIME][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME]);

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET].pFct_OnProcess = WMenuItem_TimeSet_Process;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME_TIME_SET].pFct_OnValidateParam = WConfig_SetTime;



	/* 0.0.2. LCD */
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].Type_E = WMENU_ITEM_TYPE_MENU;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].Id_UL = WMENU_ITEM_SETTINGS_LCD;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].NavIndex_UL = 2;

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_LCD][GL_GlobalConfig_X.Language_E].c_str();
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD_BACKLIGHT]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].ppOnNavItem_X[WMENU_NAVBUTTON_UP] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_DATETIME]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD].ppOnNavItem_X[WMENU_NAVBUTTON_DOWN] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET]);



	/* 0.0.2.0. LCD Backlight */
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD_BACKLIGHT].Type_E = WMENU_ITEM_TYPE_MENU;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD_BACKLIGHT].Id_UL = WMENU_ITEM_SETTINGS_LCD_BACKLIGHT;
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD_BACKLIGHT].NavIndex_UL = 0;

	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD_BACKLIGHT].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_LCD_BACKLIGHT][GL_GlobalConfig_X.Language_E].c_str();
	//GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD_BACKLIGHT].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD_BACKLIGHT_SET]);
	GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD_BACKLIGHT].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD]);


	/* 0.0.2.0.0. Set Backlight */



    /* 0.0.3. Reset */
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET].Type_E = WMENU_ITEM_TYPE_MENU;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET].Id_UL = WMENU_ITEM_SETTINGS_RESET;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET].NavIndex_UL = 3;

    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_RESET][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET_CONFIRM]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET].ppOnNavItem_X[WMENU_NAVBUTTON_UP] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_LCD]);

    /* 0.0.3.0. Confirm Reset */
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET_CONFIRM].Type_E = WMENU_ITEM_TYPE_INFO;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET_CONFIRM].Id_UL = WMENU_ITEM_SETTINGS_RESET_CONFIRM;
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET_CONFIRM].NavIndex_UL = 0;

    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET_CONFIRM].ppText_UB[0] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_RESET][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET_CONFIRM].ppText_UB[1] = GL_ppWMenuItemText_Str[WMENU_ITEM_SETTINGS_RESET_CONFIRM][GL_GlobalConfig_X.Language_E].c_str();
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET_CONFIRM].ppOnNavItem_X[WMENU_NAVBUTTON_ENTER] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET]);
    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET_CONFIRM].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = &(GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET]);

    GL_pWMenuItem_X[WMENU_ITEM_SETTINGS_RESET_CONFIRM].pFct_OnEnter = WMenuItem_ResetConfirm_OnEnter;



	/* ---------------------------- */
	/* Initialize Application Items */
	/* ---------------------------- */

	if (GL_GlobalConfig_X.App_X.hasMenu_B) {
		
		GL_GlobalConfig_X.App_X.pFctInitMenu();	// init all items from Application
		GL_pWMenuItem_X[WMENU_ITEM_IDLE_SCREEN].ppOnNavItem_X[WMENU_NAVBUTTON_BACK] = GL_GlobalConfig_X.App_X.pFctGetFirstItem();	// Assign starting point

		GL_WMenuFromApp_X.Step_UL = 0;
		GL_WMenuFromApp_X.TimeOut_UL = 0;

		for (int i = 0; i < (sizeof(GL_WMenuFromApp_X.pCharArray_UB) / sizeof(GL_WMenuFromApp_X.pCharArray_UB[0])); i++) {
			GL_WMenuFromApp_X.pCharArray_UB[i] = 0x00;
		}
	}

}


void ProcessIdle(void) {
	if (GL_WMenuManagerEnabled_B) {
		InitMenuItem();
		TransitionToWelcomeScreen();
	}
}

void ProcessWelcomeScreen(void) {
	if (((millis() - GL_AbsoluteTime_UL) >= 5000) || (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER])) {

		if (GL_GlobalConfig_X.App_X.hasMenu_B) {
			// Enter first item after Welcome Screen if App has dedicated menu
			GL_pWMenuCurrentItem_X = GL_GlobalConfig_X.App_X.pFctGetFirstItem();
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}
		else {
			// Enter Idle Screen after Welcome Screen
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_ENTER];
			TransitionToInfo();
		}
	}
}


void ProcessMenu(void) {

	// > DOWN : Go to Next Menu
	if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_DOWN]) {

		if (GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_DOWN]->Type_E == WMENU_ITEM_TYPE_MENU) {
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_DOWN];
			TransitionToMenu();
		}

	} // > UP : Go to Previous Menu
	else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_UP]) {

		if (GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_UP]->Type_E == WMENU_ITEM_TYPE_MENU) {
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_UP];
			TransitionToMenu();
		}

	}  // > ENTER : Go inside Menu
	else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER]) {

		if (GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_ENTER]->Type_E != WMENU_ITEM_TYPE_NULL) {
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_ENTER];

			// Change state
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}

	} // > BACK : Go to Previous Screen
	else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK]) {

		if (GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_BACK]->Type_E != WMENU_ITEM_TYPE_NULL) {
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_BACK];

			// Change state
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}
	}

}

void ProcessInfo(void) {

	// Allow jump from application
	if(GL_pWMenuCurrentItem_X->IsFromApp_B)
		WMenu_ManageJumpFromApp();


	// Call item-specific Process()
	GL_pWMenuCurrentItem_X->pFct_OnProcess(&GL_ItemParam_X);


	// > Get item-specific condition 
	if (GL_pWMenuCurrentItem_X->pFct_GetCondition(GL_pWMenuCurrentItem_X)) {
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Item-specific condition met -> change Item");
		if (GL_pWMenuCurrentItem_X->pOnConditionNavItem_X->Type_E != WMENU_ITEM_TYPE_NULL) {
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->pOnConditionNavItem_X;

			// Change state
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}
	}


	// > Get item timing-based transition
	if (GL_pWMenuCurrentItem_X->TimerValue_UL != 0) {
		if ((millis() - GL_AbsoluteTime_UL) >= GL_pWMenuCurrentItem_X->TimerValue_UL) {
			DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Timing-based transition reached -> change Item");
			if (GL_pWMenuCurrentItem_X->pOnTimerNavItem_X->Type_E != WMENU_ITEM_TYPE_NULL) {
                GL_pWMenuCurrentItem_X->pFct_OnTimerElapsed(GL_pWMenuCurrentItem_X);
				GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->pOnTimerNavItem_X;

				// Change state
				if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
					TransitionToInfo();
				else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
					TransitionToParam();
				else
					TransitionToMenu();
			}
		}
	}


	// > BACK : Quit Screen
	if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK]) {

		if (GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_BACK]->Type_E != WMENU_ITEM_TYPE_NULL) {
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_BACK];

			// Change state
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}

	} // > ENTER : Quit Screen
	else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER]) {

		if (GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_ENTER]->Type_E != WMENU_ITEM_TYPE_NULL) {
            GL_pWMenuCurrentItem_X->pFct_OnEnter(GL_pWMenuCurrentItem_X);
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_ENTER];

			// Change state
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}
	}


	// > F1
	if (GL_pFunctionButtonPressed_B[WMENU_FCTBUTTON_F1]) {

		if (GL_pWMenuCurrentItem_X->ppOnFctItem_X[WMENU_FCTBUTTON_F1]->Type_E != WMENU_ITEM_TYPE_NULL) {
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnFctItem_X[WMENU_FCTBUTTON_F1];

			// Change state
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}
	} // > F2
	else if (GL_pFunctionButtonPressed_B[WMENU_FCTBUTTON_F2]) {

		if (GL_pWMenuCurrentItem_X->ppOnFctItem_X[WMENU_FCTBUTTON_F2]->Type_E != WMENU_ITEM_TYPE_NULL) {
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnFctItem_X[WMENU_FCTBUTTON_F2];

			// Change state
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}
	} // > F3
	else if (GL_pFunctionButtonPressed_B[WMENU_FCTBUTTON_F3]) {

		if (GL_pWMenuCurrentItem_X->ppOnFctItem_X[WMENU_FCTBUTTON_F3]->Type_E != WMENU_ITEM_TYPE_NULL) {
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnFctItem_X[WMENU_FCTBUTTON_F3];

			// Change state
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}
	}

}

void ProcessParam(void) {  

	// Call item-specific Process()
	GL_pWMenuCurrentItem_X->pFct_OnProcess(&GL_ItemParam_X);	// process function with parameters


	// > BACK : Quit Screen
	if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK]) {

		// Reset item parameters
		GL_ItemParam_X.ParamIndex_UL = 0;

		if (GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_BACK]->Type_E != WMENU_ITEM_TYPE_NULL) {
			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_BACK];

			// Change state
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}

	} // > ENTER : Quit Screen
	else if (GL_pNavButtonPressed_B[WMENU_NAVBUTTON_ENTER]) {

		// Reset item parameters
		GL_ItemParam_X.ParamIndex_UL = 0;

		if (GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_ENTER]->Type_E != WMENU_ITEM_TYPE_NULL) {

			// Call specific function
			GL_pWMenuCurrentItem_X->pFct_OnValidateParam(GL_ItemParam_X.pParam_UB);
			if (GL_pWMenuCurrentItem_X->Id_UL == WMENU_ITEM_SETTINGS_LANGUAGE_SELECT)   InitMenuItem();     // Re-Init Menu if Language has changed

			GL_pWMenuCurrentItem_X = GL_pWMenuCurrentItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_ENTER];

			// Change state
			if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_INFO)
				TransitionToInfo();
			else if (GL_pWMenuCurrentItem_X->Type_E == WMENU_ITEM_TYPE_PARAM)
				TransitionToParam();
			else
				TransitionToMenu();
		}
	}

}


void TransitionToIdle(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To IDLE");
    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_IDLE;
}

void TransitionToWelcomeScreen(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To WELCOME SCREEN");

	GL_pWMenuCurrentItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_WELCOME_SCREEN]);	// Assign first item

	GL_AbsoluteTime_UL = millis();
	GL_ExAppTime_UL = millis();

	WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
	WMenu_AssignEnterBackCallbacks();

	GL_pWMenuCurrentItem_X->pFct_OnTransition(GL_pWMenuCurrentItem_X);
    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_WELCOME_SCREEN;
}

void TransitionToMenu(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To MENU");

	GL_GlobalData_X.Lcd_H.disableCursor();

	GL_AbsoluteTime_UL = millis();
	GL_ExAppTime_UL = millis();

	WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
	WMenu_AssignNavigationCallbacks();

	GL_pWMenuCurrentItem_X->pFct_OnTransition(GL_pWMenuCurrentItem_X);
    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_MENU;
}

void TransitionToInfo(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To INFO");

	GL_GlobalData_X.Lcd_H.disableCursor();

	GL_AbsoluteTime_UL = millis();
	GL_ExAppTime_UL = millis();

	WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
	WMenu_AssignEnterBackCallbacks();

	GL_ItemParam_X.pSenderItem_H = GL_pWMenuCurrentItem_X;
	GL_pWMenuCurrentItem_X->pFct_OnTransition(GL_pWMenuCurrentItem_X);
    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_INFO;
}

void TransitionToParam(void) {
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Transition To PARAM");

	GL_GlobalData_X.Lcd_H.disableCursor();

	GL_AbsoluteTime_UL = millis();
	GL_ExAppTime_UL = millis();

	WMenu_DisplayItem(GL_pWMenuCurrentItem_X);
	WMenu_AssignNumericKeyCallbacks();

	GL_ItemParam_X.pSenderItem_H = GL_pWMenuCurrentItem_X;
	GL_pWMenuCurrentItem_X->pFct_OnTransition(GL_pWMenuCurrentItem_X);
    GL_WMenuManager_CurrentState_E = WMENU_STATE::WMENU_PARAM;
}


/* ******************************************************************************** */
/* Manage Items
/* ******************************************************************************** */

void WMenu_ManageJumpToApp(void) {

	// Jump to application menu after timeout
	if ((millis() - GL_ExAppTime_UL) >= WMENU_JUMP_TO_APP_TIMER_MS) {
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Jump to Application Menu (timer elapsed)");
		GL_pWMenuCurrentItem_X = GL_GlobalConfig_X.App_X.pFctGetFirstItem();
		TransitionToInfo();
	}
}

void WMenu_ManageJumpFromApp(void) {
	int i = 0;
	boolean PasswordOk_B = true;

	// Check password
	for (i = 0; i < (sizeof(GL_pWMenuPassword_UB)/sizeof(GL_pWMenuPassword_UB[0])); i++) {
		if (GL_WMenuFromApp_X.pCharArray_UB[i] != GL_pWMenuPassword_UB[i]) {
			PasswordOk_B = false;
			break;
		}
	}

	// Switch if password OK
	if (PasswordOk_B) {
		DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Password OK -> Switch to WMenu");

		GL_WMenuFromApp_X.Step_UL = 0;
		for (i = 0; i < (sizeof(GL_WMenuFromApp_X.pCharArray_UB) / sizeof(GL_WMenuFromApp_X.pCharArray_UB[0])); i++) {
			GL_WMenuFromApp_X.pCharArray_UB[i] = 0x00;
		}

		GL_pWMenuCurrentItem_X = &(GL_pWMenuItem_X[WMENU_ITEM_IDLE_SCREEN]);
		TransitionToInfo();
	}


	// Timeout in sequence
	if (GL_WMenuFromApp_X.Step_UL != 0) {
		if ((millis() - GL_WMenuFromApp_X.TimeOut_UL) >= WMENU_JUMP_FROM_APP_SEQUENCE_TIMEOUT_MS) {
			DBG_PRINTLN(DEBUG_SEVERITY_WARNING, "Failed to jump to WMenu !");

			GL_WMenuFromApp_X.Step_UL = 0;
			for (i = 0; i < (sizeof(GL_WMenuFromApp_X.pCharArray_UB) / sizeof(GL_WMenuFromApp_X.pCharArray_UB[0])); i++) {
				GL_WMenuFromApp_X.pCharArray_UB[i] = 0x00;
			}
		}
	}
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
    // > Display the first line of text as the Menu text
    // > Display the second line of text as the Param text
    case WMENU_ITEM_TYPE_PARAM:
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppText_UB[0]);
		GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, pMenuItem_X->ppText_UB[1]);/*
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_BACK]->ppText_UB[0]);
        GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, pMenuItem_X->ppText_UB[0]);*/
        break;

    // MENU
    // > Display the first line of text as the Menu text
    // > Display the second line of text as the Next Menu text
    case WMENU_ITEM_TYPE_MENU:
        if (((pMenuItem_X->NavIndex_UL) % 2) == 0) {
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, 0, ">");
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, pMenuItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_DOWN]->ppText_UB[0]);
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE2, 0, " ");
        }
        else {
            GL_GlobalData_X.Lcd_H.writeDisplay(LCD_DISPLAY_LINE1, pMenuItem_X->ppOnNavItem_X[WMENU_NAVBUTTON_UP]->ppText_UB[0]);
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

	for (int i = 0; i < 3; i++)
		GL_pFunctionButtonPressed_B[i] = false;
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
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button ENTER/RIGHT Pressed");
}

void WMenuCallback_OnBack(char * pKey_UB) {
    GL_pNavButtonPressed_B[WMENU_NAVBUTTON_BACK] = true;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button BACK/LEFT Pressed");
}

void WMenuCallback_OnFunction1(char * pKey_UB) {
	GL_pFunctionButtonPressed_B[WMENU_FCTBUTTON_F1] = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button F1 Pressed");
}

void WMenuCallback_OnFunction2(char * pKey_UB) {
	GL_pFunctionButtonPressed_B[WMENU_FCTBUTTON_F2] = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button F2 Pressed");
}

void WMenuCallback_OnFunction3(char * pKey_UB) {
	GL_pFunctionButtonPressed_B[WMENU_FCTBUTTON_F3] = true;
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Button F3 Pressed");
}


void WMenuCallback_OnNumericKey(char * pKey_UB) {
	GL_ExAppTime_UL = millis();
	GL_ItemParam_X.KeyPressed_B = true;
	GL_ItemParam_X.Key_UB = *pKey_UB;
    DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Numeric key pressed : " + String(GL_ItemParam_X.Key_UB));
}

void WMenuManager_PushKey(char * pKey_UB) {
	GL_WMenuFromApp_X.pCharArray_UB[GL_WMenuFromApp_X.Step_UL] = *pKey_UB;		// Push Key in array
	GL_WMenuFromApp_X.TimeOut_UL = millis();									// Reset timer
	DBG_PRINTLN(DEBUG_SEVERITY_INFO, "Key pushed in array : " + String(GL_WMenuFromApp_X.pCharArray_UB[GL_WMenuFromApp_X.Step_UL]));
	GL_WMenuFromApp_X.Step_UL++;
	if (GL_WMenuFromApp_X.Step_UL > (sizeof(GL_WMenuFromApp_X.pCharArray_UB) / sizeof(GL_WMenuFromApp_X.pCharArray_UB[0])))	GL_WMenuFromApp_X.Step_UL = 0;
}

void WMenu_AssignResetCallbacks(void) {
    for (int i = 0; i < 16; i++)
        GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent((FLAT_PANEL_KEY_ENUM)i, WMenuManager_PushKey);
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

	// Assign dedicated functions keys
	GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_F1, WMenuCallback_OnFunction1);
	GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_F2, WMenuCallback_OnFunction2);
	GL_GlobalData_X.FlatPanel_H.assignOnKeyPressedEvent(FLAT_PANEL_KEY_F3, WMenuCallback_OnFunction3);
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
