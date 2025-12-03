.data
str_0: .asciiz "Hello World!"
.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    la $a0, str_0
    li $v0, 4
    syscall
    li $v0, 10
    syscall
