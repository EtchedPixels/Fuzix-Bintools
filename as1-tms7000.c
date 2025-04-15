/*
 * TMS7000 assembler.
 * Assemble one line of input.
 */
#include	"as.h"

/* FIXME: we should malloc/realloc this on non 8bit machines */
static uint8_t reltab[1024];
static unsigned int nextrel;

/*
 *	Set up for the start of each pass
 */
int passbegin(int pass)
{
	segment = 1;		/* Default to code */
	if (pass == 3)
		nextrel = 0;
	return 1;
}

void doflush(void)
{
}

static void setnextrel(int flag)
{
	if (nextrel == 8 * sizeof(reltab))
		aerr(TOOMANYJCC);
	if (flag)
		reltab[nextrel >> 3] |= (1 << (nextrel & 7));
	nextrel++;
}

static unsigned int getnextrel(void)
{
	unsigned int n = reltab[nextrel >> 3] & (1 << (nextrel & 7));
	nextrel++;
	return n;
}

#define IS_A(ap)	(((ap)->a_type & (TMMODE|TMREG)) == (TBR|RA))
#define IS_B(ap)	(((ap)->a_type & (TMMODE|TMREG)) == (TBR|RB))

/*
 * Read in an address
 * descriptor, and fill in
 * the supplied "ADDR" structure
 * with the mode and value.
 * Exits directly to "qerr" if
 * there is no address field or
 * if the syntax is bad.
 */
void getaddr(ADDR *ap)
{
	int c;

	/* Address descriptors are really simple as the instruction
	   always explicitly implies the following type */
	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_sym = NULL;

	c = getnb();

	if (c == '<')
		ap->a_flags |= A_LOW;
	else if (c == '>')
		ap->a_flags |= A_HIGH;
	else
		unget(c);
	expr1(ap, LOPRI, 0);
}

static void constify(ADDR *ap)
{
	if ((ap->a_type & TMMODE) == (TUSER|TMINDIR))
		ap->a_type = TUSER;
}


/* The following syntaxes are permitted
	%nn		immediate
	%nnnn		immediate
	%nnnn(B)	indexed via B
	Px		Peripheral space (256 + x)
	Rx		Register space (x)
	A		0
	B		1
 */

static void getaddr_r(ADDR *ap)
{
	int c, c2, c3;

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_value = 0;
	ap->a_segment = UNKNOWN;
	ap->a_sym = NULL;

	c = getnb();
	if (c == '%') {
		getaddr(ap);
		c = getnb();
		if (c == '(') {
			c2 = getnb();
			c3 = getnb();
			if (c2 == 'b' && c3 ==')') {
				/* TOOD: check rest is B) */
				ap->a_type |= TIDXB;
				return;
			} else {
				unget(c3);
				unget(c2);
			}
		}
		unget(c);
		ap->a_type |= TIMMED;
		return;
	}
	/* r%u */
	if (c == 'r') {
		getaddr(ap);
		/* r%d */
		if (ap->a_value > 255)
			aerr(RANGE);
		ap->a_type |= TR;
		ap->a_segment = ZP;
		return;
	}
	if (c == 'p') {
		getaddr(ap);
		/* p%d */
		if (ap->a_value > 255)
			aerr(RANGE);
		ap->a_type = TPR;
		return;
	}
	if (c == 'a') {
		ap->a_type = TBR | TR | RA;
		ap->a_value = 0;
		ap->a_segment = ZP;
		return;
	}
	if (c == 'b') {
		ap->a_type = TBR | TR | RB;
		ap->a_value = 1;
		ap->a_segment = ZP;
		return;
	}
	if (c == 's') {
		c2 = getnb();
		if (c2 == 't') {
			ap->a_type = TSR | TR | RST;
			return;
		}
		unget(c2);
	}
	unget(c);
	aerr(BAD_MODE);
}

/* Indirect forms */
static void getaddr_i(ADDR *ap)
{
	int c, c2, c3;

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_value = 0;
	ap->a_sym = NULL;

	c = getnb();
	if (c == '@') {
		getaddr(ap);
		c = getnb();
		if (c == '(') {
			c2 = getnb();
			c3 = getnb();
			if (c2 == 'b' && c3 ==')') {
				/* TOOD: check rest is B) */
				ap->a_type |= TIDXB;
				return;
			} else {
				unget(c3);
				unget(c2);
			}
		}
		unget(c);
		ap->a_type |= TIMMED;
		return;
	}
	if (c == '*') {
		c2 = getnb();
		if (c2 == 'r') {
			getaddr(ap);
			/* r%d */
			if (ap->a_value > 255)
				aerr(RANGE);
			ap->a_type |= TR | TBR;
			ap->a_segment = ZP;
			return;
		}
		unget(c2);
	}
	unget(c);
	aerr(BAD_MODE);
}

/* You can't relative branch between segments */
static int segment_incompatible(ADDR *ap)
{
	if (ap->a_segment == segment)
		return 0;
	return 1;
}

static void outreg(ADDR *ap)
{
	if ((ap->a_type & TMADDR) != TR)
		aerr(REG_REQUIRED);
	outrab(ap);
}

