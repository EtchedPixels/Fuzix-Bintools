/*
 * 80x96 assembler.
 * Assemble one line of input.
 * Knows all the dirt.
 */
#include	"as.h"


/*
 * CPU specific pass setup
 */

static int cputype;
/* FIXME: we should malloc/realloc this on non 8bit machines */
static uint8_t reltab[1024];
static unsigned int nextrel;

int passbegin(int pass)
{
	cputype = 6800;
	segment = 1;		/* Default to code */
	if (pass == 3)
		nextrel = 0;
	return 1;		/* All passes required */
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

/*
 * In some cases (JSR JMP and definitions - eg .word)
 * $ABCD means a constant everywhere else that is #ABCD
 */

static void constify(ADDR *ap)
{
	if ((ap->a_type & TMMODE) == (TUSER|TMINDIR))
		ap->a_type = TUSER;
}

/* Handle the corner case of labels in direct page being used as relative
   branches from the overlapping 'absolute' space */
static int segment_incompatible(ADDR *ap)
{
	if (ap->a_segment == segment)
		return 0;
	if (ap->a_segment == 4 && segment == 0 && ap->a_value < 256)
		return 0;
	return 1;
}

/* Can we make a reference via direct page ? */
static unsigned is_reg(ADDR *ap)
{
	if (ap->a_segment == ZP)
		return 1;
	if (ap->a_segment == ABSOLUTE && ap->a_value < 256)
		return 1;
	return 0;
}

static void parse_indirect(ADDR *ap)
{
	ADDR a2;
	int c;

	expr1(a2, LOPRI, 0);
	if ((a2.a_type & TMMODE) == TBR) {
		ap->a_type &= ~(TMMODE|TMREG);
		ap->a_type |= TBR | TMINDIR;
		ap->a_type |= (a2.a_type & TMREG);
	}
	else if ((a2.a_type & TMMODE) == TWR) {
		ap->a_type &= ~(TMMODE|TMREG);
		ap->a_type |= TWR | TMINDIR;
		ap->a_type |= (a2.a_type & TMREG);
		/* TODO: check and disallow odd */
	} else
		aerr(REG_REQUIRED);
	c = getnb();
	if (c != ']')
		aerr(BRKT_EXPECTED);
	c = getnb();
	if (c == '+')
		ap->a_type |= TMAUTOINC;
	else
		unget(c);
}
	
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
	int dp = 0;

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
		constify(ap);
		istuser(ap);
		ap->a_type |= TIMMED;
		return;
	}
	if (c == '[') {
		parse_indirect(ap);
		return;
	}
	expr1(ap, LOPRI, 1);
	c = getnb();
	if (c == '[') {
		parse_indirect();
		return;
	}
	unget(c);
	ap->a_type = TUSER;
	if (is_reg(ap))
		ap->a_type |= TDIRECT;
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
	int disp;
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
		/* Pass 0 we compute the worst cases
		   Pass 1 we generate according to those 
		   Pass 2 we set them in stone (the shrinkage from pass 1
					        allowing us a lot more)
		   Pass 3 we output accodingly */
		if (pass == 0) {
			/* Catch duplicates on phase 0 */
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
			/* Phase 2 defined the values so a misalignment here
			   is fatal */
			if ((sp->s_type&TMMDF) != 0)
				err('m', MULTIPLE_DEFS);
			if (sp->s_value != dot[segment]) {
				err('p', PHASE_ERROR);
			}
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
		/* TODO: double check this logic and test validity */
		/* On pass 1 we expect to see ourself in the mirror, jsut
		   update the value */
		if (pass != 1) {
			if ((sp->s_type&TMMODE) != TNEW
			&&  (sp->s_type&TMASG) == 0)
				err('m', MULTIPLE_DEFS);
		}
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
		segment = ABSOLUTE;
		dot[segment] = a1.a_value;
		/* Tell the binary generator we've got a new absolute
		   segment. */
		outabsolute(a1.a_value);
		break;

	case TEXPORT:
		getid(id, getnb());
		sp = lookup(id, uhash, 1);
		/* FIXME: make a new common error, and push to other ports */
		if (((sp->s_type & TMMODE) == TNEW) && pass == 3)
			aerr(ADDR_REQUIRED);
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
	case TREG:
		wreg(&a1);
		outab(opcode);
		outab(a1.a_type & TMREG);
		break;	
	case TREGB:
		breg(&a1);
		outab(opcode);
		outab(a1.a_type & TMREG);
		break;	
	case TREGL:
		lreg(&a1);
		outab(opcode);
		outab(a1.a_type & TMREG);
		break;	
	case T_SH:
		/* Funny rule where 0-15 is shift, 16+ is register */
	case T_SHB:
	case T_SHL:
	case T_NORM:
		lreg(&a1);
		comma();
		breg(&a2);
		outab(opcode);
		outab(a2.a_type & TMREG);
		outab(a1.a_type & TMREG);
		break;
	case TXCH:
		getaddr(&a1);
		/* Allowed direct or indexed only */
	case TXCHB:
		getaddr(&a1);
		/* Allowed direct or indexed only */
	case TSHORT:
		/* 11bit top merged with opcode */
	case TJBIT:
		/* two or 3 bit ops */
	case TOP23:
	case TOP23B:
	case TWOOP:
		wreg(&a1);
		comma();
		getaddr(&a2);
		encode_aop(opcode, &a1, &a2);
		break;
	case TWOOPB:
		breg(&a1);
		comma();
		getaddr(&a2);
		encode_aop(opcode, &a1, &a2);
		break;
	case TSTORE:
		wreg(&a1);
		comma();
		getaddr(&a2);
		if (a2.a_type & TIMMED)
			aerr(BAD_ADDR_MODE);
		encode_aop(opcode, &a1, &a2);
		break;
	case TSTOREB:
		breg(&a1);
		comma();
		getaddr(&a2);
		if (a2.a_type & TIMMED)
			aerr(BAD_ADDR_MODE);
		encode_aop(opcode, &a1, &a2);
		break;
	case TBMOV:
	case TWOOPL:
		lreg(&a1);
		comma();
		getaddr(&a2);
		encode_aop(opcode, &a1, &a2);
		break;
	case TONE:
		getaddr(&a2);
		encode_aop(opcode, NULL, &a2);
		break;
	case TDONE:
		getaddr(&a2);
		if (a2.a_type & TIMMED)
			aerr(BAD_ADDR_MODE);
		encode_aop(opcode, NULL, &a2);
		break;
	case TDJNZ:
		breg(&a1);
		comma();
		getaddr(&a2);
		/* FIXME: do wo need to check this is constant ? */
		disp = a2.a_value - dot[segment]-2;
		/* Only on pass 3 do we know the correct offset for a forward branch
		   to a label where other stuff with Jcc has been compacted */
		if (pass == 3 && (disp<-128 || disp>127 || segment_incompatible(&a2)))
			aerr(BRA_RANGE);
		outab(opcode);
		outab(disp);
		break;
	case TDJNZW:
		wreg(&a1);
		comma();
		getaddr(&a2);
		/* FIXME: do wo need to check this is constant ? */
		disp = a2.a_value - dot[segment]-2;
		/* Only on pass 3 do we know the correct offset for a forward branch
		   to a label where other stuff with Jcc has been compacted */
		if (pass == 3 && (disp<-128 || disp>127 || segment_incompatible(&a2)))
			aerr(BRA_RANGE);
		outab(opcode);
		outab(disp);
		break;
	case TJCC:
		getaddr(&a1);
		/* FIXME: do wo need to check this is constant ? */
		disp = a1.a_value - dot[segment]-2;
		/* Only on pass 3 do we know the correct offset for a forward branch
		   to a label where other stuff with Jcc has been compacted */
		if (pass == 3 && (disp<-128 || disp>127 || segment_incompatible(&a1)))
			aerr(BRA_RANGE);
		outab(opcode);
		outab(disp);
		break;
	case TBR:
		c = getnb();
		if (c != '[')
			aerr(INDIR_ONLY);
		wreg(&a1);
		c = getnb();
		if (c != ']')
			aerr(BRKT_EXPECTED);
		c = getnb();
		if (c == '+')
			aerr(INDIR_ONLY);
		else
			unget(c);
		outab(opcode);
		outab(a1.a_type & TMREG);
		break;
	case TIMM16:
		getaddr(&a1);
		outarelaw(&a1);	/* 16bit PC relative */
	case TMUL:
		if (opcode & 0xFF00)
			outab(opcode >> 8);
		outab(opcode);
	case TDIV:
		if (opcode & 0xFF00)
			outab(opcode >> 8);
		outab(opcode);
	case TMULB:
		if (opcode & 0xFF00)
			outab(opcode >> 8);
		outab(opcode);
	case TDIVB:
		if (opcode & 0xFF00)
			outab(opcode >> 8);
		outab(opcode);

	case TBRA16:	/* Relative branch or reverse and jump for range */

		/* Algorithm:
			Pass 0: generate worst case code. We then know things
				that can safely be turned short because more
				shortening will only reduce gap
			Pass 1: generate code case based upon pass 0 but now
				using short branch conditionals
			Pass 2: repeat this because pass 1 causes a lot of
				collapses. Pin down the choices we made.
			Pass 3: generate code. We don't "fix" any further
				possible shortenings because we need the
				addresses in pass 3 to exactly match pass 2
		*/
		getaddr(&a1);
		/* disp may change between pass1 and pass2 but we know it won't
		   get bigger so we can be sure that we still fit the 8bit disp
		   in pass 2 if we did in pass 1 */
		disp = a1.a_value - dot[segment] - 2;
		/* For pass 0 assume the worst case. Then we optimize on
		   pass 1 when we know what may be possible */
		if (pass == 3)
			c = getnextrel();
		else {
			c = 0;
			/* Cases we know it goes big */
			if (pass == 0 || segment_incompatible(&a1) || disp < -128 || disp > 127)
				c = 1;
			/* On pass 2 we lock down our choices in the table */
			if (pass == 2)
				setnextrel(c);
		}
		if (c) {
			outab(opcode^1);	/* Inverted branch */
			outab(3);		/* Skip over the jump */
			outab(0x7E);		/* Jump */
			outraw(&a1);
		} else {
			outab(opcode);
			/* Should never happen */
			if (disp < -128 || disp > 127)
				aerr(BRA_RANGE);
			outab(disp);
		}
		break;

	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}
