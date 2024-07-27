/*
 *	Perkin Elmer / Interdata 8-16 assembler
 *
 *	TODO: different parsing type for FP and DP (always pairs and f%d say ?)
 *	      check more ops about pairs
 */
#include	"as.h"

int passbegin(int pass)
{
	segment = 1;
	if (pass == 1 || pass == 2)
		return 0;
	return 1;
}

void doflush(void)
{
}

static void constify(ADDR *ap)
{
	if ((ap->a_type & TMMODE) == (TUSER|TMINDIR))
		ap->a_type = TUSER;
}

/*
 *	Read in a register descriptor. Also works for short forms
 */

void getaddr_reg(ADDR *ap)
{
	/* There are syntaxs using r0-r15, 0-15 etc. We use r as it's
	   harder to screw up with an explicit r */
	unsigned c = getnb();
	if (c != 'r')
		qerr(REGREQUIRED);

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_sym = NULL;
	ap->a_value = 0;

	expr1(ap, LOPRI, 0);
	istuser(ap);
	if (ap->a_value < 0 || ap->a_value > 15 || ap->a_segment == UNKNOWN)
		qerr(REGINVALID);
	ap->a_type = ap->a_value | TWR;
	ap->a_value = 0;
}

void getaddr_sf(ADDR *ap)
{
	/* There are syntaxs using r0-r15, 0-15 etc. We use r as it's
	   harder to screw up with an explicit r */
	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_sym = NULL;
	ap->a_value = 0;

	expr1(ap, LOPRI, 0);
	istuser(ap);
	if (ap->a_value < 0 || ap->a_value > 15 || ap->a_segment == UNKNOWN)
		qerr(SFINVALID);
}

/* RI format RI1 and RI2
         immediate
         immediate(reg)
 */
void getaddr_ir(ADDR *ap, unsigned len)
{
	ADDR r;
	unsigned c;

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_sym = NULL;
	ap->a_value = 0;

	/* The format is addr or addr(reg) */
	expr1(ap, LOPRI, 0);
	c = getnb();
	if (c != '(') {
		unget(c);
		istuser(ap);
		return;
	}
	getaddr_reg(&r);
	if ((r.a_type & TMREG) == 0)
		qerr(NO0INDEX);
	ap->a_type |= TIMMED | (r.a_type & TMREG);
	c = getnb();
	if (c != ')')
		aerr(SYNTAX_ERROR);
}

/* 
 * RX is a lot more complex than the PE16.
 *
 * RX1
 *	addr		14bit abs
 *	addr(reg)
 * RX2
 *	addr		15bit PC relative signed
 *	addr(reg)	Ditto + index
 *
 * RX3
 *	addr		24 bit address
 *	addr(reg)	24bit address + reg
 *	addr(reg,reg2)	24bit address + reg + reg2
 */

static void pick_rxform(ADDR *ap)
{
	/* Now decide what kind of RX format */
	if (ap->a_segment == ABSOLUTE && ap->a_value <= 0x3FFF) {
		ap->a_type |= TRX1;
		return;
	}
	if (ap->a_segment == segment) {
		/* Demonstrably PC relative */
		int32_t diff = ap->a_value - segment[dot] - 5;
		if (diff < 0x8000 || diff > 0x7FFF) {
			ap->a_value -= 5;
			ap->a_type |= TRX2;
			return;
		}
	}
	/* Either unknown so assume full size or known and too big */
	ap->a_type |= TRX3;
	ap->a_value &= 0xFFFFFF;
	return;
}

void getaddr_rx(ADDR *ap)
{
	ADDR r, r2;
	unsigned c;

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_sym = NULL;

	/* The format is addr or addr(reg) */
	expr1(ap, LOPRI, 0);
	istuser(ap);
	c = getnb();
	if (c != '(') {
		pick_rxform(ap);
		return;
	}
	getaddr_reg(&r);
	if ((r.a_type & TMREG) == 0)
		qerr(NO0INDEX);

	ap->a_type |= r.a_type & TMREG;

	c = getnb();
	if (c == ',') {
		getaddr_reg(&r2);
		if ((r2.a_type & TMREG) == 0)
			qerr(NO0INDEX);
		c = getnb();
		if (c != ')')
			qerr(SYNTAX_ERROR);
		ap->a_value &= 0xFFFFFF;
		/* Hack for double reg indexing */
		ap->a_value |= (r2.a_type & TMREG) << 24;
		ap->a_type |= TRX3;
		return;
	}
	if (c != ')')
		qerr(SYNTAX_ERROR);
	pick_rxform(ap);
}

void getaddr(ADDR *ap)
{
	int c = getnb();
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
}

void check_pair(ADDR *ap)
{
	if (ap->a_value & 1)
		aerr(ODD_REGISTER);
}

void outaw(unsigned v)
{
	outab(v >> 8);
	outab(v & 0xFF);
}

