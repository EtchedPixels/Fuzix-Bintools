/*
 * Series16 assembler.
 * Assemble one line of input.
 *
 * TODO:
 *	- relocations
 *	- literals
 */
#include	"as.h"

static int asmflags = 0;
static unsigned basepage = 0;

#define FEAT_EXT	1
#define FEAT_516	2
#define FEAT_HSA	4

/*
 * Word addressed machine. When we create a label we use dot/2 for the
 * value as the internal thinking is in bytes here and in the linker
 *
 * This should mean we can boot the word size hacks out of the linker
 */

#define BYTES_PER_WORD	2

static void require_feat(unsigned f)
{
	if (asmflags & f)
		return;
	err('1', REQUIRE_FEATURE);
}

/*
 *	Set up for the start of each pass
 */
int passbegin(int pass)
{
	asmflags = 0;
	basepage = 0;
	segment = 1;		/* Default to code */
	/*  we do not do pass 1 and 2 . May need to revisit for literals */
	if (pass == 1 || pass == 2)
		return 0;
	return 1;
}

void doflush(void)
{
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

static void memref(ADDR *ap, unsigned type)
{
	/* TODO: check by type */
	unsigned indirect = 0;
	unsigned literal = 0;
	unsigned indexed = 0;
	unsigned dp = 0;

	int c = getnb();
	if (c == '*')
		indirect = 1;
	else if (c == '#')
		literal = 1;
	else
		unget(c);

	c = getnb();
	if (indirect && c == '*') {
		/* ** special form */
		ap->a_value = 0;
		c = getnb();
		/* * ** or ** */
		if (c != '*') {
			indirect = 0;
			unget(c);
		}
		goto encode;
	} else if (literal == 0 && c == '@')
		/* Force segment 0 */
		dp = 1;
	else
		unget(c);

	/* Now we should have an address */
	getaddr(ap);
	if (!literal) {
		/* And it may be followed by ,1 for indexed */
		c = getnb();
		if (c == ',') {
			c = getnb();
			if (c == '1')
				indexed = 1;
			else
				aerr(SYNTAX_ERROR);
		} else
			unget(c);
	}
encode:
	if (indirect)
		ap->a_type |= TINDIRECT;
	if (literal)
		ap->a_type |= TIMMED;
	if (indexed)
		ap->a_type |= TINDEX;
	if (dp)
		ap->a_segment = ZP;
}

static void outaw(unsigned v)
{
	outab((v >> 8) & 0xFF);
	outab(v & 0xFF);
}

/*
 *	opbase
 *	1	- memory ref (wil be turned to 3 in extended)
 *	2	- stx/ldx
 */
static void write_memref(unsigned opcode, ADDR *ap, unsigned opbase)
{
	if (ap->a_type & TINDIRECT)
		opcode |= 0x8000;
	if (ap->a_type & TINDEX)
		opcode |= 0x4000;
	if (ap->a_segment != ZP)
		opcode |= 0x0200;
	/*if (ap->a_type & TIMMED)
			need to allocate a word etc */
	ap->a_value |= opcode;
	outraw(ap);
#if 0
	if (opbase == 1 && (asmflags & FEAT_EXT))
		opbase = 3;
	/* ? What do we do with abs values > range ? */
	/* Literal - IMMED never mixes with others */
	if (ap->a_type & TIMMED)
		opbase = 0;
	/* Can write directly */
	if (ap->a_segment == ABSOLUTE && ap->a_sym == NULL) {
		ap->a_value |= opcode;
		outab((ap->a_value >> 8) & 0xFF);
		outab(ap->a_value & 0xFF);
		return;
	}
	if (ap->a_sym) {
		outbyte(opbase + 4);
		outbyte(a->sym->s_number & 0xFF);
		outbyte(a->sym->s_number >> 8);
	} else {
		outbyte(opbase);
		outbyte(ap->a_segment);
	}
	outbyte((ap->a_value >> 8) & 0xFF);
	outbyte(ap->a_value & 0xFF);
	outab(opcode >> 8)
	outab(opcode);
#endif	
}

/* Tell the linker this is a place to stuff literals safely */
static void write_literals(void)
{
#if 0
	outbyte(REL_ESC);
	outbyte(REL_TARGET);	/* Target specific */
	outbyte(0x10);		/* Literals go here */
#endif
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
			sp->s_value = dot[segment] / BYTES_PER_WORD;
			sp->s_segment = segment;
		} else {
			if ((sp->s_type&TMMDF) != 0)
				err('m', MULTIPLE_DEFS);
			if (sp->s_value != dot[segment] / BYTES_PER_WORD)
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
		/* Force alignment as really are a word machine */
		if (dot[segment] & 1)
			outab(0);
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
		/* Force alignment as really are a word machine */
		if (dot[segment] & 1)
			outab(0);
		break;

	case TDEFS:
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		/* Write out the bytes. The BSS will deal with the rest */
		for (value = 0 ; value < a1.a_value; value++) {
			/* We are a word machine so space is in words */
			outab(0);
			outab(0);
		}
		break;

	case TXAC:
		memref(&a1, 2);
		write_memref(0, &a1, 1);
		break;

	case TLITMARK:
		write_literals();
		break;

	case TSETBASE:
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		basepage = a1.a_value >> 9;
		break;
		
	case TSETFLAG:
		asmflags |= opcode;
		if (opcode & FEAT_HSA)
			cpu_flags |= OA_H316_HSA;
		if (opcode & FEAT_516)
			cpu_flags |= OA_H316_516;
		break;

	case TCLRFLAG:
		asmflags &= ~opcode;
		break;

	case TG5:
		require_feat(FEAT_516);
		/* Fall through */
	case TG:
		outaw(opcode);
		break;

	case TGHSA:
		require_feat(FEAT_HSA);
		outaw(opcode);
		break;

	case TSH:
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		if (a1.a_value > 31)
			aerr(RANGE);
		/* TODO strictly this is 6bits twos complement */
		outaw((opcode << 6)| a1.a_value);
		break;

	case TIO:
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		if (a1.a_value > 1023)
			aerr(RANGE);
		outaw((opcode << 10)| a1.a_value);
		break;

	case TMRX:
		memref(&a1, 0);
		if (a1.a_type & TINDEX)
			aerr(NOTINDEX);
		write_memref(opcode, &a1, 2);
		/* JMP forms we can stuff literals after */
		if (opcode == (1 << 10))
			write_literals();
		break;

	case TMRHSA:
		require_feat(FEAT_HSA);
		/* Fall throug */

	case TMR:
		memref(&a1, 1);
		write_memref(opcode, &a1, 1);
		break;

	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}
