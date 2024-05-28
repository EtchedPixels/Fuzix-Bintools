/*
 * 6303 assembler.
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

static void constant_to_zp(ADDR *ap, int dp)
{
	/* dp means we wrote @foo meaning we want a DP reference and the symbol
	   must be direct page */
	if (dp) {
		if (ap->a_segment != ABSOLUTE && ap->a_segment != ZP && ap->a_segment != UNKNOWN)
			qerr(MUST_BE_ABSOLUTE);
		if (ap->a_value > 255)
			aerr(CONSTANT_RANGE);
		/* Preserve constants and don't relocate them */
		if (ap->a_segment != ABSOLUTE)
			ap->a_segment = ZP;
	}
}

/* Handle the corner case of labels in direct page being used as relative
   branches from the overlapping 'absolute' space */
static int segment_incompatible(ADDR *ap)
{
	if (ap->a_segment == segment)
		return 0;
	if (ap->a_segment == ZP && segment == 0 && ap->a_value < 256)
		return 0;
	return 1;
}

/* Can we make a reference via direct page ? */
static unsigned can_dp(ADDR *ap)
{
	if (ap->a_segment == ZP)
		return 1;
	if (ap->a_segment == ABSOLUTE && ap->a_value < 256)
		return 1;
	return 0;
}

