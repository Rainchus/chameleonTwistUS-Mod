#include "../include/menu.h"

#define X_COORD_PER_LETTER 4.5
// shift x value per print per length of string (8px per letter) then print ON/OFF

#define FUNCS_PER_PAGE 8
#define FUNCS_PER_LAST_PAGE 3

typedef s32 (*menuProc) (void);
#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))
#define ARRAY_COUNT_INDEX(arr) ARRAY_COUNT(arr) - 1

u8 toggles[] = {
    2, // NO_TOGGLE
    1, // TOGGLE_HIDE_SAVESTATE_TEXT
    1, // TOGGLE_HIDE_IGT
    0,  // TOGGLE_INFINITE_HEALTH
    0,  // TOGGLE_CUSTOM_DEBUG_TEXT
    0,  // TOGGLE_CAVE_SKIP_PRACTICE
};

s32 toggleHideSavestateText(void) {
    toggles[TOGGLE_HIDE_SAVESTATE_TEXT] ^= 1;
    return 1;
}

s32 toggleHideIGT(void) {
    toggles[TOGGLE_HIDE_IGT] ^= 1;
    return 1;
}

s32 toggleInfiniteHealth(void) {
    toggles[TOGGLE_INFINITE_HEALTH] ^= 1;
    return 1;
}

s32 toggleCustomDebugText(void) {
    toggles[TOGGLE_CUSTOM_DEBUG_TEXT]++;
    if (toggles[TOGGLE_CUSTOM_DEBUG_TEXT] >= 3) {
        toggles[TOGGLE_CUSTOM_DEBUG_TEXT] = 0;
    }
    return 1;
}

s32 toggleCaveSkipPractice(void) {
    toggles[TOGGLE_CAVE_SKIP_PRACTICE] ^= 1;
    return 1;
}


typedef struct menuPage {
    /* 0x08 */ s32 optionCount;
    /* 0x0C */ s32 pageIndex;
    /* 0x10 */ char* options[FUNCS_PER_PAGE];
    /* 0x30 */ s32 (*menuProc[FUNCS_PER_PAGE]) (void);
    /* 0x50 */ u8 flags[FUNCS_PER_PAGE];
} menuPage;

s32 teleportToStageBoss(void) {
    return 1;
}

s32 toggleObjectSpawnsOff(void) {
    return 1;
}

menuPage page1 = {
    2, //optionCount
    PAGE_JL, //pageIndex
    { //options
        "Load Boss\n",
        "Infinite Health\n",
    },
    { //menuProc
        &teleportToStageBoss,
        &toggleInfiniteHealth,
    },
    { //flags
        NO_TOGGLE,
        TOGGLE_INFINITE_HEALTH
    }
};

menuPage page0 = {
    5, //optionCount
    PAGE_MAIN, //pageIndex
    { //options
        "Savestate Text\n",
        "In Game Timer\n",
        "Custom Debug Text\n",
        "Cave Skip Practice\n",
        "Disable Actors\n"
    },
    { //menuProc
        &toggleHideSavestateText,
        &toggleHideIGT,
        &toggleCustomDebugText,
        &toggleCaveSkipPractice,
        &toggleObjectSpawnsOff
    },
    { //flags
        TOGGLE_HIDE_SAVESTATE_TEXT,
        TOGGLE_HIDE_IGT,
        TOGGLE_CUSTOM_DEBUG_TEXT,
        TOGGLE_CAVE_SKIP_PRACTICE,
        TOGGLE_ENEMY_SPAWNS_OFF
    }
};

menuPage* pageList[] = {
    &page0,
    &page1,
    //&page2,
};

//testing func ptr
s32 printCustomDebugText(void);

void colorTextWrapper(s32* color) {
    SetTextGradient(color[0], color[1], color[2], color[3], color[4], color[5], color[6], color[7],
             color[8], color[9], color[10], color[11], color[12], color[13], color[14], color[15]);
}


s32 textCyanColor[] = {
    0x2A, 0xEE, 0xE9, 0xFF, // top
    0x00, 0xC0, 0xDA, 0xFF, // bottom
    0x2A, 0xEE, 0xE9, 0xFF, // top
    0x00, 0xC0, 0xDA, 0xFF  // bottom
};

// Gradient
s32 textGreenColor[] = {
    0x0A, 0xFF, 0x00, 0xFF, // top
    0xFF, 0xFF, 0xFF, 0xFF, // bottom
    0x0A, 0xFF, 0x00, 0xFF, // top
    0xFF, 0xFF, 0x00, 0x0F  // bottom
};

