/*
 * Perkin Elmer 16 assembler.
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

	/* Instructions are highly regular with only 4 real formats. As
	   was common for the period the instruction name indicates the format */
	{	0,	"lhr",		TRR,		0x08	},
	{	0,	"lis",		TSF,		0x24	},
	{	0,	"lcs",		TSF,		0x25	},
	{	0,	"lh",		TRX,		0x48	},
	{	0,	"lhi",		TRI,		0xC8	},
	{	0,	"lm",		TRX,		0xD1	},
	{	0,	"lb",		TRX,		0xD3	},
	{	0,	"lbr",		TRR,		0x93	},
	{	0,	"exbr",		TRR,		0x94	},
	{	0,	"sth",		TRX,		0x40	},
	{	0,	"stm",		TRX,		0xD0	},
	{	0,	"stb",		TRX,		0xD2	},
	{	0,	"stbr",		TRR,		0x92	},
	{	0,	"clh",		TRX,		0x45	},
	{	0,	"clhr",		TRR,		0x05	},
	{	0,	"clhi",		TRI,		0xC5	},
	{	0,	"clb",		TRX,		0xD4	},
	{	0,	"nh",		TRX,		0x44	},
	{	0,	"nhr",		TRR,		0x04	},
	{	0,	"nhi",		TRI,		0xC4	},
	{	0,	"oh",		TRX,		0x46	},
	{	0,	"ohr",		TRR,		0x06	},
	{	0,	"ohi",		TRI,		0xC6	},
	{	0,	"xh",		TRX,		0x47	},
	{	0,	"xhr",		TRR,		0x07	},
	{	0,	"xhi",		TRR,		0xC7	},
	{	0,	"thi",		TRI,		0xC3	},
	{	0,	"sll",		TRIL,		0xED	},
	{	0,	"srl",		TRIL,		0xEC	},
	{	0,	"slhl",		TRI,		0xCD	},
	{	0,	"slls",		TSF,		0x91	},
	{	0,	"srhl",		TRI,		0xCC	},
	{	0,	"srls",		TSF,		0x90	},
	{	0,	"rll",		TRI,		0xEB	},
	{	0,	"rrl",		TRI,		0xEA	},
	{	0,	"atl",		TRX,		0x64	},
	{	0,	"abl",		TRX,		0x65	},
	{	0,	"rtl",		TRX,		0x66	},
	{	0,	"rbl",		TRX,		0x67	},
	{	0,	"btc",		TRX,		0x42	},
	{	0,	"btcr",		TRR,		0x02	},
	{	0,	"btbs",		TSF,		0x20	},
	{	0,	"btfs",		TSF,		0x21	},
	{	0,	"bfc",		TRX,		0x43	},
	{	0,	"bfcr",		TRR,		0x03	},
	{	0,	"bfbs",		TSF,		0x22	},
	{	0,	"bffs",		TSF,		0x23	},
	{	0,	"bal",		TRX,		0x41	},
	{	0,	"balr",		TRR,		0x01	},
	{	0,	"bxle",		TRX,		0xC1	},
	{	0,	"bxh",		TRX,		0xC0	},
	/* TODO aliases for branches (page 4-7) */	
	{	0,	"ah",		TRX,		0x4A	},
	{	0,	"ahr",		TRR,		0x0A	},
	{	0,	"ahi",		TRI,		0xCA	},
	{	0,	"ais",		TSF,		0x26	},
	{	0,	"ahm",		TRX,		0x61	},
	{	0,	"sh",		TRX,		0x4B	},
	{	0,	"shr",		TRR,		0x0B	},
	{	0,	"shi",		TRI,		0xCB	},
	{	0,	"sis",		TSF,		0x27	},
	{	0,	"ach",		TRX,		0x4E	},
	{	0,	"achr",		TRR,		0x0E	},
	{	0,	"sch",		TRX,		0x4F	},
	{	0,	"schr",		TRR,		0x0F	},
	{	0,	"ch",		TRX,		0x49	},
	{	0,	"chr",		TRR,		0x05	},
	{	0,	"chi",		TRI,		0xC9	},
	{	0,	"mh",		TRXL,		0x4C	},
	{	0,	"mhr",		TRRL,		0x0C	},
	{	0,	"mhu",		TRXL,		0xDC	},
	{	0,	"mhur",		TRRL,		0x9C	},
	{	0,	"dh",		TRXL,		0x4D	},
	{	0,	"dhr",		TRRL,		0x0D	},
	{	0,	"sla",		TRIL,		0xEF	},
	{	0,	"srha",		TRI,		0xCE	},
	{	0,	"sra",		TRI,		0xEE	},
	{	0,	"slha",		TRI,		0xCF	},
	{	0,	"le",		TRXF,		0x68	},
	{	0,	"ler",		TRRF,		0x28	},
	{	0,	"lme",		TRXF,		0x72	},
	{	0,	"ste",		TRXF,		0x60	},
	{	0,	"stme",		TRXF,		0x71	},
	{	0,	"ae",		TRXF,		0x6A	},
	{	0,	"aer",		TRRF,		0x2A	},
	{	0,	"se",		TRXF,		0x6B	},
	{	0,	"ser",		TRRF,		0x2B	},
	{	0,	"ce",		TRXF,		0x69	},
	{	0,	"cer",		TRRF,		0x29	},
	{	0,	"me",		TRXF,		0x6C	},
	{	0,	"mer",		TRRF,		0x2C	},
	{	0,	"de",		TRXF,		0x6D	},
	{	0,	"der",		TRRF,		0x2D	},
	{	0,	"fxr",		TRR,		0x2E	},
	{	0,	"flr",		TRR,		0x2F	},
	{	0,	"ld",		TRXF,		0x78	},
	{	0,	"ldr",		TRRF,		0x38	},
	{	0,	"lmd",		TRXF,		0x7F	},
	{	0,	"std",		TRXF,		0x70	},
	{	0,	"stmd",		TRXF,		0x7E	},
	{	0,	"ad",		TRXF,		0x7A	},
	{	0,	"adr",		TRRF,		0x3A	},
	{	0,	"sd",		TRXF,		0x7B	},
	{	0,	"sdr",		TRRF,		0x3B	},
	{	0,	"cd",		TRXF,		0x79	},
	{	0,	"cdr",		TRRF,		0x39	},
	{	0,	"md",		TRXF,		0x7C	},
	{	0,	"mdr",		TRRF,		0x3C	},
	{	0,	"dd",		TRXF,		0x7D	},
	{	0,	"ddr",		TRRF,		0x3D	},
	{	0,	"fxdr",		TRR,		0x3E	},
	{	0,	"fldr",		TRR,		0x3F	},
	{	0,	"lpsr",		TRR,		0x33	},
	{	0,	"lps",		TRX,		0x73	},
	{	0,	"setmr",	TRR,		0x13	},
	{	0,	"setm",		TRX,		0x53	},
	{	0,	"lpsw",		TRX,		0xC2	},
	{	0,	"epsr",		TRR,		0x95	},
	{	0,	"sint",		TRX,		0xE2	},
	{	0,	"svc",		TRX,		0xE1	},
	{	0,	"ack",		TRX,		0xDF	},
	{	0,	"ackr",		TRR,		0x9F	},
	{	0,	"ss",		TRX,		0xDD	},
	{	0,	"ssr",		TRR,		0x9D	},
	{	0,	"oc",		TRX,		0xDE	},
	{	0,	"ocr",		TRR,		0x9E	},
	{	0,	"rd",		TRX,		0xDB	},
	{	0,	"rdr",		TRR,		0x9B	},
	{	0,	"rh",		TRX,		0xD9	},
	{	0,	"rhr",		TRR,		0x99	},
	{	0,	"rb",		TRX,		0xD7	},
	{	0,	"rbr",		TRR,		0x97	},
	{	0,	"wd",		TRX,		0xDA	},
	{	0,	"wdr",		TRR,		0x9A	},
	{	0,	"wh",		TRX,		0xD8	},
	{	0,	"whr",		TRR,		0x98	},
	{	0,	"wb",		TRX,		0xD6	},
	{	0,	"wbr",		TRR,		0x96	},
	{	0,	"al",		TRX,		0xD5,	},
	/* Actually a branch alias */
	{	0,	"nopr",		TIMPL,		0x0200	},
	{	0,	"nop",		TIMPLZ,		0x4200	},
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
	"short form out of range",	/* 17 */
	"cannot index with r0",		/* 18 */
	"address required",		/* 19 */
	"invalid id",			/* 20 */
	"register required",		/* 21 */
	"register must be even",	/* 22 */
	"constant out of range",	/* 23 */
	"data in BSS",			/* 24 */
	"segment overflow",		/* 25 */
	"data in ZP",			/* 26 */
	"segment conflict",		/* 27 */
	"invalid register number",	/* 28 */
	"divide by zero"		/* 29 */
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
