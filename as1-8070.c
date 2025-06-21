/*
 * INS8070 assembler.
 *
 * The one oddity with this target is that almost all instructions
 * expect a default of a P0 (PC) relative value. So for example
 * 47 actually means "PC + 47". We thus have two getaddr forms because
 * things like .word do not expect PC relative default.
 *
 * TODO:
 *	- How to handle direct page encodings. DP is at FFxx not 00xx
 *	  like other targets. May need a linker tweak ?
 *	- Review all ops and forms
 *	- Check all other forms error
 */
#include	"as.h"


/*
 * CPU specific pass setup
 */

/* FIXME: we should malloc/realloc this on non 8bit machines */
static uint8_t reltab[1024];
static unsigned int nextrel;

int passbegin(int pass)
{
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

static void a_required(ADDR *ap)
{
	if (ap->a_type != (TBR | A))
		aerr(AREQUIRED);
}

static void index_required(ADDR *ap)
{
	if ((ap->a_type & TMMODE) != TWR || (ap->a_type & TMREG) == EA)
		aerr(POINTER_REQ);
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
 * Permitted forms
 *
 *	#constant	constant		(NS use =const)
 *	:dp		direct page
 *	a/ea/p1/etc	register
 *	addr,p		index register offset
 *	@addr,p		index register offset auto-index
 *	addr,@p		ditto but implied sign is different TODO
 *
 *	There is no immediate addressing except for the direct page
 *	JMP and JSR appear to do so but in fact are really aliases
 *	of constant load and pli
 *
 *	TODO:
 *	-	addr,@p form
 *	-	when using ,p0 (or implied) generate PC relative relocations
 *		for symbols
 */
void getaddr_i(ADDR *ap)
{
	int c;
	unsigned dp = 0;
	unsigned con = 0;
	unsigned autoi = 0;
	unsigned m;
	ADDR a2;

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_sym = NULL;

	a2.a_type = 0;
	a2.a_flags = 0;
	a2.a_sym = NULL;

	c = getnb();

	/* =foo also allow #foo for more consistency with other targets */
	if (c == '#' || c == '=')
		con = 1;
	/* :foo */
	/* Our own syntax for DP form labels */
	else if (c == ':')
		dp = 1;
	/* Auto-index tag */
	else if (c == '@')
		autoi = 1;
	else
		unget(c);

	c = getnb();
	if (c == '<')
		ap->a_flags |= A_LOW;
	else if (c == '>')
		ap->a_flags |= A_HIGH;
	else
		unget(c);

	expr1(ap, LOPRI, 0);
	m = ap->a_type & TMMODE;
	if (con) {
		/* Constant */
		if (m != TUSER)
			qerr(BADMODE);
		ap->a_type |= TIMMED;
		return;
	}
	if (dp) {
		if (m != TUSER)
			qerr(BADMODE);
		ap->a_type |= TDIRECT;
		return;
	}
	/* Check for register */
	if (m == TBR || m == TWR) {
		if (dp || autoi || con)
			qerr(BADMODE);
		return;
	}
	c = getnb();
	if (c != ',') {
		unget(c);
		/* Reg 0 relative (ie PC relative) */
		ap->a_type |= TINDEX;
		ap->a_type &= ~TMREG;
		ap->a_type &= ~TMMODE;
		ap->a_type |= TWR;
		return;
	}
	/* Now an index register */
	expr1(&a2, LOPRI, 0);
	index_required(&a2);
	ap->a_type &= ~TMREG;
	ap->a_type |= a2.a_type & TMREG;
	ap->a_type &= ~TMADDR;
	if (autoi)
		ap->a_type |= TAUTOINDEX;
	else
		ap->a_type |= TINDEX;
}

/*
 *	For classic addresses rather than the PC relative implied
 *	that are used for most instruction forms
 */
void getaddr(ADDR *ap)
{
	int c;

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

/*
 *	Turn a pointer relative address into a relocation byte
 *	Must be called after the opcode is written but before anything
 *	else, so that dot is where it is expected
 */

void outptr(ADDR *ap)
{
	unsigned r = ap->a_type & TMREG;
	int off;
	/* Simple case - not PC relative */
	if (r != 0) {
		outrab(ap);
		return;
	}
	/* We are PC relative */
	/* Not a symbol so go with the value given as relative */
	if (ap->a_sym == NULL) {
		if (ap->a_segment != segment)
			aerr(RANGE);
		off = 0;
	} else {
		/* If it's another segment you lose */
		if (ap->a_sym->s_segment != segment)
			aerr(RANGE);
		/* Work out the offset from the symbol value */
		off = ap->a_sym->s_value;
	}
	off += ap->a_value;
	off -= dot[segment];
	if (off < -128 || off > 127)
		aerr(RANGE);
	outab(off);
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
	unsigned r, r2;
	unsigned mode;

loop:
	if ((c=getnb())=='\n' || c==';')
		return;
	if (is_symstart(c) == 0 && c != '.')
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
			/* Phase 1 defined the values so a misalignment here
			   is fatal */
			if ((sp->s_type&TMMDF) != 0)
				err('m', MULTIPLE_DEFS);
			if (sp->s_value != dot[segment]) {
				printf("Phase 2: Dot %x Should be %x\n",
					dot[segment], sp->s_value);
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
		/* On pass 1 we expect to see ourself in the mirror, just
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

	case TADDR:	/* Write value - 1 */
		do {
			getaddr(&a1);
			constify(&a1);
			istuser(&a1);
			a1.a_value--;
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

	/*	opcode is fixed */
	case TIMPL:
		outab(opcode);
		break;
	/* Has a surplus but required A register name */
	case TAONLY:
		getaddr_i(&a1);
		if ((a1.a_type & TMMODE) != TBR ||
			(a1.a_type & TMREG) != A)
			aerr(ADDR_REQUIRED);
		outab(opcode);
		break;
	/* Can be used with A or EA */
	case TAEA:
		getaddr_i(&a1);
		mode = a1.a_type & TMMODE;
		r = a1.a_type & TMREG;
		if (mode == TBR && r == A)
			outab(opcode >> 8);
		else if (mode == TWR && r == EA)
			outab(opcode);
		else
			aerr(AEAREQUIRED);
		break;
	/* Mandatory but implied EA,T */
	case TEAT:
		getaddr_i(&a1);
		comma();
		getaddr_i(&a2);
		/* Maybe add some more clearer errors ? */
		if ((a1.a_type & (TMMODE|TMREG)) != (TWR|EA))
			aerr(BADMODE);
		if ((a2.a_type & (TMMODE|TMREG)) != (TWR|T))
			aerr(BADMODE);
		outab(opcode);
		break;
	/* Stack - A EA P2 or P3 */
	case TSTACK:
		getaddr_i(&a1);
		mode = a1.a_type & TMMODE;
		r = a1.a_type & TMREG;
		if (mode == TBR && r == A)
			outab(opcode ^ 2);
		else if (mode == TWR) {
			switch(a1.a_type & TMREG) {
			case EA:
				outab(opcode);
				break;
			case P2:
				outab((opcode >> 8) | 2);
				break;
			case P3:
				outab((opcode >> 8) | 3);
				break;
			default:
				aerr(BADREG);
			}
		} else
			aerr(BADREG);
		break;
	/* Special */
	case TCALL:
		getaddr(&a1);
		constify(&a1);
		/* FIXME; check constant enforcement */
		if ((a1.a_type & TMMODE) != TUSER || a1.a_value > 15 || a1.a_value < 0)
			aerr(RANGE);
		outab(opcode | a1.a_value);
		break;
	/* Exchanges */
	case TXCH:
		getaddr_i(&a1);
		mode = a1.a_type & TMMODE;
		if (getnb() != ',')
			aerr(MISSING_COMMA);
		getaddr_i(&a2);

		r = a1.a_type & TMREG;
		r2 = a2.a_type & TMREG;

		/* Byte with byte, word with word */
		if ((a2.a_type & TMMODE) != mode)
			aerr(BADREG);

		/* Swap so we only have to check one order */
		if (r > r2) {
			unsigned tmp = r;
			r = r2;
			r2 = tmp;
		}
		if (r == A && r2 == E) {
			outab(0x01);
			break;
		} else  if (r <= P3 && r2 == EA) {
			outab(0x4C + r);
			break;
		}
		aerr(BADREG);
		break;
	case TMEM8:
		/* DLD A, addr */
		/* ILD A, addr */
		getaddr_i(&a1);
		if (getnb() != ',') {
			aerr(MISSING_COMMA);
			break;
		}
		getaddr_i(&a2);
		a_required(&a1);
		/* This can be indexed or autoindexed but must be a word
		   register, or it can be direct */
		switch(a2.a_type & TMADDR) {
		case TINDEX:
			outab(opcode | (a2.a_type & TMREG));
			outptr(&a2);
			break;
		case TAUTOINDEX:
			outab(opcode | (a2.a_type & TMREG) | 4);
			outptr(&a2);
			break;
		case TDIRECT:
			outab(opcode | 5);
			outrab(&a2);
			break;
		default:
			/* No =foo form etc */
			aerr(BADMODE);
			break;
		}
		break;

		/* 8bit maths and logic */
	case TLOGIC:
		getaddr_i(&a1);
		comma();
		getaddr_i(&a2);
		if ((a1.a_type & TMMODE) != TBR)
			aerr(BADMODE);
		r = a1.a_type & TMREG;
		if (r != A && r != S)
			aerr(BADREG);
		if (r == S) {
			opcode >>= 8;
			/* Special cases */
			if (opcode && (a2.a_type & TMMODE) == TUSER) {
				outab(opcode);
				outrab(&a2);
				break;
			}
			aerr(BADREG);
			break;
		}
		/* Handle the special case of A, E */
		if(r == A && (a2.a_type & (TMMODE|TMREG)) == (TBR|E)) {
			outab(opcode & 0x7F);
			break;
		}
		switch(a2.a_type & TMADDR) {
		case TIMMED:
			outab(opcode + 4);
			outrab(&a2);
			break;
		case TDIRECT:
			outab(opcode + 5);
			outrab(&a2);	/* Need to look at linker */
			break;
		case TINDEX:
			outab(opcode | (a2.a_type & TMREG));
			outptr(&a2);
			break;
		case  TAUTOINDEX:
			outab((opcode | (a2.a_type & TMREG))  + 4);
			outptr(&a2);
			break;
		default:
			aerr(BADMODE);
		}
		break;
		/* Same idea for byte/word operations - uses EA */
	case TLOGIC16:
		/* Like tlogic but with EA forms and no S forms */
		getaddr_i(&a1);
		comma();
		getaddr_i(&a2);
		mode = a1.a_type & TMMODE;
		r = a1.a_type & TMREG;

		if (mode != TBR && mode != TWR)
			aerr(BADMODE);
		if (r == A)
			opcode >>= 8;
		else if (r != EA)
			aerr(BADREG);
		if(r == A && (a2.a_type & (TMMODE|TMREG)) == (TBR|E)) {
			outab(opcode & 0x7F);
			break;
		}
		switch(a2.a_type & TMADDR) {
		case TIMMED:
			outab(opcode | 4);
			if (r == A)
				outrab(&a2);
			else
				outraw(&a2);
			break;
		case TDIRECT:
			outab(opcode + 5);
			outrab(&a2);
			break;
		case TINDEX:
			outab(opcode | (a2.a_type & TMREG));
			outptr(&a2);
			break;
		case TAUTOINDEX:
			outab((opcode | (a2.a_type & TMREG))  + 4);
			outptr(&a2);
			break;
		default:
			aerr(BADMODE);
		}
		break;
	/* Must be P2 or P3 */
	case TP2P3:
		getaddr_i(&a1);
		if ((a1.a_type & TMMODE) != TWR)
			aerr(AEAREQUIRED);
		switch(a1.a_type & TMREG) {
		case P2:
			outab(opcode | 2);
			break;
		case P3:
			outab(opcode | 3);
			break;
		default:
			aerr(AEAREQUIRED);
		}
		break;
	/* 16bit immediate (JSR JMP) */
	case TIMM16:
		getaddr(&a1);
		outab(opcode);
		a1.a_value--;	/* Offset by 1 */
		outraw(&a1);
		break;
	case TPIMM16:
		/* Used for PLI (JSR is PLI 0,n) */
		getaddr_i(&a1);
		index_required(&a1);
		comma();
		/* Unlike JSR/JMP the = is required */
		getaddr_i(&a2);
		if ((a2.a_type & TMADDR) != TIMMED)
			aerr(BADMODE);
		outab(opcode | (a1.a_type & TMREG));
		outraw(&a2);
		break;
	case TLOAD:
		/* This is a wonderful mess of combinations */
		getaddr_i(&a1);
		mode = a1.a_type & TMMODE;
		r = a1.a_type & TMREG;
		if (mode != TBR && mode != TWR)
			aerr(BADMODE);
		comma();
		if (r == T)
			opcode = 0xA0;
		else if (r == A)
			opcode = 0xC0;
		else if (r == EA)
			opcode = 0x80;
		else
			opcode = 0x00;
		/* If none of these opcode is 0 */
		getaddr_i(&a2);
		/* reg/reg pairs */
		r2 = a2.a_type & TMREG;
		if ((a2.a_type & TMMODE) == TBR || (a2.a_type & TMMODE) == TWR) {
			/* Register, register forms, somewhat irregular */
			if (r == A && r2 == S) {
				outab(0x06);
				break;
			}
			if (r == S && r2 == A) {
				outab(0x07);
				break;
			}
			if (r == T && r2 == EA) {
				outab(0x09);
				break;
			}
			if (r == EA && r2 == T) {
				outab(0x0B);
				break;
			}
			if (r == A && r2 == E) {
				outab(0x40);
				break;
			}
			if (r == E && r2 == A) {
				outab(0x48);
				break;
			}
			if (r == EA && r2 <= P3)  {
				outab(0x30 + r2);
				break;
			}
			if (r <= P3 && r2 == EA) {
				outab(0x44 + r);
				break;
			}
			aerr(BADREG);
			break;
		}
		switch(a2.a_type & TMADDR) {
		case TIMMED:
			/* Immediate */
			if (r == A) {
				outab(0xC4);
				outrab(&a2);
				break;
			}
			if (r == EA) {
				outab(0x84);
				outraw(&a2);
				break;
			}
			/* LD P0, =nnnn is actually also JMP */
			if (r >= P0 && r <= P3) {
				outab(0x24 | r);
				outraw(&a2);
				break;
			}
			if (r == T) {
				outab(0xA4);
				outraw(&a2);
				break;
			}
			aerr(BADMODE);
			break;
		case TINDEX:
			if (r == A) {
				outab(0xC0 | r2);
				outptr(&a2);
				break;
			}
			if (r == EA) {
				outab(0x80 | r2);
				outptr(&a2);
				break;
			}
			if (r == T) {
				outab(0xA0 | r2);
				outptr(&a2);
				break;
			}
			aerr(BADREG);
			break;
		case TAUTOINDEX:
			if (r == A) {
				outab(0xC4 | r2);
				outptr(&a2);
				break;
			}
			if (r == EA) {
				outab(0x84 | r2);
				outptr(&a2);
				break;
			}
			if (r == T) {
				outab(0xA0 | r2);
				outptr(&a2);
				break;
			}
			aerr(BADMODE);
			break;
		case TDIRECT:
			/* Direct forms. */
			if (r == A) {
				outab(0xC5);
				outrab(&a2);
				break;
			}
			if (r == EA) {
				outab(0x85);
				outrab(&a2);
				break;
			}
			if (r == T) {
				outab(0xA5);
				outrab(&a2);
				break;
			}
			aerr(BADMODE);
			break;
		default:
			aerr(BADMODE);
		}
		break;
	case TSTORE:
		/* A similar but different mix */
		getaddr_i(&a1);
		mode = a1.a_type & TMMODE;
		r = a1.a_type & TMREG;
		if (mode != TBR && mode != TWR)
			aerr(BADMODE);
		/* Can't store pointers, T etc */
		if (r != A && r != EA)
			aerr(AEAREQUIRED);
		if (r == A)
			opcode = 0xC8;
		else
			opcode = 0x88;
		if (getnb() != ',') {
			aerr(MISSING_COMMA);
			break;
		}
		getaddr_i(&a2);
		switch(a2.a_type & TMADDR) {
		case TDIRECT:
			outab(opcode | 5);
			outrab(&a2);
			break;
		case TINDEX:
			outab(opcode | (a2.a_type & TMREG));
			outptr(&a2);
			break;
		case TAUTOINDEX:
			outab(opcode | (a2.a_type & TMREG) | 4);
			outptr(&a2);
			break;
		default:
			aerr(BADMODE);
		}
		break;
	/* This one is a bit weird. We can branch relative to any pointer (even stack!)
	   but we need to check PC relative labels */
	case TBRA:
		getaddr_i(&a1);
		index_required(&a1);
		if ((a1.a_type & TMREG) == P0) {
			disp = a1.a_value - dot[segment] - 2;
			/* Only on pass 3 do we know the correct offset for a forward branch
			   to a label where other stuff with Jcc has been compacted */
			if (pass == 3 && (disp<-128 || disp>127 || segment_incompatible(&a1)))
				aerr(BRA_RANGE);
		}
		outab(opcode | (a1.a_type & TMREG));
		outab(disp);
		break;
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
		getaddr_i(&a1);
		index_required(&a1);
		if ((a1.a_type & TMREG) != P0) {
			/* Not clear what the right relocation rules are */
			outab(opcode);
			outrab(&a1);
			break;
		}
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
			if (opcode != 0x74) {	/* BRA -> JMP */
				/* We only support extending BZ and BNZ as BP and BND
				   have no inverse */
				outab(opcode^0x10);	/* Inverted branch */
				outab(3);		/* Skip over the jump */
			}
			outab(0x24);
			/* Jump target is the byte before the expected one */
			a1.a_value--;
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
