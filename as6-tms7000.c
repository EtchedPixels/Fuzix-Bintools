/*
 * TMS7000
 *
 * Basic symbol tables.
 * Contain all of the instructions
 * and register names.
 */
#include	"as.h"

/*
 * This array of symbol nodes
 * make up the basic symbol table.
 * The "syminit" routine links these
 * nodes into the builtin symbol hash
 * table at start-up time.
 */
SYM	sym[] = {
	{	0,	"a",		TBR,		RA	},
	{	0,	"b",		TBR,		RB	},
	{	0,	"st",		TSR,		RST	},
	{	0,	"defb",		TDEFB,		XXXX	},
	{	0,	"defw",		TDEFW,		XXXX	},
	{	0,	"defs",		TDEFS,		XXXX	},
	{	0,	"defm",		TDEFM,		XXXX	},
	{	0,	"org",		TORG,		XXXX	},
	{	0,	"equ",		TEQU,		XXXX	},
	{	0,	"export",	TEXPORT,	XXXX	},
	{	0,	".byte",	TDEFB,		XXXX	},
	{	0,	".word",	TDEFW,		XXXX	},
	{	0,	".ds",		TDEFS,		XXXX	},
	{	0,	".ascii",	TDEFM,		XXXX	},
	{	0,	".org",		TORG,		XXXX	},
	{	0,	".equ",		TEQU,		XXXX	},
	{	0,	".export",	TEXPORT,	XXXX	},
	{	0,	"cond",		TCOND,		XXXX	},
	{	0,	"endc",		TENDC,		XXXX	},
	{	0,	"abs",		TSEGMENT,	ABSOLUTE},
	{	0,	"code",		TSEGMENT,	CODE	},
	{	0,	"data",		TSEGMENT,	DATA	},
	{	0,	"bss",		TSEGMENT,	BSS	},
	{	0,	"discard",	TSEGMENT,	DISCARD	},
	{	0,	"common",	TSEGMENT,	COMMON	},
	{	0,	"literal",	TSEGMENT,	LITERAL	},
	{	0,	"commondata",	TSEGMENT,	COMMONDATA },
	{	0,	"buffers",	TSEGMENT,	BUFFERS	},
	{	0,	".abs",		TSEGMENT,	ABSOLUTE},
	{	0,	".code",	TSEGMENT,	CODE	},
	{	0,	".data",	TSEGMENT,	DATA	},
	{	0,	".bss",		TSEGMENT,	BSS	},
	{	0,	".discard",	TSEGMENT,	DISCARD	},
	{	0,	".common",	TSEGMENT,	COMMON	},
	{	0,	".literal",	TSEGMENT,	LITERAL	},
	{	0,	".commondata",	TSEGMENT,	COMMONDATA },
	{	0,	".buffers",	TSEGMENT,	BUFFERS	},
	{	0,	".reg",		TSEGMENT,	ZP	},
	{	0,	".zp",		TSEGMENT,	ZP	},

	/* Mov is the 7 op form but has 3 extra forms */
	/* x0 is implicit specials */

	{	0,	"mov",		TMOV,		0x02	},

	/* Operations with 7 forms */
	/* x0 and x1 block are not used */
	{	0,	"and",		TMULT,		0x03	},
	{	0,	"or",		TMULT,		0x04	},
	{	0,	"xor",		TMULT,		0x05	},
	/* x6 is BTJO x7 is BTJZ */
	{	0,	"add",		TMULT,		0x08	},
	{	0,	"adc",		TMULT,		0x09	},
	{	0,	"sub",		TMULT,		0x0A	},
	{	0,	"sbb",		TMULT,		0x0B	},
	{	0,	"mpy",		TMULT,		0x0C	},
	{	0,	"cmp",		TMULT,		0x0D	},
	{	0,	"dac",		TMULT,		0x0E	},
	{	0,	"dsb",		TMULT,		0x0F	},

	/* Ditto with offset following */
	{	0,	"btjo",		TMULTO,		0x06	},
	{	0,	"btjz",		TMULTO,		0x07	},

	/* Three form ops (BR, CALL etc) */
	{	0,	"lda",		TIND,		0x0A	},
	{	0,	"sta",		TIND,		0x0B	},
	{	0,	"br",		TIND,		0x0C	},
	{	0,	"cmpa",		TIND,		0x0D	},
	{	0,	"call",		TIND,		0x0E	},
	
	/* Three form ops (CLR etc) */
	{	0,	"dec",		TR3,		0xB2	},
	{	0,	"inc",		TR3,		0xB3	},
	{	0,	"inv",		TR3,		0xB4	},
	{	0,	"clr",		TR3,		0xB5	},
	{	0,	"xchb",		TR3,		0xB6	},
	{	0,	"swap",		TR3,		0xB7	},
	/* B8/B9 are push/pop */
	/* BA is djnz */
	{	0,	"decd",		TR3,		0xBB	},
	{	0,	"rr",		TR3,		0xBC	},
	{	0,	"rrc",		TR3,		0xBD	},
	{	0,	"rl",		TR3,		0xBE	},
	{	0,	"rlc",		TR3,		0xBF	},

	/* Slightly odd - and have ST forms 08 0E*/
	{	0,	"push",		TPP,		0xB8	},
	{	0,	"pop",		TPP,		0xB9	},

	/* Three form ops with offset */
	{	0,	"djnz",		TR3O,		0xBA	},

	/* P form ops */
	/* MOVP has extra forms */
	{	0,	"movp",		TMOVP,		0x82	},
	{	0,	"andp",		TFORM,		0x83	},
	{	0,	"orp",		TFORM,		0x84	},
	{	0,	"xorp",		TFORM,		0x85	},

	/* P form with offset */
	{	0,	"btjop",	TFORMO,		0x86	},
	{	0,	"btjzp",	TFORMO,		0x87	},

	
	/* 0x0X implicits */
	{	0,	"nop",		TIMPL,		0x00	},
	{	0,	"idle",		TIMPL,		0x01	},
	{	0,	"eint",		TIMPL,		0x05	},
	{	0,	"dint",		TIMPL,		0x06	},
	{	0,	"setc",		TIMPL,		0x07	},
	/* pop st */
	{	0,	"stsp",		TIMPL,		0x09	},
	{	0,	"rets",		TIMPL,		0x0A	},
	{	0,	"reti",		TIMPL,		0x0B	},
	{	0,	"ldsp",		TIMPL,		0x0D	},
	/* push st */

	/* Branches with offset */
	{	0,	"jmp",		TJMP,		0xE0	},
	{	0,	"jn",		TBRA,		0xE1	},
	{	0,	"jeq",		TBRA,		0xE2	},
	{	0,	"jz",		TBRA,		0xE2	},
	{	0,	"jc",		TBRA,		0xE3	},
	{	0,	"jhs",		TBRA,		0xE3	},
	{	0,	"jp",		TREL,		0xE4	},
	{	0,	"jpz",		TBRA,		0xE5	},
	{	0,	"jne",		TBRA,		0xE6	},
	{	0,	"jnz",		TBRA,		0xE6	},
	{	0,	"jl",		TBRA,		0xE7	},
	{	0,	"jnc",		TBRA,		0xE7	},
	
	/* Oddities */
	{	0,	"clrc",		TIMPL,		0xB0	},
	{	0,	"tsta",		TIMPL,		0xB0	},
	{	0,	"tstb",		TIMPL,		0xC1	},
	{	0,	"trap",		TTRAP,		0xE8	},
	{	0,	"movd",		TMOVD,		0x88	},
};