static void outpab(ADDR *ap)
{
	if ((ap->a_type & TMADDR) != TPR)
		aerr(PREG_REQUIRED);
	outrab(ap);
}

/*
 *	B,A
 *	Rn,A
 *	Rn,B
 *	Rn,Rn
 *	%u8,A
 *	%u8,B
 *	%u,Rn
 */
void as_multop(unsigned opcode)
{
	ADDR a1, a2;

	getaddr_r(&a1);
	comma();
	getaddr_r(&a2);

	if ((a1.a_type & TMADDR) == TIMMED) {
		if (IS_A(&a2)) {
			outab(opcode + 0x20);
			outrab(&a1);
		} else if (IS_B(&a2)) {
			outab(opcode + 0x50);
			outrab(&a1);
		} else {
			/* TODO: check ordering */
			outab(opcode + 0x70);
			outrab(&a1);
			outreg(&a2);
		}
		return;
	}
	if (IS_B(&a1) && IS_A(&a2)) {
		outab(opcode + 0x60);
		return;
	}
	/* Special form for MOV only */
	if (opcode == 0x02 && IS_A(&a1) && IS_B(&a2)) {
		outab(0xC0);
		return;
	}
	if (IS_A(&a2) && (a1.a_type & TMADDR) == TR) {
		outab(opcode + 0x10);
		outreg(&a1);
		return;
	}
	if (IS_B(&a2) && (a1.a_type & TMADDR) == TR) {
		outab(opcode + 0x30);
		outreg(&a1);
		return;
	}
	/* Other MOV special forms */
	if (opcode == 0x02) {
		if ((a2.a_type & TMADDR) == TR) {
			if (IS_A(&a1))
				outab(0xD0);
			else if (IS_B(&a1))
				outab(0xD1);
			else {
				outab(opcode + 0x40);
				outreg(&a1);
			}
			outreg(&a2);
			return;
		}
	}
	outab(opcode + 0x40);
	outreg(&a1);
	outreg(&a2);
}

/* FIXME: This is a relative offset so needs adjusting */
void as_reloff(void)
{
	ADDR a1;
	getaddr(&a1);
	constify(&a1);
	istuser(&a1);
	a1.a_value -= dot[segment];
	a1.a_value -= 2;
	outrabrel(&a1);
}

/* A B or Rd. Push and Pop also allow 'st' for a separate push/pop status
   instruction */
void as_regop(unsigned opcode, unsigned pp)
{
	ADDR a1;
	getaddr_r(&a1);
	/* There are two different ops for push/pop of st that the instruction
	   set treats as "pop/push st" */
	if (pp && (a1.a_type & TMMODE) == TSR) {
		if (opcode == 0xB9)	/* POP */
			outab(0x08);
		else
			outab(0x0E);
		return;
	}
	if (IS_A(&a1))
		outab(opcode);
	else if (IS_B(&a1))
		outab(opcode + 0x10);
	else {
		outab(opcode + 0x20);
		outreg(&a1);
	}
}

/* A,Pd , B, Pd, %u8,Pd
   Solely for MOVP   Ps,A and Ps,B */
