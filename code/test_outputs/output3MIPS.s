.data
str_1: .asciiz "no"
str_0: .asciiz "ok"
flt_0: .float 3.140000
.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    addi $t0, $zero, 5
    sw $t0, 0($fp)
    la $t0, flt_0
    lwc1 $f12, 0($t0)
   swc1 $f12, 4($fp)
    lw $t1, 0($fp)
    move $t2, $t1
    addi $t0, $zero, 3
    move $t3, $t0
    slt $1, $t3, $t2
    bne $1, $zero, L0
    beq $1, $zero, L1
L0:
    la $a0, str_0
    li $v0, 4
    syscall
    j L2
L1:
    la $a0, str_1
    li $v0, 4
    syscall
L2:
    li $v0, 10
    syscall
