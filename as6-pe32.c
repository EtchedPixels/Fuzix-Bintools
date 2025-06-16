/*
 * Perkin Elmer 32 assembler.
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

	{	0,	"l",		TRX,		0x58	},
	{	0,	"lr",		TRR,		0x08	},
	{	0,	"li",		TRI2,		0xF8	},
	{	0,	"lis",		TSF,		0x24	},
	{	0,	"lcs",		TSF,		0x25	},
	{	0,	"lh",		TRX,		0x48	},
	{	0,	"lhi",		TRI1,		0xC8	},
	{	0,	"la",		TRX,		0xE6	},
	{	0,	"lra",		TRX,		0x63	},
	{	0,	"lhl",		TRX,		0x73	},
	{	0,	"lm",		TRX,		0xD1	},
	{	0,	"lb",		TRX,		0xD3	},
	{	0,	"lbr",		TRR,		0x93	},
	{	0,	"exhr",		TRR,		0x34	},
	{	0,	"exbr",		TRR,		0x94	},
	{	0,	"st",		TRX,		0x50	},
	{	0,	"sth",		TRX,		0x40	},
	{	0,	"stm",		TRX,		0xD0	},
	{	0,	"stb",		TRX,		0xD2	},
	{	0,	"stbr",		TRR,		0x92	},
	{	0,	"cl",		TRX,		0x55	},
	{	0,	"clr",		TRR,		0x05	},
	{	0,	"cli",		TRI2,		0xF5	},
	{	0,	"clh",		TRX,		0x45	},
	{	0,	"clhi",		TRI1,		0xC5	},
	{	0,	"clb",		TRX,		0xD4	},
	{	0,	"n",		TRX,		0x54	},
	{	0,	"nr",		TRR,		0x04	},
	{	0,	"ni",		TRI2,		0xF4	},
	{	0,	"nh",		TRX,		0x44	},
	{	0,	"nhi",		TRI1,		0xC4	},
	{	0,	"o",		TRX,		0x56	},
	{	0,	"or",		TRR,		0x06	},
	{	0,	"oi",		TRI2, 		0xF6	},
	{	0,	"oh",		TRX,		0x46	},
	{	0,	"ohi",		TRI1,		0xC6	},
	{	0,	"x",		TRX,		0x57	},
	{	0,	"xr",		TRR,		0x07	},
	{	0,	"xi",		TRI2,		0xF7	},
	{	0,	"xh",		TRX,		0x47	},
	{	0,	"xhi",		TRI1,		0xC7	},
	{	0,	"ti",		TRI2,		0xF3	},
	{	0,	"thi",		TRI1,		0xC3	},
	{	0,	"sll",		TRI1,		0xED	},
	{	0,	"slls",		TSF,		0x11	},
	{	0,	"srl",		TRI1,		0xEC	},
	{	0,	"srls",		TSF,		0x10	},
	{	0,	"slhl",		TRI1,		0xCD	},
	{	0,	"slhls",	TSF,		0x91	},
	{	0,	"srhl",		TRI1,		0xCC	},
	{	0,	"srhls",	TSF,		0x90	},
	{	0,	"rll",		TRI1,		0xEB	},
	{	0,	"rrl",		TRI1,		0xEA	},
	{	0,	"ts",		TRX,		0xE0	},
	{	0,	"tbt",		TRX,		0x74	},
	{	0,	"sbt",		TRX,		0x75	},
	{	0,	"cbt",		TRX,		0x77	},
	{	0,	"rbt",		TRX,		0x76	},
	{	0,	"crc12",	TRX,		0x5E	},
	{	0,	"crc16",	TRX,		0x5F	},
	{	0,	"tlate",	TRX,		0xE7	},
	{	0,	"atl",		TRX,		0x64	},
	{	0,	"abl",		TRX,		0x65	},
	{	0,	"rtl",		TRX,		0x66	},
	{	0,	"rbl",		TRX,		0x67	},
	{	0,	"a",		TRX,		0x5A	},
	{	0,	"ar",		TRR,		0x0A	},
	{	0,	"ai",		TRI2,		0xFA	},
	{	0,	"ais",		TSF,		0x26	},
	{	0,	"ah",		TRX,		0x4A	},
	{	0,	"ahi",		TRI1,		0xCA	},
	{	0,	"am",		TRX,		0x51	},
	{	0,	"ahm",		TRX,		0x61	},
	{	0,	"s",		TRX,		0x5B	},
	{	0,	"sr",		TRR,		0x0B	},
	{	0,	"si",		TRI2,		0xFB	},
	{	0,	"sis",		TSF,		0x27	},
	{	0,	"sh",		TRX,		0x4B	},
	{	0,	"shi",		TRI1,		0xCB	},
	{	0,	"c",		TRX,		0x59	},
	{	0,	"cr",		TRR,		0x09	},
	{	0,	"ci",		TRI2,		0xF9	},
	{	0,	"ch",		TRX,		0x49	},
	{	0,	"chi",		TRI1,		0xC9	},
	{	0,	"m",		TRXF,		0x5C	},
	{	0,	"mr",		TRRF,		0x1C	},
	{	0,	"mh",		TRX,		0x4C	},
	{	0,	"mhr",		TRR,		0x0C	},
	{	0,	"d",		TRXF,		0x5D	},
	{	0,	"dr",		TRRF,		0x1D	},
	{	0,	"dh",		TRX,		0x4D	},
	{	0,	"dhr",		TRR,		0x0D	},
	{	0,	"sla",		TRI1,		0xEF	},
	{	0,	"slha",		TRI1,		0xCF	},
	{	0,	"sra",		TRI1,		0xEE	},
	{	0,	"srha",		TRI1,		0xCE	},
	{	0,	"chvr",		TRR,		0x12	},
	/* Floats. Same formats but referencing fp regs */
	/* FP single precision optional feature */
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
	/* FP single and double precision option */
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
	/* Supervisory ops */
	{	0,	"lpsw",		TRX0,		0xC2	},
	{	0,	"lpswr",	TR0,		0x18	},
	{	0,	"epsr",		TRR,		0x95	},
	{	0,	"sint",		TRI1,		0xE2	},
	{	0,	"svc",		TSVC,		0xE1	},
	/* I/O instructions */
	{	0,	"oc",		TRX,		0xDE	},
	{	0,	"ocr",		TRR,		0x9E	},
	{	0,	"ss",		TRX,		0xDD	},
	{	0,	"ssr",		TRR,		0x9D	},
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
	{	0,	"al",		TRX,		0xD5	},
	{	0,	"scp",		TRX,		0xE3	},
	/* Optional processing ops */
	{	0,	"pb",		TRX,		0x62	},
	{	0,	"pbr",		TRR,		0x32	},
	{	0,	"mpbsr",	TRR,		0x30	},
	/* Branches - same formats but first reg becomes cond code */
	/* offsets in SF form are in 16bit half words */
	{	0,	"btc",		TBFRX,		0x42	},
	{	0,	"btcr",		TBFRR,		0x02	},
	{	0,	"btbs",		TBFSF,		0x20	},
	{	0,	"btfs",		TBFSF,		0x21	},
	{	0,	"bfc",		TBFRX,		0x43	},
	{	0,	"bfcr",		TBFRR,		0x03	},
	{	0,	"bfbs",		TSF,		0x22	},
	{	0,	"bffs",		TSF,		0x23	},
	{	0,	"bal",		TRX,		0x41	},
	{	0,	"balr",		TRR,		0x01	},
	{	0,	"bxle",		TRX,		0xC1	},
	{	0,	"bxh",		TRX,		0xC0	},
	/* Branch aliases */
	{	0,	"bc",		TBRX,		0x4280	},
	{	0,	"bcr",		TBRR,		0x28	},
	{	0,	"bcs",		TBSF,		0x2080	},
	{	0,	"bnc",		TBRX,		0x0438	},
	{	0,	"bncr",		TBRR,		0x38	},
	{	0,	"bncs",		TBSF,		0x2280	},
	{	0,	"be",		TBRX,		0x4330	},
	{	0,	"ber",		TBRR,		0x33	},
	{	0,	"bes",		TBSF,		0x2230	},
	{	0,	"bne",		TBRX,		0x4230	},
	{	0,	"bner",		TBRR,		0x23	},
	{	0,	"bnes",		TBSF,		0x2030	},
	{	0,	"bl",		TBRX,		0x4280	},
	{	0,	"blr",		TBRR,		0x28	},
	{	0,	"bls",		TBSF,		0x2080	},
	{	0,	"bnl",		TBRX,		0x4380	},
	{	0,	"bnlr",		TBRR,		0x38	},
	{	0,	"bnls",		TBSF,		0x2280	},
	{	0,	"bm",		TBRX,		0x4210	},
	{	0,	"bmr",		TBRR,		0x21	},
	{	0,	"bms",		TBSF,		0x2010	},
	{	0,	"bnm",		TBRX,		0x4310	},
	{	0,	"bnmr",		TBRR,		0x31	},
	{	0,	"bnms",		TBSF,		0x2210	},
	{	0,	"bp",		TBRX,		0x4220	},
	{	0,	"bpr",		TBRR,		0x22	},
	{	0,	"bps",		TBSF,		0x2020	},
	{	0,	"bnp",		TBRX,		0x4320	},
	{	0,	"bnpr",		TBRR,		0x32	},
	{	0,	"bnps",		TBSF,		0x2220	},
	{	0,	"bo",		TBRX,		0x4240	},
	{	0,	"bor",		TBRR,		0x24	},
	{	0,	"bos",		TBSF,		0x2040	},
	{	0,	"bno",		TBRX,		0x4340	},
	{	0,	"bnor",		TBRR,		0x34	},
	{	0,	"bnos",		TBSF,		0x2240	},
	{	0,	"bz",		TBRX,		0x4330	},
	{	0,	"bzr",		TBRR,		0x33	},
	{	0,	"bzs",		TBSF,		0x2230	},
	{	0,	"bnz",		TBRX,		0x4230	},
	{	0,	"bnzr",		TBRR,		0x23	},
	{	0,	"bnzs",		TBSF,		0x2030	},
	{	0,	"b",		TBRX,		0x4300	},
	{	0,	"br",		TBRR,		0x30	},
	{	0,	"bs",		TBSF,		0x2200	},
	{	0,	"nop",		TBRX,		0x4200	},
	{	0,	"nopr",		TBRR,		0x20	},
	/* 8/32 ops */
	/* MMU translating version of la giving bus addresses. On later 7/32
	   and 8/32 */
	{	0,	"lra",		TRX,		0x63	},
	/* Control store */
	{	0,	"wdcs",		TWDCS,		0xE80	},
	{	0,	"rdcs",		TRDCS,		0xE82	},
	{	0,	"bdcs",		TRX,		0xE5	},
	{	0,	"ecs",		TRI1,		0xE9	}
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