static unsigned index_size(ADDR *ap)
{
	/* Direct page will always resolve to 1 byte */
	if (ap->a_segment == ZP)
		return 1;
	/* These might resolve to big values at link time, for example it's common to
	   lda array,x for indexing */
	if (ap->a_segment != ABSOLUTE || ap->a_sym)
		return 2;
	if (ap->a_value > 255)
		return 2;
	if (ap->a_value == 0)
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
 * TODO: 68HC08 modes
 *	n,x+
 *	x+  (same as ,x+)
 *	n,s{p}
 */
void getaddr(ADDR *ap)
{
	int c, nc;
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

	/* :foo */
	/* Our own syntax for DP form labels */
	if (c == '@')
		dp = 1;
	/* X+ is an allowed 68HC08 form for ,X+ but not X for ,X */
	else if (c == 'x') {
		nc = getnb();
		if (nc == '+') {
			ap->a_type = TXPLUS | TUSER;
			ap->a_value = 0;
			ap->a_segment = ABSOLUTE;
			return;
		}
		unget(nc);
	}
	/* Allow naked ",x"  and ",s{p}" form */
	else if (c == ',') {
		c = getnb();
		if (c == 's') {
			nc = getnb();
			if (nc != 'p')
				unget(nc);
			ap->a_type = TSP | TUSER;
			ap->a_value = 0;
			ap->a_segment = ABSOLUTE;
			return;
		}
		if (c != 'x')
			aerr(SYNTAX_ERROR);
		c = getnb();
		if (c == '+') {	
			ap->a_type = TXPLUS | TUSER;
			ap->a_value = 0;
			ap->a_segment = ABSOLUTE;
			return;
		}
		unget(c);
		ap->a_type = TINDEX | TUSER;
		ap->a_value = 0;
		ap->a_segment = ABSOLUTE;
		return;
	} else {
		if (c == '<')
			ap->a_flags |= A_LOW;
		else if (c == '>')
			ap->a_flags |= A_HIGH;
		else
			unget(c);
	}

	expr1(ap, LOPRI, 1);

	c = getnb();
	
	/* foo,[x|sp] */
	if (c == ',') {
		c = getnb();
		if (c == 'x') {
			istuser(ap);
			constify(ap);
			nc = getnb();
			if (nc == '+')
				ap->a_type = TXPLUS;
			else {
				unget(nc);
				ap->a_type |= TINDEX;
			}
			return;
		}
		if (c == 's') {
			/* Accept S or SP */
			nc = getnb();
			if (nc != 'p')
				unget(nc);
			istuser(ap);
			constify(ap);
			ap->a_type |= TSP;
			return;
		}
		unget(c);
	}
	unget(c);
	constant_to_zp(ap, dp);
	if (can_dp(ap))
		ap->a_type = TDIRECT|TUSER|TMINDIR;
	else
		ap->a_type = TUSER|TMINDIR;
}

static void need_hc08(void)
{	
	if (cputype < 6808)
		qerr(BADCPU);
	if (pass == 3)
		cpu_flags |= OA_6805_HC08;
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
	unsigned ta1, ta2;

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

	case TSETCPU:
		cputype = opcode;
		break;

	case T08IMPL:
		need_hc08();
		/* Fall through */
	case TIMPL:
		outab(opcode);
		break;

	case TBIT:
		getaddr(&a1);
		comma();
		getaddr(&a2);
		constify(&a1);
		istuser(&a1);
		if (a1.a_value > 7)
			aerr(CONSTANT_RANGE);
		if ((a2.a_type & TMADDR) != TDIRECT)
			aerr(BADMODE);
		outab(opcode + 2 * a1.a_value);
		outrab(&a2);
		break;

	case TBITREL:
		getaddr(&a1);
		comma();
		getaddr(&a2);
		constify(&a1);
		istuser(&a1);
		if (a1.a_value > 7)
			aerr(CONSTANT_RANGE);
		if ((a2.a_type & TMADDR) != TDIRECT)
			aerr(BADMODE);
		outab(opcode + 2 * a1.a_value);
		outrab(&a2);
		comma();
		getaddr(&a1);
		/* FIXME: do wo need to check this is constant ? */
		disp = a1.a_value - dot[segment]-1;
		/* Only on pass 3 do we know the correct offset for a forward branch
		   to a label where other stuff with Jcc has been compacted */
		if (pass == 3 && (disp<-128 || disp>127 || segment_incompatible(&a1)))
			aerr(BRA_RANGE);
		outab(disp);
		break;

	case T08REL8:
		need_hc08();
		/* Fall through */
	case TREL8:
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

	case T08REL:
		need_hc08();
		/* Fall through */
	case TREL:
		/* Relative branch or reverse and jump for range */

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
			outab(0xCC);		/* Jump */
			outraw(&a1);
		} else {
			outab(opcode);
			/* Should never happen */
			if (disp < -128 || disp > 127)
				aerr(BRA_RANGE);
			outab(disp);
		}
		break;
	
	case TMEM:
		getaddr(&a1);
		switch(a1.a_type & TMADDR) {
		case TIMMED:
			outab(opcode | 0xA0);
			outrab(&a1);
			break;
		case TDIRECT:
			outab(opcode | 0xB0);
			outrab(&a1);
			break;
		case TINDEX:
			switch(index_size(&a1)) {
			case 0:
				outab(opcode | 0xF0);
				break;
			case 1:
				outab(opcode | 0xE0);
				outrab(&a1);
				break;
			case 2:
				outab(opcode | 0xD0);
				outraw(&a1);
				break;
			}
			break;
		case TSP:
			need_hc08();
			switch(index_size(&a1)) {
			case 0:
			case 1:
				outab(0x9E);
				outab(opcode | 0xE0);
				outrab(&a1);
				break;
			case 2:
				outab(0x9E);
				outab(opcode | 0xD0);
				outraw(&a1);
				break;
			}
			break;
		case 0:
			outab(opcode | 0xC0);
			outraw(&a1);
			break;
		default:
			aerr(BADMODE);
			break;
		}
		break;
	case TJMP:
		getaddr(&a1);
		switch(a1.a_type & TMADDR) {
		case TIMMED:
			aerr(ADDR_REQUIRED);
			break;
		case TDIRECT:
			outab(opcode | 0xB0);
			outrab(&a1);
			break;
		case TINDEX:
			switch(index_size(&a1)) {
			case 0:
				outab(opcode | 0xF0);
				break;
			case 1:
				outab(opcode | 0xE0);
				outrab(&a1);
				break;
			case 2:
				outab(opcode | 0xD0);
				outraw(&a1);
				break;
			}
			break;
		case 0:
			outab(opcode | 0xC0);
			outraw(&a1);
			break;
		/* No SP forms on jmp/jsr */
		default:
			aerr(BADMODE);
			break;
		}
		break;
	case TMEMNI:
		getaddr(&a1);
		switch(a1.a_type & TMADDR) {
		case TIMMED:
			aerr(ADDR_REQUIRED);
			break;
		case TDIRECT:
			outab(opcode | 0xB0);
			outrab(&a1);
			break;
		case TINDEX:
			switch(index_size(&a1)) {
			case 0:
				outab(opcode | 0xF0);
				break;
			case 1:
				outab(opcode | 0xE0);
				outrab(&a1);
				break;
			case 2:
				outab(opcode | 0xD0);
				outraw(&a1);
				break;
			}
			break;
		case 0:
			outab(opcode | 0xC0);
			outraw(&a1);
			break;
		case TSP:
			need_hc08();
			switch(index_size(&a1)) {
			case 0:
			case 1:
				outab(0x9E);
				outab(opcode | 0xE0);
				outrab(&a1);
				break;
			case 2:
				outab(0x9E);
				outab(opcode | 0xD0);
				outraw(&a1);
				break;
			}
			break;
		default:
			aerr(BADMODE);
			break;
		}
		break;
	case TRMW:
		/* the lsla etc forms appear as implicit */
		getaddr(&a1);
		switch(a1.a_type & TMADDR) {
		case TDIRECT:
			outab(opcode | 0x30);
			outrab(&a1);
			break;
		case TINDEX:
			switch(index_size(&a1)) {
			case 0:
				outab(opcode | 0x70);
				break;
			case 1:
				outab(opcode | 0x60);
				outrab(&a1);
				break;
			case 2:
				aerr(CONSTANT_RANGE);
				break;
			}
			break;
		case TSP:
			need_hc08();
			switch(index_size(&a1)) {
			case 0:
			case 1:
				outab(0x9E);
				outab(opcode | 0x60);
				outrab(&a1);
				break;
			case 2:
				aerr(CONSTANT_RANGE);
				break;
			}
			break;
		default:
			aerr(BADMODE);
		}
		break;
	case TIMM8:
		need_hc08();
		getaddr(&a1);
		if ((a1.a_type & TMADDR) != TIMMED)
			aerr(BADMODE);
		if (a1.a_value > 127 || ((int16_t)a1.a_value) < -128)
			aerr(CONSTANT_RANGE);
		outrab(&a1);
		break;
	case TIM16:
		need_hc08();
		getaddr(&a1);
		switch(a1.a_type & TMADDR) {
		case TDIRECT:
			outab(opcode + 0x10);
			outrab(&a1);
			break;
		case TIMMED:
			outab(opcode);
			outraw(&a1);
			break;
		default:
			aerr(BADMODE);
		}
		break;
	case TDIR:
		need_hc08();
		getaddr(&a1);
		if ((a1.a_type & TMADDR) != TDIRECT)
			aerr(BADMODE);
		outrab(&a1);
		break;
	case TCBEQR:
		need_hc08();
		getaddr(&a1);
		comma();
		getaddr(&a2);
		if ((a1.a_type & TMADDR) != TIMMED)
			aerr(BADMODE);
		/* TODO: do we need this check on bra etc */
//		if ((a2.a_type & TMMODE) != TUSER)
//			aerr(BADMODE);
		disp = a2.a_value - dot[segment]-3;
		/* Only on pass 3 do we know the correct offset for a forward branch
		   to a label where other stuff with Jcc has been compacted */
		if (pass == 3 && (disp<-128 || disp>127 || segment_incompatible(&a2)))
			aerr(BRA_RANGE);
		outab(opcode);
		outrab(&a1);
		outab(disp);
		break;
	case TCBEQ:
		need_hc08();
		getaddr(&a1);
		comma();
		getaddr(&a2);
		/* TODO: check a2 is valid rel */
		ta1 = a1.a_type & TMADDR;
		if (ta1 == TXPLUS) {
			switch(index_size(&a1)) {
			case 0:
				outab(opcode | 0x70);
				break;
			case 1:
				outab(opcode | 0x60);
				outrab(&a1);
				break;
			default:
				aerr(CONSTANT_RANGE);
				break;
			}
		} else if (ta1 == TSP) {
			switch(index_size(&a1)) {
			case 0:
			case 1:
				outab(0x9E);
				outab(opcode | 0x60);
				outrab(&a1);
				break;
			default:
				aerr(CONSTANT_RANGE);
				break;
			}
		} else if (ta1 == TDIRECT) {
			outab(opcode | 0x31);
			outrab(&a1);
		/* TODO: sp forms */
		} else
			aerr(BADMODE);
		disp = a2.a_value - dot[segment] - 1;
		/* Only on pass 3 do we know the correct offset for a forward branch
		   to a label where other stuff with Jcc has been compacted */
		if (pass == 3 && (disp<-128 || disp>127 || segment_incompatible(&a2)))
			aerr(BRA_RANGE);
		outab(disp);
		break;
	case TDBNZ:
		need_hc08();
		getaddr(&a1);
		comma();
		getaddr(&a2);
//		if ((a2.a_type & TMMODE) != TUSER)
//			aerr(BADMODE);
		switch(a1.a_type & TMADDR) {
		case TDIRECT:
			outab(opcode | 0x30);
			outrab(&a1);
			break;
		case TINDEX:
			switch(index_size(&a1)) {
			case 0:
				outab(opcode | 0x70);
				break;
			case 1:
				outab(opcode | 0x6B);
				outrab(&a1);
				break;
			case 2:
				aerr(CONSTANT_RANGE);
				break;
			}
			break;
		/* TODO sp mode */
		default:
			aerr(BADMODE);
		}
		disp = a2.a_value - dot[segment] - 1;
		/* Only on pass 3 do we know the correct offset for a forward branch
		   to a label where other stuff with Jcc has been compacted */
		if (pass == 3 && (disp<-128 || disp>127 || segment_incompatible(&a2)))
			aerr(BRA_RANGE);
		outab(disp);
		break;
	case TMOV:
		need_hc08();
		getaddr(&a1);
		comma();
		getaddr(&a2);
		ta1 = a1.a_type & TMADDR;
		ta2 = a2.a_type & TMADDR;
		/* mov x+,@d */
		if (ta1 == TXPLUS && ta2 == TDIRECT) {
			if (a1.a_value)
				aerr(CONSTANT_RANGE);
			outab(0x7E);
			outrab(&a2);
			break;
		}
		/* mov @d,x+ */
		if (ta1 == TDIRECT && ta2 == TXPLUS) {
			if (a2.a_value)
				aerr(CONSTANT_RANGE);
			outab(0x5E);
			outrab(&a1);
			break;
		}
		/* mov d,d */
		if (ta1 == TDIRECT && ta2 == TDIRECT) {
			outab(0x4E);
			outrab(&a1);
			outrab(&a2);
			break;
		}
		if (ta2 == TDIRECT && ta1 == TIMMED) {
			outab(0x6E);
			outrab(&a1);
			outrab(&a2);
			break;
		}
		aerr(BADMODE);
		break;
	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}
