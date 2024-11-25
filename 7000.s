	adc b,a
	adc r4,a
	adc r120,b
	adc r4,r4
	adc %5,a
	adc %5,b
	adc %10,r4

	add b,a

	and b,a

	andp a,p1
	andp b,p1
	andp %12,p30

loop:
	btjo b,a,loop
	btjo r12,a,loop
	btjo r12,b,loop
	btjo r12,r4,loop
	btjo %12,a,loop
	btjo %12,b,loop
	btjo %12,r12,loop

	btjop b,p1,loop
	btjop b,p2,loop
	btjop %1,p3,loop

	btjz b,a,loop
	btjz r4,a,loop
	btjz r5,b,loop
	btjz %1,a,loop
	btjz %2,b,loop
	btjz %3,r4,loop

	btjzp a,p12,loop
	btjzp b,p12,loop
	btjzp %12,p4,loop

	br @loop
	br @512
	br @loop(b)
	br *r4

	call @loop
	call @loop(b)
	call *r4

	clr a
	clr b
	clr r12

	clrc

	cmp b,a

	cmpa @loop
	cmpa @loop(b)
	cmpa *r4

	dac b,a

	dec a

	decd a

	dint

	djnz a,loop
	djnz b,loop
	djnz r4,loop

	dsb b,a

	eint

	idle

	inc a

	inv a

	jmp loop

	jp loop

	jc loop
	jeq loop
	jhs loop
	jl loop
	jn loop
	jnc loop
	jne loop
	jnz loop
	jpz loop
	jz loop

	lda @loop
	lda @loop(b)
	lda *r12

	ldsp

	mov a,b
	mov a,r4
	mov b,a
	mov b,r4
	mov r4,a
	mov r4,b
	mov r4,r5
	mov %12,a
	mov %12,b
	mov %12,r4

	movd %12,r4
	movd %12(b),r4
	movd r3,r4

	movp a,p4
	movp b,p5
	movp %4,p4
	movp p4,a
	movp p5,b

	mpy b,a

	nop

	or b,a

	orp a,p4

	pop a
	pop b
	pop r4
	pop st

	push a
	push b
	push r4
	push st

	reti

	rets

	rl a
	rl b
	rl r4

	rlc a

	rr a

	rrc a

	sbb b,a

	setc

	sta @loop
	sta @loop(b)
	sta *r4

	stsp

	sub b,a

	swap a
	swap b
	swap r4

	trap 0
	trap 23

	tsta

	tstb

	xchb a
	xchb r4

	xor b,a

	xorp a,p4

