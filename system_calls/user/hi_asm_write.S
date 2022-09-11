#include "kernel/syscall.h"

.text
.global main
main:
    # register a7 used to determine which system call
    addi sp, sp, -8
    sd ra, (sp)

    li a0, 1
    la a1, hello_str
    li a2, 14
    li a7, SYS_write

    ecall

    ld ra, (sp)
    addi sp, sp, 8
    jr ra

.section .rodata
hello_str:
    .string "Hello, world!\n"