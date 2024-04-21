	.86

	aaa
	aad
	aam
	aas
	adc ax,1
	adc bx,4
	adc ax,bx
	adc ax,[fred]
	adc [fred],ax
	adc [fred],1
	adc al,1
	adc bl,4
	adc al,bl
	adc al,[fred]
	adc [fred],al
	add ax,1
	add bx,4
	add ax,bx
	add ax,[fred]
	add [fred],ax
	add [fred],1
	add al,1
	add bl,4
	add al,bl
	add al,[fred]
	add [fred],al
	and ax,1
	and bx,4
	and ax,bx
	and ax,[fred]
	and [fred],ax
	and [fred],1
	and al,1
	and bl,4
	and al,bl
	and al,[fred]
	and [fred],al
;	call
;	callf
	cbw
	clc
	cld
	cli
	cmc
	cmp ax,1
	cmp bx,4
	cmp ax,bx
	cmp ax,[fred]
	cmp [fred],ax
	cmp [fred],1
	cmp al,1
	cmp bl,4
	cmp al,bl
	cmp al,[fred]
	cmp [fred],al
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
;	jmpf fred
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
	rcl bx
	rcr bl
	rep nop
	repe nop
	repz nop
	repne nop
	repnz lock nop
	ret
	retf
	rol ax
	ror dl
	sahf
	sal cl
	sar bx
	sbb ax,1
	sbb bx,4
	sbb ax,bx
	sbb ax,[fred]
	sbb [fred],ax
	sbb [fred],1
	sbb al,1
	sbb bl,4
	sbb al,bl
	sbb al,[fred]
	sbb [fred],al
;	scas
	scasb
	scasw
	shl cx
	shr cl
	stc
	std
	sti
;	stos
	stosb
	stosw
	sub ax,1
	sub bx,4
	sub ax,bx
	sub ax,[fred]
	sub [fred],ax
	sub [fred],1
	sub al,1
	sub bl,4
	sub al,bl
	sub al,[fred]
	sub [fred],al
;	test
	wait
	xchg ax,bx
	xchg al,bl
	xchg ax,[_fred]
	xchg [_fred],ax
	xlat
	xor ax,1
	xor bx,4
	xor ax,bx
	xor ax,[fred]
	xor [fred],ax
	xor [fred],1
	xor al,1
	xor bl,4
	xor al,bl
	xor al,[fred]
	xor [fred],al

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
