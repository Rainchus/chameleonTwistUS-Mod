#ifndef _CT1_H
#define _CT1_H

#include "types.h"
#include "enums.h"
#include "stdarg.h"

extern s32 p1PressedButtons;
extern s32 p1HeldButtons;
extern s32 gameMode;
extern s32 subGameMode;
//extern s32 debugBool;

//OS function defines
void osInvalICache(void*, s32);
void osInvalDCache(void*, s32);
int __osDpDeviceBusy();
int __osSpDeviceBusy();
int __osSiDeviceBusy();
void TextPrint(f32 xPos, f32 yPos, f32 scale, void *text, s32 num);
//user functions
void optimized_memcpy(void* destination, const void* source, u32 num_bytes);
void decompress_lz4_ct_default(int srcSize, int savestateCompressedSize, u8* compressBuffer);
s32 compress_lz4_ct_default(const u8* srcData, int srcSize, u8* bufferAddr);
void* my_memmove(void* dest, const void* src, int n);
void printText(f32 xPos, f32 yPos, f32 zero, f32 scale, f32 zero2, f32 zero3, void* text, s32);
extern s32 gIsPaused;
extern char decompressBuffer[0x803B5000 - 0x800EE1C0];
void SetTextGradient(u8 arg0, u8 arg1, u8 arg2, u8 arg3, u8 arg4, u8 arg5, u8 arg6, u8 arg7, u8 arg8, u8 arg9, u8 argA, u8 argB, u8 argC, u8 argD, u8 argE, u8 argF);
void _bzero(void*, s32);
extern s32 currPageNo;
extern s32 currOptionNo;
s32 ct_strlen(const u8 *s);
void pageMainDisplay(s32 currPageNo, s32 currOptionNo);
extern s32 isMenuActive;
void* crash_screen_copy_to_buf(void* dest, const char* src, u32 size);
void _sprintf(void* destination, void* fmt, ...);
void *ct_memcpy(void *s1, const void *s2, u32 n);
char* strncpy_custom(char* dest, const char* src, s32 n);
char* strcat(char* dest, const char* src);
void _sprintf(void* destination, void* fmt, ...);
void _sprintfcat(void* destination, void* fmt, ...);
s32 strlength_custom(const char* str);
void sprintf_custom(char* buffer, char c);
void convertAsciiToText(void* buffer, char* source);
typedef char *outfun(char*,const char*,unsigned int);
int _Printf(outfun prout, char *arg, const char *fmt, va_list args);
extern s32 previouslyHeldButtons;
extern s32 currentlyPressedButtons;
extern s32 previouslyPressedButtons;
void PrintIGT(void);

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

typedef struct Vec3f {
               f32 x;
               f32 y;
               f32 z;
} Vec3f;

