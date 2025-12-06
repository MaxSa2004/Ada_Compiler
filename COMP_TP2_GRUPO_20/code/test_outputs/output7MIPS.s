.data
str_1: .asciiz "bad"
str_0: .asciiz "ok"
.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    addi $t0, $zero, 1
    move $t3, $t0
    addi $t1, $zero, 2
    move $t4, $t1
    add $t1, $t3, $t4
    move $t2, $t1
    addi $t0, $zero, 0
    sub $t1, $t0, $t2
    sw $t1, 0($fp)
    lw $t1, 0($fp)
    move $t5, $t1
    addi $t0, $zero, 3
    move $t7, $t0
    addi $t1, $zero, 0
    sub $t1, $t1, $t7
    move $t6, $t1
    beq $t5, $t6, L0
    bne $t5, $t6, L1
L0:
    la $a0, str_0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    j L2
L1:
    la $a0, str_1
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
L2:
    li $v0, 10
    syscall
