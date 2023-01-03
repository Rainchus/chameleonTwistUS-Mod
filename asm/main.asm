.n64 // Let armips know we're coding for the N64 architecture
.open "rom/ct1.us.z64", "rom/ct1.us.mod.z64", 0 // Open the ROM file
.include "asm/bootPatch.asm"
.headersize 0x7F86E000
.org 0x80401000
.include "asm/symbols.asm"
.include "asm/customASM.asm"
.importobj "obj/ct1.o"
.importobj "obj/lz4.o"
.importobj "obj/lib.o"
.close // Close the ROM file
