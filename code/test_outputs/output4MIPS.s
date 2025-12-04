.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    addi $t0, $zero, 42
    sw $t0, 0($fp)
    lw $t1, 0($fp)
    move $a0, $t1
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    addi $t0, $zero, 0
    sw $t0, 4($fp)
    li $v0, 5
    syscall
    sw $v0, 4($fp)
    lw $t1, 4($fp)
    move $t3, $t1
    lw $t0, 0($fp)
    move $t4, $t0
    add $t1, $t3, $t4
    move $t2, $t1
    move $a0, $t2
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $v0, 10
    syscall
