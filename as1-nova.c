/*
 * DG Nova assembler.
 * Assemble one line of input.
 * Knows all the dirt.
 *
 * TODO: Add support to the core as code to error attempts to shift a pointer
 * and lose bits (eg if you attempt to make a word pointer of a byte aligned
 * object).
 */
#include	"as.h"

static unsigned is_bp[OSEG + 1];

int passbegin(int pass)
{
	unsigned i;
	segment = 1;		/* Default to code */
	for (i = 0; i < OSEG + 1; i++)
		is_bp[i] = 1;		/* Linker initially assumes byteptrs */
	outscale(0,16);		/* Reset internal tracking */
	if (pass == 1 || pass == 2)
		return 0;
	return 1;		/* All passes required */
}

void byteptr(unsigned n)
{
	if (n == is_bp[segment])
		return;
	is_bp[segment] = n;
	if (n == 0)
		outscale(1, 16);
	else
		outscale(0, 16);
}

/*
 * Read in an address
 * descriptor, and fill in
 * the supplied "ADDR" structure
 * with the mode and value.
 * Exits directly to "qerr" if
 * there is no address field or
 * if the syntax is bad.
 * TODO: FIXME - who owns decoding indirect and passing it around
 */
void getaddr(ADDR *ap)
{
	int c;

	ap->a_sym = NULL;
	ap->a_flags = 0;
	ap->a_type = 0;
	ap->a_value = 0;
	ap->a_segment = ABSOLUTE;

	/* We only have one addressing format we ever use.. an address.
	   Quite how we encode it is another saga because our memory ops
	   use register relative or pc relative */
	c = getnb();
	if (/*c != '#' && */c != '@')	/* Until we support literal adding */
		unget(c);
	expr1(ap, LOPRI, 1);
	switch (ap->a_type&TMMODE) {
	case TUSER:
		break;
	default:
		qerr(SYNTAX_ERROR);
	}
	if (c == '@')	/* Indirect */
		ap->a_value |= 0x8000;
}

static int accumulator(void)
{
	int c = getnb();
	if (c < '0' || c > '3') {
		aerr(BAD_ACCUMULATOR);
		unget(c);
		return 0;
	}
	return c - '0';
}

static int carryop(char c)
{
	c = toupper(c);
	if (c == 'Z')
		return 1;
	if (c == 'O')
		return 2;
	if (c == 'C')
		return 3;
	return 0;
}

static int postop(char c)
{
	c = toupper(c);
	if (c == 'L')
		return 1;
	if (c == 'R')
		return 2;
	if (c == 'S')
		return 3;
	return 0;
}

/*
 *	Byte handling. We queue bytes into pairs then add them. If we
 *	write a word then outaw flushes any dangling byte into the left
 *	half of the word.
 */

void flushbyte(void)
{
	if (dot[segment] & 1)
		outab(0);
}

/*
 *	Flush any final byte in case we end with an odd sized
 *	segment.
 */

void doflush(void)
{
	for (segment = 1; segment < OSEG; segment++)
		flushbyte();
}


/*
 *	Word sized binary output
 *	We write machine words in network order
 */

void outaw(uint16_t v)
{
	/* Flush pending byte and align */
	flushbyte();
	outab(v >> 8);
	outab(v);
}

/*
 * Assemble one line.
 * The line in in "ib", the "ip"
 * scans along it. The code is written
 * right out.
 */
