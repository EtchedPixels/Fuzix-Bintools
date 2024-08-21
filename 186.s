	.86

	aaa
	aad
	aam
	aas
	adc ax,1
	adc bx,4
	adc ax,bx
	adc ax,[bx]
	adc ax,[si]
	adc ax,[di]
	adc ax,[bx + di]
	adc ax,[bx + si]
	adc ax,[bp + di]
	adc ax,[bp + si]
	adc ax,[bp + 0]
	adc ax,[bx + 1]
	adc ax,[di + 2]
	adc ax,[si + 3]
	adc ax,[bx + di + 4]
	adc ax,[bx + si + 5]
	adc ax,[bp + di + 6]
	adc ax,[bp + si + 7]
	adc ax,[bp + 512]
	adc ax,[bx + 513]
	adc ax,[di + 514]
	adc ax,[si + 515]
	adc ax,[bx + di + 516]
	adc ax,[bx + si + 517]
	adc ax,[bp + di + 518]
	adc ax,[bp + si + 519]
	adc ax,[fred]
	adc ax,cs:[fred]
	adc ax,ds:[fred]
	adc ax,es:[fred]
	adc ax,ss:[fred]
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
	call fred
	call ds:fred
	call [bx]
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
	jmp fred
	jmp eric
;	jmp [bx]	need to fix the jmp handling to not use TJCC
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
	mov es,ax
	mov ax,ds
	mov ax,11
	mov al,12
	mov ah,10
	mov bx,12
	mov ax,[0x1234]
	mov al,[0x1234]
	mov [0x1234],ax
	mov [0x1234],al
	mov bx,[0x1234]
	mov [0x1234],bx
	mov [bp + 4], 12
	mov ax,[bx]
	mov [bx],ax
	mov [bx],12
	mov [1234],12
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
	or ax,bx
	or ax,0x1234
	or al,5
	or ax,6
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
	ret 4
	retf 12
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
	test ax,[bx]
	test ax,0x1234
	test ax,1
	test al,0
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

	bound cx,[bx]
	enter 0,1
;	ins
	insb
	insw
	leave
;	outs
	outsb
	outsw
	popa
	pusha
