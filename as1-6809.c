/*
 * 6809 assembler.
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
	cputype = 6809;
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

/* Handle the corner case of labels in direct page being used as relative
   branches from the overlapping 'absolute' space */
static int segment_incompatible(ADDR *ap)
{
	/* No idea for sizing externals */
	if (ap->a_segment == UNKNOWN)
		return 1;
	if (ap->a_segment == segment)
		return 0;
	if (ap->a_segment == ZP && segment == 0 && ap->a_value < 256)
		return 0;
	return 1;
}

/*
 * Deal with the syntactic mess 6809 assembler has
 *
 * In some cases (JSR JMP and definitions - eg .word)
 * $ABCD means a constant everywhere else that is #ABCD
 */

static void constify(ADDR *ap)
{
	if ((ap->a_type & TMMODE) == (TUSER|TMINDIR))
		ap->a_type = TUSER;
}

/*
 *	Parse a register. We do this by hand due to the ++/-- formats etc
 *	and also because we need it for excg/tfr/psh/pop
 */

unsigned parse_register(unsigned c)
{
	if (c == 'a')
		return TBR|A;
	if (c == 'b')
		return TBR|B;
	if (c == 'c') {
		c = getnb();
		if (c == 'c')
			return TBR|CC;
		qerr(NEED_REGISTER);
		return 0;
	}
	if (c == 'd') {
		c = getnb();
		if (c == 'p')
			return TBR|DP;
		unget(c);
		return TWR|D;
	}
	if (c == 's')
		return TWR|S;
	if (c == 'u')
		return TWR|U;
	if (c == 'x')
		return TWR|X;
	if (c == 'y')
		return TWR|Y;
	if (c != 'p') {
		unget(c);
		return 0;
	}
	c = getnb();
	if (c != 'c') {
		qerr(NEED_REGISTER);
		return 0;
	}
	c = getnb();
	if (c != 'r')
		unget(c);
	return TWR|PCR;
}

unsigned register_to_mask(unsigned r)
{
	switch(r) {
	case TBR|A:
		return 0x02;
	case TBR|B:
		return 0x04;
	case TBR|CC:
		return 0x01;
	case TWR|D:
		return 0x06;
	case TBR|DP:
		return 0x08;
	case TWR|S:
	case TWR|U:
		return 0x40;
	case TWR|X:
		return 0x10;
	case TWR|Y:
		return 0x20;
	case TWR|PCR:
		return 0x80;
	default:
		qerr(INVALID_FORM);
		return 0xFF;
	}
}

unsigned register_to_nybble(unsigned r)
{
	switch(r) {
	case TBR|A:
		return 0x8;
	case TBR|B:
		return 0x9;
	case TBR|CC:
		return 0xA;
	case TWR|D:
		return 0x0;
	case TBR|DP:
		return 0xB;
	case TWR|S:
		return 0x4;
	case TWR|U:
		return 0x3;
	case TWR|X:
		return 0x1;
	case TWR|Y:
		return 0x2;
	case TWR|PCR:
		return 0x5;
	default:
		qerr(INVALID_FORM);
		return 0xFF;
	}
}

unsigned register_mask(unsigned exclude)
{
	unsigned mask = 0;
	unsigned n;
	unsigned c;
	/* Parse a list of registers split by comma */
	do {
		c = getnb();
		n = parse_register(c);
		if (n == exclude)
			qerr(INVALID_FORM);
		mask |= register_to_mask(n);
		c = getnb();
	} while(c == ',');
	unget(c);
	return mask;
}

unsigned exchange_pair(void)
{
	unsigned s, d;
	s = parse_register(getnb());
	comma();
	d = parse_register(getnb());
	/* Can't swap mixed sizes */
	if ((s & TMMODE) != (d & TMMODE))
		qerr(INVALID_FORM);
	return (register_to_nybble(s) << 4) | register_to_nybble(d);
}
	