void as_opp(unsigned opcode, unsigned mov)
{
	ADDR a1, a2;
	getaddr_r(&a1);
	comma();
	getaddr_r(&a2);
	if (mov) {
		if (IS_A(&a2)) {
			outab(0x80);
			outpab(&a1);
			return;
		} else if (IS_B(&a2)) {
			outab(0x91);
			outpab(&a1);
			return;
		}
	}
	if (IS_A(&a1)) {
		outab(opcode);
		outpab(&a2);
	} else if (IS_B(&a1)) {
		outab(opcode + 0x10);
		outpab(&a2);
	} else if ((a1.a_type & TMADDR) == TIMMED) {
		outab(opcode + 0x20);
		outrab(&a1);
		outpab(&a2);
	} else
		aerr(BAD_MODE);
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
	int opcode;
	VALUE value;
	int delim;
	SYM *sp1;
	char id[NCPS];
	char id1[NCPS];
	ADDR a1, a2;
	int disp;

loop:
	if ((c=getnb())=='\n' || c==';')
		return;
	if (is_symstart(c) == 0 && c != '.')
		qerr(UNEXPECTED_CHR);
	getid(id, c);
	if ((c=getnb()) == ':') {
		sp = lookup(id, uhash, 1);
		if (pass == 0) {
			if ((sp->s_type&TMMODE) != TNEW
			&&  (sp->s_type&TMASG) == 0)
				sp->s_type |= TMMDF;
			sp->s_type &= ~TMMODE;
			sp->s_type |= TUSER;
			sp->s_value = dot[segment];
			sp->s_segment = segment;
		} else if (pass !=3) {
			/* Don't check for duplicates, we did it already
			   and we will confuse ourselves with the pass
			   before. Instead blindly update the values */
			sp->s_type &= ~TMMODE;
			sp->s_type |= TUSER;
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
	/*
	 * If the first token is an
	 * id and not an operation code,
	 * assume that it is the name in front
	 * of an "equ" assembler directive.
	 */
	if ((sp=lookup(id, phash, 0)) == NULL) {
		getid(id1, c);
		if ((sp1=lookup(id1, phash, 0)) == NULL
		||  (sp1->s_type&TMMODE) != TEQU) {
			err('o', SYNTAX_ERROR);
			return;
		}
		getaddr(&a1);
		constify(&a1);
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
		constify(&a1);
		istuser(&a1);
		if (a1.a_segment != ABSOLUTE)
			qerr(MUST_BE_ABSOLUTE);
		outsegment(ABSOLUTE);
		dot[segment] = a1.a_value;
		/* Tell the binary generator we've got a new absolute
		   segment. */
		outabsolute(a1.a_value);
		break;

	case TEXPORT:
		getid(id, getnb());
		sp = lookup(id, uhash, 1);
		sp->s_type |= TPUBLIC;
		break;
		/* .code etc */
	case TSEGMENT:
		segment = sp->s_value;
		/* Tell the binary generator about a segment switch to a non
		   absolute segnent */
		outsegment(segment);
		break;

	case TDEFB:
		do {
			getaddr(&a1);
			constify(&a1);
			istuser(&a1);
			outrab(&a1);
		} while ((c=getnb()) == ',');
		unget(c);
		break;

	case TDEFW:
		do {
			getaddr(&a1);
			constify(&a1);
			istuser(&a1);
			outrab(&a1);
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
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		/* Write out the bytes. The BSS will deal with the rest */
		for (value = 0 ; value < a1.a_value; value++)
			outab(0);
		break;

	case TIMPL:
		outab(opcode);
		break;
	case TMOV:
		/* Extended multop */
	case TMULT:
		as_multop(opcode);
		break;
	case TMULTO:
		as_multop(opcode);
		comma();
		as_reloff();
		break;
	case TIND:
		/* This is like a one op MOVD but in order to fit in with the TI
		   instruction mindset they add gratuitious 'indirect' markers */
		getaddr_i(&a1);
		switch(a1.a_type & TMADDR) {
		case TR:
			outab(opcode + 0x90);
			outreg(&a1);
			return;
		case TIMMED:
			outab(opcode + 0x80);
			outraw(&a1);
			return;
		case TIDXB:
			outab(opcode + 0xA0);
			outraw(&a1);
			return;
		}
		aerr(BAD_MODE);
		break;
	case TR3:
		as_regop(opcode, 0);
		break;
	case TR3O:
		as_regop(opcode, 0);
		comma();
		as_reloff();
		break;
	case TPP:
		as_regop(opcode, 1);
		break;
	case TMOVP:
		as_opp(opcode, 1);
		break;
	case TFORM:
		as_opp(opcode, 0);
		break;
	case TFORMO:
		as_opp(opcode, 0);
		comma();
		as_reloff();
		break;
	case TREL:
		/* 8bit relative with no trivial inverse */
		outab(opcode);
		as_reloff();
		break;
	case TJMP:
		/* Unconditional branch - convert to BR if need be */
		getaddr(&a1);
		disp = a1.a_value - dot[segment] - 2;
		if (pass == 3)
			c = getnextrel();
		else {
			c = 0;
			if (pass == 0 || segment_incompatible(&a1) || disp < -128 || disp > 127)
				c = 1;
			if (pass == 2)
				setnextrel(c);
		}
		if (c) {
			outab(0x8C);
			outraw(&a1);
		} else {
			outab(opcode);
			a1.a_value -= dot[segment];
			a1.a_value --;
			outrabrel(&a1);
		}
		break;
	case TBRA:
		/* Branch .. like reloff but we can do smart reverses etc */
		getaddr(&a1);
		disp = a1.a_value - dot[segment] - 2;
		if (pass == 3)
			c = getnextrel();
		else {
			c = 0;
			if (pass == 0 || segment_incompatible(&a1) || disp < -128 || disp > 127)
				c = 1;
			if (pass == 2)
				setnextrel(c);
		}
		if (c) {
			outab(opcode ^ 0x04);
			outab(3);
			outab(0x8C);
			outraw(&a1);
		} else {
			outab(opcode);
			a1.a_value -= dot[segment];
			a1.a_value --;
			outrabrel(&a1);
		}
		break;
	case TMOVD:
		getaddr_r(&a1);
		comma();
		getaddr_r(&a2);
		/* MOVD is just special */
		if ((a1.a_type & TMADDR) == TIMMED) {
			outab(0x88);
			outraw(&a1);
			outreg(&a2);
			return;
		}
		if ((a1.a_type & TMADDR) == TIDXB) {
			outab(0x98);
			outraw(&a1);
			outreg(&a2);
			return;
		}
		outreg(&a1);
		outreg(&a2);
		break;
	case TTRAP:
		/* TRAP is also unique */
		getaddr(&a1);
		constify(&a1);
		a1.a_value += 0xE8;
		if (a1.a_value < 0xE8 || a1.a_value > 0xFF)
			aerr(CONSTANT_RANGE);
		outrab(&a1);
		break;
	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}
