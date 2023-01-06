#include "../include/ct1.h"
#include "../include/OS.h"

#define SAVE_MODE 0
#define LOAD_MODE 1
#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))
#define ramStartAddr (void*)0x800E72C0
#define ramEndAddr (void*)0x803B5000
#define KB *(1 <<10)
#define MB *(1 <<20)
#define GB *(1U<<30)

#define ramAddrSavestateDataSlot1 (void*)0x804C0000
#define ramAddrSavestateDataSlot2 (void*)0x805D0000
#define ramAddrSavestateDataSlot3 (void*)0x806E0000 //hopefully doesn't overflow into 0x807FFFDC (though if it does we were screwed anyway)
#define DPAD_LEFT_CASE 0
#define DPAD_UP_CASE 1
#define DPAD_RIGHT_CASE 2

extern int getStatusRegister(void); //returns status reg
extern int setStatusRegister(s32); //returns status reg
void __osDisableInt(void);
void __osRestoreInt(void);

s32 savestateCurrentSlot = 0;
s32 stateCooldown = 0;
s32 saveOrLoadStateMode = 0;

s32 savestate1Size = 0;
s32 savestate2Size = 0;
s32 savestate3Size = 0;

s32 isMenuActive = 0;

s32 isSaveOrLoadActive = 0;

typedef struct CustomThread {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ char stack[0xC000];
    /* 0x9B0 */ OSMesgQueue queue;
    /* 0x9C8 */ OSMesg mesg;
    /* 0x9CC */ u16* frameBuf;
    /* 0x9D0 */ u16 width;
    /* 0x9D2 */ u16 height;
} CustomThread; // size = 0x9D4

CustomThread gCustomThread = {0};

void cBootFunction(void) {
    isSaveOrLoadActive = 0;
}

int __osPiDeviceBusy() {
    register u32 stat = IO_READ(PI_STATUS_REG);
    if (stat & (PI_STATUS_DMA_BUSY | PI_STATUS_IO_BUSY))
        return 1;
    return 0;
}

void loadstateMain(void) {
    isSaveOrLoadActive = 1;
    s32 register status = getStatusRegister();
    //wait on rsp
    while (__osSpDeviceBusy() == 1) {}

    //wait on rdp
    while ( __osDpDeviceBusy() == 1) {}

    //wait on SI
    while (__osSiDeviceBusy() == 1) {}

    //wait on PI
    while (__osPiDeviceBusy() == 1) {}

    //invalidate caches
    osInvalICache((void*)0x80000000, 0x2000);
	osInvalDCache((void*)0x80000000, 0x2000);
    __osDisableInt();

    switch (savestateCurrentSlot) {
        case DPAD_LEFT_CASE:
            if (savestate1Size != 0 && savestate1Size != -1) {
                decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate1Size, ramAddrSavestateDataSlot1); //always decompresses to `ramStartAddr`
            }  
        break;
        case DPAD_UP_CASE:
            if (savestate2Size != 0 && savestate2Size != -1) {
                decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate2Size, ramAddrSavestateDataSlot2); //always decompresses to `ramStartAddr`
            }
        break;
        case DPAD_RIGHT_CASE:
            if (savestate3Size != 0 && savestate3Size != -1) {
                decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate3Size, ramAddrSavestateDataSlot3); //always decompresses to `ramStartAddr`
            }  
        break;
    }

    setStatusRegister(status);
    __osRestoreInt();
    isSaveOrLoadActive = 0;
}
    
void savestateMain(void) {
    isSaveOrLoadActive = 1;

    //push status
    s32 register status = getStatusRegister();
    //wait on rsp
    while (__osSpDeviceBusy() == 1) {}

    //wait on rdp
    while ( __osDpDeviceBusy() == 1) {}

    //wait on SI
    while (__osSiDeviceBusy() == 1) {}

    //wait on PI
    while (__osPiDeviceBusy() == 1) {}

    //invalidate caches
    osInvalICache((void*)0x80000000, 0x2000);
	osInvalDCache((void*)0x80000000, 0x2000);

    __osDisableInt();
    switch (savestateCurrentSlot) {
        case DPAD_LEFT_CASE:
            savestate1Size = compress_lz4_ct_default(ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot1);
        break;
        case DPAD_UP_CASE:
            savestate2Size = compress_lz4_ct_default(ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot2);
        break;
        case DPAD_RIGHT_CASE:
            savestate3Size = compress_lz4_ct_default(ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot3);
        break;
    }
    setStatusRegister(status);
    __osRestoreInt();
    isSaveOrLoadActive= 0;
}

void checkInputsForSavestates(void) {
    savestateCurrentSlot = -1;//set to invalid

    if (p1PressedButtons & DPAD_LEFT) {
        savestateCurrentSlot = 0;
    }

    if (p1PressedButtons & DPAD_UP) {
        savestateCurrentSlot = 1;
    }

    if (p1PressedButtons & DPAD_RIGHT) {
        savestateCurrentSlot = 2;
    }

    if (savestateCurrentSlot == -1 || stateCooldown != 0){
        return;
    }

    if (gameMode != GAME_MODE_STAGE_SELECT &&
    gameMode != GAME_MODE_NEW_GAME_MENU &&
    gameMode != GAME_MODE_TITLE_SCREEN &&
    gameMode != GAME_MODE_JUNGLE_LAND_MENU) {
        if (saveOrLoadStateMode == SAVE_MODE) {
            osCreateThread(&gCustomThread.thread, 255, (void*)savestateMain, NULL,
                    gCustomThread.stack + sizeof(gCustomThread.stack), 255);
            osStartThread(&gCustomThread.thread);
            stateCooldown = 5;
        } else {
            osCreateThread(&gCustomThread.thread, 255, (void*)loadstateMain, NULL,
                    gCustomThread.stack + sizeof(gCustomThread.stack), 255);
            osStartThread(&gCustomThread.thread);
            stateCooldown = 5;            
        }
    }
}

// Wrapper for in game printText()
void textPrint(f32 xPos, f32 yPos, f32 scale, void *text, s32 num) {
    printText(xPos, yPos, 0, scale, 0, 0, text, num);
}

void printSaveOrLoad(void) {
    char message[10];
    if (saveOrLoadStateMode == SAVE_MODE) {
        message[0] =  0xA3;
        message[1] = 0x60 + 's';
    } else {
        message[0] = 0xA3;
        message[1] = 0x60 + 'l';
    }
    message[2] = 0;
    textPrint(13.0f, 208.0f, 0.65f, &message, 3);
}

void mainCFunction(void) {
    // if (debugBool == 0xFFFFFFFF) {
    //     debugBool = 0;
    // }

    if (stateCooldown == 0) {
        if ((p1HeldButtons & R_BUTTON) && (p1PressedButtons & DPAD_UP)) {
            //debugBool ^= 1;
        } else if ((p1HeldButtons & R_BUTTON) && (p1PressedButtons & DPAD_DOWN)) {
            isMenuActive ^= 1;
        } else if (p1PressedButtons & DPAD_DOWN) {
            saveOrLoadStateMode ^= 1;
        } else {
            checkInputsForSavestates();
        }
    }

    if (stateCooldown > 0) {
        stateCooldown--;
    }

    printSaveOrLoad();

    while (isSaveOrLoadActive != 0) {}
}