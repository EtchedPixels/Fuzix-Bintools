/*
 * PDP4/7/9/15 assembler.
 *
 * The 4/7/9/15 is a fairly regular 18bit machine. As we are building on non
 * 18bit systems we work in 32bit words and then strip down to 18bit at the
 * end.
 *
 * Memory addressing grows through time
 * Initially the address is a 13bit address
 * Later this becomes the low 13bits of the current "bank"
 * When indexing was added index mode also permits this to be the low 12bits
 * of an address and borrows a bit for index mode.
 *
 * We track things in bytes internally so all our address scaling is by 4 but
 * we need to be careful about constants therefore
 *
 * TODO:
 * import all nova scaling and make by 4
 * import nova const handling for scaling
 * teach linker about /4 word not just /2
 * teach linker about 18bit 9/9 split for hi/lo or handle it with mask/scale ?
 * custom link ops for page/bank mode
 * immediate relocations and ld immediate merging
 */
#include	"as.h"

static unsigned cputype;
static unsigned index_mode;
static unsigned last_width;
static unsigned last_scale;

static void reloc_width(unsigned w, unsigned s)
{
	if (last_width != w || last_scale != s) {
		last_width = w;
		last_scale = s;
		outscale(s, w);
	}
}

/*
 *	Set up for the start of each pass
 */
int passbegin(int pass)
{
	cputype = 4;
	segment = 1;		/* Default to code */
	index_mode = 0;
	last_scale = 0;		/* Force the reloc_width to issue */
	reloc_width(18, 0);
	/* We have no variable sized branches to fix up so
	   we do not do pass 1 and 2 */
	if (pass == 1 || pass == 2)
		return 0;
	return 1;
}

void doflush(void)
{
}

static void outword(addr_t v)
{
	outab(v >> 24);
	outab(v >> 16);
	outab(v >> 8);
	outab(v);
}

static void align4(void)
{
	while(dot[segment] & 3)
		outab(0);
}

/* FIXME: this won't work instead we need custom reloc types for
	BANK/INDEX - low bits are machine word offset upper fixed
	overflow into other bank is error */
void fix_constant(ADDR *a)
{
	/* Compensate for scaling of non constants as we work in bytespace */
	if (a->a_segment == ABSOLUTE)
		reloc_width(last_width, 0);
	else
		reloc_width(last_width, 2);
	if (last_width == 12)
		/* Work out what our page is */
		/* TODO: we assume our segment started on a page. We will
		   eventually turn this into relocs and do a linker implicits
		   merge */
		a->a_value -= dot[segment] & ~07777;
	else  if (last_width == 13)
		a->a_value -= dot[segment] & ~017777;
}

static void outrelword(ADDR *a)
{
	fix_constant(a);
	reloc_width(18, 2);
	outrad(a);
}

/* FIXME: set scale according to symbol or const */
/* TODO: range checks */
static void outraw_op(uint32_t opcode, ADDR *a1, unsigned mode)
{
	fix_constant(a1);
	if (mode) {
		reloc_width(12, 0);
		a1->a_value &= 07777;
		if (a1->a_type & TMINDIR)
			a1->a_value |= 020000;
		if ((a1->a_type & TMADDR) == TINDEX)
			a1->a_value |= 010000;
	} else {
		reloc_width(13, 0);
		a1->a_value &= 017777;
		if (a1->a_type & TMINDIR)
			a1->a_value |= 020000;
		if ((a1->a_type & TMADDR) == TINDEX)
			aerr(NOINDEX);
	}
	a1->a_value |= opcode;
}

/* Ditto for the 9bit offset forms */
static void outraw_9bit(uint32_t opcode, ADDR *a1)
{
	reloc_width(9,0);
	a1->a_value &= 0777;
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

	/* A leading '*' means an indirect reference */
	c = getnb();
	if (c == '*')
		ap->a_type |= TMINDIR;

#if 0
	/* Will need custom handling if we bother */
	else if (c == '<')
		ap->a_flags |= A_LOW;
	else if (c == '>')
		ap->a_flags |= A_HIGH;
#endif
	else
		unget(c);

	/* This should be an absolute 12 or 11 bit constant depending on
	   mode; TODO: dot opt for mode info and sanity checks */
	expr1(ap, LOPRI, 0);
	c = getnb();
	/* Indexed ? */
	if (c != ',') {
		if (index_mode) {
			if (ap->a_value < 0 || ap->a_value > 01777)
				aerr(RANGE);
			ap->a_flags |= TUSER;
		} else {
			if (ap->a_value < 0 || ap->a_value > 07777)
				aerr(RANGE);
			ap->a_type |= TUSER;
		}
		unget(c);
		return;
	}
	/* ,X should be present */
	if (index_mode == 0) {
		aerr(SYNTAX_ERROR);
		return;
	}
	c = getnb();
	/* TODO error on older CPU */
	if (c != 'X' && c != 'x')
		aerr(SYNTAX_ERROR);
	if (ap->a_value < 0 || ap->a_value > 03777)
		aerr(RANGE);
	ap->a_flags |= TINDEX | TUSER;
}

static void constify(ADDR *ap)
{
	if ((ap->a_type & TMMODE) == (TUSER|TMINDIR))
		ap->a_type = TUSER;
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
		dot[segment] = a1.a_value * 4;	/* In words */
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
			fix_constant(&a1);
			outrab(&a1);
		} while ((c=getnb()) == ',');
		unget(c);
		align4();
		break;

	case TDEFW:
		do {
			getaddr(&a1);
			constify(&a1);
			istuser(&a1);
			fix_constant(&a1);
			outrelword(&a1);
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
		align4();
		break;

	case TDEFS:
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		/* Write out the bytes. The BSS will deal with the rest */
		for (value = 0 ; value < a1.a_value; value++)
			outword(0);
		break;

	case TSETCPU:
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		if (a1.a_value != 4 && a1.a_value != 7 && a1.a_value != 9 &&
		     a1.a_value != 15)
			aerr(SYNTAX_ERROR);
		cputype = a1.a_value;
		break;

	case TMODE:
		if (opcode && cputype < 15)	/* 9 or 15 ? */
			aerr(WRONGCPU);
		index_mode = opcode;
		break;

	case TIMPL7:
		if (cputype < 7)
			aerr(WRONGCPU);
		outword(opcode);
		break;
	case TIMPL9:
		if (cputype < 9)
			aerr(WRONGCPU);
		outword(opcode);
		break;
	case TIMPL15:
		if (cputype < 15)
			aerr(WRONGCPU);
		outword(opcode);
		break;
	case TIMPLE:	/* For now assume EAE */
	case TIMPL:
		outword(opcode);
		break;
	case TMEM:
		/* top 4 bits opcode, then indirect, indexed/banked, operand */
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		/* We page align so nothing here is relocatable */
		/* TODO: fix this to allow symbol low bits */
		outraw_op(opcode, &a1, index_mode);
		break;
	case TLAW:
		/* Load negative constant */
		getaddr(&a1);
		constify(&a1);;
		istuser(&a1);
		outraw_op(opcode, &a1, index_mode);
		break;
	case T9BIT:
		/* Opcode with 9bit constant attached */
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		if (a1.a_value < 0 || a1.a_value > 0777)
			aerr(RANGE);
		/* Lives in the low 9bits */
		outraw_9bit(opcode, &a1);
		break;
	/* EAE */
	case TEAE:
	case TOPR:
	case TIOT:
	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}
