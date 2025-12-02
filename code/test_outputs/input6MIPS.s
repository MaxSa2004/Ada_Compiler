  .data
newline: .asciiz "\n"
t: .word 0
b: .word 0
a: .word 0
str_1: .asciiz "false"
str_0: .asciiz "true"

    .text
  .globl main
main:
  addi $sp, $sp, -72
  sw $ra, 68($sp)     # save ra
  sw $fp, 64($sp)     # save fp
  addi $fp, $sp, 72
  #TAC: opcode:7
  la $t9, a
  lw $t0, 0($t9)
  #TAC: opcode:7
  la $t9, b
  lw $t0, 0($t9)
  #TAC: opcode:7
  la $t9, t
  lw $t0, 0($t9)
  #TAC: opcode:7
  li $t0, 3
  #TAC: opcode:7
  #TAC: opcode:7
  li $t0, 5
  #TAC: opcode:7
  #TAC: opcode:16
  slt $t0, $t0, $t1
  #TAC: opcode:7
  li $t0, 0
  #TAC: opcode:14
  xor $t2, $t0, $t1
  sltiu $t0, $t2, 1
  #TAC: opcode:22
  sltiu $t0, $t0, 1
  #TAC: opcode:20
  and $t0, $t0, $t1
  #TAC: opcode:7
  li $t0, 5
  #TAC: opcode:18
  slt $t2, $t1, $t0
  xori $t0, $t2, 1
  #TAC: opcode:21
  or  $t0, $t0, $t1
  #TAC: opcode:7
  #TAC: opcode:15
  li $t1, 0
  xor $t2, $t0, $t1
  sltu  $t0, $zero, $t2
  #TAC: opcode:11
  beq $t0, $zero, Label1
  #TAC: opcode:10
  j Label0
  #TAC: opcode:9
Label0:
  #TAC: opcode:12
  la $a0, str_0
  li $v0, 4
  syscall
  la $a0, newline
  li $v0, 4
  syscall
  #TAC: opcode:10
  j Label2
  #TAC: opcode:9
Label1:
  #TAC: opcode:12
  la $a0, str_1
  li $v0, 4
  syscall
  la $a0, newline
  li $v0, 4
  syscall
  #TAC: opcode:9
Label2:
  lw $ra, 68($sp)     # restore ra
  lw $fp, 64($sp)     # restore fp
  addi $sp, $sp, 72
  li $v0, 10
  syscall

    # Helper routines
# integer power helper (pow_int). expects $a0=base, $a1=exp; returns $v0
pow_int:
  li $t0, 0
  blez $a1, pow_done
  li $v0, 1
pow_loop:
  mul $v0, $v0, $a0
  addi $a1, $a1, -1
  bgtz $a1, pow_loop
pow_done:
  jr $ra
