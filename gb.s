	nop
	ld bc,0x1234
	ld (bc),a
	inc bc
	inc b
	dec b
	ld b,12
	rlca
	ld (0x1234),sp
	add hl,bc
	ld a,(bc)
	dec bc
	inc c
	dec c
	ld c,1
	rrca
	stop 0
	ld de,0x1234
	ld (de),a
	inc de
	inc d
	dec d
	ld d,1
	rla
fred:
	jr fred
	add hl,de
	ld a,(de)
	dec de
	inc e
	dec e
	ld e,1
	rra
	jr nz, fred
	ld hl,0x1234
	ldi (hl),a
	inc hl
	inc h
	dec h
	ld h,1
	daa
	jr z,fred
	add hl,hl
	ldi a,(hl)
	dec hl
	inc l
	dec l
	ld l,1
	cpl
	jr nc,fred
	ld sp,0x1234
	ldd (hl),a
	inc sp
	inc (hl)
	dec (hl)
	ld (hl),1
	scf
	jr c,fred
	add hl,sp
	ldd a,(hl)
	dec sp
	inc a
	dec a
	ld a,1
	ccf
	ld b,b
	ld d,c
	ld h,d
	ld (hl),c
	ld c,(hl)
	add a,b
	sub c
	xor d
	cp (hl)
	ret nz
	pop bc
	jp nz,fred
	jp fred
	call nz,fred
	push bc
	add a,4
	rst 0
	ret z
	ret
	jp z,fred
	call z,fred
	call fred
	adc a,1
	rst 8
	ret nc
	pop de
	jp nc, fred
	call nc, fred
	push de
	sub 1
	rst 16
	ret c
	reti
	jp c, fred
	call c,fred
	sbc a,1
	rst 24
	ldh (1),a
	pop hl
	ldh (c),a
	push hl
	and 1
	rst 32
	add sp,4
	jp (hl)
	ld (1234),a
	xor 1
	rst 40
	ldh a,(4)
	pop af
	ldh a,(c)
	di
	push af
	or 1
	rst 48
	ld hl,sp+4
	ld sp,hl
	ld a,(0x1234)
	ei
	cp 1
	rst 56