void index_required(ADDR *ap)
{
	if ((ap->a_type & TMMODE) != TWR ||
		(ap->a_type & TMREG) == D) {
		qerr(NEED_INDEX);
	}
}

void prepost_required(ADDR *ap)
{
	if ((ap->a_type & TMMODE) != TWR ||
		(ap->a_type & TMREG) == D ||
		(ap->a_type & TMREG) == PCR) {
		qerr(NEED_PREPOST);
	}
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
 * The 6809 has a bunch of addressing modes we need to decode
 *
 * Immediate:  #constant
 * Postbyte for EXG/TFR/PSH/PUL - handled separately
 * Direct @foo   (usually >foo but that is ambiguous with our upper
 *		  and lower forms)
 * Indexed  {const},[XYUS]
 * Accumulator offset: [ABD],[XYUS]
 * Autoinc byte   ,[XYUS]+
 * Autoinc word   ,[XYUS]++
 * Autodec byte   ,[XYUS]+
 * Autodec word   ,[XYUS]++
 * Indirected forms of the above for all except immediate
 * PC relative	{const},PC
 */

void getaddr_op(ADDR *ap, unsigned noreg)
{
	unsigned indirect = 0;
	unsigned predec = 0;
	unsigned postinc = 0;
	unsigned hasleft = 0;	/* was a left side to indexed from */
	unsigned dp = 0;
	unsigned type;
	unsigned c;

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_value = 0;
	ap->a_sym = NULL;
	ap->a_segment = ABSOLUTE;

	c = getnb();

	/* Immediate */
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
		ap->a_type |= TIMMED;
		return;
	}

	/* Request for constants only (eg defb) */
	if (noreg) {
		if (c == '<')
			ap->a_flags |= A_LOW;
		else if (c == '>')
			ap->a_flags |= A_HIGH;
		else
			unget(c);
		expr1(ap, LOPRI, 0);
		istuser(ap);
		ap->a_type |= TEXT;
		return;
	}
	if (c == '@') {
		dp = 1;
		c = getnb();
	} else if (c == '[') {
		indirect = 1;
		c = getnb();
	}
	if (dp || c != ',') {
		/* Get the expression */
		if (c == '<')
			ap->a_flags |= A_LOW;
		else if (c == '>')
			ap->a_flags |= A_HIGH;
		else
			unget(c);
		expr1(ap, LOPRI, 0);
		if (dp) {
			ap->a_type |= TDIR;
			return;
		}
		c = getnb();
		/* [addr] */
		if (c == ']' && indirect) {
			/* off is in fact the result but indirected */
			ap->a_type |= TEXT|TMINDIR; /* Set indirect addr bit */
			return;
		}
		/* Not an indirection */
		if (c != ',') {
			ap->a_type |= TEXT;
			unget(c);
			return;
		}
		hasleft = 1;
	}

	type = TINDEX;

	/* A, B, D, forms are awkward as we now have two registers. Fudge
	   it to fit the standard formats */
	if (ap->a_type == (TBR|A))
		type = TINDEXA;
	else if (ap->a_type == (TBR|B))
		type = TINDEXB;
	else if (ap->a_type == (TWR|D))
		type = TINDEXD;
	else if ((ap->a_type & TMMODE) == TBR ||
		 (ap->a_type & TMMODE) == TWR) {
			 aerr(INVALID_FORM);
			 type = TINDEXD;
			 /* Dummy type to avoid cascading errors */
	}

	/* We can't just throw the right hand side at the expression
	   parser because it's full of + and - but not an expression so
	   we do it by hand */
	c = getnb();
	if (c == '-' && !hasleft && type == TINDEX) {
		predec = 1;
		c = getnb();
		if (c == '-') {
			predec++;
			c = getnb();
		}
	}
	/* now a register name or PC */
	ap->a_type |= parse_register(c);

	c = getnb();
	if (c == '+' && !hasleft && !predec && type == TINDEX) {
		postinc = 1;
		c = getnb();
		if (c == '+')  {
			postinc = 2;
			c = getnb();
		}
	}
	if (indirect) {
		if (c != ']')
			qerr(NEED_CLSQUARE);
	} else
		unget(c);

	if (postinc || predec) {
		prepost_required(ap);
		/* byte+ -byte indirect is not legal */
		if (indirect && (postinc == 1 || predec == 1))
			qerr(INVALID_FORM);
	} else
		index_required(ap);

	/* A B and D, forms do not allow pre/post */
	if (type == TINDEXA || type == TINDEXB || type == TINDEXD) {
		if (predec || postinc)
			qerr(INVALID_FORM);
		ap->a_value = 0;
	}

	/* Now put together the address descriptor */
	/* Set up the TMADDR bits */
	if (indirect)
		ap->a_type |= TMINDIR;
	if (predec == 1)
		ap->a_type |= TPRE1;
	else if (predec == 2)
		ap->a_type |= TPRE2;
	else if (postinc == 1)
		ap->a_type |= TPOST1;
	else if (postinc == 2)
		ap->a_type |= TPOST2;
	else
		ap->a_type |= type;
}

