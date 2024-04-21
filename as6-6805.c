/*
 * 6303 assembler.
 *
 * Tables
 *
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

	{	0,	".05",		TSETCPU,	6805	},
	{	0,	".hc05",	TSETCPU,	6805	},
	{	0,	".hc08",	TSETCPU,	6808	},

	{	0,	"adc",		TMEM,		0x09	},
	{	0,	"add",		TMEM,		0x0B	},
	{	0,	"and",		TMEM,		0x04	},
	{	0,	"bit",		TMEM,		0x05	},
	{	0,	"cmp",		TMEM,		0x01	},
	{	0,	"cpx",		TMEM,		0x03	},
	{	0,	"eor",		TMEM,		0x08	},
	{	0,	"lda",		TMEM,		0x06	},
	{	0,	"ldx",		TMEM,		0x0E	},
	{	0,	"ora",		TMEM,		0x0A	},
	{	0,	"sbc",		TMEM,		0x02	},
	{	0,	"sta",		TMEMNI,		0x07	},
	{	0,	"stx",		TMEMNI,		0x0F	},
	{	0,	"sub",		TMEM,		0x00	},
	
	{	0,	"jmp",		TJMP,		0x0C	},
	{	0,	"jsr",		TJMP,		0x0D	},

	{	0,	"asl",		TRMW,		0x08	},
	{	0,	"asla",		TIMPL,		0x48	},
	{	0,	"aslx",		TIMPL,		0x58	},
	{	0,	"asr",		TRMW,		0x07	},
	{	0,	"asra",		TIMPL,		0x47	},
	{	0,	"asrx",		TIMPL,		0x57	},
	{	0,	"clr",		TRMW,		0x0F	},
	{	0,	"clra",		TIMPL,		0x4F	},
	{	0,	"clrx",		TIMPL,		0x5F	},
	{	0,	"com",		TRMW,		0x03	},
	{	0,	"coma",		TIMPL,		0x43	},
	{	0,	"comx",		TIMPL,		0x53	},
	{	0,	"dec",		TRMW,		0x0A	},
	{	0,	"deca",		TIMPL,		0x4A	},
	{	0,	"decx",		TIMPL,		0x5A	},
	{	0,	"inc",		TRMW,		0x0C	},
	{	0,	"inca",		TIMPL,		0x4C	},
	{	0,	"incx",		TIMPL,		0x5C	},
	{	0,	"lsl",		TRMW,		0x08	},
	{	0,	"lsla",		TIMPL,		0x48	},
	{	0,	"lslx",		TIMPL,		0x58	},
	{	0,	"lsr",		TRMW,		0x04	},
	{	0,	"lsra",		TIMPL,		0x44	},
	{	0,	"lsrx",		TIMPL,		0x54	},
	{	0,	"neg",		TRMW,		0x00	},
	{	0,	"nega",		TIMPL,		0x40	},
	{	0,	"negx",		TIMPL,		0x50	},
	{	0,	"rol",		TRMW,		0x09	},
	{	0,	"rola",		TIMPL,		0x49	},
	{	0,	"rolx",		TIMPL,		0x59	},
	{	0,	"ror",		TRMW,		0x06	},
	{	0,	"rora",		TIMPL,		0x46	},
	{	0,	"rorx",		TIMPL,		0x56	},
	{	0,	"tst",		TRMW,		0x0D	},
	{	0,	"tsta",		TIMPL,		0x4D	},
	{	0,	"tstx",		TIMPL,		0x5D	},
	
	{	0,	"bset",		TBIT,		0x10	},
	{	0,	"bclr",		TBIT,		0x11	},
	{	0,	"brset",	TBITREL,	0x00	},
	{	0,	"brclr",	TBITREL,	0x01	},

	{	0,	"bcc",		TREL,		0x24	},
	{	0,	"bcs",		TREL,		0x25	},
	{	0,	"beq",		TREL,		0x27	},
	{	0,	"bhcc",		TREL,		0x28	},
	{	0,	"bhcs",		TREL,		0x29	},
	{	0,	"bhi",		TREL,		0x22	},
	{	0,	"bhs",		TREL,		0x24	},
	{	0,	"bih",		TREL,		0x2F	},
	{	0,	"bil",		TREL,		0x2E	},
	{	0,	"blo",		TREL,		0x25	},
	{	0,	"bls",		TREL,		0x23	},
	{	0,	"bmc",		TREL,		0x2C	},
	{	0,	"bmi",		TREL,		0x2B	},
	{	0,	"bms",		TREL,		0x2D	},
	{	0,	"bne",		TREL,		0x26	},
	{	0,	"bpl",		TREL,		0x2A	},
	{	0,	"bra",		TREL,		0x20	},
	{	0,	"brn",		TREL,		0x21	},
	
	{	0,	"bsr",		TREL8,		0xAD	},

	{	0,	"clc",		TIMPL,		0x98	},
	{	0,	"cli",		TIMPL,		0x9A	},
	{	0,	"mul",		TIMPL,		0x42	},
	{	0,	"nop",		TIMPL,		0x9D	},
	{	0,	"rsp",		TIMPL,		0x9C	},
	{	0,	"rti",		TIMPL,		0x80	},
	{	0,	"rts",		TIMPL,		0x81	},
	{	0,	"sec",		TIMPL,		0x99	},
	{	0,	"sei",		TIMPL,		0x9B	},
	{	0,	"stop",		TIMPL,		0x8E	},
	{	0,	"swi",		TIMPL,		0x83	},
	{	0,	"tax",		TIMPL,		0x97	},
	{	0,	"txa",		TIMPL,		0x9F	},
	{	0,	"wait",		TIMPL,		0x8F	},

	/* 68HC08 additions */
	/* As well as these the HC08 adds n,SP forms to some instructions - not entirely
	   consistently */
	/* ais/aix sign extend an 8bit signed value */
	{	0,	"ais",		TIMM8,		0xA7	},
	{	0,	"aix",		TIMM8,		0xAF	},
	{	0,	"bge",		T08REL,		0x90	},
	{	0,	"bgt",		T08REL,		0x92	},
	{	0,	"ble",		T08REL,		0x93	},
	{	0,	"blt",		T08REL,		0x91	},
	{	0,	"cbeq",		TCBEQ,		0x01	},
	{	0,	"cbeqa",	TCBEQR,		0x41	},
	{	0,	"cbeqx",	TCBEQR,		0x51	},
	{	0,	"cphx",		TIM16,		0x65	},
	{	0,	"daa",		T08IMPL,	0x72	},
	{	0,	"dbnz",		TDBNZ,		0x0B	},
	{	0,	"dbnza",	T08REL8,	0x4B	},
	{	0,	"dbnzx",	T08REL8,	0x5B	},
	{	0,	"div",		TIMPL,		0x52	},
	{	0,	"ldhx",		TIM16,		0x45	},
	{	0,	"mov",		TMOV,		0x0E	},
	{	0,	"nsa",		T08IMPL,	0x62	},
	{	0,	"psha",		T08IMPL,	0x87	},
	{	0,	"pshh",		T08IMPL,	0x8B	},
	{	0,	"pshx",		T08IMPL,	0x89	},
	{	0,	"pula",		T08IMPL,	0x86	},
	{	0,	"pulh",		T08IMPL,	0x8A	},
	{	0,	"pulx",		T08IMPL,	0x88	},
	{	0,	"sthx",		TDIR,		0x35	},
	{	0,	"tap",		T08IMPL,	0x84	},
	{	0,	"tpa",		T08IMPL,	0x85	},
	{	0,	"tsx",		T08IMPL,	0x95	},
	{	0,	"txs",		T08IMPL,	0x94	}
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
	"index out of range",		/* 18 */
	"address required",		/* 19 */
	"invalid ID",			/* 20 */
	"bad addressing mode",		/* 21 */
	"divide by 0",			/* 22 */
	"constant out of range",	/* 23 */
	"data in BSS",			/* 24 */
	"segment overflow",		/* 25 */
	"data in direct page",		/* 26 */
	"segment conflict",		/* 27 */
	"unsupported by this CPU",	/* 28 */
	"too many Jcc instructions",	/* 29 */
	"invalid addressing mode"	/* 30 */
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
