//OS functions
.definelabel osInvalICache, 0x800D5BD0
.definelabel osInvalDCache, 0x800D5C50
.definelabel __osDisableInt, 0x800DBCE0
.definelabel __osRestoreInt, 0x800DBD00
.definelabel __osSpDeviceBusy, 0x800DF900
.definelabel __osSiDeviceBusy, 0x800E4CE0
.definelabel osContGetReadData, 0x800D4A24
.definelabel osContStartReadData, 0x800D4960

//functions
.definelabel printText, 0x8007C8A4
.definelabel _bzero, 0x800D94C0
.definelabel dma_copy, 0x800809A0
.definelabel ct_strlen, 0x800E503C
.definelabel ct_strchr, 0x800E5064
.definelabel ct_memcpy, 0x800E5010

//data
.definelabel decompressBuffer, 0x800E72C0