.data
__tmp_6_var: .float 5.000000
__tmp_7_var: .asciiz "\nthis better not be a bullshit value (expecting 5.0)\n"
__zero_var: .word 0
__tmp_4_var: .asciiz "wrong"
__tmp_5_var: .float 3.120000
__tmp_2_var: .float 3.140000
__tmp_3_var: .float 3.141300
__tmp_0_var: .float 3.140000
__tmp_1_var: .float 3.140000
pi_var: .float 0.000000
__one_var: .word 1
.text
main:
lw $t0, __tmp_0_var
sw $t0, pi_var
l1: lw $t1, pi_var
lw $t2, __tmp_1_var
beq $t1, $t2, l2
j l6
l2: lw $t1, pi_var
lw $t2, __tmp_2_var
beq $t1, $t2, l3
j l4
l3: lw $t0, __tmp_3_var
sw $t0, pi_var
j l5
l4: la $a0, __tmp_4_var
li $v0, 4
syscall
lw $t0, __tmp_5_var
sw $t0, pi_var
l5: j l1
l6: lw $t0, __tmp_6_var
sw $t0, pi_var
li $v0, 2
l.s $f12, pi_var
syscall
la $a0, __tmp_7_var
li $v0, 4
syscall
jr $ra
