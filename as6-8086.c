/*
 * 8086 assembler.
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

	{	0,	".86",		TSETCPU,	86	},
	{	0,	".186",		TSETCPU,	186	},

	{	0,	"al",		TBR,		AL	},
	{	0,	"cl",		TBR,		CL	},
	{	0,	"dl",		TBR,		DL	},
	{	0,	"bl",		TBR,		BL	},
	{	0,	"ah",		TBR,		AH	},
	{	0,	"ch",		TBR,		CH	},
	{	0,	"dh",		TBR,		DH	},
	{	0,	"bh",		TBR,		BH	},

	{	0,	"ax",		TWR,		AX	},
	{	0,	"cx",		TWR,		CX	},
	{	0,	"dx",		TWR,		DX	},
	{	0,	"bx",		TWR,		BX	},
	{	0,	"sp",		TWR,		SP	},
	{	0,	"bp",		TWR,		BP	},
	{	0,	"si",		TWR,		SI	},
	{	0,	"di",		TWR,		DI	},
	
	{	0,	"es",		TSR,		ES	},
	{	0,	"cs",		TSR,		CS	},
	{	0,	"ss",		TSR,		SS	},
	{	0,	"ds",		TSR,		DS	},

	{	0,	"byte",		TSMOD,		0	},
	{	0,	"word",		TSMOD,		1	},		

	/* The 8086 has a fairly small set of instruction encodings with
	   consistent forms. Like the 6809 there are prefixes and also
	   format information bytes */

	 {	0,	"aaa",		TIMPL,		0x37	},
	 {	0,	"aad",		TIMPL,		0xD5	},
	 {	0,	"aam",		TIMPL,		0xD4	},
	 {	0,	"aas",		TIMPL,		0x3F	},
	 {	0,	"adc",		TMEM,		0x10	},
	 {	0,	"add",		TMEM,		0x00	},
	 {	0,	"and",		TMEM,		0x20	},
	 /* call is TMEM but also has far forms */
	 {	0,	"call",		TCALL,		0x00	},
	 {	0,	"cbw",		TIMPL,		0x98	},
	 {	0,	"clc",		TIMPL,		0xF8	},
	 {	0,	"cld",		TIMPL,		0xFC	},
	 {	0,	"cli",		TIMPL,		0xFA	},
	 {	0,	"cmc",		TIMPL,		0xF5	},
	 {	0,	"cmp",		TMEM,		0x38	},
	 /* Implicit but with sizing */
	 {	0,	"cmps",		TIMPLS,		0xA6	},
	 {	0,	"cmpsb",	TIMPL,		0xA6	},
	 {	0,	"cmpsw",	TIMPL,		0xA7	},
	 {	0,	"cwd",		TIMPL,		0x99	},
	 {	0,	"daa",		TIMPL,		0x27	},
	 {	0,	"das",		TIMPL,		0x2F	},
	 {	0,	"dec",		TRM,		0xFE08	},
	 /* Extra forms with 186 on div else TR */
	 {	0,	"div",		TDIV,		0xF630	},
	 {	0,	"hlt",		TIMPL,		0xF4	},
	 {	0,	"idiv",		TDIV,		0xF638	},
	 /* Extra 186 forms */
	 {	0,	"imul",		TMUL,		0xF628	},
	 {	0,	"in",		TIO,		0xE4	},
	 {	0,	"inc",		TRM,		0xFE00	},
	 {	0,	"int",		TIMM8,		0xCC	},
	 {	0,	"into",		TIMPL,		0xCE	},
	 {	0,	"iret",		TIMPL,		0xCF	},
	 
	 {	0,	"ja",		TJCC,		0x77	},
	 {	0,	"jae",		TJCC,		0x73	},
	 {	0,	"jb",		TJCC,		0x72	},
	 {	0,	"jbe",		TJCC,		0x76	},
	 {	0,	"jcxz",		TJCX,		0xE3	},
	 {	0,	"je",		TJCC,		0x74	},
	 {	0,	"jg",		TJCC,		0x7F	},
	 {	0,	"jge",		TJCC,		0x7D	},
	 {	0,	"jl",		TJCC,		0x7C	},
	 {	0,	"jle",		TJCC,		0x7E	},
	 {	0,	"jna",		TJCC,		0x76	},
	 {	0,	"jnae",		TJCC,		0x72	},
	 {	0,	"jnb",		TJCC,		0x73	},
	 {	0,	"jne",		TJCC,		0x75	},
	 {	0,	"jng",		TJCC,		0x7E	},
	 {	0,	"jnge",		TJCC,		0x7C	},
	 {	0,	"jnl",		TJCC,		0x7D	},
	 {	0,	"jnle",		TJCC,		0x7F	},
	 {	0,	"jno",		TJCC,		0x71	},
	 {	0,	"jnp",		TJCC,		0x7B	},
	 {	0,	"jns",		TJCC,		0x79	},
	 {	0,	"jnz",		TJCC,		0x75	},
	 {	0,	"jo",		TJCC,		0x70	},
	 {	0,	"jp",		TJCC,		0x7A	},
	 {	0,	"jpe",		TJCC,		0x7A	},
	 {	0,	"jpo",		TJCC,		0x7B	},
	 {	0,	"js",		TJCC,		0x78	},
	 {	0,	"jz",		TJCC,		0x74	},
	 {	0,	"jmp",		TJMP,		0xEB	},
	 {	0,	"lahf",		TIMPL,		0x9F	},
	 {	0,	"lds",		TLDS,		0xC5	},
	 {	0,	"lea",		TLEA,		0x8D	},
	 {	0,	"les",		TLDS,		0xC4	},
	 {	0,	"lock",		TPREFIX,	0xF0	},
	 {	0,	"lods",		TIMPLS,		0xAC	},
	 {	0,	"lodsb",	TIMPL,		0xAC	},
	 {	0,	"lodsw",	TIMPL,		0xAD	},
	 {	0,	"loop",		TJCC,		0xE2	},
	 {	0,	"loopz",	TJCC,		0xE1	},
	 {	0,	"loopnz",	TJCC,		0xE0	},
	 {	0,	"loope",	TJCC,		0xE1	},
	 {	0,	"loopne",	TJCC,		0xE0	},
	 /* mov is TMEM but also has segment forms */
	 {	0,	"mov",		TMOV,		0x00	},
	 {	0,	"movs",		TIMPLS,		0xA4	},
	 {	0,	"movsb",	TIMPL,		0xA4	},
	 {	0,	"movsw",	TIMPL,		0xA5	},
	 {	0,	"mul",		TMUL,		0xF620	},
	 {	0,	"neg",		TRM,		0xF618	},
	 {	0,	"nop",		TIMPL,		0x90	},
	 {	0,	"not",		TRM,		0xF610	},
	 {	0,	"or",		TMEM,		0x08	},
	 {	0,	"out",		TIO,		0xE6	},
	 /* Like TRM but also seg reg */
	 {	0,	"pop",		TPOP,		0x8F	},
	 {	0,	"popf",		TIMPL,		0x9D	},
	 {	0,	"push",		TPUSH,		0xFF	},
	 {	0,	"pushf",	TIMPL,		0x9C	},
	 {	0,	"rcl",		TSHIFT,		0xD010	},
	 {	0,	"rcr",		TSHIFT,		0xD018	},
	 {	0,	"rep",		TPREFIX,	0xF3	},
	 {	0,	"repe",		TPREFIX,	0xF3	},
	 {	0,	"repz",		TPREFIX,	0xF3	},
	 {	0,	"repne",	TPREFIX,	0xF2	},
	 {	0,	"repnz",	TPREFIX,	0xF2	},
	 {	0,	"ret",		TRET,		0xC2	},
	 {	0,	"retf",		TRET,		0xCA	},
	 {	0,	"rol",		TSHIFT,		0xD000	},
	 {	0,	"ror",		TSHIFT,		0xD008	},
	 {	0,	"sahf",		TIMPL,		0x9E	},
	 {	0,	"sal",		TSHIFT,		0xD020	},
	 {	0,	"sar",		TSHIFT,		0xD038	},
	 {	0,	"sbb",		TMEM,		0x18	},
	 {	0,	"scas",		TIMPLS,		0xAE	},
	 {	0,	"scasb",	TIMPL,		0xAE	},
	 {	0,	"scasw",	TIMPL,		0xAF	},
	 {	0,	"shl",		TSHIFT,		0xD020	},
	 {	0,	"shr",		TSHIFT,		0xD028	},
	 {	0,	"stc",		TIMPL,		0xF9	},
	 {	0,	"std",		TIMPL,		0xFD	},
	 {	0,	"sti",		TIMPL,		0xFB	},
	 {	0,	"stos",		TIMPLS,		0xAA	},
	 {	0,	"stosb",	TIMPL,		0xAA	},
	 {	0,	"stosw",	TIMPL,		0xAB	},
	 {	0,	"sub",		TMEM,		0x28	},
	 {	0,	"test",		TMEM,		0x10	},
	 {	0,	"wait",		TIMPL,		0x9B	},
	 {	0,	"xchg",		TXCHG,		0x00	},
	 {	0,	"xlat",		TIMPL,		0xD7	},
	 {	0,	"xor",		TMEM,		0x30	},
	 
	 /* 186 added */
	 {	0,	"bound",	T186RM,		0x62	},
	 {	0,	"enter",	TENTER,		0xC8	},
	 {	0,	"ins",		T186IMPLS,	0x6C	},
	 {	0,	"insb",		T186IMPL,	0x6C	},
	 {	0,	"insw",		T186IMPL,	0x6D	},
	 {	0,	"leave",	T186IMPL,	0xC9	},
	 {	0,	"outs",		T186IMPLS,	0x6E	},
	 {	0,	"outsb",	T186IMPL,	0x6E	},
	 {	0,	"outsw",	T186IMPL,	0x6F	},
	 {	0,	"popa",		T186IMPL,	0x61	},
	 {	0,	"pusha",	T186IMPL,	0x60	},
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
	"UNUSED",			/* 18 */
	"address required",		/* 19 */
	"invalid id",			/* 20 */
	"bad mode",			/* 21 */
	"divide by zero",		/* 22 */
	"constant out of range",	/* 23 */
	"data in BSS",			/* 24 */
	"segment overflow",		/* 25 */
	"80816 instruction",		/* 26 */
	"segment conflict",		/* 27 */
	"too many jcc",			/* 28 */
	"register only",		/* 29 */
	"word register only",		/* 30 */
	"byte register only",		/* 31 */
	"segment register only",	/* 32 */
	"out of range",			/* 33 */
	"bad size",			/* 34 */
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
	unsigned mode;
	unsigned reg = ap->a_type & TMREG;

	mode = ap->a_type&TMMODE;
	if (mode == TWR) {
		switch(reg) {
		case BX:
		case BP:
		case DI:
		case SI:
			return;
		}
	}
	if (mode == TUSER)
		return;
	aerr(ADDR_REQUIRED);
}