void getaddr_r(ADDR *ap)
{
	getaddr_op(ap, 0);
}

void getaddr(ADDR *ap)
{
	getaddr_op(ap, 1);
}

unsigned is_indexed(ADDR *ap)
{
	unsigned ta = ap->a_type & (TMADDR | TMINDIR);
	if (ta != TINDEX && ta != TINDEXA && ta != TINDEXB && ta != TINDEXD)
		return 0;
	return 1;
}

unsigned is_postbyte(ADDR *ap)
{
	unsigned ta = ap->a_type & TMADDR;
	if (ta >= TINDEX)
		return 1;
	if (ta != TEXT)
		return 0;
	/* [ext] is a postbyte form */
	if (ap->a_type & TMINDIR)
		return 1;
	return 0;
}

/* Turn an internal register representation into an RR encoding */
unsigned map_register(unsigned r)
{
	if (r == X)
		return 0x00;
	if (r == Y)
		return 0x20;
	if (r == U)
		return 0x40;
	if (r == S)
		return 0x60;
	/* Other forms are not an error we just won't be using RR */
	return 0xFF;	/* So bugs show */
}

unsigned can_shorten(ADDR *ap)
{
	if (ap->a_segment == ABSOLUTE)
		return 1;
	return 0;
}

unsigned range_5bit(ADDR *ap)
{
	if ((int16_t)ap->a_value >= -16 && (int16_t)ap->a_value <= 15)
		return 1;
	return 0;
}

unsigned range_8bit(ADDR *ap)
{
	if ((int16_t)ap->a_value >= -128 && (int16_t)ap->a_value <= 127)
		return 1;
	return 0;
}

/* 6809 people expect the assembler to deal with both relative and
   symbolic representations for pc relative. So it's valid to do
   write 12,pcr  meaning pc + 12,  offset .equ 12 offset,pcr meaning
   pc + 12 but also if fred is a symbol in the code etc that
   fred,pcr means a reference to fred not fred + pc */
void write_rel16(ADDR *ap)
{
	/* Absolute value - outraw will write it as is */
	if (ap->a_segment == ABSOLUTE) {
		outraw(ap);
		return;
	}
	/* The PC relative applies versus the byte *after* the two we are about to reloc */
	ap->a_value -= 2;
	outrawrel(ap);
}