s32 textGreenMatColor[] = {
    0x00, 0xFF, 0x00, 0xFF, // top
    0x00, 0xFF, 0x00, 0xFF, // bottom
    0x00, 0xFF, 0x00, 0xFF, // top
    0x00, 0xFF, 0x00, 0xFF  // bottom
};

s32 textWhiteColor[] = {
    0xFF, 0xFF, 0xFF, 0xFF, // top
    0xFF, 0xFF, 0xFF, 0xFF, // bottom
    0xFF, 0xFF, 0xFF, 0xFF, // top
    0xFF, 0xFF, 0xFF, 0xFF  // bottom
};

s32 textRedColor[] = {
    0xFF, 0x00, 0x00, 0xFF, // top
    0xFF, 0x00, 0x00, 0xFF, // bottom
    0xFF, 0x00, 0x00, 0xFF, // top
    0xFF, 0x00, 0x00, 0xFF  // bottom
};

s32 currPageNo = 0;
s32 currOptionNo = 0;

char menuOptionBuffer[50] = { 0 };  // Buffer for menu options text
char menuOptionBufferConverted[100] = { 0 };  // Buffer for menu options text converted to chameleon twist format

s32 isMenuActive = 0;

void updateMenuInput(void){
    if (currentlyPressedButtons & CONT_UP) {
        if (currOptionNo > 0) {
            currOptionNo--;
            //playSound(0x2A, (void*)0x80168DA8, 0);
        }
    }
    else if (currentlyPressedButtons & CONT_DOWN) {
        if (currOptionNo < pageList[currPageNo]->optionCount - 1) {
            currOptionNo++;
            //playSound(0x2A, (void*)0x80168DA8, 0);
        }
    }
    else if (currentlyPressedButtons & CONT_LEFT) {
        if (currPageNo > 0) {
            currPageNo--;
            currOptionNo = 0;
        }
    }
    else if (currentlyPressedButtons & CONT_RIGHT) {
        if (currPageNo < ARRAY_COUNT_INDEX(pageList)) {
            currPageNo++;
            currOptionNo = 0;
        }
    }
    else if (currentlyPressedButtons & CONT_B) {
        isMenuActive = 0;
    }
    else if (currentlyPressedButtons & CONT_A) {
        pageList[currPageNo]->menuProc[currOptionNo]();
    }
}

void pageMainDisplay(s32 currPageNo, s32 currOptionNo) {
    menuPage* currPage = pageList[currPageNo];
    f32 xPos = 20.0f;
    f32 yPos = 35.0f;
    
    for (int i = 0; i < currPage->optionCount; i++) {
        _bzero(&menuOptionBuffer, sizeof(menuOptionBuffer)); //clear buffer
        _sprintf(menuOptionBuffer, "%s", currPage->options[i]);
        _bzero(&menuOptionBufferConverted, sizeof(menuOptionBufferConverted)); //clear buffer 2
        convertAsciiToText(&menuOptionBufferConverted, (char*)&menuOptionBuffer);
        s32 strLength = ct_strlen((u8*)&menuOptionBufferConverted);

        
        if (i == currOptionNo) {
            colorTextWrapper(textCyanColor);
        } else {
            colorTextWrapper(textWhiteColor);
        }

        if (currPage->flags[i] == -1) {
            TextPrint(xPos, (yPos + (i * 15.0f)), 0.5f, &menuOptionBufferConverted, 1);
            continue;
        }

        //get offset into toggle array we should read
        TextPrint(xPos, (yPos + (i * 15.0f)), 0.5f, &menuOptionBufferConverted, 1);
        
        //clear buffers
        _bzero(&menuOptionBuffer, sizeof(menuOptionBuffer));
        _bzero(&menuOptionBufferConverted, sizeof(menuOptionBufferConverted));

        if (toggles[currPage->flags[i]] >= 1) {
            colorTextWrapper(textGreenMatColor);
            _sprintf(menuOptionBuffer, "ON");
        } else if (toggles[currPage->flags[i]] == 0) {
            colorTextWrapper(textRedColor);
            _sprintf(menuOptionBuffer, "OFF");
        }

        convertAsciiToText(&menuOptionBufferConverted, (char*)&menuOptionBuffer);
        TextPrint(xPos + (strLength * X_COORD_PER_LETTER), (yPos + (i * 15.0f)), 0.5f, &menuOptionBufferConverted, 1);  

    }
}



