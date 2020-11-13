.data
gbl_b: .quad 0
.text
lbl_fun_bae: lbl_fun_bae: pushq %rbp
movq %rsp, %rbp
addq %16, %rbp
subq $8, %rsp
Enter bae
lbl_0: Leave bae
main:
lbl_main: pushq %rbp
movq %rsp, %rbp
addq %16, %rbp
subq $32, %rsp
Enter main
movq $20. %rax
movq %rax, gbl_b(%rbp)
LitASCII amovq $0, %rax
movq %rax, -24(%rbp)
movq-24, %rax
movq %rax, -32(%rbp)
movq rax(%rbp), -40
movq-32, rax
movq-24, rax
movq -40(%rbp), %rax
cmpq $0, %rax
je lbl_2
movq $666. %rax
movq %rax, gbl_b(%rbp)
lbl_2: nop
movq $[n], %rdi
call bae
movq $0. %rax
jmp lbl_1
lbl_1: Leave main
