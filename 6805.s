	adc #1
	adc @foo
	adc foobar
	adc 500,x
	adc 50,x
	adc ,x

	add #1
	and #1

	asl @foo
	asla
	aslx
	asl 50,x
	asl ,x

	asra
	asrx
	asr 1,x

fred:
	bcc fred
	bclr 1,@foo
	bcs fred
	beq fred
	bhcc fred
	bhcs fred
	bhi fred
	bhs fred
	bih fred
	bil fred
	bit #2
	blo fred
	bls fred
	bmc fred
	bmi fred
	bms fred
	bne fred
	bpl fred
	bra fred
	brclr 2,@foo,fred
	brn fred
	brset 1,@foo,fred
	bset 1,@foo
	bsr fred
	clc
	cli
	clr @foo
	clra
	clrx
	cmp #1
	com @foo
	coma
	comx
	cpx #1
	dec @foo
	deca
	decx
	eor @foo
	inc @foo
	inca
	incx
	jmp @foo
	jmp fred
	jmp ,x
	jmp 2,x
	jmp fred,x
	jsr fred
	lda #4
	ldx 2,x
	lsl ,x
	lsla
	lslx
	lsr 2,x
	lsra
	lsrx
	mul
	neg @foo
	nega
	negx
	nop
	ora ,x
	rol @foo
	rola
	rolx
	ror @foo
	rora
	rorx
	rsp
	rti
	rts
	sbc 500,x
	sec
	sei
	sta 500,x
	stop
	stx 0x1234,x
	sub #12
	swi
	tax
	tst @foo
	tsta
	tstx
	txa
	wait
