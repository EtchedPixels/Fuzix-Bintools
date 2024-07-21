
fred:
	nop
	mvii 4,r1
	movr r2,r3
	mvi #1,r1
	mvi fred,r2
	mvi @r4,r3
	mvo r1,foo
	mvo r1,r2
	mvo r1,@r6
	sllc r1
	rlc r1,2
	rrc r1,1
	nop
	sin
	sin 2
	sin 1
	clrc
	b fred
	bc fred
	bext 4,fred
	jsr r4,fred
	j fred
	gswd r3
	sbdb mvii 4,r1
	sbdb mvi #1,r1
	pshr r4
	pulr r4
	clrr r4
	nopp fred
	tstr r4
