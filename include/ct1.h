#ifndef _CT1_H
#define _CT1_H

#include "types.h"
#include "enums.h"

extern s32 p1PressedButtons;
extern s32 p1HeldButtons;
extern s32 gameMode;
extern s32 subGameMode;
//extern s32 debugBool;

#define DPAD_UP 0x08000000
#define DPAD_DOWN 0x04000000
#define DPAD_LEFT 0x02000000
#define DPAD_RIGHT 0x01000000
#define L_BUTTON 0x00200000
#define R_BUTTON 0x00100000
#define B_BUTTON 0x40000000
#define A_BUTTON 0x80000000

//OS function defines
void osInvalICache(void*, s32);
void osInvalDCache(void*, s32);
int __osDpDeviceBusy();
int __osSpDeviceBusy();
int __osSiDeviceBusy();

//user functions
void decompress_lz4_ct_default(int srcSize, int savestateCompressedSize, u8* compressBuffer);
s32 compress_lz4_ct_default(const u8* srcData, int srcSize, u8* bufferAddr);
void* my_memmove(void* dest, const void* src, int n);
void printText(f32 xPos, f32 yPos, f32 zero, f32 scale, f32 zero2, f32 zero3, void* text, s32);

extern char decompressBuffer[0x803B5000 - 0x800EE1C0];

#endif