/* Write out the post byte and data */
void write_data(ADDR *ap, unsigned size)
{
	unsigned t = ap->a_type & (TMADDR | TMINDIR);
	unsigned r = map_register(ap->a_type & TMREG);

	if (t == TIMMED) {
		if (size == 1)
			outrab(ap);
		else
			outraw(ap);
		return;
	}
	if (t == TDIR) {
		outrab(ap);
		return;
	}
	if (t == TEXT) {
		outraw(ap);
		return;
	}
	/* PC relative indexing is a bit different to the others */
	if (t == TINDEX && (ap->a_type & TMREG) == PCR) {
		if (can_shorten(ap) && range_8bit(ap)) {
			outab(0x8C);
			outrab(ap);
			return;
		}
		outab(0x8D);
		write_rel16(ap);
		return;
	}
	if (t == (TINDEX|TMINDIR) && (ap->a_type & TMREG) == PCR) {
		if (can_shorten(ap) && range_8bit(ap)) {
			outab(0x9C);
			outrab(ap);
			return;
		}
		outab(0x9D);
		write_rel16(ap);
		return;
	}
	if (t == TINDEX) {
		if (can_shorten(ap)) {
			if (ap->a_value == 0) {
				outab(0x84 | r);
				return;
			}
			/* Can have 5 or 8bit offset encoding */
			if (range_5bit(ap)) {
				outab(r + (ap->a_value & 0x1F));
				return;
			}
			if (range_8bit(ap)) {
				outab(0x88 | r);
				outrab(ap);
				return;
			}
		}
		outab(0x89 | r);
		outraw(ap);
		return;
	}
	if (t == (TMINDIR|TINDEX)) {
		if (can_shorten(ap)) {
			if (ap->a_value == 0) {
				outab(0x94 | r);
				return;
			}
			if (range_8bit(ap)) {
				outab(0x98 | r);
				outrab(ap);
				return;
			}
		}
		outab(0x99 | r);
		outraw(ap);
		return;
	}
	/* Pre/post must match size */
	if (t == TPOST1) {
		if (size == 2)
			qerr(INVALID_FORM);
		outab(0x80 | r);
		return;
	}
	if (t == TPOST2) {
		if (size == 1)
			qerr(INVALID_FORM);
		outab(0x81 | r);
		return;
	}
	if (t == TPRE1) {
		if (size == 2)
			qerr(INVALID_FORM);
		outab(0x82 | r);
		return;
	}
	if (t == TPRE2) {
		if (size == 1)
			qerr(INVALID_FORM);
		outab(0x83 | r);
		return;
	}
	/* Indirect pre/post always word and was checked in getaddr_r */
	if (t == (TMINDIR|TPOST2)) {
		outab(0x91 | r);
		return;
	}
	if (t == (TMINDIR|TPRE2)) {
		outab(0x93 | r);
		return;
	}
	if (t == TINDEXA) {
		outab(0x86 | r);
		return;
	}
	if (t == TINDEXB) {
		outab(0x85 | r);
		return;
	}
	if (t == TINDEXD) {
		outab(0x8B | r);
		return;
	}
	if (t == (TMINDIR|TINDEXA)) {
		outab(0x96 | r);
		return;
	}
	if (t == (TMINDIR|TINDEXB)) {
		outab(0x95 | r);
		return;
	}
	if (t == (TMINDIR|TINDEXD)) {
		outab(0x9B | r);
		return;
	}
	if (t == (TMINDIR|TEXT)) {
		outab(0x9F);
		outraw(ap);
		return;
	}
	fprintf(stderr, "Internal encode %x\n", t);
}

