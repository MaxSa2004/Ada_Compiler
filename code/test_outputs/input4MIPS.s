  .data
newline: .asciiz "\n"
x: .word 0

    .text
  .globl main
main:
  addi $sp, $sp, -24
  sw $ra, 20($sp)     # save ra
  sw $fp, 16($sp)     # save fp
  addi $fp, $sp, 24
  #TAC: opcode:7
  la $t9, x
  lw $t0, 0($t9)
  #TAC: opcode:7
  li $t0, 42
  #TAC: opcode:7
  #TAC: opcode:12
  li $v0, 1
  syscall
  la $a0, newline
  li $v0, 4
  syscall
  lw $ra, 20($sp)     # restore ra
  lw $fp, 16($sp)     # restore fp
  addi $sp, $sp, 24
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
