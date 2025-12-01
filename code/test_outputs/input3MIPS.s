  .data
newline: .asciiz "\n"
y: .word 0
x: .word 0
str_0: .asciiz "ok"

    .text
  .globl main
main:
  addi $sp, $sp, -40
  sw $ra, 36($sp)     # save ra
  sw $fp, 32($sp)     # save fp
  addi $fp, $sp, 40
  #TAC: opcode:7
  la $t9, x
  lw $t0, 0($t9)
  #TAC: opcode:7
  la $t9, y
  lw $t0, 0($t9)
  #TAC: opcode:7
  li $t0, 5
  #TAC: opcode:7
  #TAC: opcode:7
  # FLOAT CONST NOT FULLY SUPPORTED! val: 3.14
  # Skipping integer register load for float const to avoid clobbering.
  #TAC: opcode:7
  #TAC: opcode:7
  li $t0, 3
  #TAC: opcode:17
  slt $t0, $t1, $t0
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
  #TAC: opcode:9
Label2:
  lw $ra, 36($sp)     # restore ra
  lw $fp, 32($sp)     # restore fp
  addi $sp, $sp, 40
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
