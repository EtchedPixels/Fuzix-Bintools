/*
 * System 16 assembler.
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
	{	0,	"defw",		TDEFW,		XXXX	},
	{	0,	"defs",		TDEFS,		XXXX	},
	{	0,	"defm",		TDEFM,		XXXX	},
	{	0,	"org",		TORG,		XXXX	},
	{	0,	"equ",		TEQU,		XXXX	},
	{	0,	"export",	TEXPORT,	XXXX	},
	{	0,	".byte",	TDEFB,		XXXX	},
	{	0,	".word",	TDEFW,		XXXX	},
	{	0,	".blkb",	TDEFS,		XXXX	},
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
	{	0,	"dp",		TSEGMENT,	ZP	},
	{	0,	"discard",	TSEGMENT,	DISCARD	},
	{	0,	"common",	TSEGMENT,	COMMON	},
	{	0,	"literal",	TSEGMENT,	LITERAL	},
	{	0,	"commondata",	TSEGMENT,	COMMONDATA },
	{	0,	"buffers",	TSEGMENT,	BUFFERS	},
	{	0,	".abs",		TSEGMENT,	ABSOLUTE},
	{	0,	".code",	TSEGMENT,	CODE	},
	{	0,	".data",	TSEGMENT,	DATA	},
	{	0,	".bss",		TSEGMENT,	BSS	},
	{	0,	".dp",		TSEGMENT,	ZP	},
	{	0,	".discard",	TSEGMENT,	DISCARD	},
	{	0,	".common",	TSEGMENT,	COMMON	},
	{	0,	".literal",	TSEGMENT,	LITERAL	},
	{	0,	".commondata",	TSEGMENT,	COMMONDATA },
	{	0,	".buffers",	TSEGMENT,	BUFFERS	},

	{	0,	".316",		TCLRFLAG,	2	},
	{	0,	".516",		TSETFLAG,	2	},
	{	0,	".exd",		TSETFLAG,	1	},
	{	0,	".lxd",		TCLRFLAG,	1	},
	{	0,	".hsa",		TSETFLAG,	4	},
	{	0,	".nhsa",	TCLRFLAG,	4	},

	{	0,	"xac",		TXAC,		0	},

	{	0,	".setb",	TSETBASE,	0	},
	
	{	0,	".litmark",	TLITMARK,	0	},

	/* Instructions by class */
	{	0,	"jmp",		TMR,		1 << 10	},
	{	0,	"lda",		TMR,		2 << 10	},
	{	0,	"ana",		TMR,		3 << 10	},
	{	0,	"sta",		TMR,		4 << 10	},
	{	0,	"era",		TMR,		5 << 10	},
	{	0,	"add",		TMR,		6 << 10	},
	{	0,	"sub",		TMR,		7 << 10	},
	{	0,	"jst",		TMR,		8 << 10	},
	{	0,	"cas",		TMR,		9 << 10	},
	{	0,	"irs",		TMR,		10 << 10	},
	{	0,	"ima",		TMR,		11 << 10	},

	{	0,	"mpy",		TMRHSA,		14 << 10 },
	{	0,	"div",		TMRHSA,		15 << 10 },

	{	0,	"ldx",		TMRX,		(13 << 10) |( 1 << 14) },
	{	0,	"stx",		TMRX,		13 << 10	},

	{	0,	"aca",		TG,		0141216	},
	{	0,	"aoa",		TG,		0141206	},
	{	0,	"cal",		TG,		0141050	},
	{	0,	"car",		TG,		0141044	},
	{	0,	"cra",		TG,		0140040	},
	{	0,	"csa",		TG,		0140320	},
	{	0,	"chs",		TG,		0140024	},
	{	0,	"cma",		TG,		0140401	},
	{	0,	"enb",		TG,		0000401	},
	{	0,	"hlt",		TG,		0000000 },
	{	0,	"iab",		TG,		0000201 },
	{	0,	"ica",		TG,		0141340	},
	{	0,	"icl",		TG,		0141140	},
	{	0,	"icr",		TG,		0141240	},
	{	0,	"inh",		TG,		0001001	},
	{	0,	"ink",		TG,		0000043	},
	{	0,	"nop",		TG,		0101000 },
	{	0,	"otk",		TG,		0171020	},
	{	0,	"rcb",		TG,		0140200	},
	{	0,	"scb",		TG,		0140600	},
	{	0,	"skp",		TG,		0100000 },
	{	0,	"sln",		TG,		0101100	},
	{	0,	"slz",		TG,		0100100	},
	{	0,	"smi",		TG,		0101400	},
	{	0,	"snz",		TG,		0101040	},
	{	0,	"spl",		TG,		0100400	},
	{	0,	"sr1",		TG,		0100020	},
	{	0,	"sr2",		TG,		0100040	},
	{	0,	"sr3",		TG,		0100004	},
	{	0,	"sr4",		TG,		0100002	},
	{	0,	"src",		TG,		0100001 },
	{	0,	"ss1",		TG,		0101020 },
	{	0,	"ss2",		TG,		0101010	},
	{	0,	"ss3",		TG,		0101004	},
	{	0,	"ss4",		TG,		0101002	},
	{	0,	"ssc",		TG,		0101001	},
	{	0,	"ssr",		TG,		0100036	},
	{	0,	"sss",		TG,		0101036	},
	{	0,	"sze",		TG,		0100040	},
	{	0,	"ssm",		TG,		0140500	},
	{	0,	"ssp",		TG,		0140100	},
	{	0,	"tca",		TG,		0141407	},

	{	0,	"alr",		TSH,		0416	},
	{	0,	"als",		TSH,		0415	},
	{	0,	"arr",		TSH,		0406	},
	{	0,	"ars",		TSH,		0405	},
	{	0,	"lgl",		TSH,		0414	},
	{	0,	"lgr",		TSH,		0404	},
	{	0,	"lll",		TSH,		0410	},
	{	0,	"llr",		TSH,		0412	},	/* 1412 ? */
	{	0,	"lls",		TSH,		0411	},
	{	0,	"lrl",		TSH,		0400	},
	{	0,	"lrr",		TSH,		0402	},
	{	0,	"lrs",		TSH,		0401	},
	
	{	0,	"ina",		TIO,		054	},
	{	0,	"ocp",		TIO,		014	},
	{	0,	"ota",		TIO,		074	},
	{	0,	"smk",		TIO,		074	},
	{	0,	"sks",		TIO,		034	},

	{	0,	"dxa",		TG5,		0000011	},
	{	0,	"exa",		TG5,		0000013	},
	{	0,	"rmp",		TG5,		0000021	},
	{	0,	"spn",		TG5,		0100200	},
	{	0,	"sps",		TG5,		0101200	},

	{	0,	"erm",		TG5,		0001401	},

	{	0,	"nrm",		TGHSA,		0000101	},
	{	0,	"sca",		TGHSA,		0000041	},
	{	0,	"dbl",		TGHSA,		0000007	},
	{	0,	"sgl",		TGHSA,		0000005	},
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
	"unexpected character",		/* 10 */
	"phase error",			/* 11 */
	"multiple definitions",		/* 12 */
	"syntax error",			/* 13 */
	"must be absolute",		/* 14 */
	"missing delimiter",		/* 15 */
	"invalid constant",		/* 16 */
	"out of range",			/* 17 */
	"required feature set not enabled",	/* 18 */
	"address required",		/* 19 */
	"invalid id",			/* 20 */
	"constant out of range",	/* 21 */
	"data in BSS",			/* 22 */
	"segment overflow",		/* 23 */
	"unknown symbol",		/* 24 */
	"divide by zero",		/* 25 */
	"segment conflict",		/* 26 */
	"cannot be indexed"		/* 27 */
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