void asmline(void)
{
	SYM *sp;
	int c;
	int acs, acd;
	int opcode;
	VALUE value;
	int delim;
	SYM *sp1;
	char id[NCPS];
	char id1[NCPS];
	char iid[NCPS];
	ADDR a1;
	char *p;

loop:
	if ((c=getnb())=='\n' || c==';')
		return;
	if (is_symstart(c) == 0 && c != '.')
		qerr(UNEXPECTED_CHR);
	getid(id, c);
	if ((c=getnb()) == ':') {
		sp = lookup(id, uhash, 1);
		/* We don't allow pointers to non word for now TODO */
		flushbyte();
		if (pass == 0) {
			if ((sp->s_type&TMMODE) != TNEW
			&&  (sp->s_type&TMASG) == 0)
				sp->s_type |= TMMDF;
			sp->s_type &= ~TMMODE;
			sp->s_type |= TUSER;
			/* Word machine so half the byte count */
			sp->s_value = dot[segment];
			sp->s_segment = segment;
		} else {
			if ((sp->s_type&TMMDF) != 0)
				err('m', MULTIPLE_DEFS);
			if (sp->s_value != dot[segment])
				err('p', PHASE_ERROR);
		}
		goto loop;
	}
	/* We have to do ugly things here because the Nova instruction set
	   merges the opcode and flags */
	memcpy(iid, id, 4);
	memset(iid + 3, 0, NCPS - 3);

	/*
	 * If the first token is an
	 * id and not an operation code,
	 * assume that it is the name in front
	 * of an "equ" assembler directive.
	 */

	/* Check for .equ forms */
	if ((sp = lookup(id, phash, 0)) == NULL &&
		(sp = lookup(iid, phash, 0)) == NULL) {
		getid(id1, c);
		if ((sp1=lookup(id1, phash, 0)) == NULL
		||  (sp1->s_type&TMMODE) != TEQU) {
			err('o', SYNTAX_ERROR);
			return;
		}
		getaddr(&a1);
		istuser(&a1);
		sp = lookup(id, uhash, 1);
		if ((sp->s_type&TMMODE) != TNEW
		&&  (sp->s_type&TMASG) == 0)
			err('m', MULTIPLE_DEFS);
		sp->s_type &= ~(TMMODE|TPUBLIC);
		sp->s_type |= TUSER|TMASG;
		sp->s_value = a1.a_value;
		sp->s_segment = a1.a_segment;
		/* FIXME: review .equ to an external symbol/offset and
		   what should happen */
		goto loop;
	}

	unget(c);
	opcode = sp->s_value;

	switch (sp->s_type&TMMODE) {
	case TORG:
		getaddr(&a1);
		istuser(&a1);
		if (a1.a_segment != ABSOLUTE)
			qerr(MUST_BE_ABSOLUTE);
		flushbyte();	/* Deposit any dangling byte first */
		outsegment(ABSOLUTE);
		dot[segment] = a1.a_value * 2;	/* dot is in bytes */
		/* Tell the binary generator we've got a new absolute
		   segment. */
		outabsolute(dot[segment]);
		break;

	case TEXPORT:
		getid(id, getnb());
		sp = lookup(id, uhash, 1);
		sp->s_type |= TPUBLIC;
		break;
		/* .code etc */
	case TSEGMENT:
		/* Tell the binary generator about a segment switch to a non
		   absolute segnent */
		flushbyte();
		segment = sp->s_value;
		outsegment(segment);
		break;

	case TDEFB:
		do {
			getaddr(&a1);
			istuser(&a1);
			/* FIXME: how to integrate word machines into core nicely */
			if (a1.a_segment == ABSOLUTE)
				a1.a_value *= 2;
			outrab(&a1);
		} while ((c=getnb()) == ',');
		unget(c);
		break;

	case TDEFW:
		flushbyte();
		byteptr(0);
		do {
			getaddr(&a1);
			istuser(&a1);
			/* FIXME: how to integrate word machines into core nicely */
			if (a1.a_segment == ABSOLUTE)
				a1.a_value *= 2;
			outraw(&a1);
		} while ((c=getnb()) == ',');
		unget(c);
		break;

	case TBPTR:
		flushbyte();
		byteptr(1);
		do {
			getaddr(&a1);
			istuser(&a1);
			outraw(&a1);
		} while ((c=getnb()) == ',');
		unget(c);
		break;

	case TDEFM:
		if ((delim=getnb()) == '\n')
			qerr(MISSING_DELIMITER);
		while ((c=get()) != delim) {
			if (c == '\n')
				qerr(MISSING_DELIMITER);
			outab(c);
		}
		break;

	case TDEFS:
		flushbyte();
		getaddr(&a1);
		istuser(&a1);
		/* Write out the words. The BSS will deal with the rest */
		for (value = 0 ; value < a1.a_value; value++)
			outaw(0);
		break;

	case TCPUOPT:
		cpu_flags |= sp->s_value;
		break;

	case TMEMORY:
	{
		int indirect = 0;
		/*
		 *	Memory operations are either
		 *	0,disp		Word 0-255 (zero page)
		 *	1,signed disp	PC relative
		 *	2.disp		ac2 + offset
		 *	3,disp		ac3 + offset
		 *
		 *	We don't enforce any rules on ac2/ac3 but encode
		 *	them on the basis the user knows what they are doing
		 *
		 *	0,disp is encoded is an 8 bit relocation for ZP
		 *	1,disp FIXME needs to be encoded as an 8bit PCREL
		 *
		 *	There is *no* immediate load nor is there anyway
		 *	to load arbitrary addresses.
		 *
		 *	FIXME: we need some Nova specific meta ops as a
		 *	result
		 *
		 *	.nomodify	- don't sneak in data words
		 *	.modify		- allowed to sneak in data words
		 *	.local		- local data word
		 *	.flushlocal	- write locals out here
		 *
		 *	local data words are placed in a queue with their
		 *	relocation address. During pass 0 we try to place them
		 *	by adding ,skp to TALU instructions and putting one
		 *	after it. If we reach the point it won't fit we add a
		 *	JMP around the data and load with data. Likewise we
		 *	can fill after a jump.
		 *
		 *	This has its own fun... a jump is itself pcrel or
		 *	constrained. Fortunately however we can encode an
		 *	arbitrary jump as JMP #.+1 and a word. We can't do
		 *	this ourself with JSR but compilers can jmp 3,1
		 *	and do it.
		 *
		 *	For A2/A3 the same way to write stuff is likely to be
		 *
		 *	; a2 is loaded with foo
		 *
		 *	LDA 1,bar-foo,2
		 *
		 *	and the assembler with turn bar-foo into an ABSOLUTE
		 */
		acd = accumulator();
		comma();
		c = get();
		if (c == '@')
			indirect = 1;
		else {
			unget(c);
			indirect = 0;
		}
		getaddr(&a1);
		c = get();
		if (c == ',')
			acs = accumulator();
		else {
			acs = 0;
			unget(c);
		}
		/* ,0 means zero page */
		if (acs == 0) {
			if (a1.a_segment == UNKNOWN)
				a1.a_segment = ZP;
			if (a1.a_segment != ZP && a1.a_segment != ABSOLUTE)
				aerr(NEED_ZPABS);
		}
		/* ,2 + are indexes so we can't really police them for sanity */
		/* ,1 is PC relative so must be in this segment */
		if (acs == 1) {
			if (a1.a_segment == UNKNOWN)
				a1.a_segment = segment;
			else if (a1.a_segment != ABSOLUTE && a1.a_segment != segment)
				aerr(BAD_PCREL);
			if (a1.a_segment != ABSOLUTE) {
				a1.a_type |= TPCREL;
				a1.a_value -= dot[segment];
			}
		}
		/* Need to push this into the core somehow */
		if (a1.a_segment == ABSOLUTE)
			/* Turn into byte form temporarily */
			a1.a_value <<= 1;
		/* Insert the accumulators */
		opcode |= (acd << 11);
		opcode |= (acs << 8);
		if (indirect)
			opcode |= 0x0400;

		flushbyte();

		byteptr(0);

		outab(opcode >> 8);
		if (acs)
			outrabrel(&a1);	/* Signed */
		else
			outrab(&a1);	/* Unsigned */
		break;
	}

	/* Akin to TMEMORY but with no accumulator */
	case TMEMNA:
	{
		int indirect = 0;
		/*
		 *	Memory operations are either
		 *	0,disp		Word 0-255 (zero page)
		 *	1,signed disp	PC relative
		 *	2.disp		ac2 + offset
		 *	3,disp		ac3 + offset
		 *
		 *	We don't enforce any rules on ac2/ac3 but encode
		 *	them on the basis the user knows what they are doing
		 *
		 *	0,disp is encoded is an 8 bit relocation for ZP
		 *	1,disp FIXME needs to be encoded as an 8bit PCREL
		 *
		 *	There is *no* immediate load nor is there anyway
		 *	to load arbitrary addresses.
		 *
		 *	FIXME: we need some Nova specific meta ops as a
		 *	result
		 *
		 *	.nomodify	- don't sneak in data words
		 *	.modify		- allowed to sneak in data words
		 *	.local		- local data word
		 *	.flushlocal	- write locals out here
		 *
		 *	local data words are placed in a queue with their
		 *	relocation address. During pass 0 we try to place them
		 *	by adding ,skp to TALU instructions and putting one
		 *	after it. If we reach the point it won't fit we add a
		 *	JMP around the data and load with data. Likewise we
		 *	can fill after a jump.
		 *
		 *	This has its own fun... a jump is itself pcrel or
		 *	constrained. Fortunately however we can encode an
		 *	arbitrary jump as JMP #.+1 and a word. We can't do
		 *	this ourself with JSR but compilers can jmp 3,1
		 *	and do it.
		 *
		 *	For A2/A3 the same way to write stuff is likely to be
		 *
		 *	; a2 is loaded with foo
		 *
		 *	LDA 1,bar-foo,2
		 *
		 *	and the assembler with turn bar-foo into an ABSOLUTE
		 */
		c = get();
		if (c == '@')
			indirect = 1;
		else {
			unget(c);
			indirect = 0;
		}
		getaddr(&a1);
		c = get();
		if (c == ',')
			acs = accumulator();
		else {
			acs = 0;
			unget(c);
		}
		/* ,0 means zero page */
		if (acs == 0) {
			if (a1.a_segment == UNKNOWN)
				a1.a_segment = ZP;
			if (a1.a_segment != ZP && a1.a_segment != ABSOLUTE)
				aerr(NEED_ZPABS);
		}
		/* ,2 + are indexes so we can't really police them for sanity */
		/* ,1 is PC relative so must be in this segment */
		if (acs == 1) {
			if (a1.a_segment == UNKNOWN)
				a1.a_segment = segment;
			else if (a1.a_segment != ABSOLUTE && a1.a_segment != segment)
				aerr(BAD_PCREL);
			if (a1.a_segment != ABSOLUTE) {
				a1.a_type |= TPCREL;
				a1.a_value -= dot[segment];
			}
		}
		if (a1.a_segment == ABSOLUTE)
			/* Turn into byte form temporarily */
			a1.a_value <<= 1;
		/* Insert the accumulators */
		opcode |= (acs << 8);
		if (indirect)
			opcode |= 0x0400;

		flushbyte();
		byteptr(0);

		outab(opcode >> 8);
		if (acs)
			outrabrel(&a1);	/* Signed */
		else if (acs == 0)
			outrab(&a1);	/* Unsigned */
		break;
	}

	case TALU:
	{
		SYM *skp = NULL;
		int drop;
		int cf;
		int sh;

		p = id + 3;
		cf = carryop(*p);
		if (cf)
			p++;
		sh = postop(*p);
		if (sh)
			p++;
		c = getnb();
		if (c == '#')
			drop = 1;
		else  {
			unget(c);
			drop = 0;
		}

		acs = accumulator();
		comma();
		acd = accumulator();
		c = getnb();
		if (c == ',') {
			getid(id1,getnb());
			skp = lookup(id1, phash, 0);
			if (skp == NULL || skp->s_type != TCC)
				err('s',SYNTAX_ERROR);
		} else
			unget(c);
		opcode |= (acs << 13);
		opcode |= (acd << 11);
		opcode |= (sh << 6);
		opcode |= (cf << 4);
		opcode |= (drop << 3);
		if (skp)
			opcode |= skp->s_value;
		flushbyte();
		outaw(opcode);
		break;
	}


	case TIO:
		p = id + 3;
		if (*p == 's')
			opcode |= 1 << 6;
		else if (*p == 'c')
			opcode |= 2 << 6;
		else if (*p == 'p')
			opcode |= 3 << 6;
		else if (*p)
			aerr(SYNTAX_ERROR);
		acs = accumulator();
		comma();
		getaddr(&a1);
		istuser(&a1);
		if (a1.a_value > 63)
			err('d', BADDEVICE);
		opcode |= (acs << 11);
		opcode |= a1.a_value;
		flushbyte();
		outaw(opcode);
		break;

	case TNIO:
		p = id + 3;
		if (*p == 's')
			opcode |= 1 << 6;
		else if (*p == 'c')
			opcode |= 2 << 6;
		else if (*p == 'p')
			opcode |= 3 << 6;
		else if (*p)
			aerr(SYNTAX_ERROR);
		/* Fall through */
	case TDEV:
		/* SKP is ambiguous so we list the four forms directly
		   in the table to avoid this */
		getaddr(&a1);
		istuser(&a1);
		if (a1.a_value > 63)
			err('d', BADDEVICE);
		opcode |= a1.a_value;
		flushbyte();
		outaw(opcode);
		break;

	case TAC:
		acs = accumulator();
		opcode |= (acs << 11);
		flushbyte();
		outaw(opcode);
		break;

	case TIMPL:
		flushbyte();
		outaw(opcode);
		break;

	case TBYTE:
		acs = accumulator();
		comma();
		acd = accumulator();
		opcode |= (acd << 11);
		opcode |= (acs << 6);
		flushbyte();
		outaw(opcode);
		break;

	case TTRAP:
		acs = accumulator();
		comma();
		acd = accumulator();
		comma();
		getaddr(&a1);
		/* We can't relocate these yet FIXME */
		istuser(&a1);
		opcode |= (acs << 15);
		opcode |= (acd << 13);
		opcode |= (a1.a_value << 4);
		flushbyte();
		outaw(opcode);
		break;
	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}

