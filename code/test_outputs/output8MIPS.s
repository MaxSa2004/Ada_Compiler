.data
str_1: .asciiz "high"
str_0: .asciiz "low"
.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    addi $t0, $zero, 0
    sw $t0, 0($fp)
    j cond
loop:
    lw $t1, 0($fp)
    move $t2, $t1
    addi $t0, $zero, 2
    move $t3, $t0
    slt $1, $t2, $t3
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
    lw $t1, 0($fp)
    move $t4, $t1
    addi $t0, $zero, 1
    move $t5, $t0
    add $t1, $t4, $t5
    sw $t1, 0($fp)
cond:
    lw $t1, 0($fp)
    move $t6, $t1
    addi $t0, $zero, 4
    move $t7, $t0
    slt $1, $t6, $t7
    bne $1, $zero, loop
    beq $1, $zero, end
end:
    lw $t1, 0($fp)
    move $a0, $t1
    li $v0, 1
    syscall
    li $v0, 10
    syscall
