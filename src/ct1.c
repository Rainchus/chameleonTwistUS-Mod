#include "../include/ct1.h"
#include "../include/OS.h"

typedef struct {
	u16     button;
	s8      stick_x;		/* -80 <= stick_x <= 80 */
	s8      stick_y;		/* -80 <= stick_y <= 80 */
	u8	errno;
} OSContPad;

#define CONT_A      0x8000
#define CONT_B      0x4000
#define CONT_G	    0x2000
#define CONT_START  0x1000
#define CONT_UP     0x0800
#define CONT_DOWN   0x0400
#define CONT_LEFT   0x0200
#define CONT_RIGHT  0x0100
#define CONT_L      0x0020
#define CONT_R      0x0010
#define CONT_E      0x0008
#define CONT_D      0x0004
#define CONT_C      0x0002
#define CONT_F      0x0001
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
#define savestateBackup (void*)0x806E0000
#define MAXCONTROLLERS 4
extern OSContPad D_80181450[MAXCONTROLLERS];
s32 saveStateBackupSize = 0;
extern int getStatusRegister(void); //returns status reg
extern int setStatusRegister(s32); //returns status reg
s32 __osDisableInt(void);
void __osRestoreInt(u32);

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

void loadstateMainBackup(void) {
    u32 saveMask;
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
    saveMask = __osDisableInt();

    decompress_lz4_ct_default(ramEndAddr - ramStartAddr, saveStateBackupSize, savestateBackup);

    __osRestoreInt(saveMask);
    isSaveOrLoadActive = 0; //allow thread 3 to continue
}

void loadstateMain(void) {
    u32 saveMask;
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
    saveMask = __osDisableInt();

    switch (savestateCurrentSlot) {
        case 0: //dpad left
            if (savestate1Size != 0 && savestate1Size != -1) {
                decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate1Size, ramAddrSavestateDataSlot1);
            }
            break;
        case 1:
            if (savestate2Size != 0 && savestate2Size != -1) {
                decompress_lz4_ct_default(ramEndAddr - ramStartAddr, savestate2Size, ramAddrSavestateDataSlot2);
            }
            break;
    }

    __osRestoreInt(saveMask);
    isSaveOrLoadActive = 0; //allow thread 3 to continue
}
    
void savestateMain(void) {
    u32 saveMask;
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

    saveMask = __osDisableInt();

    switch (savestateCurrentSlot) {
        case 0:
            if (savestate1Size == 0) {
                if (savestate2Size == 0) {
                    //both states blank, create state
                    savestate1Size = compress_lz4_ct_default((void*)ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot1);
                } else {
                    //savestate 1 isn't initialized but savestate 2 is, therefore backup state 2
                    optimized_memcpy(savestateBackup, ramAddrSavestateDataSlot2, savestate2Size);
                    saveStateBackupSize = savestate2Size;
                    savestate1Size = compress_lz4_ct_default((void*)ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot1);
                }
            } else {
                //savestate 1 is already created, therefore backup state 1
                optimized_memcpy(savestateBackup, ramAddrSavestateDataSlot1, savestate1Size);
                saveStateBackupSize = savestate1Size;
                savestate1Size = compress_lz4_ct_default((void*)ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot1);                
            }
        break;

        case 1:
            if (savestate2Size == 0) {
                if (savestate1Size == 0) {
                    //both states blank, create state
                    savestate2Size = compress_lz4_ct_default((void*)ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot2);
                } else {
                    //savestate 2 isn't initialized but savestate 2 is, therefore backup state 1
                    optimized_memcpy(savestateBackup, ramAddrSavestateDataSlot1, savestate1Size);
                    saveStateBackupSize = savestate1Size;
                    savestate2Size = compress_lz4_ct_default((void*)ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot2);
                }
            } else {
                //savestate 2 is already created, therefore backup state 2
                optimized_memcpy(savestateBackup, ramAddrSavestateDataSlot2, savestate2Size);
                saveStateBackupSize = savestate2Size;
                savestate2Size = compress_lz4_ct_default((void*)ramStartAddr, ramEndAddr - ramStartAddr, ramAddrSavestateDataSlot2);                
            }
        break;
    }
    
    __osRestoreInt(saveMask);
    isSaveOrLoadActive = 0; //allow thread 3 to continue
}

void checkInputsForSavestates(void) {
    if (stateCooldown != 0){
        return;
    }

    if (gameMode != GAME_MODE_STAGE_SELECT && gameMode != GAME_MODE_NEW_GAME_MENU &&
        gameMode != GAME_MODE_TITLE_SCREEN && gameMode != GAME_MODE_JUNGLE_LAND && gIsPaused == 0)
    {
        if (D_80181450[0].button & CONT_LEFT) {
            isSaveOrLoadActive = 1;
            osCreateThread(&gCustomThread.thread, 255, (void*)savestateMain, NULL,
                    gCustomThread.stack + sizeof(gCustomThread.stack), 255);
            osStartThread(&gCustomThread.thread);
            stateCooldown = 5;
        } else if (D_80181450[0].button & CONT_RIGHT) {
            isSaveOrLoadActive = 1;
            osCreateThread(&gCustomThread.thread, 255, (void*)loadstateMain, NULL,
                    gCustomThread.stack + sizeof(gCustomThread.stack), 255);
            osStartThread(&gCustomThread.thread);
            stateCooldown = 5;   
        } else if (D_80181450[0].button & CONT_UP) {
            if (saveStateBackupSize != 0) {
                isSaveOrLoadActive = 1;
                osCreateThread(&gCustomThread.thread, 255, (void*)loadstateMainBackup, NULL,
                        gCustomThread.stack + sizeof(gCustomThread.stack), 255);
                osStartThread(&gCustomThread.thread);
                stateCooldown = 5;
            }
        } else if (D_80181450[0].button & CONT_DOWN) {
            stateCooldown = 8;
            savestateCurrentSlot ^= 1; //flip from 0 to 1 or vice versa (2 saveslots)
        }
    }
}

// Wrapper for in game Text()
void TextPrint(f32 xPos, f32 yPos, f32 scale, void *text, s32 num) {
    printText(xPos, yPos, 0, scale, 0, 0, text, num);
}

void PrintStateSlotText(void) {
    s32 index = 0;
    char textBuffer[8];
    if (savestateCurrentSlot == 0) {
        textBuffer[index++] =  0xA3;
        textBuffer[index++] = 0xB1; //s 1
    } else {
        textBuffer[index++] =  0xA3;
        textBuffer[index++] = 0xB2; //s 2         
    }

    textBuffer[index++] = 0;
    TextPrint(13.0f, 208.0f, 0.65f, &textBuffer, 3);
}

extern u8 D_8020850E;
extern u8 D_80208510;

void mainCFunction(void) {
    D_8020850E = 0xFF; //unlock levels
    D_80208510 = 0xFF; //unlock levels
    // if (debugBool == 0xFFFFFFFF) {
    //     debugBool = 0;
    // }

    if (stateCooldown > 0) {
        stateCooldown--;
    }

    if (isMenuActive == 0 ) {
        checkInputsForSavestates();
    }
    
    PrintStateSlotText();

    while (isSaveOrLoadActive != 0) {}
}