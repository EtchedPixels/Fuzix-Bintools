	.86

	aaa
	aad
	aam
	aas
;	adc
;	add
;	and
;	call
	cbw
	clc
	cld
	cli
	cmc
;	cmp
;	cmps
	cmpsb
	cmpsw
	cwd
	daa
	das
	dec bx
	dec al
	dec [fred]
	div bx
	div [fred]
	hlt
	idiv bx
	idiv [fred]
	imul cx
	imul [fred]
	in ax,3
	in ax,dx
	in al,5
	in al,dx
	inc ax
	dec al
	inc [fred]
	int 4
	into
	iret
fred:
	ja fred
	jae fred
	jb fred
	jbe fred
	jcxz fred
	je fred
	jg fred
	jge fred
	jl fred
	jle fred
	jna fred
	jnae fred
	jnb fred
	jne fred
	jng fred
	jnge fred
	jnl fred
	jno fred
	jnp fred
	jns fred
	jnz fred
	jo fred
	jp fred
	jpe fred
	jpo fred
	js fred
	jz fred
;	jmp fred
	lahf
	lds [fred]
	lea ax,[fred]
	les [fred]
	lock nop
;	lods
	lodsb
	lodsw
foo:
	loop foo
	loopz foo
	loopnz foo
	loope foo
	loopne foo
;	mov
;	movs
	movsb
	movsw
	mul ax
	mul [fred]
	neg al
	neg ax
	neg [fred]
	nop
	not bl
	not cx
	not [fred]
;	or
	out ax,12
	out al,12
	out al,dx
	out ax,dx
	pop ax
	pop dx
	pop [fred]
	push [fred]
	push ax
	push dx
	popf
	pushf
;	rcl
;	rcr
	rep nop
	repe nop
	repz nop
	repne nop
	repnz lock nop
	ret
	retf
;	rol
;	ror
	sahf
;	sal
;	sar
;	sbb
;	scas
	scasb
	scasw
;	shl
;	shr
	stc
	std
	sti
;	stos
	stosb
	stosw
;	sub
;	test
	wait
	xchg ax,bx
	xchg al,bl
	xchg ax,[_fred]
	xchg [_fred],ax
	xlat
;	xor

	.186

;	bound
;	enter 0,0
;	ins
	insb
	insw
	leave
;	outs
	outsb
	outsw
	popa
	pusha
