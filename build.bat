mkdir obj
mips64-elf-gcc -Wall -O2 -mtune=vr4300 -march=vr4300 -mabi=32 -fomit-frame-pointer -G0 -c src/ct1.c
move ct1.o obj/
mips64-elf-gcc -Wall -O2 -mtune=vr4300 -march=vr4300 -mabi=32 -fomit-frame-pointer -G0 -c src/lz4.c
move lz4.o obj/
mips64-elf-gcc -Wall -O2 -mtune=vr4300 -march=vr4300 -mabi=32 -fomit-frame-pointer -G0 -c src/lib.c
move lib.o obj/
mips64-elf-gcc -Wall -O2 -mtune=vr4300 -march=vr4300 -mabi=32 -fomit-frame-pointer -G0 -c src/menu.c
move menu.o obj/
mips64-elf-gcc -Wall -O2 -mtune=vr4300 -march=vr4300 -mabi=32 -fomit-frame-pointer -G0 -c src/text.c
move text.o obj/
armips asm/main.asm
n64crc "rom/ct1.us.mod.z64"