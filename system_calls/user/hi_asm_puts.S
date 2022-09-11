.text
.globl main
main:
    #TODO functie-oproep voorbereiden (hint: ra!)
    addi sp, sp, -8
    sd ra, (sp)
    la a0, hello_str

    jal ra, puts # dit springt naar de C-functie puts in user/puts.c
    #TODO return adres ra herstellen
    ld ra, (sp)
    addi sp, sp, 8
    

    jr ra

.section .rodata
hello_str: .string "Hello, world!"