  .data
newline: .asciiz "\n"
x: .word 0
str_1: .asciiz "neg"
str_0: .asciiz "nonneg"

    .text
  .globl main
main:
  addi $sp, $sp, -88
  sw $ra, 84($sp)     # save ra
  sw $fp, 80($sp)     # save fp
  addi $fp, $sp, 88
  #TAC: opcode:7
  la $t9, x
  lw $t0, 0($t9)
  #TAC: opcode:7
  li $t0, 1
  #TAC: opcode:7
  li $t0, 2
  #TAC: opcode:7
  li $t0, 3
  #TAC: opcode:2
  mul $t0, $t0, $t1
  #TAC: opcode:0
  add $t0, $t0, $t1
  #TAC: opcode:7
  li $t0, 4
  #TAC: opcode:7
  li $t0, 2
  #TAC: opcode:3
  div $t0, $t1
  mflo $t0
  #TAC: opcode:1
  sub $t0, $t0, $t1
  #TAC: opcode:7
  li $t0, 5
  #TAC: opcode:7
  li $t0, 6
  #TAC: opcode:7
  li $t0, 1
  #TAC: opcode:1
  sub $t0, $t0, $t1
  #TAC: opcode:1
  sub $t0, $t0, $t1
  #TAC: opcode:0
  add $t0, $t0, $t1
  #TAC: opcode:7
  #TAC: opcode:7
  li $t0, 0
  #TAC: opcode:19
  slt $t2, $t0, $t1
  xori $t0, $t2, 1
  #TAC: opcode:11
  beq $t0, $zero, Label1
  #TAC: opcode:10
  j Label0
  #TAC: opcode:9
Label0:
  #TAC: opcode:12
  li $v0, 1
  syscall
  la $a0, newline
  li $v0, 4
  syscall
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
  lw $ra, 84($sp)     # restore ra
  lw $fp, 80($sp)     # restore fp
  addi $sp, $sp, 88
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