/*
 * Set up the symbol table.
 * Sweep through the initializations
 * of the "phash", and link them into the
 * buckets. Because it is here, a
 * "sizeof" works.
 */
void syminit(void)
{
	SYM *sp;
	int hash;

	sp = &sym[0];
	while (sp < &sym[sizeof(sym)/sizeof(SYM)]) {
		hash = symhash(sp->s_id);
		sp->s_fp = phash[hash];
		phash[hash] = sp;
		++sp;
	}
}

char *etext[] = {
/*10*/	"unexpected character",
	"phase error",
	"multiple definitions",
	"syntax error",
	"must be absolute",
/*15*/	"missing delimiter",
	"invalid constant",
	"address required",
	"invalid id",
	"bad mode",
/*20*/	"constant out of range",
	"data in BSS",
	"segment overflow",
	"segment conflict",
	"branch out of range",
/*25*/	"out of range",
	"too many branches",
	"divide by zero",
	"register required",
	"P register required"
/* 30 */
};

/*
 * Make sure that the
 * mode and register fields of
 * the type of the "ADDR" pointed to
 * by "ap" can participate in an addition
 * or a subtraction.
 */
void isokaors(ADDR *ap, int paren)
{
	int mode;

	mode = ap->a_type&TMMODE;
	if (mode == TUSER)
		return;
	aerr(ADDR_REQUIRED);
}
