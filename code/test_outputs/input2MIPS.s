  .data
newline: .asciiz "\n"
condition: .word 0
flag: .word 0
z: .word 0
y: .word 0
x: .word 0
name: .word 0
str_3: .asciiz "Final value of X:"
str_2: .asciiz "Incrementing X..."
str_1: .asciiz "Condition is false!"
str_0: .asciiz "Condition is true!"

    .text
  .globl main
main:
  addi $sp, $sp, -128
  sw $ra, 124($sp)     # save ra
  sw $fp, 120($sp)     # save fp
  addi $fp, $sp, 128
  #TAC: opcode:7
  la $t9, name
  lw $t0, 0($t9)
  #TAC: opcode:7
  la $t9, x
  lw $t0, 0($t9)
  #TAC: opcode:7
  la $t9, y
  lw $t0, 0($t9)
  #TAC: opcode:7
  la $t9, z
  lw $t0, 0($t9)
  #TAC: opcode:7
  la $t9, flag
  lw $t0, 0($t9)
  #TAC: opcode:7
  la $t9, condition
  lw $t0, 0($t9)
  #TAC: opcode:13
  li $v0, 5
  syscall
  move $t0, $v0
  #TAC: opcode:7
  li $t0, 5
  #TAC: opcode:24
  sub $t0, $zero, $t0
  #TAC: opcode:7
  #TAC: opcode:7
  li $t0, 10
  #TAC: opcode:7
  #TAC: opcode:2
  mul $t0, $t0, $t1
  #TAC: opcode:7
  li $t0, 3
  #TAC: opcode:1
  sub $t0, $t0, $t1
  #TAC: opcode:7
  li $t0, 2
  #TAC: opcode:3
  div $t0, $t1
  mflo $t0
  #TAC: opcode:0
  add $t0, $t0, $t1
  #TAC: opcode:7
  #TAC: opcode:7
  li $t0, 1
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
  li $t0, 20
  #TAC: opcode:19
  slt $t2, $t0, $t1
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
  #TAC: opcode:9
Label3:
  #TAC: opcode:7
  li $t0, 15
  #TAC: opcode:16
  slt $t0, $t0, $t1
  #TAC: opcode:11
  beq $t0, $zero, Label5
  #TAC: opcode:10
  j Label4
  #TAC: opcode:9
Label4:
  #TAC: opcode:7
  li $t0, 1
  #TAC: opcode:0
  add $t0, $t0, $t1
  #TAC: opcode:7
  #TAC: opcode:12
  la $a0, str_2
  li $v0, 4
  syscall
  la $a0, newline
  li $v0, 4
  syscall
  #TAC: opcode:10
  j Label3
  #TAC: opcode:9
Label5:
  #TAC: opcode:12
  la $a0, str_3
  li $v0, 4
  syscall
  la $a0, newline
  li $v0, 4
  syscall
  #TAC: opcode:12
  li $v0, 1
  syscall
  la $a0, newline
  li $v0, 4
  syscall
  lw $ra, 124($sp)     # restore ra
  lw $fp, 120($sp)     # restore fp
  addi $sp, $sp, 128
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
