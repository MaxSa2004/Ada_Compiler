  .data
newline: .asciiz "\n"
str_0: .asciiz "Hello World!"

    .text
  .globl main
main:
  addi $sp, $sp, -16
  sw $ra, 12($sp)     # save ra
  sw $fp, 8($sp)     # save fp
  addi $fp, $sp, 16
  #TAC: opcode:12
  la $a0, str_0
  li $v0, 4
  syscall
  la $a0, newline
  li $v0, 4
  syscall
  lw $ra, 12($sp)     # restore ra
  lw $fp, 8($sp)     # restore fp
  addi $sp, $sp, 16
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
