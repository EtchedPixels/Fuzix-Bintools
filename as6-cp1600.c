/*
 * CP1600 assembler.
 *
 * No 10bit support at this point.
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
	{	0,	"common",	TSEGMENT,	COMMON	},
	{	0,	"zp",		TSEGMENT,	ZP	},
	{	0,	".abs",		TSEGMENT,	ABSOLUTE},
	{	0,	".code",	TSEGMENT,	CODE	},
	{	0,	".data",	TSEGMENT,	DATA	},
	{	0,	".bss",		TSEGMENT,	BSS	},
	{	0,	".discard",	TSEGMENT,	DISCARD	},
	{	0,	".common",	TSEGMENT,	COMMON	},
	{	0,	".zp",		TSEGMENT,	ZP	},
	{	0,	".literal",	TSEGMENT,	LITERAL	},
	{	0,	".commondata",	TSEGMENT,	COMMONDATA },
	{	0,	".buffers",	TSEGMENT,	BUFFERS	},

	/*{	0,	".bits",	TBITS,		0	}, */

	/* Reg/mmemory instructions, also includes indirected, and
	   immediates */
	{	0,	"mvo",		TRA,		0x240	},
	{	0,	"mvi",		TAR,		0x280	},
	{	0,	"add",		TAR,		0x2C0	},
	{	0,	"sub",		TAR,		0x300	},
	{	0,	"cmp",		TAR,		0x340	},
	{	0,	"and",		TAR,		0x380	},
	{	0,	"xor",		TAR,		0x3C0	},

	{	0,	"mvii",		TARI,		0x280	},
	{	0,	"addi",		TARI,		0x2C0	},
	{	0,	"subi",		TARI,		0x300	},
	{	0,	"cmpi",		TARI,		0x340	},
	{	0,	"andi",		TARI,		0x380	},
	{	0,	"xori",		TARI,		0x3C0	},

	/* Weird jump instruction formats */
	{	0,	"j",		TJMP,		0x00	},
	{	0,	"jd",		TJMP,		0x01	},
	{	0,	"je",		TJMP,		0x02	},
	{	0,	"jsr",		TJSR,		0x00	},
	{	0,	"jsrd",		TJSR,		0x01	},
	{	0,	"jsre",		TJSR,		0x02	},
	
	/* Relative branches */
	{	0,	"b",		TBRA,		0x200	},
	{	0,	"bc",		TBRA,		0x201	},
	{	0,	"blgt",		TBRA,		0x201	},
	{	0,	"bnc",		TBRA,		0x209	},
	{	0,	"bllt",		TBRA,		0x209	},
	{	0,	"bov",		TBRA,		0x202	},
	{	0,	"bnov",		TBRA,		0x20A	},
	{	0,	"bpl",		TBRA,		0x203	},
	{	0,	"bmi",		TBRA,		0x20B	},
	{	0,	"bze",		TBRA,		0x204	},
	{	0,	"beq",		TBRA,		0x204	},
	{	0,	"bnze",		TBRA,		0x20C	},
	{	0,	"bneq",		TBRA,		0x20C	},
	{	0,	"blt",		TBRA,		0x205	},
	{	0,	"bge",		TBRA,		0x20D	},
	{	0,	"ble",		TBRA,		0x206	},
	{	0,	"bgt",		TBRA,		0x20E	},
	{	0,	"busc",		TBRA,		0x207	},
	{	0,	"besc",		TBRA,		0x20F	},

	{	0,	"bext",		TBEXT,		0x210	},

	/* Reg, reg ops */
	/* These mirror the r/addr ops but without 0x200 bit set and
	   with no mvi equivalent */
	{	0,	"movr",		TRR,		0x080	},
	{	0,	"addr",		TRR,		0x0C0	},
	{	0,	"subr",		TRR,		0x100	},
	{	0,	"cmpr",		TRR,		0x140	},
	{	0,	"andr",		TRR,		0x180	},
	{	0,	"xorr",		TRR,		0x1C0	},

	/* Reg ops */
	{	0,	"incr",		TR,		0x08	},
	{	0,	"decr",		TR,		0x10	},
	{	0,	"comr",		TR,		0x18	},
	{	0,	"negr",		TR,		0x20	},
	{	0,	"adcr",		TR,		0x28	},
	{	0,	"gswd",		TGSWD,		0x30	},
	{	0,	"rswd",		TR,		0x38	},
	{	0,	"swap",		TSHIFT,		0x40	},
	{	0,	"sll",		TSHIFT,		0x48	},
	{	0,	"rlc",		TSHIFT,		0x50	},
	{	0,	"sllc",		TSHIFT,		0x58	},
	{	0,	"slr",		TSHIFT,		0x60	},
	{	0,	"sar",		TSHIFT,		0x68	},
	{	0,	"rrc",		TSHIFT,		0x70	},
	{	0,	"sarc",		TSHIFT,		0x78	},

	/* Implicit forms */
	{	0,	"hlt",		TIMPL,		0x00	},
	{	0,	"eis",		TIMPL,		0x02	},
	{	0,	"dis",		TIMPL,		0x03	},
	{	0,	"tci",		TIMPL,		0x05	},
	{	0,	"clrc",		TIMPL,		0x06	},
	{	0,	"setc",		TIMPL,		0x07	},

	{	0,	"nop",		TSIN,		0x34	},
	{	0,	"sin",		TSIN,		0x36	},

	/* Special prefix - requires mucho magic to handle TODO */
	{	0,	"sbdb",		TSBDB,		0x01	},

	/* Aliases */
	/* movr to r7 */
	{	0,	"jr",		TJR,		0x87	},
	/* move @r6 forms */
	{	0,	"pshr",		TR,		0x270	},
	{	0,	"pulr",		TR,		0x2B0	},
	/* sub r,r */
	{	0,	"clrr",		TRSAME,		0x1C0	},
	/* branch never */
	{	0,	"nopp",		TBRA,		0x208	},
	/* cmp r,r */
	{	0,	"tstr",		TRSAME,		0x80	},
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
	"register required",		/* 18 */
	"address required",		/* 19 */
	"invalid ID",			/* 20 */
	"invalid register",		/* 21 */
	"divide by 0",			/* 22 */
	"constant out of range",	/* 23 */
	"data in BSS",			/* 24 */
	"segment overflow",		/* 25 */
	"data in direct page",		/* 26 */
	"segment conflict",		/* 27 */
	"unsupported in 10bit mode",	/* 28 */
	"too many Jcc instructions",	/* 29 */
	"invalid instruction form"	/* 30 */
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
