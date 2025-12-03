.data
str_1: .asciiz "neg"
str_0: .asciiz "nonneg"
.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    addi $t0, $zero, 1
    move $t6, $t0
    addi $t1, $zero, 2
    move $t8, $t1
    addi $t0, $zero, 3
    move $t9, $t0
    mul $t1, $t8, $t9
    move $t7, $t1
    add $t1, $t6, $t7
    move $t4, $t1
    addi $t1, $zero, 4
    move $s0, $t1
    addi $t0, $zero, 2
    move $s1, $t0
    div $s0, $s1
    mflo $t1
    move $t5, $t1
    sub $t1, $t4, $t5
    move $t2, $t1
    addi $t1, $zero, 5
    move $s2, $t1
    addi $t0, $zero, 6
    move $s4, $t0
    addi $t1, $zero, 1
    move $s5, $t1
    sub $t1, $s4, $s5
    move $s3, $t1
    sub $t1, $s2, $s3
    move $t3, $t1
    add $t1, $t2, $t3
    sw $t1, 0($fp)
    lw $t0, 0($fp)
    move $s6, $t0
    addi $t1, $zero, 0
    move $s7, $t1
    slt $1, $s6, $s7
    bne $1, $zero, L1
    beq $1, $zero, L0
L0:
    lw $t0, 0($fp)
    move $a0, $t0
    li $v0, 1
    syscall
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
