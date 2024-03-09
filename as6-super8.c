/*
 * Super8 assembler. (Z8 8800 series)
 *
 * This is almost but not quite a Z8. There are instruction and encoding
 * differences and as it and the Z8 classic are not compatible we keep the
 * two assemblers separate - for now at least.
 */

#include	"as.h"

SYM	sym[] = {
	{	0,	"defb",		TDEFB,		XXXX	},
	{	0,	"defw",		TDEFW,		XXXX	},
	{	0,	"defs",		TDEFS,		XXXX	},
	{	0,	"defm",		TDEFM,		XXXX	},
	{	0,	"org",		TORG,		XXXX	},
	{	0,	"equ",		TEQU,		XXXX	},
	{	0,	"export",	TEXPORT,	XXXX	},
	{	0,	".byte",	TDEFB,		XXXX	},
	{	0,	".word",	TDEFW,		XXXX	},
	{	0,	".blkb",	TDEFS,		XXXX	},
	{	0,	".ds",		TDEFS,		XXXX	},
	{	0,	".ascii",	TDEFM,		XXXX	},
	{	0,	".org",		TORG,		XXXX	},
	{	0,	".equ",		TEQU,		XXXX	},
	{	0,	".export",	TEXPORT,	XXXX	},
	{	0,	"abs",		TSEGMENT,	ABSOLUTE},
	{	0,	"code",		TSEGMENT,	CODE	},
	{	0,	"data",		TSEGMENT,	DATA	},
	{	0,	"bss",		TSEGMENT,	BSS	},
	{	0,	"discard",	TSEGMENT,	DISCARD	},
	{	0,	"zp",		TSEGMENT,	ZP	},
	{	0,	"common",	TSEGMENT,	COMMON	},
	{	0,	"literal",	TSEGMENT,	LITERAL	},
	{	0,	"commondata",	TSEGMENT,	COMMONDATA },
	{	0,	"buffers",	TSEGMENT,	BUFFERS	},
	{	0,	".abs",		TSEGMENT,	ABSOLUTE},
	{	0,	".code",	TSEGMENT,	CODE	},
	{	0,	".data",	TSEGMENT,	DATA	},
	{	0,	".bss",		TSEGMENT,	BSS	},
	{	0,	".discard",	TSEGMENT,	DISCARD	},
	{	0,	".zp",		TSEGMENT,	ZP	},
	{	0,	".common",	TSEGMENT,	COMMON	},
	{	0,	".literal",	TSEGMENT,	LITERAL	},
	{	0,	".commondata",	TSEGMENT,	COMMONDATA },
	{	0,	".buffers",	TSEGMENT,	BUFFERS	},
	
	/* Condition codes */
	
	{	0,	"f",		TCC,		0x00	},
	{	0,	"lt",		TCC,		0x01	},
	{	0,	"le",		TCC,		0x02	},
	{	0,	"ule",		TCC,		0x03	},
	{	0,	"ov",		TCC,		0x04	},
	{	0,	"mi",		TCC,		0x05	},
	{	0,	"eq",		TCC,		0x06	},
	{	0,	"z",		TCC,		0x06	},
	{	0,	"c",		TCC,		0x07	},
	{	0,	"ult",		TCC,		0x07	},
	/* 0x08 is 'true' */
	{	0,	"ge",		TCC,		0x09	},
	{	0,	"gt",		TCC,		0x0A	},
	{	0,	"ugt",		TCC,		0x0B	},
	{	0,	"nov",		TCC,		0x0C	},
	{	0,	"pl",		TCC,		0x0D	},
	{	0,	"ne",		TCC,		0x0E	},
	{	0,	"nz",		TCC,		0x0E	},
	{	0,	"nc",		TCC,		0x0F	},
	{	0,	"uge",		TCC,		0x0F	},

	/* The Z8 has a gloriously clean instruction set */
	
	{	0,	"add",		TOP4BIT,	0x00	},
	{	0,	"adc",		TOP4BIT,	0x10	},
	{	0,	"sub",		TOP4BIT,	0x20	},
	{	0,	"sbc",		TOP4BIT,	0x30	},
	{	0,	"or",		TOP4BIT,	0x40	},
	{	0,	"and",		TOP4BIT,	0x50	},
	{	0,	"tcm",		TOP4BIT,	0x60	},
	{	0,	"tm",		TOP4BIT,	0x70	},
	/* 8 and 9 are a collection of operations stuffed into the space
	   on the Super8 */
	{	0,	"cp",		TOP4BIT,	0xA0	},
	{	0,	"xor",		TOP4BIT,	0xB0	},
	/* C and D likewise */
	{	0,	"jr",		TCRA,		0x0B	},
	/* jp is an oddity as it's got two real forms 30 and xD */
	{	0,	"jp",		TJMP,		0x0D	},

	/* Implicit */	
	{	0,	"next",		TIMPL,		0x0F	},
	{	0,	"enter",	TIMPL,		0x1F	},
	{	0,	"exit",		TIMPL,		0x2F	},
	{	0,	"wfi",		TIMPL,		0x3F	},
	{	0,	"sb0",		TIMPL,		0x4F	},
	{	0,	"sb1",		TIMPL,		0x5F	},
	/* 6F and 7F are unused */
	{	0,	"di",		TIMPL,		0x8F	},
	{	0,	"ei",		TIMPL,		0x9F	},
	{	0,	"ret",		TIMPL,		0xAF	},
	{	0,	"iret",		TIMPL,		0xBF	},
	{	0,	"rcf",		TIMPL,		0xCF	},
	{	0,	"scf",		TIMPL,		0xDF	},
	{	0,	"ccf",		TIMPL,		0xEF	},
	{	0,	"nop",		TIMPL,		0xFF	},

	/* R / IR */
	{	0,	"dec",		TRIR,		0x00	},
	{	0,	"rlc",		TRIR,		0x10	},
	/* inc is weird and has two forms - we handle the r form in the code
	   as a special case */
	{	0,	"inc",		TRIR,		0x20	},
	/* 30 is used for jump IRR 31 is SRP/SRP0/SRP1 */
	{	0,	"da",		TRIR,		0x40	},
	{	0,	"pop",		TRIR,		0x50	},
	{	0,	"com",		TRIR,		0x60	},
	{	0,	"push",		TRIR,		0x70	},
	{	0,	"decw",		TRRIR,		0x80	},
	{	0,	"rl",		TRIR,		0x90	},
	{	0,	"incw",		TRRIR,		0xA0	},
	{	0,	"clr",		TRIR,		0xB0	},
	{	0,	"rrc",		TRIR,		0xC0	},
	{	0,	"sra",		TRIR,		0xD0	},
	{	0,	"rr",		TRIR,		0xE0	},
	{	0,	"swap",		TRIR,		0xF0	},

	/* x7 ops (one less short form than the Z8 classic to make
	   room for them */
	{	0,	"bor",		TBIT,		0x07	},
	{	0,	"bcp",		TBCP,		0x17	},
	{	0,	"bxor",		TBIT,		0x27	},
	{	0,	"btjrf",	TBJMP,		0x0037	},
	{	0,	"btjrt",	TBJMP,		0x0137	},
	{	0,	"ldb",		TBIT,		0x47	},
	{	0,	"bitc",		TBITX,		0x57	},
	{	0,	"band",		TBIT,		0x67	},
	{	0,	"bitr",		TBITX,		0x0077	},
	{	0,	"bits",		TBITX,		0x0177	},
	/* 87 and 97 are LD */
	{	0,	"ldc",		TLDC,		0x00A7	},
	{	0,	"lde",		TLDC,		0x01A7	},
	/* B7 is another LDC form */
	/* C7/D7 are LD, E7 is an LDC form, F7 is an LDC form */

	/* The less regular bits */
	{	0,	"srp",		TIMM8M,		0x0031	},
	{	0,	"srp0",		TIMM8M,		0x0231	},
	{	0,	"srp1",		TIMM8M,		0x0131	},

	/* Call has a weird mix of scattered forms */
	{	0,	"call",		TCALL,		0xD4	},
	{	0,	"djnz",		TRA,		0x0A	},

	/* New ops */
	{	0,	"pushud",	TPUSHU,		0x82	},
	{	0,	"pushui",	TPUSHU,		0x83	},
	{	0,	"popud",	TPOPU,		0x92	},
	{	0,	"popui",	TPOPU,		0x93	},
	/* Maths ops */
	{	0,	"mult",		TMULDIV,	0x84	},
	{	0,	"div",		TMULDIV,	0x94	},
	/* Compare inc branch */
	{	0,	"cpije",	TCPIJ,		0xC2	},
	{	0,	"cpijne",	TCPIJ,		0xD2	},

	{	0,	"ldcd",		TLDCI,		0x00E2	},
	{	0,	"lded",		TLDCI,		0x01E2	},
	{	0,	"ldci",		TLDCI,		0x00E3	},
	{	0,	"ldei",		TLDCI,		0x01E3	},
	{	0,	"ldcpd",	TLDCPI,		0x00F2	},
	{	0,	"ldepd",	TLDCPI,		0x01F2	},
	{	0,	"ldcpi",	TLDCPI,		0x00F3	},
	{	0,	"ldepi",	TLDCPI,		0x01F3	},

	/* LDW is all over */
	{	0,	"ldw",		TLDW,		0xC4	},

	/* And load which has a small army of forms */
	{	0,	"ld",		TLOAD,		0x00	},
	
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
	"Bcc out of range",		/* 17 */
	"register 0-15 required",	/* 18 */
	"address required",		/* 19 */
	"invalid ID",			/* 20 */
	"invalid instruction form",	/* 21 */
	"divide by 0",			/* 22 */
	"constant out of range",	/* 23 */
	"data in BSS",			/* 24 */
	"segment overflow",		/* 25 */
	"data in ZP",			/* 26 */
	"segment conflict",		/* 27 */
	"register must be even",	/* 28 */
	"too manj jr instructions",	/* 29 */
	"invalid bit number",		/* 30 */
	"register may not be 0 or 1"	/* 31 */
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
