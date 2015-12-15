.data
__tmp_6_var: .word 0
__tmp_7_var: .asciiz "i: "
__zero_var: .word 0
__tmp_4_var: .word 8
__tmp_5_var: .word 1
__tmp_2_var: .word 0
__tmp_3_var: .asciiz "Okay! Testing to display i from 1 to 9...\n"
__tmp_0_var: .asciiz "Testing declaration+affectation float f=3.5...\n"
__tmp_1_var: .float 3.500000
__tmp_8_var: .asciiz "\n"
__tmp_9_var: .asciiz "Okay! Testing f!=3.5 and else...\n"
__tmp_17_var: .word 2
__tmp_24_var: .float 3.500000
__tmp_35_var: .word 2
__tmp_42_var: .float 1.000000
__tmp_16_var: .asciiz "Okay! Testing nested if elses...\n"
__tmp_25_var: .asciiz "3 levels of if then else ! Testing i++ and i--..."
__tmp_34_var: .word 5
__tmp_43_var: .float 3.000000
__tmp_15_var: .asciiz "\n"
__tmp_26_var: .asciiz "\ni++ = "
__tmp_37_var: .word 0
__tmp_40_var: .float 7.000000
__tmp_14_var: .float 3.500000
__tmp_27_var: .asciiz "\ni-- = "
__tmp_36_var: .word 5
__tmp_41_var: .float 3.000000
__tmp_13_var: .float 3.000000
__tmp_20_var: .word 3
__tmp_31_var: .word 0
__tmp_46_var: .float 3.500000
__tmp_12_var: .asciiz "Okay! Testing print(f+3)...\n"
__tmp_21_var: .asciiz "Wrong comparasion (2>(3+5))\n"
__tmp_30_var: .word 3
__tmp_47_var: .asciiz "Wrong comparasion (3.0e-19<f+1)"
__tmp_11_var: .asciiz "wrong! You're FIRED !\n"
__tmp_22_var: .float 0.000000
__tmp_33_var: .word 8
__tmp_44_var: .float 3.500000
__tmp_10_var: .float 3.500000
__tmp_23_var: .float 1.000000
__tmp_32_var: .word 5
__tmp_45_var: .float 3.000000
__tmp_48_var: .asciiz "\nYou got it!\n"
__tmp_28_var: .asciiz "\n"
__tmp_39_var: .float 3.000000
__tmp_29_var: .asciiz "Okay! Testing surprise declaration with syscall name and operations !!\n"
__tmp_38_var: .asciiz "\nOkay! Testing float operations !!\n"
__tmp_19_var: .word 5
__tmp_18_var: .word 3
__one_var: .word 1
f_var: .float 3.500000
i_var: .word 0
syscall_var: .word 0
.text
main:
la $a0, __tmp_0_var
li $v0, 4
syscall
lw $t0, __tmp_1_var
sw $t0, f_var
lw $t0, __tmp_2_var
sw $t0, i_var
la $a0, __tmp_3_var
li $v0, 4
syscall
l1: lw $t1, i_var
lw $t2, __tmp_4_var
ble $t1, $t2, l2
j l3
l2: lw $t1, i_var
lw $t2, __tmp_5_var
add $t0, $t1, $t2
sw $t0, __tmp_6_var
lw $t0, __tmp_6_var
sw $t0, i_var
la $a0, __tmp_7_var
li $v0, 4
syscall
li $v0, 1
lw $a0, i_var
syscall
la $a0, __tmp_8_var
li $v0, 4
syscall
j l1
l3: la $a0, __tmp_9_var
li $v0, 4
syscall
lw $t1, f_var
lw $t2, __tmp_10_var
bne $t1, $t2, l4
j l5
l4: la $a0, __tmp_11_var
li $v0, 4
syscall
li $v0, 2
l.s $f12, f_var
syscall
j l10
l5: la $a0, __tmp_12_var
li $v0, 4
syscall
l.s $f1, f_var
l.s $f2, __tmp_13_var
add.s $f0, $f1, $f2
s.s $f0, __tmp_14_var
li $v0, 2
l.s $f12, __tmp_14_var
syscall
la $a0, __tmp_15_var
li $v0, 4
syscall
la $a0, __tmp_16_var
li $v0, 4
syscall
lw $t1, __tmp_18_var
lw $t2, __tmp_19_var
add $t0, $t1, $t2
sw $t0, __tmp_20_var
lw $t1, __tmp_17_var
lw $t2, __tmp_20_var
bgt $t1, $t2, l6
j l7
l6: la $a0, __tmp_21_var
li $v0, 4
syscall
j l10
l7: l.s $f1, f_var
l.s $f2, __tmp_23_var
add.s $f0, $f1, $f2
s.s $f0, __tmp_24_var
l.s $f1, __tmp_22_var
l.s $f2, __tmp_24_var
c.lt.s $f1, $f2
bc1t l8
j l9
l8: la $a0, __tmp_25_var
li $v0, 4
syscall
la $a0, __tmp_26_var
li $v0, 4
syscall
lw $t1, i_var
lw $t2, __one_var
add $t0, $t1, $t2
sw $t0, i_var
li $v0, 1
lw $a0, i_var
syscall
la $a0, __tmp_27_var
li $v0, 4
syscall
lw $t1, i_var
lw $t2, __one_var
sub $t0, $t1, $t2
sw $t0, i_var
li $v0, 1
lw $a0, i_var
syscall
la $a0, __tmp_28_var
li $v0, 4
syscall
la $a0, __tmp_29_var
li $v0, 4
syscall
lw $t1, i_var
lw $t2, __tmp_30_var
add $t0, $t1, $t2
sw $t0, __tmp_31_var
lw $t1, __tmp_32_var
lw $t2, __tmp_33_var
mult $t1, $t2
mflo $t0
sw $t0, __tmp_34_var
lw $t1, __tmp_34_var
lw $t2, __tmp_35_var
div $t1, $t2
mflo $t0
sw $t0, __tmp_36_var
lw $t1, __tmp_31_var
lw $t2, __tmp_36_var
sub $t0, $t1, $t2
sw $t0, __tmp_37_var
lw $t0, __tmp_37_var
sw $t0, syscall_var
li $v0, 1
lw $a0, syscall_var
syscall
la $a0, __tmp_38_var
li $v0, 4
syscall
l.s $f1, __tmp_39_var
l.s $f2, __tmp_40_var
mul.s $f0, $f1, $f2
s.s $f0, __tmp_41_var
l.s $f1, __tmp_41_var
l.s $f2, __tmp_42_var
div.s $f0, $f1, $f2
s.s $f0, __tmp_43_var
l.s $f1, f_var
l.s $f2, __tmp_43_var
add.s $f0, $f1, $f2
s.s $f0, __tmp_44_var
l.s $f1, __tmp_44_var
l.s $f2, __tmp_45_var
sub.s $f0, $f1, $f2
s.s $f0, __tmp_46_var
lw $t0, __tmp_46_var
sw $t0, f_var
li $v0, 2
l.s $f12, f_var
syscall
j l10
l9: la $a0, __tmp_47_var
li $v0, 4
syscall
l10: la $a0, __tmp_48_var
li $v0, 4
syscall
jr $ra