void outrx(unsigned opcode, ADDR *ap)
{
	opcode |= ap->a_type & TMREG;
	switch(ap->a_type & TMADDR) {
	case TRX1:
		outaw(opcode);
		outaw(ap->a_value & 0x3FFF);
		break;	
	case TRX2:
		outaw(opcode);
		outaw(0x8000 | (ap->a_value & 0x7FFF));
		break;
	case TRX3:
		outaw(opcode);
		outab(0x40 | ((ap->a_value >> 24) & 0x0F));
		outratrel(ap);
		break;
	default:
		aerr(SYNTAX_ERROR);
	}
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
	ADDR a1;
	ADDR a2;
	int32_t diff;

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

	case TBFRR:
		getaddr_sf(&a1);
		comma();
		getaddr_reg(&a2);
		opcode <<= 8;
		opcode |= (a1.a_type & TMREG) << 4;
		opcode |= a2.a_type & TMREG;
		outaw(opcode);
		break;

	case TRR:
		getaddr_reg(&a1);
		comma();
		getaddr_reg(&a2);
		opcode <<= 8;
		opcode |= (a1.a_type & TMREG) << 4;
		opcode |= a2.a_type & TMREG;
		outaw(opcode);
		break;

	case TRRF:
		getaddr_reg(&a1);
		comma();
		getaddr_reg(&a2);
		check_pair(&a1);
		check_pair(&a2);
		opcode <<= 8;
		opcode |= (a1.a_type & TMREG) << 4;
		opcode |= a2.a_type & TMREG;
		outaw(opcode);
		break;

	case TBRR:
		/* TODO: Need to look at branch relocs here properly */
		getaddr_reg(&a1);
		opcode <<= 8;
		opcode |= a1.a_type & TMREG;
		outaw(opcode);
		break;

	case TR0:
		getaddr_reg(&a1);
		opcode <<= 8;
		opcode |= a1.a_type & TMREG;
		outaw(opcode);
		break;

	case TBSF:
		/* Also bit in opcode for +ve/-ve */
		getaddr(&a1);
		istuser(&a1);
		/* Now check is valid */
		if (a1.a_segment == ABSOLUTE)
			diff = a1.a_value;
		else if (a1.a_segment == segment) {
			diff = a1.a_value - dot[segment];
			diff /= 2;
		} else
			aerr(SFINVALID);

		if (diff >= 0)
			opcode |= 0x0100;
		else
			diff = -diff;

		if (diff > 15)
			qerr(SFINVALID);
		opcode |= diff & 0x0F;
		outaw(opcode);
		break;

	case TBFSF:
		/* Also bit in opcode for +ve/-ve */
		getaddr_sf(&a1);
		comma();
		getaddr(&a2);
		istuser(&a2);
		opcode <<= 8;
		/* Now check is valid */
		if (a2.a_segment == ABSOLUTE)
			diff = a2.a_value;
		else if (a2.a_segment == segment) {
			diff = a2.a_value - dot[segment];
			diff /= 2;
			if (diff >= 0)
				opcode |= 0x0100;
			else
				diff = -diff;
		} else
			aerr(SFINVALID);

		if (diff > 15 || diff < 0)
			qerr(SFINVALID);
		opcode |= a1.a_value << 4;
		opcode |= diff & 0x0F;
		outaw(opcode);
		break;

	case TSF:
		getaddr_reg(&a1);
		comma();
		getaddr_sf(&a2);
		opcode <<= 8;
		opcode |= a1.a_value << 4;
		opcode |= a2.a_value;
		outaw(opcode);
		break;

	case TSVC:	/* N rather than reg */
		getaddr_sf(&a1);
		comma();
		getaddr_rx(&a2);
		opcode <<= 8;
		opcode |= a1.a_value << 4;
		opcode |= a2.a_type & TMREG;
		outaw(opcode);
		outraw(&a2);
		break;

	case TRX:
		getaddr_reg(&a1);
		comma();
		getaddr_rx(&a2);
		opcode <<= 8;
		opcode |= (a1.a_type & TMREG) << 4;
		outrx(opcode, &a2);
		break;

	case TBFRX:
		getaddr_sf(&a1);
		comma();
		getaddr_rx(&a2);
		opcode <<= 8;
		opcode |= a1.a_value << 4;
		outrx(opcode, &a2);
		break;

	case TRXF:
		getaddr_reg(&a1);
		check_pair(&a1);
		comma();
		getaddr_rx(&a2);
		opcode <<= 8;
		opcode |= (a1.a_type & TMREG) << 4;
		outrx(opcode, &a2);
		break;

	case TBRX:
		getaddr_rx(&a1);
		outrx(opcode, &a1);
		break;

	case TRX0:
		getaddr_rx(&a1);
		opcode <<= 8;
		outrx(opcode, &a1);
		break;

	case TRI1:
		getaddr_reg(&a1);
		comma();
		getaddr_ir(&a2, 16);
		opcode <<= 8;
		opcode |= a1.a_value << 4;
		opcode |= a2.a_type & TMREG;
		outaw(opcode);
		outraw(&a2);
		break;

	case TRI2:
		getaddr_reg(&a1);
		comma();
		getaddr_ir(&a2, 32);
		opcode <<= 8;
		opcode |= a1.a_value << 4;
		opcode |= a2.a_type & TMREG;
		outaw(opcode);
		outrad(&a2);
		break;


	/* TWDCS TRDCS */
	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}
