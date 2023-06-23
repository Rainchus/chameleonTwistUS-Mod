//Payload related information
.definelabel PAYLOAD_START_ROM, 0xB92000
.definelabel PAYLOAD_START_RAM, 0x80400000

//func_800809A0 - ROM 0x5BD10
.headersize 0x80024C90
.org 0x8008A824
LUI a0, hi(PAYLOAD_START_ROM)
ADDIU a0, a0, lo(PAYLOAD_START_ROM)
LUI a1, hi(PAYLOAD_START_RAM)
ADDIU a1, a1, lo(PAYLOAD_START_RAM)
LUI a2, hi(0xC00000 - 0xB92000)
JAL dma_copy
ADDIU a2, a2, lo(0xC00000 - 0xB92000)
J originalCode
NOP

.org 0x8008A8A8
J mainASMFunction
NOP

.headersize 0x7F86E000
.org 0x80400000
originalCode:
JAL cBootFunction
NOP
//restore from hook and jump back
LUI v0, 0x800F
LW v0, 0xA0A8 (v0)
ADDIU at, r0, 0xFFFF
LUI a0, 0x8012
BEQ v0, at, label0
ADDIU a0, a0, 0x5190
LUI at, 0x800F
SW v0, 0x68D8 (at)
LUI at, 0x800F
SW r0, 0x68DC (at)
label0:
LUI at, 0x800F
J 0x8008A84C
SW r0, 0x68DC (at)