/* The 80-FF space is highly regular */
void encode_high_op(unsigned opcode, unsigned size, unsigned has_imm)
{
	ADDR a1;
	unsigned ta1;

	getaddr_r(&a1);

	ta1 = a1.a_type & TMADDR;
	if (opcode >> 8)
		outab(opcode >> 8);
	if (ta1 == TIMMED && has_imm)
		outab(opcode);
	else if (ta1 == TDIR)
		outab(opcode + 0x10);
	else if (is_postbyte(&a1))
		outab(opcode + 0x20);
	else if (ta1 == TEXT)
		outab(opcode + 0x30);
	else
		qerr(INVALID_FORM);
	write_data(&a1, size);
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
	unsigned ta1;

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
			/* Phase 2 defined the values so a misalignment here
			   is fatal */
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
	case TBRA:
		getaddr(&a1);
		disp = a1.a_value-dot[segment]-2;
		if (pass == 3)
			c = getnextrel();
		else {
			c = 0;
			/* Will it fit, do we know ? */
			if (pass == 0 || segment_incompatible(&a1) || disp<-128 || disp>127)
				c = 1;
			/* On pass 2 we lock down our choices in the table */
			if (pass == 2)
				setnextrel(c);
		}
		if (c) {	/* LBxx is 0x10, Bxx, ... */
			if (opcode == 0x8D)	/* BSR -> LBSR */
				outab(0x17);
			else if (opcode == 0x20) /* BRA -> LBRA */
				outab(0x16);
			else {	/* Conditional ones expand with 0x10 */
				outab(0x10);
				outab(opcode);
			}
			write_rel16(&a1);
		} else {
			a1.a_value -= dot[segment];
			a1.a_value -= 2;
			outab(opcode);
			outrabrel(&a1);
		}
		break;
	case TLBRA:
		getaddr(&a1);
		if (a1.a_segment != segment && a1.a_segment != UNKNOWN)
			aerr(BRA_RANGE);
		if (opcode >> 8)
			outab(opcode >> 8);
		outab(opcode);
		write_rel16(&a1);
		break;
	/* No following data. Instruction may be one or two bytes long */
	case TIMP:
		if (opcode >> 8)
			outab(opcode >> 8);
		outab(opcode);
		break;
	/* Byte sized operations in lower half of instruction set */
	case TLO:
		getaddr_r(&a1);
		ta1 = a1.a_type & (TMADDR | TMINDIR);
		if (opcode >> 8)
			outab(opcode >> 8);
		if (ta1 == TEXT)
			outab(opcode + 0x70);
		else if (ta1 == TDIR)
			outab(opcode);
		else if (is_postbyte(&a1))
			outab(opcode + 0x60);
		else
			aerr(INVALID_FORM);
		write_data(&a1, 2);
		break;				
	case TJSR:
		/* JSR is stuffed into odd spots */
		getaddr_r(&a1);
		ta1 = a1.a_type & (TMADDR | TMINDIR);
		if (opcode >> 8)
			outab(opcode >> 8);
		if (ta1 == TEXT)
			outab(opcode + 0x20);
		else if (ta1 == TDIR)
			outab(opcode);
		else if (is_postbyte(&a1))
			outab(opcode + 0x10);
		else
			aerr(INVALID_FORM);
		write_data(&a1, 2);
		break;
	/* LEA instruction. Only forms are an indexed address which may
	   include the use of A or B. Offsets are signed */
	case TLEA:
		getaddr_r(&a1);
		if (!is_indexed(&a1))
			aerr(INVALID_FORM);
		outab(opcode);
		write_data(&a1, 2);
		break;
	case THI:	/* Byte sized operation with immediate */
		encode_high_op(opcode, 1, 1);
		break;
	case THIW:	/* Word sized operation with immediate */
		encode_high_op(opcode, 2, 1);
		break;
	case THINOIMM:	/* Byte sized with no immediate (eg stores) */
		encode_high_op(opcode, 1, 0);
		break;
	case THIWNOIMM:	/* Word sized with no immediate (eg stores) */
		encode_high_op(opcode, 2, 0);
		break;
	case TIMM8:	/* 8bit immediate - cwait, and/orcc */
		getaddr(&a1);
		if ((a1.a_type & TMADDR) != TIMMED)
			aerr(CONSTANT_ONLY);
		if (opcode >> 8)
			outab(opcode >> 8);
		outab(opcode);
		outrab(&a1);
		break;
	case TEXG:
		/* tfr, xchg */
		outab(opcode);
		outab(exchange_pair());
		break;
		/* Bitmash of registers for push and pop */
	case TPUSH:
		outab(opcode);
		if (opcode == 0x34 || opcode == 0x35)
			outab(register_mask(TWR|S));
		else
			outab(register_mask(TWR|U));
		break;
	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}
