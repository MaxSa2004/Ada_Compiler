.data
flt_0: .float 3.140000
.text
.globl main
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    addi $fp, $sp, 16
    addi $t0, $zero, 5
    move $t2, $t0
    addi $t1, $zero, 3
    move $t3, $t1
    move $t4, $t2
    move $t2, $t3
    addi $t3, $zero, 1
    beq $t2, $zero, pow_end_x
pow_loop_x:
    mul $t3, $t3, $t4
    addi $t2, $t2, -1
    bne $t2, $zero, pow_loop_x
pow_end_x:
    move $t1, $t3
    sw $t1, 0($fp)
    lw $t0, 0($fp)
    move $a0, $t0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    la $t0, flt_0
    lwc1 $f12, 0($t0)
   swc1 $f12, 4($fp)
    lwc1 $f12, 4($fp)
    li $v0, 2
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $v0, 10
    syscall
