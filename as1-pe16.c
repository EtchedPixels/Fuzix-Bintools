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

	expr1(ap, LOPRI, 0);
	istuser(ap);
	if (ap->a_value < 0 || ap->a_value > 15 || ap->a_segment == UNKNOWN)
		qerr(REGINVALID);
}

void getaddr_sf(ADDR *ap)
{
	/* There are syntaxs using r0-r15, 0-15 etc. We use r as it's
	   harder to screw up with an explicit r */
	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_sym = NULL;

	expr1(ap, LOPRI, 0);
	istuser(ap);
	if (ap->a_value < 0 || ap->a_value > 15 || ap->a_segment == UNKNOWN)
		qerr(SFINVALID);
}

void getaddr_rx(ADDR *ap)
{
	ADDR r;
	unsigned c;

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_sym = NULL;

	/* The format is addr or addr(reg) */
	expr1(ap, LOPRI, 0);
	c = getnb();
	if (c != '(') {
		ap->a_value &= ~(TMREG|TMADDR);
		return;
	}
	getaddr_reg(&r);
	if (r.a_value == 0)
		qerr(NO0INDEX);
	ap->a_type |= TINDEX | r.a_value;
	c = getnb();
	if (c != ')')
		qerr(SYNTAX_ERROR);
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

	case TIMPL:
		/* There are no real implicit ops - these are aliases */
		outaw(opcode);
		break;
	case TIMPLZ:
		/* There are no real implicit ops - these are aliases */
		outaw(opcode);
		outaw(0);
		break;
	case TRR:
		getaddr_reg(&a1);
		comma();
		getaddr_reg(&a2);
		opcode <<= 8;
		opcode |= a1.a_value << 4;
		opcode |= a2.a_value;
		outaw(opcode);
		break;
	case TRRL:
		getaddr_reg(&a1);
		comma();
		getaddr_reg(&a2);
		check_pair(&a1);
		opcode <<= 8;
		opcode |= a1.a_value << 4;
		opcode |= a2.a_value;
		outaw(opcode);
		break;
	case TRRF:
		getaddr_reg(&a1);
		comma();
		getaddr_reg(&a2);
		check_pair(&a1);
		check_pair(&a2);
		opcode <<= 8;
		opcode |= a1.a_value << 4;
		opcode |= a2.a_value;
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
	case TRI:
	case TRX:
		getaddr_reg(&a1);
		comma();
		getaddr_rx(&a2);
		opcode <<= 8;
		opcode |= a1.a_value << 4;
		opcode |= a2.a_type & TMREG;
		outaw(opcode);
		outraw(&a2);
		break;
	case TRIL:
	case TRXL:
		getaddr_reg(&a1);
		check_pair(&a1);
		comma();
		getaddr_rx(&a2);
		opcode <<= 8;
		opcode |= a1.a_value << 4;
		opcode |= a2.a_type & TMREG;
		outaw(opcode);
		outraw(&a2);
		break;
	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}
