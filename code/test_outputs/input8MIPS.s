  .data
newline: .asciiz "\n"
i: .word 0
str_1: .asciiz "high"
str_0: .asciiz "low"

    .text
  .globl main
main:
  addi $sp, $sp, -48
  sw $ra, 44($sp)     # save ra
  sw $fp, 40($sp)     # save fp
  addi $fp, $sp, 48
  #TAC: opcode:7
  la $t9, i
  lw $t0, 0($t9)
  #TAC: opcode:7
  li $t0, 0
  #TAC: opcode:7
  #TAC: opcode:9
Label0:
  #TAC: opcode:7
  li $t0, 4
  #TAC: opcode:16
  slt $t0, $t0, $t1
  #TAC: opcode:11
  beq $t0, $zero, Label2
  #TAC: opcode:10
  j Label1
  #TAC: opcode:9
Label1:
  #TAC: opcode:7
  li $t0, 2
  #TAC: opcode:16
  slt $t0, $t0, $t1
  #TAC: opcode:11
  beq $t0, $zero, Label4
  #TAC: opcode:10
  j Label3
  #TAC: opcode:9
Label3:
  #TAC: opcode:12
  la $a0, str_0
  li $v0, 4
  syscall
  la $a0, newline
  li $v0, 4
  syscall
  #TAC: opcode:10
  j Label5
  #TAC: opcode:9
Label4:
  #TAC: opcode:12
  la $a0, str_1
  li $v0, 4
  syscall
  la $a0, newline
  li $v0, 4
  syscall
  #TAC: opcode:9
Label5:
  #TAC: opcode:7
  li $t0, 1
  #TAC: opcode:0
  add $t0, $t0, $t1
  #TAC: opcode:7
  #TAC: opcode:10
  j Label0
  #TAC: opcode:9
Label2:
  lw $ra, 44($sp)     # restore ra
  lw $fp, 40($sp)     # restore fp
  addi $sp, $sp, 48
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
