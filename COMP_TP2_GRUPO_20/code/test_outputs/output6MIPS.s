.data
str_1: .asciiz "false"
str_0: .asciiz "true"
.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    addi $t0, $zero, 3
    sw $t0, 0($fp)
    addi $t1, $zero, 5
    sw $t1, 4($fp)
    lw $t0, 0($fp)
    move $t6, $t0
    lw $t1, 4($fp)
    move $t7, $t1
    slt $1, $t6, $t7
    bne $1, $zero, L0
    beq $1, $zero, L1
L0:
    addi $t0, $zero, 1
    move $t4, $t0
    j L2
L1:
    addi $t1, $zero, 0
    move $t4, $t1
L2:
    lw $t0, 0($fp)
    move $t9, $t0
    addi $t1, $zero, 0
    move $s0, $t1
    beq $t9, $s0, L3
    bne $t9, $s0, L4
L3:
    addi $t0, $zero, 1
    move $t8, $t0
    j L5
L4:
    addi $t1, $zero, 0
    move $t8, $t1
L5:
    addi $t0, $zero, 0
    bne $t8, $t0, L6
    beq $t8, $t0, L7
L6:
    addi $t1, $zero, 0
    move $t5, $t1
    j L8
L7:
    addi $t0, $zero, 1
    move $t5, $t0
L8:
    and $t1, $t4, $t5
    move $t2, $t1
    lw $t1, 4($fp)
    move $s1, $t1
    addi $t0, $zero, 5
    move $s2, $t0
    slt $1, $s2, $s1
    bne $1, $zero, L10
    beq $1, $zero, L9
L9:
    addi $t1, $zero, 1
    move $t3, $t1
    j L11
L10:
    addi $t0, $zero, 0
    move $t3, $t0
L11:
    or $t1, $t2, $t3
    sw $t1, 8($fp)
    lw $t1, 8($fp)
    move $s3, $t1
    addi $t0, $zero, 0
    bne $s3, $t0, L12
    beq $s3, $t0, L13
L12:
    la $a0, str_0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    j L14
L13:
    la $a0, str_1
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
L14:
    li $v0, 10
    syscall
