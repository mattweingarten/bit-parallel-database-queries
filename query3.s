	.file	"query3.c"
	.text
	.globl	q3
	.type	q3, @function
q3:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%rdx, -72(%rbp)
	movq	%rcx, -80(%rbp)
	movl	%r8d, -84(%rbp)
	movl	%r9d, -88(%rbp)
	movq	$0, -16(%rbp)
	movl	-88(%rbp), %edx
	movl	24(%rbp), %eax
	addl	%edx, %eax
	cltq
	movq	%rax, -8(%rbp)
	movl	$0, -36(%rbp)
	jmp	.L2
.L10:
	movl	$0, -32(%rbp)
	jmp	.L3
.L9:
	movl	-36(%rbp), %eax
	imull	-88(%rbp), %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	-56(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -28(%rbp)
	movl	-32(%rbp), %eax
	imull	24(%rbp), %eax
	cltq
	addq	$1, %rax
	leaq	0(,%rax,4), %rdx
	movq	-64(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -24(%rbp)
	movl	-32(%rbp), %eax
	imull	24(%rbp), %eax
	cltq
	addq	$2, %rax
	leaq	0(,%rax,4), %rdx
	movq	-64(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -20(%rbp)
	cmpl	$0, -24(%rbp)
	je	.L4
	movl	-28(%rbp), %eax
	movl	$0, %edx
	divl	-24(%rbp)
	movl	%edx, %eax
	cmpl	%eax, -20(%rbp)
	jne	.L4
	movl	$0, -44(%rbp)
	movl	$0, -40(%rbp)
	jmp	.L5
.L6:
	movl	-36(%rbp), %eax
	imull	-88(%rbp), %eax
	movl	%eax, %edx
	movl	-44(%rbp), %eax
	addl	%edx, %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	-56(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movq	-16(%rbp), %rax
	imulq	-8(%rbp), %rax
	movq	%rax, %rdx
	movl	-44(%rbp), %eax
	cltq
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-72(%rbp), %rax
	addq	%rax, %rdx
	movl	(%rcx), %eax
	movl	%eax, (%rdx)
	addl	$1, -44(%rbp)
.L5:
	movl	-44(%rbp), %eax
	cmpl	-88(%rbp), %eax
	jl	.L6
	jmp	.L7
.L8:
	movl	-32(%rbp), %eax
	imull	24(%rbp), %eax
	movl	%eax, %edx
	movl	-40(%rbp), %eax
	addl	%edx, %eax
	cltq
	leaq	0(,%rax,4), %rdx
	movq	-64(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movq	-16(%rbp), %rax
	imulq	-8(%rbp), %rax
	movq	%rax, %rdx
	movl	-44(%rbp), %eax
	cltq
	addq	%rax, %rdx
	movl	-40(%rbp), %eax
	cltq
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-72(%rbp), %rax
	addq	%rax, %rdx
	movl	(%rcx), %eax
	movl	%eax, (%rdx)
	addl	$1, -40(%rbp)
.L7:
	movl	-40(%rbp), %eax
	cmpl	24(%rbp), %eax
	jl	.L8
	addq	$1, -16(%rbp)
.L4:
	addl	$1, -32(%rbp)
.L3:
	movl	-32(%rbp), %eax
	cmpl	16(%rbp), %eax
	jl	.L9
	addl	$1, -36(%rbp)
.L2:
	movl	-36(%rbp), %eax
	cmpl	-84(%rbp), %eax
	jl	.L10
	movq	-80(%rbp), %rax
	movq	-16(%rbp), %rdx
	movq	%rdx, (%rax)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	q3, .-q3
	.globl	q3_index
	.type	q3_index, @function
q3_index:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -56(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%rdx, -72(%rbp)
	movq	%rcx, -80(%rbp)
	movq	%r8, -88(%rbp)
	movq	%r9, -96(%rbp)
	movq	$0, -32(%rbp)
	movq	$2, -8(%rbp)
	movq	$0, -24(%rbp)
	jmp	.L12
.L16:
	movq	$0, -16(%rbp)
	jmp	.L13
.L15:
	movq	-24(%rbp), %rax
	imulq	-96(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-56(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -44(%rbp)
	movq	-16(%rbp), %rax
	imulq	24(%rbp), %rax
	addq	$1, %rax
	leaq	0(,%rax,4), %rdx
	movq	-64(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -40(%rbp)
	movq	-16(%rbp), %rax
	imulq	24(%rbp), %rax
	addq	$2, %rax
	leaq	0(,%rax,4), %rdx
	movq	-64(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -36(%rbp)
	cmpl	$0, -40(%rbp)
	je	.L14
	movl	-44(%rbp), %eax
	movl	$0, %edx
	divl	-40(%rbp)
	movl	%edx, %eax
	cmpl	%eax, -36(%rbp)
	jne	.L14
	movq	-32(%rbp), %rax
	imulq	-8(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-72(%rbp), %rax
	addq	%rdx, %rax
	movq	-24(%rbp), %rdx
	movl	%edx, (%rax)
	movq	-32(%rbp), %rax
	imulq	-8(%rbp), %rax
	addq	$1, %rax
	leaq	0(,%rax,4), %rdx
	movq	-72(%rbp), %rax
	addq	%rdx, %rax
	movq	-16(%rbp), %rdx
	movl	%edx, (%rax)
	addq	$1, -32(%rbp)
.L14:
	addq	$1, -16(%rbp)
.L13:
	movq	-16(%rbp), %rax
	cmpq	16(%rbp), %rax
	jb	.L15
	addq	$1, -24(%rbp)
.L12:
	movq	-24(%rbp), %rax
	cmpq	-88(%rbp), %rax
	jb	.L16
	movq	-80(%rbp), %rax
	movq	-32(%rbp), %rdx
	movq	%rdx, (%rax)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	q3_index, .-q3_index
	.globl	q3_weave
	.type	q3_weave, @function
q3_weave:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$136, %rsp
	movq	%rdi, -216(%rbp)
	movq	%rsi, -224(%rbp)
	movq	%rdx, -232(%rbp)
	movq	%rcx, -240(%rbp)
	movq	%r8, -248(%rbp)
	movq	%r9, -256(%rbp)
	movq	48(%rbp), %rax
	imulq	56(%rbp), %rax
	movq	%rax, -112(%rbp)
	movq	$0, -200(%rbp)
	movq	24(%rbp), %rdx
	movq	40(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -104(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	24(%rbp)
	movq	%rax, -96(%rbp)
	movq	56(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, -88(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	40(%rbp)
	movq	%rax, -80(%rbp)
	movq	56(%rbp), %rax
	imulq	-80(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	$0, -192(%rbp)
	jmp	.L18
.L38:
	movq	$0, -184(%rbp)
	jmp	.L19
.L20:
	movq	-184(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-248(%rbp), %rax
	addq	%rdx, %rax
	movl	$0, (%rax)
	addq	$1, -184(%rbp)
.L19:
	movq	-184(%rbp), %rax
	cmpq	24(%rbp), %rax
	jb	.L20
	movq	-192(%rbp), %rax
	imulq	24(%rbp), %rax
	movl	$0, %edx
	divq	-112(%rbp)
	movq	%rax, -64(%rbp)
	movq	-192(%rbp), %rax
	movl	$0, %edx
	divq	-88(%rbp)
	movq	%rdx, %rax
	movl	$0, %edx
	divq	-96(%rbp)
	movq	%rax, -56(%rbp)
	movq	-192(%rbp), %rax
	movl	$0, %edx
	divq	-96(%rbp)
	movq	24(%rbp), %rax
	imulq	%rdx, %rax
	movq	%rax, -48(%rbp)
	movq	$0, -176(%rbp)
	jmp	.L21
.L24:
	movq	-64(%rbp), %rax
	imulq	-112(%rbp), %rax
	movq	%rax, %rdx
	movq	-176(%rbp), %rax
	imulq	56(%rbp), %rax
	addq	%rax, %rdx
	movq	-56(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
	movq	$0, -168(%rbp)
	jmp	.L22
.L23:
	movq	-8(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-216(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	-48(%rbp), %rax
	movl	%eax, %ecx
	movq	-168(%rbp), %rax
	addl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -204(%rbp)
	movq	-168(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-248(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	48(%rbp), %rax
	movl	%eax, %ecx
	movq	-176(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-204(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movl	%esi, %ecx
	movq	-168(%rbp), %rax
	leaq	0(,%rax,4), %rsi
	movq	-248(%rbp), %rax
	addq	%rsi, %rax
	addl	%ecx, %edx
	movl	%edx, (%rax)
	addq	$1, -168(%rbp)
.L22:
	movq	-168(%rbp), %rax
	cmpq	24(%rbp), %rax
	jb	.L23
	addq	$1, -176(%rbp)
.L21:
	movq	-176(%rbp), %rax
	cmpq	48(%rbp), %rax
	jb	.L24
	movq	$0, -160(%rbp)
	jmp	.L25
.L37:
	movq	$0, -152(%rbp)
	jmp	.L26
.L27:
	movq	-152(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-256(%rbp), %rax
	addq	%rdx, %rax
	movl	$0, (%rax)
	addq	$1, -152(%rbp)
.L26:
	movq	-152(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L27
	movq	-160(%rbp), %rax
	imulq	40(%rbp), %rax
	movl	$0, %edx
	divq	-112(%rbp)
	movq	%rax, -40(%rbp)
	movq	-160(%rbp), %rax
	movl	$0, %edx
	divq	-72(%rbp)
	movq	%rdx, %rax
	movl	$0, %edx
	divq	-80(%rbp)
	movq	%rax, -32(%rbp)
	movq	-160(%rbp), %rax
	movl	$0, %edx
	divq	-80(%rbp)
	movq	40(%rbp), %rax
	imulq	%rdx, %rax
	movq	%rax, -24(%rbp)
	movq	$0, -144(%rbp)
	jmp	.L28
.L31:
	movq	-40(%rbp), %rax
	imulq	56(%rbp), %rax
	imulq	48(%rbp), %rax
	movq	%rax, %rdx
	movq	-144(%rbp), %rax
	imulq	56(%rbp), %rax
	addq	%rax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -16(%rbp)
	movq	$0, -136(%rbp)
	jmp	.L29
.L30:
	movq	-16(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-224(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	-24(%rbp), %rax
	movl	%eax, %ecx
	movq	-136(%rbp), %rax
	addl	%ecx, %eax
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -208(%rbp)
	movq	-136(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-256(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	48(%rbp), %rax
	movl	%eax, %ecx
	movq	-144(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-208(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movl	%esi, %ecx
	movq	-136(%rbp), %rax
	leaq	0(,%rax,4), %rsi
	movq	-256(%rbp), %rax
	addq	%rsi, %rax
	addl	%ecx, %edx
	movl	%edx, (%rax)
	addq	$1, -136(%rbp)
.L29:
	movq	-136(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L30
	addq	$1, -144(%rbp)
.L28:
	movq	-144(%rbp), %rax
	cmpq	48(%rbp), %rax
	jb	.L31
	movq	-256(%rbp), %rax
	addq	$4, %rax
	movl	(%rax), %eax
	testl	%eax, %eax
	je	.L32
	movq	-248(%rbp), %rax
	movl	(%rax), %eax
	movq	-256(%rbp), %rdx
	addq	$4, %rdx
	movl	(%rdx), %ecx
	movl	$0, %edx
	divl	%ecx
	movq	-256(%rbp), %rax
	addq	$8, %rax
	movl	(%rax), %eax
	cmpl	%eax, %edx
	jne	.L32
	movq	$0, -128(%rbp)
	movq	$0, -120(%rbp)
	jmp	.L33
.L34:
	movq	-128(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-248(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movq	-200(%rbp), %rax
	imulq	-104(%rbp), %rax
	movq	%rax, %rdx
	movq	-128(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-232(%rbp), %rax
	addq	%rax, %rdx
	movl	(%rcx), %eax
	movl	%eax, (%rdx)
	addq	$1, -128(%rbp)
.L33:
	movq	-128(%rbp), %rax
	cmpq	24(%rbp), %rax
	jb	.L34
	jmp	.L35
.L36:
	movq	-120(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-256(%rbp), %rax
	leaq	(%rdx,%rax), %rcx
	movq	-200(%rbp), %rax
	imulq	-104(%rbp), %rax
	movq	%rax, %rdx
	movq	-128(%rbp), %rax
	addq	%rax, %rdx
	movq	-120(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-232(%rbp), %rax
	addq	%rax, %rdx
	movl	(%rcx), %eax
	movl	%eax, (%rdx)
	addq	$1, -120(%rbp)
.L35:
	movq	-120(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L36
	addq	$1, -200(%rbp)
.L32:
	addq	$1, -160(%rbp)
.L25:
	movq	-160(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L37
	addq	$1, -192(%rbp)
.L18:
	movq	-192(%rbp), %rax
	cmpq	16(%rbp), %rax
	jb	.L38
	movq	-240(%rbp), %rax
	movq	-200(%rbp), %rdx
	movq	%rdx, (%rax)
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	q3_weave, .-q3_weave
	.globl	q3_weave_index
	.type	q3_weave_index, @function
q3_weave_index:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$104, %rsp
	movq	%rdi, -184(%rbp)
	movq	%rsi, -192(%rbp)
	movq	%rdx, -200(%rbp)
	movq	%rcx, -208(%rbp)
	movq	%r8, -216(%rbp)
	movq	%r9, -224(%rbp)
	movq	32(%rbp), %rax
	imulq	40(%rbp), %rax
	movq	%rax, -104(%rbp)
	movq	$0, -144(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	-224(%rbp)
	movq	%rax, -96(%rbp)
	movq	40(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, -88(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	24(%rbp)
	movq	%rax, -80(%rbp)
	movq	40(%rbp), %rax
	imulq	-80(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	$0, -136(%rbp)
	jmp	.L40
.L48:
	movl	$0, -168(%rbp)
	movq	-136(%rbp), %rax
	imulq	-224(%rbp), %rax
	movl	$0, %edx
	divq	-104(%rbp)
	movq	%rax, -64(%rbp)
	movq	-136(%rbp), %rax
	movl	$0, %edx
	divq	-88(%rbp)
	movq	%rdx, %rax
	movl	$0, %edx
	divq	-96(%rbp)
	movq	%rax, -56(%rbp)
	movq	-136(%rbp), %rax
	movl	$0, %edx
	divq	-96(%rbp)
	movq	-224(%rbp), %rax
	imulq	%rdx, %rax
	movq	%rax, -48(%rbp)
	movq	$0, -128(%rbp)
	jmp	.L41
.L42:
	movq	-64(%rbp), %rax
	imulq	-104(%rbp), %rax
	movq	%rax, %rdx
	movq	-128(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-56(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-184(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movq	-48(%rbp), %rdx
	movl	%edx, %ecx
	shrl	%cl, %eax
	andl	$1, %eax
	movl	%eax, -148(%rbp)
	movq	32(%rbp), %rax
	movl	%eax, %edx
	movq	-128(%rbp), %rax
	subl	%eax, %edx
	movl	%edx, %eax
	subl	$1, %eax
	movl	-148(%rbp), %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	addl	%eax, -168(%rbp)
	addq	$1, -128(%rbp)
.L41:
	movq	-128(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L42
	movq	$0, -120(%rbp)
	jmp	.L43
.L47:
	movl	$0, -164(%rbp)
	movl	$0, -160(%rbp)
	movq	-120(%rbp), %rax
	imulq	24(%rbp), %rax
	movl	$0, %edx
	divq	-104(%rbp)
	movq	%rax, -40(%rbp)
	movq	-120(%rbp), %rax
	movl	$0, %edx
	divq	-72(%rbp)
	movq	%rdx, %rax
	movl	$0, %edx
	divq	-80(%rbp)
	movq	%rax, -32(%rbp)
	movq	-120(%rbp), %rax
	movl	$0, %edx
	divq	-80(%rbp)
	movq	24(%rbp), %rax
	imulq	%rdx, %rax
	movq	%rax, -24(%rbp)
	movq	$0, -112(%rbp)
	jmp	.L44
.L45:
	movq	-40(%rbp), %rax
	imulq	40(%rbp), %rax
	imulq	32(%rbp), %rax
	movq	%rax, %rdx
	movq	-112(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movq	%rax, -16(%rbp)
	movq	-16(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-192(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	-24(%rbp), %rax
	addl	$1, %eax
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -156(%rbp)
	movq	-16(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-192(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	-24(%rbp), %rax
	addl	$2, %eax
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -152(%rbp)
	movq	32(%rbp), %rax
	movl	%eax, %edx
	movq	-112(%rbp), %rax
	subl	%eax, %edx
	movl	%edx, %eax
	subl	$1, %eax
	movl	-156(%rbp), %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	addl	%eax, -164(%rbp)
	movq	32(%rbp), %rax
	movl	%eax, %edx
	movq	-112(%rbp), %rax
	subl	%eax, %edx
	movl	%edx, %eax
	subl	$1, %eax
	movl	-152(%rbp), %edx
	movl	%eax, %ecx
	sall	%cl, %edx
	movl	%edx, %eax
	addl	%eax, -160(%rbp)
	addq	$1, -112(%rbp)
.L44:
	movq	-112(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L45
	cmpl	$0, -164(%rbp)
	je	.L46
	movl	-168(%rbp), %eax
	movl	$0, %edx
	divl	-164(%rbp)
	movl	%edx, %eax
	cmpl	%eax, -160(%rbp)
	jne	.L46
	movq	-144(%rbp), %rax
	leaq	0(,%rax,8), %rdx
	movq	-200(%rbp), %rax
	addq	%rdx, %rax
	movq	-136(%rbp), %rdx
	movl	%edx, (%rax)
	movq	-144(%rbp), %rax
	salq	$3, %rax
	leaq	4(%rax), %rdx
	movq	-200(%rbp), %rax
	addq	%rdx, %rax
	movq	-120(%rbp), %rdx
	movl	%edx, (%rax)
	addq	$1, -144(%rbp)
.L46:
	addq	$1, -120(%rbp)
.L43:
	movq	-120(%rbp), %rax
	cmpq	16(%rbp), %rax
	jb	.L47
	addq	$1, -136(%rbp)
.L40:
	movq	-136(%rbp), %rax
	cmpq	-216(%rbp), %rax
	jb	.L48
	movq	-208(%rbp), %rax
	movq	-144(%rbp), %rdx
	movq	%rdx, (%rax)
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	q3_weave_index, .-q3_weave_index
	.globl	q3_weave_index_l1_block
	.type	q3_weave_index_l1_block, @function
q3_weave_index_l1_block:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$256, %rsp
	movq	%rdi, -216(%rbp)
	movq	%rsi, -224(%rbp)
	movq	%rdx, -232(%rbp)
	movq	%rcx, -240(%rbp)
	movq	%r8, -248(%rbp)
	movq	%r9, -256(%rbp)
	movq	$0, -184(%rbp)
	movq	32(%rbp), %rax
	imulq	40(%rbp), %rax
	movq	%rax, -96(%rbp)
	movq	-248(%rbp), %rax
	imulq	-256(%rbp), %rax
	movl	$0, %edx
	divq	-96(%rbp)
	movq	%rax, -88(%rbp)
	movq	-96(%rbp), %rax
	movl	$0, %edx
	divq	-256(%rbp)
	movq	%rax, -80(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	-256(%rbp)
	movq	%rax, -72(%rbp)
	movq	40(%rbp), %rax
	imulq	-72(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	16(%rbp), %rax
	imulq	24(%rbp), %rax
	movl	$0, %edx
	divq	-96(%rbp)
	movq	%rax, -56(%rbp)
	movq	-96(%rbp), %rax
	movl	$0, %edx
	divq	24(%rbp)
	movq	%rax, -48(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	24(%rbp)
	movq	%rax, -40(%rbp)
	movq	40(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-80(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -24(%rbp)
	movq	-48(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -16(%rbp)
	movq	-48(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -8(%rbp)
	movq	$0, -176(%rbp)
	jmp	.L50
.L70:
	movq	-80(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	$0, -168(%rbp)
	jmp	.L51
.L56:
	movq	$0, -160(%rbp)
	jmp	.L52
.L55:
	movq	-176(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-168(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-216(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -192(%rbp)
	movq	$0, -152(%rbp)
	jmp	.L53
.L54:
	movq	-152(%rbp), %rax
	movl	%eax, %edx
	movq	-256(%rbp), %rax
	imull	%edx, %eax
	movl	-192(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -188(%rbp)
	movq	-160(%rbp), %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-168(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-188(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-160(%rbp), %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rcx
	movq	-152(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -152(%rbp)
.L53:
	movq	-152(%rbp), %rax
	cmpq	-72(%rbp), %rax
	jb	.L54
	addq	$1, -160(%rbp)
.L52:
	movq	-160(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L55
	addq	$1, -168(%rbp)
.L51:
	movq	-168(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L56
	movq	$0, -144(%rbp)
	jmp	.L57
.L69:
	movq	-48(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	-48(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-8(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	$0, -136(%rbp)
	jmp	.L58
.L63:
	movq	$0, -128(%rbp)
	jmp	.L59
.L62:
	movq	-144(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-136(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-128(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-224(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -204(%rbp)
	movq	$0, -120(%rbp)
	jmp	.L60
.L61:
	movq	-120(%rbp), %rax
	movl	%eax, %edx
	movq	24(%rbp), %rax
	imull	%edx, %eax
	addl	$1, %eax
	movl	-204(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -200(%rbp)
	movq	-120(%rbp), %rax
	movl	%eax, %edx
	movq	24(%rbp), %rax
	imull	%edx, %eax
	addl	$2, %eax
	movl	-204(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -196(%rbp)
	movq	-128(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rdx
	movq	-120(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-136(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-200(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-128(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rcx
	movq	-120(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-16(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-128(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rdx
	movq	-120(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-136(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-196(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-128(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rcx
	movq	-120(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-8(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -120(%rbp)
.L60:
	movq	-120(%rbp), %rax
	cmpq	-40(%rbp), %rax
	jb	.L61
	addq	$1, -128(%rbp)
.L59:
	movq	-128(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L62
	addq	$1, -136(%rbp)
.L58:
	movq	-136(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L63
	movq	$0, -112(%rbp)
	jmp	.L64
.L68:
	movq	$0, -104(%rbp)
	jmp	.L65
.L67:
	movq	-104(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	testl	%eax, %eax
	je	.L66
	movq	-112(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movq	-104(%rbp), %rdx
	leaq	0(,%rdx,4), %rcx
	movq	-16(%rbp), %rdx
	addq	%rcx, %rdx
	movl	(%rdx), %ecx
	movl	$0, %edx
	divl	%ecx
	movl	%edx, %ecx
	movq	-104(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	cmpl	%eax, %ecx
	jne	.L66
	movq	-176(%rbp), %rax
	movl	%eax, %edx
	movq	-80(%rbp), %rax
	imull	%eax, %edx
	movq	-112(%rbp), %rax
	movl	%eax, %esi
	movq	-184(%rbp), %rax
	leaq	0(,%rax,8), %rcx
	movq	-232(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-144(%rbp), %rax
	movl	%eax, %edx
	movq	-48(%rbp), %rax
	imull	%eax, %edx
	movq	-104(%rbp), %rax
	movl	%eax, %esi
	movq	-184(%rbp), %rax
	salq	$3, %rax
	leaq	4(%rax), %rcx
	movq	-232(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -184(%rbp)
.L66:
	addq	$1, -104(%rbp)
.L65:
	movq	-104(%rbp), %rax
	cmpq	-48(%rbp), %rax
	jb	.L67
	addq	$1, -112(%rbp)
.L64:
	movq	-112(%rbp), %rax
	cmpq	-80(%rbp), %rax
	jb	.L68
	addq	$1, -144(%rbp)
.L57:
	movq	-144(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jb	.L69
	addq	$1, -176(%rbp)
.L50:
	movq	-176(%rbp), %rax
	cmpq	-88(%rbp), %rax
	jb	.L70
	movq	-240(%rbp), %rax
	movq	-184(%rbp), %rdx
	movq	%rdx, (%rax)
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	q3_weave_index_l1_block, .-q3_weave_index_l1_block
	.globl	q3_unroll
	.type	q3_unroll, @function
q3_unroll:
.LFB11:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$320, %rsp
	movq	%rdi, -280(%rbp)
	movq	%rsi, -288(%rbp)
	movq	%rdx, -296(%rbp)
	movq	%rcx, -304(%rbp)
	movq	%r8, -312(%rbp)
	movq	%r9, -320(%rbp)
	movq	$0, -184(%rbp)
	movq	32(%rbp), %rax
	imulq	40(%rbp), %rax
	movq	%rax, -96(%rbp)
	movq	-312(%rbp), %rax
	imulq	-320(%rbp), %rax
	movl	$0, %edx
	divq	-96(%rbp)
	movq	%rax, -88(%rbp)
	movq	-96(%rbp), %rax
	movl	$0, %edx
	divq	-320(%rbp)
	movq	%rax, -80(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	-320(%rbp)
	movq	%rax, -72(%rbp)
	movq	40(%rbp), %rax
	imulq	-72(%rbp), %rax
	movq	%rax, -64(%rbp)
	movq	16(%rbp), %rax
	imulq	24(%rbp), %rax
	movl	$0, %edx
	divq	-96(%rbp)
	movq	%rax, -56(%rbp)
	movq	-96(%rbp), %rax
	movl	$0, %edx
	divq	24(%rbp)
	movq	%rax, -48(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	24(%rbp)
	movq	%rax, -40(%rbp)
	movq	40(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-80(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -24(%rbp)
	movq	-48(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -16(%rbp)
	movq	-48(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -8(%rbp)
	movq	$0, -176(%rbp)
	jmp	.L72
.L92:
	movq	-80(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	$0, -168(%rbp)
	jmp	.L73
.L78:
	movq	32(%rbp), %rax
	movl	%eax, %edx
	movq	-168(%rbp), %rax
	subl	%eax, %edx
	movl	%edx, %eax
	subl	$1, %eax
	movl	%eax, -256(%rbp)
	movq	$0, -160(%rbp)
	jmp	.L74
.L77:
	movq	-176(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-168(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-280(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -252(%rbp)
	movq	-176(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-168(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	addq	$1, %rax
	leaq	0(,%rax,4), %rdx
	movq	-280(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -248(%rbp)
	movq	-176(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-168(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	addq	$2, %rax
	leaq	0(,%rax,4), %rdx
	movq	-280(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -244(%rbp)
	movq	-176(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-168(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	addq	$3, %rax
	leaq	0(,%rax,4), %rdx
	movq	-280(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -240(%rbp)
	movq	-176(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-168(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	addq	$4, %rax
	leaq	0(,%rax,4), %rdx
	movq	-280(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -236(%rbp)
	movq	-176(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-168(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	addq	$5, %rax
	leaq	0(,%rax,4), %rdx
	movq	-280(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -232(%rbp)
	movq	-176(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-168(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	addq	$6, %rax
	leaq	0(,%rax,4), %rdx
	movq	-280(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -228(%rbp)
	movq	-176(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-168(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	addq	$7, %rax
	leaq	0(,%rax,4), %rdx
	movq	-280(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -224(%rbp)
	movq	$0, -152(%rbp)
	jmp	.L75
.L76:
	movq	-152(%rbp), %rax
	movl	%eax, %edx
	movq	-320(%rbp), %rax
	imull	%edx, %eax
	movl	%eax, -220(%rbp)
	movl	-220(%rbp), %eax
	movl	-252(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -216(%rbp)
	movl	-220(%rbp), %eax
	movl	-248(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -212(%rbp)
	movl	-220(%rbp), %eax
	movl	-244(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -208(%rbp)
	movl	-220(%rbp), %eax
	movl	-240(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -204(%rbp)
	movl	-220(%rbp), %eax
	movl	-236(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -200(%rbp)
	movl	-220(%rbp), %eax
	movl	-232(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -196(%rbp)
	movl	-220(%rbp), %eax
	movl	-228(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -192(%rbp)
	movl	-220(%rbp), %eax
	movl	-224(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -188(%rbp)
	movq	-160(%rbp), %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movl	-256(%rbp), %eax
	movl	-216(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-160(%rbp), %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rcx
	movq	-152(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-160(%rbp), %rax
	addq	$1, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movl	-256(%rbp), %eax
	movl	-212(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-160(%rbp), %rax
	addq	$1, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rcx
	movq	-152(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-160(%rbp), %rax
	addq	$2, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movl	-256(%rbp), %eax
	movl	-208(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-160(%rbp), %rax
	addq	$2, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rcx
	movq	-152(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-160(%rbp), %rax
	addq	$3, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movl	-256(%rbp), %eax
	movl	-204(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-160(%rbp), %rax
	addq	$3, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rcx
	movq	-152(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-160(%rbp), %rax
	addq	$4, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movl	-256(%rbp), %eax
	movl	-200(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-160(%rbp), %rax
	addq	$4, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rcx
	movq	-152(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-160(%rbp), %rax
	addq	$5, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movl	-256(%rbp), %eax
	movl	-196(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-160(%rbp), %rax
	addq	$5, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rcx
	movq	-152(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-160(%rbp), %rax
	addq	$6, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movl	-256(%rbp), %eax
	movl	-192(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-160(%rbp), %rax
	addq	$6, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rcx
	movq	-152(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-160(%rbp), %rax
	addq	$7, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movl	-256(%rbp), %eax
	movl	-188(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-160(%rbp), %rax
	addq	$7, %rax
	imulq	-72(%rbp), %rax
	movq	%rax, %rcx
	movq	-152(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -152(%rbp)
.L75:
	movq	-152(%rbp), %rax
	cmpq	-72(%rbp), %rax
	jb	.L76
	addq	$8, -160(%rbp)
.L74:
	movq	-160(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L77
	addq	$1, -168(%rbp)
.L73:
	movq	-168(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L78
	movq	$0, -144(%rbp)
	jmp	.L79
.L91:
	movq	-48(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	-48(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-8(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	$0, -136(%rbp)
	jmp	.L80
.L85:
	movq	$0, -128(%rbp)
	jmp	.L81
.L84:
	movq	-144(%rbp), %rax
	imulq	-96(%rbp), %rax
	movq	%rax, %rdx
	movq	-136(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-128(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-288(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -268(%rbp)
	movq	$0, -120(%rbp)
	jmp	.L82
.L83:
	movq	-120(%rbp), %rax
	movl	%eax, %edx
	movq	24(%rbp), %rax
	imull	%edx, %eax
	addl	$1, %eax
	movl	-268(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -264(%rbp)
	movq	-120(%rbp), %rax
	movl	%eax, %edx
	movq	24(%rbp), %rax
	imull	%edx, %eax
	addl	$2, %eax
	movl	-268(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -260(%rbp)
	movq	-128(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rdx
	movq	-120(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-136(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-264(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-128(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rcx
	movq	-120(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-16(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-128(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rdx
	movq	-120(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-136(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-260(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-128(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rcx
	movq	-120(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-8(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -120(%rbp)
.L82:
	movq	-120(%rbp), %rax
	cmpq	-40(%rbp), %rax
	jb	.L83
	addq	$1, -128(%rbp)
.L81:
	movq	-128(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L84
	addq	$1, -136(%rbp)
.L80:
	movq	-136(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L85
	movq	$0, -112(%rbp)
	jmp	.L86
.L90:
	movq	$0, -104(%rbp)
	jmp	.L87
.L89:
	movq	-104(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	testl	%eax, %eax
	je	.L88
	movq	-112(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movq	-104(%rbp), %rdx
	leaq	0(,%rdx,4), %rcx
	movq	-16(%rbp), %rdx
	addq	%rcx, %rdx
	movl	(%rdx), %ecx
	movl	$0, %edx
	divl	%ecx
	movl	%edx, %ecx
	movq	-104(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	cmpl	%eax, %ecx
	jne	.L88
	movq	-176(%rbp), %rax
	movl	%eax, %edx
	movq	-80(%rbp), %rax
	imull	%eax, %edx
	movq	-112(%rbp), %rax
	movl	%eax, %esi
	movq	-184(%rbp), %rax
	leaq	0(,%rax,8), %rcx
	movq	-296(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-144(%rbp), %rax
	movl	%eax, %edx
	movq	-48(%rbp), %rax
	imull	%eax, %edx
	movq	-104(%rbp), %rax
	movl	%eax, %esi
	movq	-184(%rbp), %rax
	salq	$3, %rax
	leaq	4(%rax), %rcx
	movq	-296(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -184(%rbp)
.L88:
	addq	$1, -104(%rbp)
.L87:
	movq	-104(%rbp), %rax
	cmpq	-48(%rbp), %rax
	jb	.L89
	addq	$1, -112(%rbp)
.L86:
	movq	-112(%rbp), %rax
	cmpq	-80(%rbp), %rax
	jb	.L90
	addq	$1, -144(%rbp)
.L79:
	movq	-144(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jb	.L91
	addq	$1, -176(%rbp)
.L72:
	movq	-176(%rbp), %rax
	cmpq	-88(%rbp), %rax
	jb	.L92
	movq	-304(%rbp), %rax
	movq	-184(%rbp), %rdx
	movq	%rdx, (%rax)
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	q3_unroll, .-q3_unroll
	.globl	q3_prune
	.type	q3_prune, @function
q3_prune:
.LFB12:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$288, %rsp
	movq	%rdi, -248(%rbp)
	movq	%rsi, -256(%rbp)
	movq	%rdx, -264(%rbp)
	movq	%rcx, -272(%rbp)
	movq	%r8, -280(%rbp)
	movq	%r9, -288(%rbp)
	movq	$0, -200(%rbp)
	movq	32(%rbp), %rax
	imulq	40(%rbp), %rax
	movq	%rax, -112(%rbp)
	movq	-280(%rbp), %rax
	imulq	-288(%rbp), %rax
	movl	$0, %edx
	divq	-112(%rbp)
	movq	%rax, -104(%rbp)
	movq	-112(%rbp), %rax
	movl	$0, %edx
	divq	-288(%rbp)
	movq	%rax, -96(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	-288(%rbp)
	movq	%rax, -88(%rbp)
	movq	40(%rbp), %rax
	imulq	-88(%rbp), %rax
	movq	%rax, -80(%rbp)
	movq	16(%rbp), %rax
	imulq	24(%rbp), %rax
	movl	$0, %edx
	divq	-112(%rbp)
	movq	%rax, -72(%rbp)
	movq	-112(%rbp), %rax
	movl	$0, %edx
	divq	24(%rbp)
	movq	%rax, -64(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	24(%rbp)
	movq	%rax, -56(%rbp)
	movq	40(%rbp), %rax
	imulq	-56(%rbp), %rax
	movq	%rax, -48(%rbp)
	movq	-96(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -40(%rbp)
	movq	-64(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -32(%rbp)
	movq	-64(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -24(%rbp)
	movq	-64(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -16(%rbp)
	movq	-64(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -8(%rbp)
	movq	$0, -192(%rbp)
	jmp	.L94
.L114:
	movq	-96(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-40(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	$0, -184(%rbp)
	jmp	.L95
.L100:
	movq	$0, -176(%rbp)
	jmp	.L96
.L99:
	movq	-192(%rbp), %rax
	imulq	-112(%rbp), %rax
	movq	%rax, %rdx
	movq	-184(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-176(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-248(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -208(%rbp)
	movq	$0, -168(%rbp)
	jmp	.L97
.L98:
	movq	-168(%rbp), %rax
	movl	%eax, %edx
	movq	-288(%rbp), %rax
	imull	%edx, %eax
	movl	-208(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -204(%rbp)
	movq	-176(%rbp), %rax
	imulq	-88(%rbp), %rax
	movq	%rax, %rdx
	movq	-168(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-184(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-204(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-176(%rbp), %rax
	imulq	-88(%rbp), %rax
	movq	%rax, %rcx
	movq	-168(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-40(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -168(%rbp)
.L97:
	movq	-168(%rbp), %rax
	cmpq	-88(%rbp), %rax
	jb	.L98
	addq	$1, -176(%rbp)
.L96:
	movq	-176(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L99
	addq	$1, -184(%rbp)
.L95:
	movq	-184(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L100
	movq	$0, -160(%rbp)
	jmp	.L101
.L113:
	movq	-64(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-32(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	-64(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	-64(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	-64(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-8(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	$0, -152(%rbp)
	jmp	.L102
.L107:
	movq	$0, -144(%rbp)
	jmp	.L103
.L106:
	movq	-160(%rbp), %rax
	imulq	-112(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-144(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-256(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -232(%rbp)
	movq	$0, -136(%rbp)
	jmp	.L104
.L105:
	movq	-136(%rbp), %rax
	movl	%eax, %edx
	movq	24(%rbp), %rax
	imull	%edx, %eax
	addl	$1, %eax
	movl	-232(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -228(%rbp)
	movq	-136(%rbp), %rax
	movl	%eax, %edx
	movq	24(%rbp), %rax
	imull	%edx, %eax
	addl	$2, %eax
	movl	-232(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -224(%rbp)
	movl	-228(%rbp), %eax
	xorl	-224(%rbp), %eax
	movl	%eax, -220(%rbp)
	movl	-220(%rbp), %eax
	andl	-228(%rbp), %eax
	movl	%eax, -216(%rbp)
	movl	-220(%rbp), %eax
	andl	-224(%rbp), %eax
	movl	%eax, -212(%rbp)
	movq	-144(%rbp), %rax
	imulq	-56(%rbp), %rax
	movq	%rax, %rdx
	movq	-136(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-152(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-228(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-144(%rbp), %rax
	imulq	-56(%rbp), %rax
	movq	%rax, %rcx
	movq	-136(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-32(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-144(%rbp), %rax
	imulq	-56(%rbp), %rax
	movq	%rax, %rdx
	movq	-136(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-152(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-224(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-144(%rbp), %rax
	imulq	-56(%rbp), %rax
	movq	%rax, %rcx
	movq	-136(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -136(%rbp)
.L104:
	movq	-136(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jb	.L105
	addq	$1, -144(%rbp)
.L103:
	movq	-144(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L106
	addq	$1, -152(%rbp)
.L102:
	movq	-152(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L107
	movq	$0, -128(%rbp)
	jmp	.L108
.L112:
	movq	$0, -120(%rbp)
	jmp	.L109
.L111:
	movq	-120(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	testl	%eax, %eax
	je	.L110
	movq	-128(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-40(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movq	-120(%rbp), %rdx
	leaq	0(,%rdx,4), %rcx
	movq	-32(%rbp), %rdx
	addq	%rcx, %rdx
	movl	(%rdx), %ecx
	movl	$0, %edx
	divl	%ecx
	movl	%edx, %ecx
	movq	-120(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	cmpl	%eax, %ecx
	jne	.L110
	movq	-192(%rbp), %rax
	movl	%eax, %edx
	movq	-96(%rbp), %rax
	imull	%eax, %edx
	movq	-128(%rbp), %rax
	movl	%eax, %esi
	movq	-200(%rbp), %rax
	leaq	0(,%rax,8), %rcx
	movq	-264(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-160(%rbp), %rax
	movl	%eax, %edx
	movq	-64(%rbp), %rax
	imull	%eax, %edx
	movq	-120(%rbp), %rax
	movl	%eax, %esi
	movq	-200(%rbp), %rax
	salq	$3, %rax
	leaq	4(%rax), %rcx
	movq	-264(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -200(%rbp)
.L110:
	addq	$1, -120(%rbp)
.L109:
	movq	-120(%rbp), %rax
	cmpq	-64(%rbp), %rax
	jb	.L111
	addq	$1, -128(%rbp)
.L108:
	movq	-128(%rbp), %rax
	cmpq	-96(%rbp), %rax
	jb	.L112
	addq	$1, -160(%rbp)
.L101:
	movq	-160(%rbp), %rax
	cmpq	-72(%rbp), %rax
	jb	.L113
	addq	$1, -192(%rbp)
.L94:
	movq	-192(%rbp), %rax
	cmpq	-104(%rbp), %rax
	jb	.L114
	movq	-272(%rbp), %rax
	movq	-200(%rbp), %rdx
	movq	%rdx, (%rax)
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	q3_prune, .-q3_prune
	.globl	q3_weave_index_l2_block
	.type	q3_weave_index_l2_block, @function
q3_weave_index_l2_block:
.LFB13:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$288, %rsp
	movq	%rdi, -248(%rbp)
	movq	%rsi, -256(%rbp)
	movq	%rdx, -264(%rbp)
	movq	%rcx, -272(%rbp)
	movq	%r8, -280(%rbp)
	movq	%r9, -288(%rbp)
	movq	$0, -216(%rbp)
	movq	32(%rbp), %rax
	imulq	40(%rbp), %rax
	movq	%rax, -112(%rbp)
	movq	-280(%rbp), %rax
	imulq	-288(%rbp), %rax
	movl	$0, %edx
	divq	-112(%rbp)
	movq	%rax, -104(%rbp)
	movq	-104(%rbp), %rax
	movl	$0, %edx
	divq	48(%rbp)
	movq	%rax, -96(%rbp)
	movq	-112(%rbp), %rax
	movl	$0, %edx
	divq	-288(%rbp)
	movq	%rax, -88(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	-288(%rbp)
	movq	%rax, -80(%rbp)
	movq	40(%rbp), %rax
	imulq	-80(%rbp), %rax
	movq	%rax, -72(%rbp)
	movq	16(%rbp), %rax
	imulq	24(%rbp), %rax
	movl	$0, %edx
	divq	-112(%rbp)
	movq	%rax, -64(%rbp)
	movq	-64(%rbp), %rax
	movl	$0, %edx
	divq	48(%rbp)
	movq	%rax, -56(%rbp)
	movq	-112(%rbp), %rax
	movl	$0, %edx
	divq	24(%rbp)
	movq	%rax, -48(%rbp)
	movl	$32, %eax
	movl	$0, %edx
	divq	24(%rbp)
	movq	%rax, -40(%rbp)
	movq	40(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, -32(%rbp)
	movq	-88(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -24(%rbp)
	movq	-48(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -16(%rbp)
	movq	-48(%rbp), %rax
	salq	$2, %rax
	movq	%rax, %rsi
	movl	$32, %edi
	call	aligned_alloc@PLT
	movq	%rax, -8(%rbp)
	movq	$0, -208(%rbp)
	jmp	.L116
.L140:
	movq	$0, -200(%rbp)
	jmp	.L117
.L139:
	movq	-88(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	$0, -192(%rbp)
	jmp	.L118
.L123:
	movq	$0, -184(%rbp)
	jmp	.L119
.L122:
	movq	-208(%rbp), %rax
	imulq	48(%rbp), %rax
	movq	%rax, %rdx
	movq	-200(%rbp), %rax
	addq	%rdx, %rax
	imulq	-112(%rbp), %rax
	movq	%rax, %rdx
	movq	-192(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-184(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-248(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -224(%rbp)
	movq	$0, -176(%rbp)
	jmp	.L120
.L121:
	movq	-176(%rbp), %rax
	movl	%eax, %edx
	movq	-288(%rbp), %rax
	imull	%edx, %eax
	movl	-224(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -220(%rbp)
	movq	-184(%rbp), %rax
	imulq	-80(%rbp), %rax
	movq	%rax, %rdx
	movq	-176(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-192(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-220(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-184(%rbp), %rax
	imulq	-80(%rbp), %rax
	movq	%rax, %rcx
	movq	-176(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-24(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -176(%rbp)
.L120:
	movq	-176(%rbp), %rax
	cmpq	-80(%rbp), %rax
	jb	.L121
	addq	$1, -184(%rbp)
.L119:
	movq	-184(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L122
	addq	$1, -192(%rbp)
.L118:
	movq	-192(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L123
	movq	$0, -168(%rbp)
	jmp	.L124
.L138:
	movq	$0, -160(%rbp)
	jmp	.L125
.L137:
	movq	-48(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	-48(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-8(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	memset@PLT
	movq	$0, -152(%rbp)
	jmp	.L126
.L131:
	movq	$0, -144(%rbp)
	jmp	.L127
.L130:
	movq	-168(%rbp), %rax
	imulq	48(%rbp), %rax
	movq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	imulq	-112(%rbp), %rax
	movq	%rax, %rdx
	movq	-152(%rbp), %rax
	imulq	40(%rbp), %rax
	addq	%rax, %rdx
	movq	-144(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-256(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movl	%eax, -236(%rbp)
	movq	$0, -136(%rbp)
	jmp	.L128
.L129:
	movq	-136(%rbp), %rax
	movl	%eax, %edx
	movq	24(%rbp), %rax
	imull	%edx, %eax
	addl	$1, %eax
	movl	-236(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -232(%rbp)
	movq	-136(%rbp), %rax
	movl	%eax, %edx
	movq	24(%rbp), %rax
	imull	%edx, %eax
	addl	$2, %eax
	movl	-236(%rbp), %edx
	movl	%eax, %ecx
	shrl	%cl, %edx
	movl	%edx, %eax
	andl	$1, %eax
	movl	%eax, -228(%rbp)
	movq	-144(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rdx
	movq	-136(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-152(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-232(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-144(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rcx
	movq	-136(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-16(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-144(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rdx
	movq	-136(%rbp), %rax
	addq	%rdx, %rax
	leaq	0(,%rax,4), %rdx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %edx
	movq	32(%rbp), %rax
	movl	%eax, %ecx
	movq	-152(%rbp), %rax
	subl	%eax, %ecx
	movl	%ecx, %eax
	subl	$1, %eax
	movl	-228(%rbp), %esi
	movl	%eax, %ecx
	sall	%cl, %esi
	movq	-144(%rbp), %rax
	imulq	-40(%rbp), %rax
	movq	%rax, %rcx
	movq	-136(%rbp), %rax
	addq	%rcx, %rax
	leaq	0(,%rax,4), %rcx
	movq	-8(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -136(%rbp)
.L128:
	movq	-136(%rbp), %rax
	cmpq	-40(%rbp), %rax
	jb	.L129
	addq	$1, -144(%rbp)
.L127:
	movq	-144(%rbp), %rax
	cmpq	40(%rbp), %rax
	jb	.L130
	addq	$1, -152(%rbp)
.L126:
	movq	-152(%rbp), %rax
	cmpq	32(%rbp), %rax
	jb	.L131
	movq	$0, -128(%rbp)
	jmp	.L132
.L136:
	movq	$0, -120(%rbp)
	jmp	.L133
.L135:
	movq	-120(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-16(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	testl	%eax, %eax
	je	.L134
	movq	-128(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	movq	-120(%rbp), %rdx
	leaq	0(,%rdx,4), %rcx
	movq	-16(%rbp), %rdx
	addq	%rcx, %rdx
	movl	(%rdx), %ecx
	movl	$0, %edx
	divl	%ecx
	movl	%edx, %ecx
	movq	-120(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	movq	-8(%rbp), %rax
	addq	%rdx, %rax
	movl	(%rax), %eax
	cmpl	%eax, %ecx
	jne	.L134
	movq	-208(%rbp), %rax
	imulq	48(%rbp), %rax
	movq	%rax, %rdx
	movq	-200(%rbp), %rax
	addq	%rdx, %rax
	movl	%eax, %edx
	movq	-88(%rbp), %rax
	imull	%eax, %edx
	movq	-128(%rbp), %rax
	movl	%eax, %esi
	movq	-216(%rbp), %rax
	leaq	0(,%rax,8), %rcx
	movq	-264(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	movq	-168(%rbp), %rax
	imulq	48(%rbp), %rax
	movq	%rax, %rdx
	movq	-160(%rbp), %rax
	addq	%rdx, %rax
	movl	%eax, %edx
	movq	-48(%rbp), %rax
	imull	%eax, %edx
	movq	-120(%rbp), %rax
	movl	%eax, %esi
	movq	-216(%rbp), %rax
	salq	$3, %rax
	leaq	4(%rax), %rcx
	movq	-264(%rbp), %rax
	addq	%rcx, %rax
	addl	%esi, %edx
	movl	%edx, (%rax)
	addq	$1, -216(%rbp)
.L134:
	addq	$1, -120(%rbp)
.L133:
	movq	-120(%rbp), %rax
	cmpq	-48(%rbp), %rax
	jb	.L135
	addq	$1, -128(%rbp)
.L132:
	movq	-128(%rbp), %rax
	cmpq	-88(%rbp), %rax
	jb	.L136
	addq	$1, -160(%rbp)
.L125:
	movq	-160(%rbp), %rax
	cmpq	48(%rbp), %rax
	jb	.L137
	addq	$1, -168(%rbp)
.L124:
	movq	-168(%rbp), %rax
	cmpq	-56(%rbp), %rax
	jb	.L138
	addq	$1, -200(%rbp)
.L117:
	movq	-200(%rbp), %rax
	cmpq	48(%rbp), %rax
	jb	.L139
	addq	$1, -208(%rbp)
.L116:
	movq	-208(%rbp), %rax
	cmpq	-96(%rbp), %rax
	jb	.L140
	movq	-272(%rbp), %rax
	movq	-216(%rbp), %rdx
	movq	%rdx, (%rax)
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	q3_weave_index_l2_block, .-q3_weave_index_l2_block
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
