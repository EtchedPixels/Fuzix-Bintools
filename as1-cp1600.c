/*
 * CP1600 assembler.
 *
 * This is a fairly regular instruction set but completely word based and
 * word addressed. It has odd support for 10bit wide memory as the instructions
 * occupy only 10bits. This includes a prefix for putting 16bit constants into
 * two 10bits as bytes and a jmp and jsr instruction pair that are totally
 * demented and need custom relocation types.
 *
 * Currently this implements the standard instructions and the sdbd prefix
 * It does not yet implement
 *	- specifying 10bit mode directly
 *	- expanding and inverting branch instructions that are out of range
 *	  (can only occur in 10bit mode)
 *	- relocations for jmp/jsr forms and for 10bit prefixed immediates
 *
 *	TODO
 *	- pure word machine linker support, or figure out how to deal with
 *	  the fun world of byteptrs that are 17bits long (seg relative ?)
 *	- linker target specific support for 10bit mode stuff
 */
#include	"as.h"


/*
 * CPU specific pass setup
 * We may need nextrel type 4 pass stuff if we do 10bit ROM form
 */

int passbegin(int pass)
{
	segment = 1;		/* Default to code */
	if (pass == 1 || pass == 2)
		return 0;
	return 1;		/* All passes required */
}

void doflush(void)
{
}

static void constify(ADDR *ap)
{
	if ((ap->a_type & TMMODE) == (TUSER|TMINDIR))
		ap->a_type = TUSER|TIMMED;
}

/* We store the file bigendian. It's a word machine so endian really has
   no meaning. It's just a file format convention */
static void outaw(uint16_t a)
{
	outab(a >> 8);
	outab(a);
}

static void outrel16(ADDR *ap)
{
	/* Absolute value - outraw will write it as is. User specified
	   something like "b -4" */
	if (ap->a_segment == ABSOLUTE) {
		outraw(ap);
		return;
	}
	/* Check what the bias is for the CP1600 TODO */
	ap->a_value--;
	outrawrel(ap);
}

/* The jmp instruction is a bit freaky */
static void outjmp(unsigned r, ADDR *ap, unsigned de)
{
	/* TODO: add a custom relocation record */
	uint32_t v = r << 8;
	uint32_t a = ap->a_value;

	constify(ap);
	istuser(ap);

	v |= ((a >> 10) & 0x3F) << 2;
	v |= de << 8;
	v |= (a & 0x3FF);
	outaw(v >> 16);
	outaw(v);
}

/*
 *	We accept the following formats
 *	r%d	-	register modes
 *	nnnn	-	indirect
 *	#nnnn	-	immediate
 *
 *	TODO
 *	pc, sp as names for r7 and r6
 *	consider (r%d) form for reg giving address forms so it's
 *	not ambiguous and we can drop xxxR formats
 */

void getaddr(ADDR *ap)
{
	int c;
	unsigned rrel = 0;

	ap->a_type = 0;
	ap->a_flags = 0;
	ap->a_value = 0;
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
	/* Can write @rn for indirect via rn in some syntaxes */
	if (c == '@') {
		rrel = 1;
		c = getnb();
	}
	if (c == 'r') {
		c = getnb();
		if (c >= '0' && c <= '7') {
			c -= '0';
			ap->a_type |= TREG | c;
			ap->a_value = 0;
			return;
		}
		/* Oops - was a symbol starting with r */
		unget(c);
		unget('r');
	} else {
		if (rrel)
			aerr(SYNTAX_ERROR); 
		unget(c);
	}

	c = getnb();

	if (c == '<')
		ap->a_flags |= A_LOW;
	else if (c == '>')
		ap->a_flags |= A_HIGH;
	else
		unget(c);

	expr1(ap, LOPRI, 1);
	ap->a_type = TUSER|TMINDIR;
}

int getaddr_r(void)
{
	ADDR ap;
	getaddr(&ap);
	if ((ap.a_type & TMADDR) != TREG)
		aerr(REG_REQUIRED);
	return ap.a_type & TMREG;
}

/* The encoding of the 10bit ops on the reg/addr ops only leaves 3 bits
   for the address encoding so the result is a bit odd
   
   0	-	(addr)
   1-3	-	(r1) (r2) (r3)
   4-5	-	(r4+) (r5+)
   6    -	(r6) post inc/pre dec depending
   7	-	immediate (effectively (r7+))
  */
   
