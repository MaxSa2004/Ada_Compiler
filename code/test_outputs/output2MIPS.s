.data
str_buf: .space 256
str_4: .asciiz "from:"
str_3: .asciiz "Final value of X:"
str_2: .asciiz "Incrementing X..."
str_1: .asciiz "Condition is false!"
str_0: .asciiz "Condition is true!"
.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    la $a0, str_buf
    li $a1, 256
    li $v0, 8
    syscall
    la $t0, str_buf
    sw $t0, 0($fp)
    addi $t0, $zero, 5
    move $t2, $t0
    addi $t1, $zero, 0
    sub $t1, $t1, $t2
    sw $t1, 4($fp)
    addi $t0, $zero, 10
    sw $t0, 8($fp)
    lw $t1, 4($fp)
    move $t5, $t1
    lw $t0, 8($fp)
    move $t6, $t0
    mul $t1, $t5, $t6
    move $t3, $t1
    lw $t1, 4($fp)
    move $t9, $t1
    addi $t0, $zero, 3
    move $s0, $t0
    sub $t1, $t9, $s0
    move $t7, $t1
    addi $t1, $zero, 2
    move $t8, $t1
    div $t7, $t8
    mflo $t1
    move $t4, $t1
    add $t1, $t3, $t4
    sw $t1, 12($fp)
    addi $t0, $zero, 1
    sw $t0, 16($fp)
    lw $t1, 4($fp)
    move $s5, $t1
    lw $t0, 8($fp)
    move $s6, $t0
    slt $1, $s5, $s6
    bne $1, $zero, L0
    beq $1, $zero, L1
L0:
    addi $t1, $zero, 1
    move $s3, $t1
    j L2
L1:
    addi $t0, $zero, 0
    move $s3, $t0
L2:
    lw $t1, 16($fp)
    move $t2, $t1
    addi $t0, $zero, 0
    move $t3, $t0
    beq $t2, $t3, L3
    bne $t2, $t3, L4
L3:
    addi $t1, $zero, 1
    move $s7, $t1
    j L5
L4:
    addi $t0, $zero, 0
    move $s7, $t0
L5:
    addi $t1, $zero, 0
    bne $s7, $t1, L6
    beq $s7, $t1, L7
L6:
    addi $t0, $zero, 0
    move $s4, $t0
    j L8
L7:
    addi $t1, $zero, 1
    move $s4, $t1
L8:
    and $t1, $s3, $s4
    move $s1, $t1
    lw $t0, 12($fp)
    move $t4, $t0
    addi $t1, $zero, 20
    move $t5, $t1
    slt $1, $t4, $t5
    bne $1, $zero, L10
    beq $1, $zero, L9
L9:
    addi $t0, $zero, 1
    move $s2, $t0
    j L11
L10:
    addi $t1, $zero, 0
    move $s2, $t1
L11:
    or $t1, $s1, $s2
    sw $t1, 20($fp)
    lw $t0, 20($fp)
    move $t6, $t0
    addi $t1, $zero, 0
    bne $t6, $t1, L12
    beq $t6, $t1, L13
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
    j cond
loop:
    lw $t0, 4($fp)
    move $t7, $t0
    addi $t1, $zero, 1
    move $t8, $t1
    add $t1, $t7, $t8
    sw $t1, 4($fp)
    la $a0, str_2
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
cond:
    lw $t0, 4($fp)
    move $t9, $t0
    addi $t1, $zero, 5
    move $s0, $t1
    slt $1, $t9, $s0
    bne $1, $zero, loop
    beq $1, $zero, end
end:
    la $a0, str_3
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $t0, 4($fp)
    move $a0, $t0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    la $a0, str_4
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    lw $a0, 0($fp)
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $v0, 10
    syscall
