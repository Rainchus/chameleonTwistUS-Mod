//OS functions
.definelabel osInvalICache, 0x800D5BD0
.definelabel osInvalDCache, 0x800D5C50
.definelabel __osDisableInt, 0x800DBCE0
.definelabel __osRestoreInt, 0x800DBD00
.definelabel __osSpDeviceBusy, 0x800DF900
.definelabel __osSiDeviceBusy, 0x800E4CE0
.definelabel osContGetReadData, 0x800D4A24
.definelabel osContStartReadData, 0x800D4960
.definelabel osCreateThread, 0x800D2AD0
.definelabel osStartThread, 0x800D2C20

//functions
.definelabel printText, 0x8007C8A4
.definelabel _bzero, 0x800D94C0
.definelabel dma_copy, 0x800809A0
.definelabel ct_strlen, 0x800E503C
.definelabel ct_strchr, 0x800E5064
.definelabel ct_memcpy, 0x800E5010

//data
.definelabel decompressBuffer, 0x800E72C0
.definelabel p1PressedButtons, 0x80181484
.definelabel p1HeldButtons, 0x80181450
.definelabel gameMode, 0x800F68D8
.definelabel subGameMode, 0x800F68DC
.definelabel D_80181450, 0x80181450
.definelabel D_8020850E, 0x8020850E
.definelabel D_80208510, 0x80208510
.definelabel gIsPaused, 0x801813F4