static void do_arop(unsigned opcode, unsigned r, ADDR *ap, int is10bit)
{
	if (r == 0 || r == 7)
		aerr(INVALID_REG);
	opcode |= r;
	switch(ap->a_type & TMADDR) {
	case TREG:
		/* Reg ref forms put the reg into the instruction */
		outaw(opcode | ((ap->a_type & TMREG) << 3));
		break;
	case TIMMED:
		/* Immediate is really r7++ , ie PC with inc */
		outaw(opcode | (7 << 3));
		if (is10bit) {
			/* TODO relocations right */
			outaw(ap->a_value >> 8);
			outaw(ap->a_value & 0xFF);
		} else
			outraw(ap);
		break;
	case 0:
		if (is10bit)
			aerr(NOT10BIT);
		outaw(opcode);
		outraw(ap);
		break;
	default:
		aerr(INVALID_FORM);
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
	unsigned twice;
	unsigned r;
	unsigned tenbit = 0;
loop:

	if (tenbit)
		tenbit--;

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
			sp->s_value = dot[segment] / 2;
			sp->s_segment = segment;
		} else if (pass != 3) {
			/* Don't check for duplicates, we did it already
			   and we will confuse ourselves with the pass
			   before. Instead blindly update the values */
			sp->s_type &= ~TMMODE;
			sp->s_type |= TUSER;
			sp->s_value = dot[segment] / 2 ;
			sp->s_segment = segment;
		} else {
			/* Phase 2 defined the values so a misalignment here
			   is fatal */
			if ((sp->s_type&TMMDF) != 0)
				err('m', MULTIPLE_DEFS);
			if (sp->s_value != dot[segment] / 2) {
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
		/* This is in words */
		dot[segment] = a1.a_value << 1;
		/* Tell the binary generator we've got a new absolute
		   segment */
		outabsolute(a1.a_value << 1);
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

	/* Totally word based and word addressed. We might want to think
	   about bytepointers of some kind later and byte values  but it's
	   not clear */
	case TDEFB:
		do {
			getaddr(&a1);
			constify(&a1);
			istuser(&a1);
			outraw(&a1);
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
			outaw(c);
		}
		break;

	case TDEFS:
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		/* Write out the bytes. The BSS will deal with the rest */
		for (value = 0 ; value < a1.a_value; value++)
			outaw(0);
		break;

	case TRA:	/* Only used for mvo */
		r = getaddr_r();	/* Source */
		comma();
		getaddr(&a1);	/* Dest */
		do_arop(opcode, r, &a1, tenbit);
		break;

	case TAR:
		getaddr(&a1);	/* Source */
		comma();
		r = getaddr_r();	/* Dest */
		do_arop(opcode, r, &a1, tenbit);
		break;

	case TARI:
		getaddr(&a1);
		constify(&a1);
		istuser(&a1);
		comma();
		r = getaddr_r();
		do_arop(opcode, r, &a1, tenbit);
		break;

	case TBRA:
		getaddr(&a1);
		outaw(opcode);
		/* TODO in 10bit mode check range */
		/* TODO 10bit mode will need a custom reloc to set
		   the sign bit in the op accordingly */
		outrel16(&a1);
		break;

	case TBEXT:
		getaddr(&a1);
		/* TODO : make sure a1 is a constant value */
		if (a1.a_value < 0 || a1.a_value > 15)
			aerr(CONSTANT_RANGE);
		constify(&a1);
		istuser(&a1);
		opcode |= a1.a_value;
		comma();
		getaddr(&a1);
		outaw(opcode);
		outrel16(&a1);
		break;

	case TRR:
		opcode |= getaddr_r() << 3;
		comma();
		opcode |= getaddr_r();
		outaw(opcode);
		break;

	case TR:
		opcode |= getaddr_r();
		outaw(opcode);
		break;

	case TGSWD:	/* GSWD 0-3 are gets, 4-5 are NOP 6/7 is used for SIN */
		r = getaddr_r();
		if (r > 3)
			aerr(INVALID_REG);
		opcode |= r;
		outaw(opcode);
		break;

	case TSHIFT:
		twice = 0;
		r = getaddr_r();
		if (r > 3)
			aerr(INVALID_REG);
		c = getnb();
		if (c == ',') {
			getaddr(&a1);
			constify(&a1);
			istuser(&a1);
			/* TODO: check const */
			if (a1.a_value == 2)
				twice = 0x04;
			else if (a1.a_value == 1)
				twice = 0x00;
			else
				aerr(CONSTANT_RANGE);
		} else
			unget(c);
		opcode |= twice;
		opcode |= r;
		outaw(opcode);
		break;
	case TSBDB:
		tenbit = 2;
	case TIMPL:
		outaw(opcode);
		break;
	case TSIN:
		c = getnb();
		if (c == '2')
			opcode |= 1;
		else if (c != '1')
			unget(c);
		outaw(opcode);
		break;
	case TJR:
		/* Actually movr rn, r7 */
		opcode |= getaddr_r() << 3;
		outaw(opcode);
		break;
	case TRSAME:
		/* Ops that are aliaes to op r,r */
		r = getaddr_r();
		opcode |= r << 3;
		opcode |= r;
		outaw(opcode);
		break;
	case TJMP:
		/* This one is a bit nuts as it assumes 10bit ROM mode. For
		   16bit mode use B */
		getaddr(&a1);
		outjmp(3, &a1, opcode);
		break;
	case TJSR:
		/* Equally nuts as it assumes 10bit ROM mode. For 16bit mode
		   you can either use it or MOVR 7,r B whatever and adjust
		   r before the return */
		r = getaddr_r();
		if (r < 4 || r > 6)
			aerr(INVALID_REG);
		comma();
		getaddr(&a1);
		outjmp(r & 3, &a1, opcode);
		break;
	default:
		aerr(SYNTAX_ERROR);
	}
	goto loop;
}
