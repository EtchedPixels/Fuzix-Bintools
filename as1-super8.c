/*
 * Super8 Z8 assembler.
 * Assemble one line of input.
 * Knows all the dirt.
 *
 * Differences from the 'standard' Z8 stuff
 *
 * - 0x is used for hex not %XX
 * - %(base)foo is not supported
 * - %L etc for line feed and the like are not supported
 * - None of the PLZ stuff
 * - ; is used for comments not '!' or '//'
 * - manufactured instructions (RES etc) are not yet supported (use the
 *   underlying and/or immediate)
 * - no built in register alias names - .EQU in ABS should do fine ?
 */
#include	"as.h"

/* FIXME: we should malloc/realloc this on non 8bit machines */
static uint8_t reltab[1024];
static unsigned int nextrel;

int passbegin(int pas)
{
	segment = 1;
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

static void constify(ADDR *ap)
{
	if ((ap->a_type & TMMODE) == (TUSER|TMINDIR))
		ap->a_type = TUSER;
}

static int segment_incompatible(ADDR *ap)
{
	if (ap->a_segment == segment)
		return 0;
	return 1;
}

/*
 * Read in an address
 * descriptor, and fill in
 * the supplied "ADDR" structure
 * with the mode and value.
 * Exits directly to "qerr" if
 * there is no address field or
 * if the syntax is bad.
 *
 * Z8 syntax wants %xx for hex and HI xx or LO xx not < >. Look at that
 * later.
 *
 * We recognize the following
 * Rn - register short form
 * RRn - register pair short form
 * @Rn - register, indirect short form
 * @RRn - register pair, indirect short form
 * n - register (sometimes implied constant DA)
 * @n - register indirect
 * #n - 8bit constant
 *
 * The implied DA ambiguity is a pain
 */
void getaddr_r(ADDR *ap)
{
	int indirect = 0;
	int pair = 0;
	int c;

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_sym = NULL;
	
	c = getnb();

	/* #foo */	
	if (c == '#') {
		c = getnb();
		if (c == '<')
			ap->a_flags |= A_LOW;
		else if (c == '>')
			ap->a_flags |= A_HIGH;
		else
			unget(c);
		expr1(ap, LOPRI, 0);
		istuser(ap);
		constify(ap);
		ap->a_type |= TIMMED;
		return;
	}
	
	if (c == '@') {
		indirect = 1;
		c = getnb();
	}
	/* Register short forms */
	if (c == 'R' || c == 'r') {
		c = getnb();
		if (c == 'R' || c == 'r')
			pair = 1;
		else
			unget(c);
			
		expr1(ap, LOPRI, 0);
		istuser(ap);
		constify(ap);
		if (ap->a_sym)
			qerr(SYNTAX_ERROR);
		if (ap->a_value < 0 || ap->a_value > 15)
			aerr(RSHORT_RANGE);
		/* Set TMREG */
		ap->a_type |= ap->a_value;
		/* No offset */
		ap->a_value = 0;
		if (pair == 0) {
			if (indirect)
				ap->a_type |= TSIND;
			else
				ap->a_type |= TRS;
		} else {
			if (indirect)
				ap->a_type |= TRRIND;
			else {
				if (ap->a_type & 1)
					aerr(ODD_REGISTER);
				ap->a_type |= TRRS;
			}
		}
		return;
	}
	unget(c);

	/* If it wasn't a short register or a constant it's a register. We
	   have no 'address' formats except for constant load and indexed */

	expr1(ap, LOPRI, 1);

	c = getnb();
	if (c == '(') {
		ADDR tmp;
		unsigned tt;
		if (indirect)
			aerr(INVALID_FORM);
		getaddr_r(&tmp);
		tt = tmp.a_type & TMADDR;
		if (tt != TRS && tt != TRRS)
			qerr(RSHORT_RANGE);
		c = getnb();
		if (c != ')')
			qerr(SYNTAX_ERROR);
		ap->a_type &= ~(TMREG | TMADDR);
		ap->a_type |= tmp.a_type & TMREG;	/* Insert register */
		if (tt == TRS)
			ap->a_type |= TINDEX;
		else
			ap->a_type |= TRROFF;
	} else {
		unget(c);
		if (indirect)
			ap->a_type = TIND;
		else
			ap->a_type = TREG;
	}
}

#define T_REG(x)	((x)->a_type & TMREG)

/* Unshorten a register form */
unsigned unshort_reg(ADDR *ap)
{
	unsigned t = ap->a_type & TMADDR;
	if (t == TRS) {
		ap->a_value = (ap->a_type & TMREG) | 0xC0;
		ap->a_type &= ~TMADDR;
		ap->a_type |= TREG;
		return TREG;
	} else if (t == TRRS) {
		ap->a_value = (ap->a_type & TMREG) | 0xC0;
		ap->a_type &= ~TMADDR;
		ap->a_type |= TREG;
		return TREG;
	} else if (t == TSIND) {
		ap->a_value = (ap->a_type & TMREG) | 0xC0;
		ap->a_type &= ~TMADDR;
		ap->a_type |= TIND;
		return TIND;
	} else if (t == TRRIND) {
		ap->a_value = (ap->a_type & TMREG) | 0xC0;
		ap->a_type &= ~TMADDR;
		ap->a_type |= TIND;
		return TIND;
	} else
		return ap->a_type & TMADDR;
}

void check_8bit(ADDR *ap)
{
	if (!(ap->a_flags & (A_HIGH|A_LOW)) && (ap->a_value < -128 || ap->a_value > 255))
		aerr(CONSTANT_RANGE);
}

void getaddr8(ADDR *ap)
{
	getaddr_r(ap);
	check_8bit(ap);
}
		
int getcond(ADDR *ap)
{
	if ((ap->a_type & TMMODE) == TCC)
		return (ap->a_type & TMREG);
	return -1;
}

void getaddr(ADDR *ap)
{
	int c = getnb();

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_sym = NULL;

	if (c == '<')
		ap->a_flags |= A_LOW;
	else if (c == '>')
		ap->a_flags |= A_HIGH;
	else
		unget(c);
	expr1(ap, LOPRI, 0);
	/* Condition code */
	if ((ap->a_type & TMMODE) == TCC)
		return;
	istuser(ap);
	constify(ap);
	ap->a_type |= TIMMED;
}

void bit_range(ADDR *ap)
{
	if ((ap->a_type & TMADDR) != TIMMED || ap->a_value < 0 || ap->a_value > 7)
		aerr(BITRANGE);
}

/* Must be a constant - will be seen as a reg form */
void check_ra(ADDR *ap)
{
	unsigned t = ap->a_type & TMADDR;
	if (t != TREG && t != TIMMED)
		aerr(INVALID_FORM);
}

/* Direct address */
void check_da(ADDR *ap)
{
	unsigned t = ap->a_type & TMADDR;
	if (t != TREG && t != TIMMED)
		aerr(INVALID_FORM);
}

void check_pair(ADDR *ap)
{
	if (ap->a_value & 1)
		aerr(ODD_REGISTER);
}

/* Generate two byte sized link records. This is a bit of a back */
void outraw_backwards(ADDR *ap)
{
	ap->a_flags |= A_LOW;
	outrab(ap);
	ap->a_flags &= ~A_LOW;
	ap->a_flags |= A_HIGH;
	outrab(ap);
	ap->a_flags &= ~A_HIGH;
}

void not_reg01(ADDR *ap)
{
	if ((ap->a_type & TMADDR) != TRS) {
		aerr(INVALID_FORM);
		return;
	}
	if (T_REG(ap) < 2)
		aerr(NOTREG01);
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
	int cc;
	VALUE value;
	int delim;
	SYM *sp1;
	char id[NCPS];
	char id1[NCPS];
	ADDR a1;
	ADDR a2;
	ADDR a3;
	int ta1;
	int ta2;
	int ta3;
	int disp;

loop:
	if ((c=getnb())=='\n' || c==';')
		return;
	if (isalpha(c) == 0 && c != '_' && c != '.')
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
		} else if (pass != 3) {
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

	/* Logic and maths operations with a 4 bit operation and 4 bits of
	   addressing information */
	case TOP4BIT:
		getaddr8(&a1);
		c = getnb();
		if (c != ',')
			qerr(MISSING_DELIMITER);
		getaddr8(&a2);
		/* Not work out how to encode it */
		/* OP r,r */
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;

		/* Short forms are  dest << 4 | soure  */
		if (ta1 == TRS && ta2 == TRS) {
			outab(opcode | 0x02);
			outab((T_REG(&a1) << 4) | T_REG(&a2));
			break;
		}
		/* OP r,Ir */
		if (ta1 == TRS && ta2 == TSIND) {
			outab(opcode | 0x03);
			outab((T_REG(&a1) << 4) | T_REG(&a2));
			break;
		}
		/* Look for long forms using 0xC0 for current reg set */
		ta1 = unshort_reg(&a1);
		ta2 = unshort_reg(&a2);
		/* Long forms  are src, dst except when the are not (sigh)*/
		/* OP R,R */
		if (ta1 == TREG && ta2 == TREG)
			outab(opcode | 0x04);
		/* OP R,@R */
		else if (ta1 == TREG && ta2 == TIND)
			outab(opcode | 0x05);
		/* OP R,IMM */
		else if (ta1 == TREG && ta2 == TIMMED)
			outab(opcode | 0x06);
		/* no @r,IMM format on the Super 8 */
		else
			qerr(INVALID_FORM);
		/* Immediate is backwards to the others */
		if (ta2 == TIMMED) {
			/* dst src */
			outab(a1.a_value);
			outrab(&a2);
		} else {
			/* src dst */
			outrab(&a2);
			outab(a1.a_value);
		}
		break;

	case TRRIR:
		/* RR or IR format */
		/* We accept both rr0 and r0 */
		getaddr8(&a1);
		switch(a1.a_type & TMADDR) {
		case TRS:
		case TRRS:
			unshort_reg(&a1);
		case TREG:
			check_pair(&a1);
			outab(opcode);
			break;
		case TSIND:
		case TRRIND:
			unshort_reg(&a1);
		case TIND:
			outab(opcode + 0x01);
			break;
		default:
			qerr(INVALID_FORM);
		}
		outrab(&a1);
		break;
	case TRIR:
		/* R or IR format */
		getaddr8(&a1);
		switch(a1.a_type & TMADDR) {
		case TRS:
			if (opcode == 0x20) {	/* INC has an r form */
				opcode = 0x0E | (T_REG(&a1) << 4);
				outab(opcode);
				break;
			}
			unshort_reg(&a1);
		case TREG:
			outab(opcode);
			outrab(&a1);
			break;
		case TSIND:
			unshort_reg(&a1);
		case TIND:
			outab(opcode + 0x01);
			outrab(&a1);
			break;
		default:
			qerr(INVALID_FORM);
		}
		break;

	case TIMM8:
		/* 8bit immediate */
		getaddr8(&a1);
		if ((a1.a_type & TMADDR) != TIMMED)
			qerr(INVALID_FORM);
		outab(opcode);
		outrab(&a1);
		break;

	case TCRA:
		getaddr(&a1);
		cc = getcond(&a1);
		if (cc != -1) {
			c = getnb();
			if (c != ',')
				qerr(MISSING_DELIMITER);
			getaddr(&a1);
		} else
			cc = 0x08; /* True */
		check_ra(&a1);
		disp = a1.a_value - dot[segment] - 2;
		if (pass == 3)
			c = getnextrel();
		else {
			c = 0;
			if (pass == 0 || segment_incompatible(&a1) ||
				disp < -128 || disp > 127)
				c = 1;
			if (pass == 2)
				setnextrel(c);
		}
		if (c) {
			/* Write a JP cc instead */
			outab(0x0D + (cc << 4));
			outraw(&a1);
		} else {
			outab(opcode + (cc << 4));
			if (disp < -128 || disp > 127)
				aerr(BRA_RANGE);
			/* Relative branches are always in segment and within our
			   generated space so don't relocate */
			outab(disp);
		}
		break;
	case TJMP:
		c = getnb();
		unget(c);
		if (c == '@') {
			getaddr_r(&a1);
			ta1 = a1.a_type & TMADDR;
			if (ta1 == TRRIND)
				ta1 = unshort_reg(&a1);
			if (ta1 == TIND) {
				check_pair(&a1);
				outab(0x30);
				outrab(&a1);
				break;
			}
			qerr(INVALID_FORM);
			break;
		}
		getaddr(&a1);
		cc = getcond(&a1);
		if (cc != -1) {
			c = getnb();
			if (c != ',')
				qerr(MISSING_DELIMITER);
			getaddr(&a1);
		} else
			cc = 0x08; /* True */
		check_da(&a1);
		outab(opcode + (cc << 4));
		/* Relocatable label */
		outraw(&a1);
		break;
	case TCALL:	/* Call */
		getaddr_r(&a1);
		ta1 = a1.a_type & TMADDR;
		if (ta1 == TRRIND)
			ta1 = unshort_reg(&a1);
		if (ta1 == TIMMED) {
			/* Weird form call #n is a call to the address found
			   at the 8bit location pair given (akin to Z80 RST) */
			outab(0xD4);
			check_8bit(&a1);
			outrab(&a1);
			break;
		} else if (ta1 == TIND) {
			/* CALL @RR */
			check_pair(&a1);
			outab(0xF4);
			outrab(&a1);
			break;
		} else if (ta1 == TREG) {
			/* Actually in this case just a value */
			outab(0xF6);
			/* Relocatable label */
			check_da(&a1);
			outraw(&a1);
			break;
		}
		/* FIXME check Z8 classic for wrong form behaviour */
		qerr(INVALID_FORM);
		break;		
	case TRA:	/* DJNZ */
		getaddr8(&a1);
		if ((a1.a_type & TMADDR) != TRS)
			qerr(INVALID_FORM);
		c = getnb();
		if (c != ',')
			qerr(MISSING_DELIMITER);
		outab(0x0A | (T_REG(&a1) << 4));
		/* And then a relative address */
		/* TODO: Check from of rel addr */
		getaddr(&a2);
		check_ra(&a2);
		a2.a_value -= dot[segment] + 1;
		outrabrel(&a2);
		break;
	case TLDC:
		/* r, Irr */
		/* Irr, r */
		/* r, xs(rr) */
		/* xs(rr), r */
		/* r, xl(rr) - not r0/1 */
		/* xl(rr), r - not r0/1 */
		/* r, da */
		/* da, r */
		/* These forms store 16bit values little endian so we will
		   need a reverse endian relocation */
		getaddr_r(&a1);
		c = getnb();
		if (c != ',')
			qerr(MISSING_DELIMITER);
		getaddr_r(&a2);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		/* r,@rr */
		if (ta1 == TRS && ta2 == TRRIND) {
			outab(0xC3);
			outab((T_REG(&a1) << 4) | T_REG(&a2) | (opcode >> 8));
			break;
		}
		/* @rr,r */
		if (ta1 == TRRIND && ta2 == TRS) {
			outab(0xD3);
			outab((T_REG(&a1) << 4) | T_REG(&a2) | (opcode >> 8));
			break;
		}
		/* r xx(rr) */
		if (ta1 == TRS && ta2 == TRROFF) {
			not_reg01(&a1);
			if (a2.a_value < 256) {
				outab(0xE7);
				outab((T_REG(&a1) << 4) | T_REG(&a2) | (opcode >> 8));
				outrab(&a2);
				break;
			}
			outab(0xA7);
			outab((T_REG(&a1) << 4) | T_REG(&a2) | (opcode >> 8));
			outraw_backwards(&a2);
			break;
		}
		/* xx(rr), r */
		if (ta1 == TRROFF && ta2 == TRS) {
			not_reg01(&a2);
			if (a1.a_value < 256) {
				outab(0xF7);
				outab((T_REG(&a2) << 4) | T_REG(&a1) | (opcode >> 8));
				outrab(&a1);
				break;
			}
			outab(0xB7);
			outab((T_REG(&a1) << 4) | T_REG(&a2) | (opcode >> 8));
			outraw_backwards(&a1);
			break;
		}
		/* r, da */
		if (ta1 == TRS && ta2 == TREG) {
			outab(0xA7);
			outab((T_REG(&a1) << 4) | (opcode >> 8));
			check_da(&a2);
			outraw_backwards(&a2);
			break;
		}
		/* da, r */
		if (ta1 == TREG && ta2 == TRS) {
			outab(0xB7);
			outab((T_REG(&a2) << 4) | (opcode >> 8));
			check_da(&a1);
			outraw_backwards(&a1);
			break;
		}
		qerr(INVALID_FORM);
		break;
	case TLDCI:
		/* post dec and post inc are r,@rr **/
		getaddr8(&a1);
		c = getnb();
		if (c != ',')
			qerr(MISSING_DELIMITER);
		getaddr8(&a2);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		if (ta1 == TRS && ta2 == TRRIND) {
			/* dst, src */
			outab(opcode);
			outab((T_REG(&a1) << 4) | T_REG(&a2) | (opcode >> 8));
		} else
			qerr(INVALID_FORM);
		break;
	case TLDCPI:
		/* pre dec and pre inc are @rr,r **/
		getaddr8(&a1);
		c = getnb();
		if (c != ',')
			qerr(MISSING_DELIMITER);
		getaddr8(&a2);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		if (ta1 == TRRIND && ta2 == TRS) {
			/* dst, src */
			outab(opcode);
			outab((T_REG(&a2) << 4) | T_REG(&a1) | (opcode >> 8));
		} else
			qerr(INVALID_FORM);
		break;
	case TLOAD:
		/* Load is its own special complicated case */
		getaddr8(&a1);
		c = getnb();
		if (c != ',')
			qerr(MISSING_DELIMITER);
		getaddr8(&a2);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		/* Now encode the load by type */
		/* LD r0,#1 */
		if (ta1 == TRS && ta2 == TIMMED) {
			outab(0x0C | (T_REG(&a1) << 4));
			outrab(&a2);
			break;
		}
		/* LD r0,sp */
		if (ta1 == TRS && ta2 == TREG) {
			/* dst|mode , src */
			outab(0x08 | (T_REG(&a1) << 4));
			outrab(&a2);
			break;
		}
		/* LD sp, r0 */
		if (ta1 == TREG && ta2 == TRS) {
			/* src|mode , dst */
			outab(0x09 | (T_REG(&a2) << 4));
			outrab(&a1);
			break;
		}
		/* LD r0,@r1 */
		if (ta1 == TRS && ta2 == TSIND) {
			/* mode | 3 , dst | src */
			outab(0xC7);
			outab((T_REG(&a1) << 4) | T_REG(&a2));
			break;
		}
		/* LD @r0,r1 */
		if (ta1 == TSIND && ta2 == TRS) {
			/* mode | 3 , dst | src */
			outab(0xD7);
			outab((T_REG(&a1) << 4) | T_REG(&a2));
			break;
		}
		if (ta1 == TRS && ta2 == TINDEX) {
			/* op , dst | x, offset */
			outab(0x87);
			outab((T_REG(&a1) << 4) | T_REG(&a2));
			outrab(&a2);
			break;
		}
		if (ta1 == TINDEX && ta2 == TRS) {
			/* op , src | x, offset */
			outab(0x97);
			outab(T_REG(&a1) | (T_REG(&a2) << 4));
			outrab(&a1);
			break;
		}
		/* Partial short form. Turn r,r into r,R not R,R */
		if (ta1 == TRS && ta2 == TRS) {
			outab(0x08 | (T_REG(&a1) << 4));
			unshort_reg(&a2);
			outab(a2.a_value);
			break;
		}
		/* If we get here in short form we need to try the long
		   form alias */
		/* FIXME these two are probably wrong on Z8 classic
		   check ld in general */
		if (ta1 == TRS || ta1 == TSIND)
			ta1 = unshort_reg(&a1);
		if (ta2 == TRS || ta2 == TSIND)
			ta2 = unshort_reg(&a2);
		if (ta1 == TREG && ta2 == TREG) {
			/* op, src, dst */
			outab(0xE4);
			outrab(&a2);
			outrab(&a1);
			break;
		}
		if (ta1 == TREG && ta2 == TIND) {
			/* op, src, dst */
			outab(0xE5);
			outrab(&a2);
			outrab(&a1);
			break;
		}
		if (ta1 == TREG && ta2 == TIMMED) {
			/* op, dst, src */
			outab(0xE6);
			outrab(&a1);
			outrab(&a2);
			break;
		}
		if (ta1 == TIND && ta2 == TIMMED) {
			/* op, dst, src */
			outab(0xD6);
			outrab(&a1);
			outrab(&a2);
			break;
		}
		if (ta1 == TIND && ta2 == TREG) {
			/* op, src, dst */
			outab(0xF5);
			outrab(&a2);
			outrab(&a1);
			break;
		}
		qerr(INVALID_FORM);
		break;
	case TBIT:
		getaddr8(&a1);
		comma();
		getaddr8(&a2);
		comma();
		getaddr8(&a3);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		ta3 = a3.a_type & TMADDR;
		/* dst,src,b */
		if (ta3 == TIMMED && ta1 == TRS) {
			bit_range(&a3);
			if (ta2 == TRS)
				unshort_reg(&a2);
			else if (ta2 != TREG)
				qerr(INVALID_FORM);
			outab(opcode);
			outab((T_REG(&a1) << 4) | (a3.a_value << 1));
			outrab(&a2);
			break;
		}
		/* dst,b,src */
		if (ta2 == TIMMED && ta3 == TRS) {
			bit_range(&a2);
			if (ta1 == TRS)
				unshort_reg(&a1);
			else if(ta1 != TREG)
				qerr(INVALID_FORM);
			outab(opcode);
			outab((T_REG(&a3) << 4) | (a2.a_value << 1) |  1);
			outrab(&a1); 
			break;
		}
		qerr(INVALID_FORM);
		break;
	case TBCP:
		/* Like bit but only one form */
		getaddr8(&a1);
		comma();
		getaddr8(&a2);
		comma();
		getaddr8(&a3);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		ta3 = a3.a_type & TMADDR;
		/* dst,src,#b */
		if (ta3 == TIMMED && ta1 == TRS) {
			bit_range(&a3);
			if (ta2 == TRS)
				unshort_reg(&a2);
			else if (ta2 != TREG)
				qerr(INVALID_FORM);
			outab(opcode);
			outab((T_REG(&a1) << 4) | (a3.a_value << 1));
			outrab(&a2);
			break;
		}
		qerr(INVALID_FORM);
		break;
	case TBITX:
		/* dst,b */
		getaddr8(&a1);
		comma();
		getaddr8(&a2);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		bit_range(&a2);
		if (ta1 == TRS) {
			outab(opcode & 0xFF);
			outab((T_REG(&a1) << 4) | (a2.a_value << 1) | (opcode >> 8));
			break;
		}
		qerr(INVALID_FORM);
		break;
	case TBJMP:
		/* ra, rs, bit */
		getaddr(&a1);
		comma();
		getaddr8(&a2);
		comma();
		getaddr8(&a3);
		ta2 = a2.a_type & TMADDR;
		bit_range(&a3);
		check_ra(&a1);
		if (ta2 == TRS) {
			outab(opcode & 0xFF);
			outab((T_REG(&a2) << 4) | (a3.a_value << 1) | (opcode >> 8));
			a1.a_value -= dot[segment] + 1;
			outrabrel(&a1);
			break;
		}
		qerr(INVALID_FORM);
		break;
	case TIMM8M:
		/* immediate but bits set by op */
		getaddr8(&a1);
		if ((a1.a_type & TMADDR) != TIMMED) {
			qerr(INVALID_FORM);
			break;
		}
		/* FIXME: cases where value is not known ? */
		if ((a1.a_value & 7) || (a1.a_value & 0xFF00)) {
			qerr(INVALID_CONST);
			break;
		}
		outab(opcode & 0xFF);
		outab(a1.a_value | (opcode >> 8));
		break;
	case TPUSHU:
		getaddr8(&a1);
		comma();
		getaddr8(&a2);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		if (ta2 == TRS)
			ta2 = unshort_reg(&a2);
		if (ta1 == TSIND)
			ta1 = unshort_reg(&a1);
		if (ta1 == TIND && ta2 == TREG) {
			outab(opcode);
			outrab(&a1);
			outrab(&a2);
			break;
		}
		qerr(INVALID_FORM);
		break;
	case TPOPU:
		getaddr8(&a1);
		comma();
		getaddr8(&a2);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		if (ta1 == TRS)
			ta1 = unshort_reg(&a1);
		if (ta2 == TSIND)
			ta2 = unshort_reg(&a2);
		if (ta1 == TREG && ta2 == TIND) {
			outab(opcode);
			outrab(&a2);
			outrab(&a1);
			break;
		}
		qerr(INVALID_FORM);
		break;
	case TMULDIV:
		/* rr,r  rr,ir  rr,im */
		getaddr8(&a1);
		comma();
		getaddr8(&a2);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;

		if (ta2 == TRS || ta2 == TSIND)
			ta2 = unshort_reg(&a2);
		if (ta1 == TRRS)
			ta1 = unshort_reg(&a1);
		if (ta1 == TREG && ta2 == TREG) {
			check_pair(&a1);
			outab(opcode);
			outrab(&a2);
			outrab(&a1);
			break;
		}
		if (ta1 == TREG && ta2 == TIND) {
			check_pair(&a1);
			outab(opcode + 1);
			outrab(&a2);
			outrab(&a1);
			break;
		}
		if (ta1 == TREG && ta2 == TIMMED) {
			check_pair(&a1);
			outab(opcode + 2);
			outrab(&a2);
			outrab(&a1);
			break;
		}
		qerr(INVALID_FORM);
		break;
	case TCPIJ:
		/* rs, rs, ra */
		getaddr8(&a1);
		comma();
		getaddr8(&a2);
		comma();
		getaddr(&a3);
		check_ra(&a3);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		if (ta1 != TRS || ta2 != TRS)
			qerr(INVALID_FORM);
		outab(opcode);
		outab((T_REG(&a2) << 4) | T_REG(&a1));
		/* And then a relative address */
		a3.a_value -= dot[segment] + 1;
		outrabrel(&a3);
		break;
	case TLDW:
		/* Forms are scattered */
		/* dst,b */
		getaddr8(&a1);
		comma();
		getaddr_r(&a2);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		if (ta1 == TRRS || ta1 == TSIND)
			ta1 = unshort_reg(&a1);
		if (ta2 == TRRS || ta2 == TSIND)
			ta2 = unshort_reg(&a2);
		if (ta1 == TREG && ta2 == TREG) {
			check_8bit(&a1);
			check_8bit(&a2);
			outab(0xC4);
			outrab(&a1);
			outrab(&a2);
			break;
		}
		if (ta1 == TREG && ta2 == TIND) {
			check_8bit(&a1);
			check_8bit(&a2);
			outab(0xC5);
			outrab(&a1);
			outrab(&a2);
			break;
		}
		if (ta1 == TREG && ta2 == TIMMED) {
			check_8bit(&a1);
			outab(0xC6);
			outrab(&a1);
			outraw(&a2);
			break;
		}
		qerr(INVALID_FORM);
		break;
	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}