typedef struct playerActor {
    /* 0x000 */ u32 playerID;
    /* 0x004 */ Vec3f pos;
    /* 0x010 */ Vec3f pos2; //slightly off from pos
    /* 0x01C */ f32 yCounter; //counts around where the y is but not at
    /* 0x020 */ f32 waterGIMP; //how much to gimp you in water? idk but its correlated
    /* 0x024 */ Vec3f vel;
    /* 0x030 */ Vec3f vaultlocity;
    /* 0x03C */ f32 yAngle;
    /* 0x040 */ f32 forwardVel; //between 0 and 20.8. gets broken on slope jumps
    /* 0x044 */ f32 forwardImpulse;
    /* 0x048 */ f32 waterFactor; //gets effected in water again
    /* 0x04C */ f32 hitboxSize; //30 default
    /* 0x050 */ f32 hitboxYStretch; //unconfirmed. 150 default.
    /* 0x054 */ u32 canJump;    //0x00 = yes, 0x01 = no
    /* 0x058 */ u32 jumpReleasedInAir;    // 0x00 = no, 0x01 = yes
    /* 0x05C */ u32 jumpAnimFrame;
    /* 0x060 */ u32 hasTumbled;    //0x00 = no, 0x01 = yes. resets on jump.
    /* 0x064 */ u32 unk64;
    /* 0x068 */ u32 inWater;//0x00 = no, 0x01 = yes.
    /* 0x06C */ u32 squishTimer;
    /* 0x070 */ f32 yScale;
    /* 0x074 */ u32 locked; //0x00 = no, 0x16 = yes. when using lock to stand in place.
    /* 0x078 */ s32 amountToShoot; //number for machine gun shoot
    /* 0x07C */ s32 surface; //-1 when off ground, diff number when on diff surface. if 0 you slow to a crawl
    /* 0x080 */ s32 wSurface; //-1 when not in water, diff number when in diff water
    /* 0x084 */ s32 surfaceSlide; //1 if you slide on a slope while standing. 0 if else (not walkable slopes)
    /* 0x088 */ s32 surfaceFine; //more accurate
    /* 0x08C */ s32 vaulting; // 0 if not, 1 if
    /* 0x090 */ f32 xFromCenter; //from center of room (when on ground)
    /* 0x094 */ f32 yFromCenter;
    /* 0x098 */ f32 zFromCenter;
    /* 0x09C */ Vec3f shift; //override(?) when on moving objects (falling bridges, etc)
    /* 0x0A8 */ Vec3f move; //override when sliding on slopes or on poles
    /* 0x0B4 */ u32 groundMovement; //0x00 = standing, 0x01 = walking, 0x02 = running
    /* 0x0B8 */ f32 globalTimer;
    /* 0x0BC */ u32 unkBC;
    /* 0x0C0 */ u32 shootLeft;
    /* 0x0C4 */ u32 vaultFall;//timer for falling after vault
    /* 0x0C8 */ u32 hp;
    /* 0x0CC */ u32 playerHURTSTATE;
    /* 0x0D0 */ u32 playerHURTTIMER;
    /* 0x0D4 */ u32 playerHURTANIM;
    /* 0x0D8 */ u32 playerHURTBY;
    /* 0x0DC */ f32 unk_DC;
    /* 0x0E0 */ f32 unk_E0;
    /* 0x0E4 */ f32 unk_E4;
    /* 0x0E8 */ f32 unk_E8;
    /* 0x0EC */ f32 unk_EC;
    /* 0x0F0 */ f32 unk_F0;
    /* 0x0F4 */ f32 unk_F4;
    /* 0x0F8 */ f32 unk_F8;
    /* 0x0FC */ f32 unk_FC;
    /* 0x100 */ f32 unk_100;
    /* 0x104 */ f32 unk_104;
    /* 0x108 */ f32 unk_108;
    /* 0x10C */ f32 timerDown;
    /* 0x110 */ f32 reticleSize;
    /* 0x114 */ s32 active; //0x00 = no, 0x01 = yes
    /* 0x118 */ s32 exists; //0x00 = no, 0x01 = yes
    /* 0x11C */ u32 power; //enum of power it has
    /* 0x120 */ u32 powerTimer; 
    /* 0x124 */ u32 powerTimerTill; 
    /* 0x128 */ f32 tongueYOffset; 
    /* 0x12C */ f32 tongueSeperation; 
} playerActor; //sizeof 0x130

typedef struct Camera {//take these with a grain of salt
    /* 0x00 */ s32 unk0;
    /* 0x04 */ Vec3f f1; //angle freecam will snap to
    /* 0x10 */ Vec3f f2;
    /* 0x1C */ Vec3f f3;
    /* 0x28 */ Vec3f f4; // perspective "eye"
    /* 0x34 */ Vec3f f5; // perspective "at"
    /* 0x40 */ s32 unk40;
    /* 0x44 */ f32 size1;
    /* 0x48 */ f32 size2;
    /* 0x4C */ u32 untouchedTimer; //timer that incs when the camera hasnt been used
    /* 0x50 */ f32 unk50;
    /* 0x54 */ s32 pushHoriz;//the impulse horizontally by the player
    /* 0x58 */ s32 unk58;
    /* 0x5C */ f32 unk5C;
    /* 0x60 */ f32 unk60;
    /* 0x64 */ f32 unk64;
    /* 0x68 */ f32 unk68;
    /* 0x6C */ f32 unk6C;
    /* 0x70 */ f32 unk70;
} Camera; //size 0x74

extern playerActor gPlayerActors[4];
extern Camera gCamera[4];
#endif
