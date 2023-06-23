mainASMFunction:
JAL mainCFunction
NOP
LUI a1, 0x800F
LW a1, 0x68D8 (a1)
J 0x8008A8B0
NOP

getStatusRegister:
MFC0 v0, $12
NOP
NOP
JR RA
NOP

setStatusRegister:
MTC0 a0, $12
NOP
NOP
JR RA
NOP

__osDpDeviceBusy:
LUI t6, 0xA410
LW a0, 0x000C (t6)
ADDIU sp, sp, -0x08
ANDI t7, a0, 0x0100
BEQZ t7, dpRet
NOP
B dpExit
ADDIU v0, r0, 1
dpRet:
OR v0, r0, r0
dpExit:
JR RA
ADDIU sp, sp, 0x08

optimized_memcpy:
    addu $t0, $zero, $zero      // Counter for remaining bytes
    addu $t1, $zero, $a2        // Number of bytes to copy
    addu $t2, $a0, $t1          // End destination address
    addiu $t2, $t2, 0xFFFF            // Last destination address
    
    // Loop to copy in chunks of 8 bytes
    ld_loop:
        addiu $t0, $t1, 0xFFF8         // Calculate remaining bytes
        blez $t0, byte_copy      // Branch if less than 8 bytes remaining
        .word 0xDCAB0000    //ld $t3, 0($a1)           // Load 8-byte chunk from source
        .word 0xFC8B0000    //sd $t3, 0($a0)           // Store 8-byte chunk to destination
        
        addiu $a0, $a0, 8        // Increment destination address by 8
        addiu $a1, $a1, 8        // Increment source address by 8
        
        
        j ld_loop               // Jump to the beginning of the loop
            addiu $t1, $t1, 0xFFF8         // Decrement remaining bytes by 8
    
    // Copy remaining bytes byte by byte
    byte_copy:
        addiu $t0, $t0, 0xFFFF         // Decrement remaining bytes by 1
        blez $t0, end_memcpy    // Branch if no more bytes remaining
            lbu $t3, 0($a1)          // Load 1 byte from source
        sb $t3, 0($a0)           // Store 1 byte to destination
        addiu $a0, $a0, 1        // Increment destination address by 1
        
        
        j byte_copy             // Jump to the beginning of the loop
            addiu $a1, $a1, 1        // Increment source address by 1
    
    end_memcpy:
        jr $ra                   // Return
        nop
