/*
 * Assembler.
 * Header file, used by all
 * parts of the assembler.
 */
#include	<stdio.h>
#include	<string.h>
#include 	<stdlib.h>
#include	<stdint.h>
#include	<ctype.h>
#include	<setjmp.h>

#include	"obj.h"

/*
 * Table sizes, etc.
 */
#define	NCPS	NAMELEN			/* # of characters in symbol */
#define	NHASH	64			/* # of hash buckets */
#define	HMASK	077			/* Mask for above */
#define	NFNAME	32			/* # of characters in filename */
#define	NERR	10			/* Size of error buffer */
#define	NCODE	128			/* # of characters in code buffer */
#define	NINPUT	128			/* # of characters in input line */
#define	NLPP	60			/* # of lines on a page */
#define	XXXX	0			/* Unused value */

/*
 * Exit codes.
 */
#define	GOOD	0
#define	BAD	1

#ifdef TARGET_Z80

/* We generate intentionally wrapping 16bit maths for relocations */
#define TARGET_RELOC_OVERFLOW_OK

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_8080
#define ARCH_FLAGS 0
#define ARCH_CPUFLAGS OA_8080_Z80

#define CPU_Z80		80
#define CPU_Z180	180
#define CPU_Z280	280
#define CPU_Z80N	1
#define CPU_EZ80	2

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x007F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define	TNOP	0x0F00			/* nop */
#define	TRST	0x1000			/* restarts */
#define	TREL	0x1100			/* djnz, jr */
#define	TRET	0x1200			/* ret */
#define	TJMP	0x1300			/* call, jp */
#define	TPUSH	0x1400			/* push, pop */
#define	TIM	0x1500			/* im */
#define	TIO	0x1600			/* in, out */
#define	TBIT	0x1700			/* set, res, bit */
#define	TSHR	0x1800			/* sl, sr et al */
#define	TINC	0x1900			/* inc, dec */
#define	TEX	0x1A00			/* ex */
#define	TADD	0x1B00			/* add, adc, sbc */
#define	TLD	0x1C00			/* ld */
#define	TCC	0x1D00			/* condition code */
#define	TSUB	0x1E00			/* sub et al */
#define TNOP180	0x2100			/* Z180 immediate */
#define TTST180	0x2200			/* TST m/g/(hl) */
#define TIMMED8	0x2300			/* TSTIO m */
#define	TMUL	0x2400			/* MUL */
#define TIO180	0x2500			/* IN0/OUT0 */
#define TSETCPU 0x2600			/* .setcpu */
#define TNOPN	0x2700			/* Z80N implied */
#define TNEXT	0x2800			/* Z80N nextreg forms */
#define TIMMED8N 0x2900			/* TIMMED8 Next only */
#define TBS2	0x2A00			/* Barrel shifts DE,B */
#define TBPTR	0x2B00			/* BCPL/B pointer */

/*
 * Registers.
 */
#define	B	0
#define	C	1
#define	D	2
#define	E	3
#define	H	4
#define	L	5
#define	M	6
#define	A	7
#define	IX	8
#define	IY	9

#define	BC	0
#define	DE	1
#define	HL	2
#define	SP	3
#define	AF	4
#define	AFPRIME	5

#define	I	0
#define	R	1

/*
 * Condition codes.
 */
#define	CNZ	0
#define	CZ	1
#define	CNC	2
#define	CC	3
#define	CPO	4
#define	CPE	5
#define	CP	6
#define	CM	7

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define CONDCODE_ONLY	18
#define INVALID_REG	19
#define ADDR_REQUIRED	20
#define INVALID_ID	21
#define REG_MUST_BE_C	22
#define DIVIDE_BY_ZERO	23
#define CONSTANT_RANGE  24
#define DATA_IN_BSS	 25
#define SEGMENT_OVERFLOW 26
#define DATA_IN_ZP	27
#define REQUIRE_Z180	28
#define	SEGMENT_CLASH	29
#define UNKNOWN_SYMBOL	30
#define TOO_MANY_JR	31
#define REQUIRE_Z80N	32

#elif defined(TARGET_GB)

/* We generate intentionally wrapping 16bit maths for relocations */
#define TARGET_RELOC_OVERFLOW_OK

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_GB
#define ARCH_FLAGS 0
#define ARCH_CPUFLAGS 0

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x007F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define	TNOP	0x0F00			/* nop */
#define	TRST	0x1000			/* restarts */
#define	TREL	0x1100			/* djnz, jr */
#define	TRET	0x1200			/* ret */
#define	TJMP	0x1300			/* call, jp */
#define	TPUSH	0x1400			/* push, pop */
#define	TIM	0x1500			/* im */
#define	TIO	0x1600			/* in, out */
#define	TBIT	0x1700			/* set, res, bit */
#define	TSHR	0x1800			/* sl, sr et al */
#define	TINC	0x1900			/* inc, dec */
#define	TEX	0x1A00			/* ex */
#define	TADD	0x1B00			/* add, adc, sbc */
#define	TLD	0x1C00			/* ld */
#define	TCC	0x1D00			/* condition code */
#define	TSUB	0x1E00			/* sub et al */
#define TSTOP	0x1F00			/* stop $n */
#define TLDDI	0x2000			/* ldd/ldh a,(hl) (hl),a */
#define TLDH	0x2100			/* ldh (I/O) */

/*
 * Registers.
 */
#define	B	0
#define	C	1
#define	D	2
#define	E	3
#define	H	4
#define	L	5
#define	M	6
#define	A	7

#define	BC	0
#define	DE	1
#define	HL	2
#define	SP	3
#define	AF	4

/*
 * Condition codes.
 */
#define	CNZ	0
#define	CZ	1
#define	CNC	2
#define	CC	3

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define CONDCODE_ONLY	18
#define INVALID_REG	19
#define ADDR_REQUIRED	20
#define INVALID_ID	21
#define REG_MUST_BE_C	22
#define DIVIDE_BY_ZERO	23
#define CONSTANT_RANGE  24
#define DATA_IN_BSS	 25
#define SEGMENT_OVERFLOW 26
#define DATA_IN_ZP	27
#define REQUIRE_Z180	28
#define	SEGMENT_CLASH	29
#define UNKNOWN_SYMBOL	30
#define TOO_MANY_JR	31

#elif TARGET_8080

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_8080
#define ARCH_FLAGS 0
#define ARCH_CPUFLAGS 0

/* We generate intentionally wrapping 16bit maths for relocations */
#define TARGET_RELOC_OVERFLOW_OK

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x007F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define	TSETCPU	0x0F00			/* setcpu */
#define	TIMPL	0x1000			/* implicit 8080 */
#define	TIMPL85	0x1100			/* implicit 8085 */
#define	TI8	0x1200			/* 8bit immediate */
#define	TI8_85	0x1300			/* 8bit immediate 8085 */
#define	TI16	0x1400			/* 16bit immediate */
#define	TI16_85	0x1500			/* 16bit immediate 8085 */
#define	TREG8	0x1600			/* 8bit register */
#define	TREG16	0x1700			/* 16bit register (sp allowed not psw )*/
#define	TREG16_P 0x1800			/* 16bit push/pop (psw allowed not sp) */
#define	TMOV	0x1900			/* mov instruction */
#define TRST    0x1A00			/* rst instruction */
#define TCC	0x1B00			/* not used but needed for core code */
#define TREG8_I8 0x1C00			/* reg,constant - mvi etc */
#define TREG16_I16 0x1D00			/* reg,constant - mvi etc */
#define	TREG8H	0x1E00			/* 8bit register bits 5-3 (inc/dec) */
#define TREG16BD 0x1F00			/* stax/ldax */
#define TBPTR	0x2000			/* BCPL / B word pointer */

/*
 * Registers.
 */
#define	B	0			/* 8bit matching encoding for 8 */
#define	C	1
#define	D	2
#define	E	3
#define	H	4
#define	L	5
#define	M	6
#define	A	7

#define	SP	8			/* 16bit cases */
#define	PSW	9

/*
 *	Error message numbers: FIXME - sort general first
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16


#define INVALID_REG	19
#define ADDR_REQUIRED	20
#define INVALID_ID	21

#define DIVIDE_BY_ZERO	23
#define CONSTANT_RANGE  24
#define DATA_IN_BSS	 25
#define SEGMENT_OVERFLOW 26
#define DATA_IN_ZP	27
#define REQUIRE_8085	28
#define	SEGMENT_CLASH	29
#define UNKNOWN_SYMBOL	30


#elif TARGET_6502

/* We generate intentionally wrapping 16bit maths for relocations */
#define TARGET_RELOC_OVERFLOW_OK

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_6502
#define ARCH_FLAGS 0
#define ARCH_CPUFLAGS OA_6502_BCD	/* For now until CPU type properly settable */


/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define	TZP		0x0010		/* 0000 is TUSER */
#define TACCUM		0x0020
#define TZPX		0x0030
#define TZPY		0x0040
#define TABSX		0x0050
#define TABSY		0x0060
#define TZPX_IND	0x0070
#define TZPY_IND	0x0080
#define TZP_IND		0x0090
#define TZP_INDL	0x00A0
#define TALX_IND	0x00B0
#define TZPYL_IND	0x00C0
#define TSR		0x00D0
#define TSRY_IND	0x00E0
#define TABSL		0x00F0

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define	TCLASS0	0x1000			/* xxxyyy00 instructions */
#define TCLASS1	0x1100			/* xxxyyy01 instructions */
#define TCLASS2	0x1200			/* xxxyyy10 instructions */
#define TCLASS2Y 0x1300			/* ditto but taking Y */
#define TJMP	0x1400			/* JMP */
#define TREL8	0x1500			/* Bcc */
#define TIMPL	0x1600			/* Implicit */
#define TBRK	0x1700			/* BRK */
#define TJSR	0x1800			/* JSR */
#define TBRA16	0x1900			/* Jcc asm magic */
#define TI	0x1A00			/* Set index size */
#define TA	0x1B00			/* Set accum size */
#define TCPU	0x1C00			/* Set CPU type */
#define TIMPLC	0x1D00			/* 65C02 implicit */
#define TIMPL16	0x1E00			/* 65C816 implicit */
#define TCLASS2A 0x1F00			/* INC/DEC A */
#define TIMM16	0x2000			/* 16bit immediate 65C816 */
#define TSTZ	0x2100			/* Store zero 65C02 */
#define TABDP	0x2200			/* Ops taking abs,dp only */
#define TMVN	0x2300			/* MVN/MVP 65C816 */
#define TPEI	0x2400			/* PEI 65C816 */
#define TREP	0x2500			/* REP 65C816 */
#define TJML	0x2600			/* JML 65C816 */
#define TLONG	0x2700			/* Abslute 24bit long 65C816 */
#define TREL8C	0x2800			/* 8bit rel, 65C02 */
#define TREL16	0x2900			/* 16bit rel, 65C816 */
#define TCLASS0X 0x2A00			/* bit is slightly odd */
#define TREL8X	0x2B00			/* Jcc pseudo ops */

/*
 * Registers.
 */
#define	A	0
#define	X	1
#define	Y	2

/*
 *	 CPU types
 */

#define CPU_6502		0
#define CPU_65C02		1
#define CPU_65C816		2

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define CONDCODE_ONLY	18
#define INVALID_REG	19
#define ADDR_REQUIRED	20
#define INVALID_ID	21
#define BADMODE		22
#define DIVIDE_BY_ZERO	23
#define CONSTANT_RANGE  24
#define DATA_IN_BSS	 25
#define SEGMENT_OVERFLOW 26
#define DATA_IN_ZP	27
#define	SEGMENT_CLASH	28
#define RANGE		29
#define BADCPU		30
#define TOO_MANY_BRA	31

#elif TARGET_DGNOVA

#define TARGET_WORD_MACHINE
#define TARGET_BIGENDIAN

/* 16 bit machine but we need to track in 32bits to allow for the fact we
   can be dealing with 2^16 words */
typedef	uint32_t	VALUE;		/* For symbol values */

#define SEGMENT_LIMIT	0x10000		/* bytes */

#define ARCH OA_DGNOVA
#define ARCH_FLAGS (OF_BIGENDIAN)
#define ARCH_CPUFLAGS 0


/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TCPUOPT	0x1100
#define TMEMORY	0x1200
#define TALU	0x1300
#define TIO	0x1400
#define TDEV	0x1500
#define TAC	0x1600
#define TIMPL	0x1700
#define TBYTE	0x1800
#define TTRAP	0x1900
#define TMEMNA	0x1A00
#define TBPTR	0x1B00			/* Nova byteptr */
#define TNIO	0x1C00			/* NIO instruction */

#define TPCREL	0x0010

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define CONDCODE_ONLY	18
#define INVALID_REG	19
#define ADDR_REQUIRED	20
#define INVALID_ID	21
#define BADMODE		22
#define DIVIDE_BY_ZERO	23
#define CONSTANT_RANGE  24
#define DATA_IN_BSS	 25
#define SEGMENT_OVERFLOW 26
#define DATA_IN_ZP	27
#define BAD_ACCUMULATOR	28
#define NEED_ZPABS	29
#define BADDEVICE	30
#define BAD_PCREL	BRA_RANGE
#define	SEGMENT_CLASH	31

#elif TARGET_6809

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_6809
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS 0

#define TARGET_BIGENDIAN
#define TARGET_USES_SQUARE

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define TMADDR 	0x00F0			/* Addressing mode info */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TCC	0x0400			/* Condition code (not used but needed by core) */
#define	TSR	0x0400			/* Special reg (not used but needed by core) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */

/* CPU specific codes */

#define THI	0x1100			/* all modes opcodes 0x80+ */
#define THIW	0x1200			/* ditto word mode */
#define THINOIMM 0x1300			/* ditto byte no immediate */
#define THIWNOIMM 0x1400		/* ditto word no immediate */
#define TIMM8	0x1500			/* 8bit immediate only */
#define TEXG	0x1600			/* registers as 8bit immediate */
#define TPUSH	0x1700			/* register mask as 8bit immediate */
#define TIMP	0x1800			/* implied by instruction */
#define TLO	0x1900			/* low instructions with some modes */
#define TLEA	0x1A00			/* lea */
#define TBRA	0x1B00			/* branch */
#define TLBRA	0x1C00			/* long branch */
#define TJSR	0x1D00			/* Ops in weird spots */

#define TIMMED	0x0010
#define TDIR	0x0020
#define TEXT	0x0030
#define TINDEX	0x0040			/* Postbyte forms all below */
#define TPOST1	0x0050			/* except TEXT|TMINDIR */
#define TPOST2	0x0060			/* Ordering is assumed in is_postbyte */
#define TPRE1	0x0070
#define TPRE2	0x0080
#define TINDEXA	0x0090
#define TINDEXB	0x00A0
#define TINDEXD	0x00B0

/*
 * Registers.
 */
#define	A	0		/* re-arrange to match bit patterns */
#define	B	1
#define	DP	2
#define	CC	3
#define	D	4
#define	U	5
#define	X	6
#define	Y	7
#define	S	8
#define PCR	9

/*
 *	Error message numbers: FIXME - sort general first
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define CONDCODE_ONLY	18
#define TOOMANYJCC	19
#define NEED_INDEX	20
#define INVALID_FORM	21
#define CONSTANT_ONLY	22
#define DIVIDE_BY_ZERO	23
#define CONSTANT_RANGE  24
#define DATA_IN_BSS	 25
#define SEGMENT_OVERFLOW 26
#define DATA_IN_ZP	27
#define REQUIRE_6309	28
#define	SEGMENT_CLASH	29
#define ADDR_REQUIRED	30
#define INVALID_INDIR	31
#define INVALID_ID	32
#define NEED_PREPOST	33
#define NEED_REGISTER	34
#define NEED_CLSQUARE	35

#elif TARGET_6800

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_6800
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS 0

#define TARGET_BIGENDIAN

/* We generate intentionally wrapping 16bit maths for relocations */
#define TARGET_RELOC_OVERFLOW_OK

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TDIRECT	0x0010			/* Direct page */
#define TINDEX	0x0020			/* Indexed */
#define TIMMED	0x0030			/* Immediate */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
/* 0x2x and 0x8D */
#define TREL8	0x1100			/* 8bit relative from . + 2 */
/* 0x6X,0x7X */
#define TXE	0x1200			/* Extended or indexed */
/* 0x8x-0xFx */
#define TDIXE	0x1300			/* All forms */
/* Stores in that range */
#define TDXE	0x1400			/* No immediate (eg store) */
/* 0x8x-0xFx */
#define T16DIXE	0x1500			/* All forms double immediate*/
/* Stores in that range */
#define T16DXE	0x1600			/* Double immediate no store */
/* 6303 specials */
#define TIMPL6303	0x1700		/* 6303 implicit */
#define TIDX6303	0x1800		/* AIM etc */
#define TIDXB6303	0x1900		/* BIT form of the above */
/* Assembler extras for resolving branch ranges */
#define TBRA16		0x1A00		/* Jcc asm magic */
#define TSETCPU		0x1B00		/* .setcpu */
/* 6803/303 specifics */
#define TIMPL6803	0x1C00
#define T16DIXE3	0x1D00
#define T16DXE3		0x1E00
#define TDXE3		0x1F00
#define TDIXE3		0x2000
/*
 * Registers.
 */
#define	A	0
#define B	1
#define	D	2
#define	X	3

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define INDX_RANGE	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define BADMODE		21
#define DIVIDE_BY_ZERO	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define DATA_IN_ZP	26
#define	SEGMENT_CLASH	27
#define BADCPU		28
#define TOOMANYJCC	29
#define INVALIDAMODE	30

#elif TARGET_6805

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_6805
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS 0

#define TARGET_BIGENDIAN

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TDIRECT	0x0010			/* Direct page */
#define TINDEX	0x0020			/* Indexed */
#define TIMMED	0x0030			/* Immediate */
#define TXPLUS	0x0040			/* X+ 68HC08 */
#define TSP	0x0050			/* ,SP forms 68HC08 */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TSETCPU		0x1100		/* .setcpu */
#define TMEM	0x1200
#define TMEMNI	0x1300
#define TJMP	0x1400
#define TRMW	0x1500
#define TBIT	0x1600
#define TBITREL	0x1700
#define TREL	0x1800
#define TREL8	0x1900
/* Formats used in 6808 instructions */
#define TIMM8	0x1A00			/* s8 */
#define T08REL	0x1B00			/* TREL 68HC08 only */
#define TCBEQ	0x1C00
#define TIM16	0x1D00			/* Immediate or extended only */
#define T08IMPL	0x1E00			/* Implicit 68HC08 only */
#define TDBNZ	0x1F00
#define TMOV	0x2000			/* Just weird */
#define TDIR	0x2100			/* Direct only */
#define TCBEQR	0x2200
#define T08REL8	0x2300

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define INDX_RANGE	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define BADMODE		21
#define DIVIDE_BY_ZERO	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define DATA_IN_ZP	26
#define	SEGMENT_CLASH	27
#define BADCPU		28
#define TOOMANYJCC	29
#define INVALIDAMODE	30

#elif TARGET_HC11

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_6800
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS OA_6800_68HC11

#define TARGET_BIGENDIAN

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TDIRECT	0x0010			/* Direct page */
#define TINDEX	0x0020			/* Indexed */
#define TIMMED	0x0030			/* Immediate */
#define TINDEY  0x0040			/* Indexed via Y */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
/* 0x2x and 0x8D */
#define TREL8	0x1100			/* 8bit relative from . + 2 */
/* 0x6X,0x7X */
#define TXE	0x1200			/* Extended or indexed */
/* 0x8x-0xFx */
#define TDIXE	0x1300			/* All forms */
/* Stores in that range */
#define TDXE	0x1400			/* No immediate (eg store) */
/* 0x8x-0xFx */
#define T16DIXE	0x1500			/* All forms double immediate*/
/* Stores in that range */
#define T16DXE	0x1600			/* Double immediate no store */
#define TBITBR	0x1700			/* Bit test and branch */
#define TBIT	0x1800			/* Bit oerations */
/* Assembler extras for resolving branch ranges */
#define TBRA16		0x1900		/* Jcc asm magic */
#define TSETCPU		0x1A00		/* .setcpu */
#define T16DIXE3	0x1B00
#define T16DXE3		0x1C00
#define TDXE3		0x1D00
#define TDIXE3		0x1E00
/*
 * Registers.
 */
#define	A	0
#define B	1
#define	D	2
#define	X	3
#define	Y	4

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define INDX_RANGE	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define BADMODE		21
#define DIVIDE_BY_ZERO	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define DATA_IN_ZP	26
#define	SEGMENT_CLASH	27
#define BADCPU		28
#define TOOMANYJCC	29
#define INVALIDAMODE	30

#elif TARGET_CP1600

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_CP1600
#define ARCH_FLAGS (OF_WORDMACHINE|OF_BIGENDIAN)	/* Mostly meaningless - word machine */
#define ARCH_CPUFLAGS 0

#define TARGET_BIGENDIAN
#define TARGET_WORD_MACHINE

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TREG	0x0010			/* Register */
#define TIMMED	0x0020			/* Immediate */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TBITS	0x1000			/* 10/16bit */
#define TRA	0x1100			/* reg, addr */
#define TAR	0x1200			/* addr, reg */
#define TARI	0x1400			/* immed, reg */
#define TJMP	0x1500			/* jmp forms */
#define TJSR	0x1600			/* jsr forms */
#define TBRA	0x1700			/* branch forms */
#define TBEXT	0x1800			/* brext (unique) */
#define TRR	0x1900			/* reg, reg ops */
#define TR	0x1A00			/* reg ops */
#define TSHIFT	0x1B00			/* shift ops */
#define TIMPL	0x1C00			/* implicit */
#define TGSWD	0x1D00			/* gswd (unique) */
#define TSIN	0x1E00			/* nop and sin */
#define TSBDB	0x1F00			/* sbdb prefix */
#define TJR	0x2000			/* aliased MOVR r,7 */
#define TRSAME	0x2100			/* aliases that are actually r,r */

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define REG_REQUIRED	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define INVALID_REG	21
#define DIVIDE_BY_ZERO	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define DATA_IN_ZP	26
#define	SEGMENT_CLASH	27
#define NOT10BIT	28
#define TOOMANYJCC	29
#define INVALID_FORM	30

#elif TARGET_Z8

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_Z8
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS OA_Z8_BASE

#define TARGET_BIGENDIAN

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TRS	0x0010			/* Register, short form */
#define TREG	0x0020			/* Register */
#define TIMMED	0x0030			/* Immediate */
#define TSIND	0x0040			/* Register short indirect */
#define TIND	0x0050			/* Indrect */
#define TRRS	0x0060			/* Register pair */
#define TRRIND	0x0070			/* Register pair indirect */
#define TINDEX	0x0080			/* value is followed by (xx) */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TOP4BIT	0x1100			/* 4bit + mode */
#define TRIR	0x1200			/* R or IR mode */
#define TRRIR	0x1300			/* RR or IR mode */
#define TCRA	0x1400			/* JR CC, rel */
#define TJMP	0x1500			/* JP cc,DA + JP cc,IRR */
#define TIMM8	0x1600			/* Only used for SRP */
#define TIRRDA	0x1700			/* For CALL */
#define TRA	0x1800			/* Relative address */
#define TLDC	0x1900			/* LDC (r,Irr  / Irr, r ) */
#define TLDCI	0x1A00			/* LDCI (Ir,Irr / Irr, Ir) */
#define TLOAD	0x1B00			/* 12 forms.. */

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define RSHORT_RANGE	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define INVALID_FORM	21
#define DIVIDE_BY_ZERO	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define DATA_IN_ZP	26
#define	SEGMENT_CLASH	27
#define ODD_REGISTER	28
#define TOOMANYJCC	29

#elif TARGET_SUPER8

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_Z8
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS OA_Z8_SUPER

#define TARGET_BIGENDIAN

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TRS	0x0010			/* Register, short form */
#define TREG	0x0020			/* Register */
#define TIMMED	0x0030			/* Immediate */
#define TSIND	0x0040			/* Register short indirect */
#define TIND	0x0050			/* Indrect */
#define TRRS	0x0060			/* Register pair (rrn)*/
#define TRRIND	0x0070			/* Register pair indirect */
#define TINDEX	0x0080			/* value is followed by (xx) */
#define TRROFF	0x0090			/* nnnn(@rr) */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TOP4BIT	0x1100			/* 4bit + mode */
#define TRIR	0x1200			/* R or IR mode */
#define TRRIR	0x1300			/* RR or IR mode */
#define TCRA	0x1400			/* JR CC, rel */
#define TJMP	0x1500			/* JP cc,DA + JP cc,IRR */
#define TIMM8	0x1600			/* Only used for SRP */
#define TIRRDA	0x1700			/* For CALL */
#define TRA	0x1800			/* Relative address */
#define TLDC	0x1900			/* LDC (r,Irr  / Irr, r ) */
#define TLDCI	0x1A00			/* LDCI r,Irr */
#define TLOAD	0x1B00			/* 12 forms.. */
#define TBIT	0x1C00			/* Bit ops */
#define TBITX	0x1D00			/* Bit ops with low bit mod */
#define TBJMP	0x1E00			/* Bit test and jump */
#define TCALL	0x1F00			/* Call ops */
#define TIMM8M	0x2000			/* Immediates with op bits in data */
#define TPUSHU	0x2100			/* pushu forms */
#define TPOPU	0x2200			/* popu forms */
#define TMULDIV	0x2300			/* mul and div forms */
#define TCPIJ	0x2400			/* compare inc and jump */
#define TLDW	0x2500			/* ldw forms */
#define TBCP	0x2600			/* bcp */
#define TLDCPI	0x2700			/* LDCPI Irr,r */

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define RSHORT_RANGE	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define INVALID_FORM	21
#define DIVIDE_BY_ZERO	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define DATA_IN_ZP	26
#define	SEGMENT_CLASH	27
#define ODD_REGISTER	28
#define TOOMANYJCC	29
#define BITRANGE	30
#define NOTREG01	31

#elif TARGET_PDP4

typedef	uint32_t	VALUE;		/* For symbol values */

#define ARCH 		OA_PDP4
#define ARCH_FLAGS	OF_BIGENDIAN	/* Meaningless - 18bit word machine */
#define ARCH_CPUFLAGS	0

#define TARGET_WORD_MACHINE		/* TODO: we write out 32bit with 18 used so its by 4 FIXME */
#define TARGET_BIGENDIAN

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TIMMED	0x0010
#define TINDEX	0x0020			/* Register, short form */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TMEM	0x1100			/* upper bits opcode, lowrer address */
#define TOPR	0x1200			/* operations */
#define TLAW	0x1300			/* LAW is unique */
#define TIOT	0x1400			/* IOT */
#define TEAE	0x1500			/* EAE */
#define TIMPLE	0x1600			/* EAE specifi implicit */
#define TIMPL7	0x1700			/* PDP7 specific IOT */
#define TIMPL9	0x1800			/* PDP9 specific */
#define TIMPL15	0x1900			/* PDP15 specific */
#define T9BIT	0x1A00			/* 9bit signed ops for the 16 (AXS etc) */
#define TSETCPU 0x1B00
#define TMODE	0x1C00

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define ADDR_REQUIRED	17
#define INVALID_ID	18
#define BAD_MODE	19
#define CONST_RANGE	20
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	21
#define SEGMENT_OVERFLOW 22
#define	SEGMENT_CLASH	23
#define DIVIDE_BY_ZERO	24
#define RANGE		25
#define WRONGCPU	26
#define NOINDEX		27

#elif TARGET_PE16

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH 		OA_PE16
#define ARCH_FLAGS	OF_BIGENDIAN
#define ARCH_CPUFLAGS	0

#define TARGET_BIGENDIAN

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TIMMED	0x0010
#define TINDEX	0x0020			/* Register, short form */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TRR	0x1100			/* r,r */
#define TRX	0x1200			/* r,addr(rx)  or r,addr */
#define TRI	0x1300			/* r,immed(rx) or r,immed */
#define TSF	0x1400			/* r,4bit */
#define TRRL	0x1500			/* versions where reg must be even */
#define TRXL	0x1600
#define TRXF	0x1600			/* Same thing for now */
#define TRIL	0x1700
#define TIMPLZ	0x1800			/* Implicit , 0 work follows */
#define TRRF	0x1900			/* Float, both even */

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define SFINVALID	17
#define NO0INDEX	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define REGREQUIRED	21
#define ODD_REGISTER	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define DATA_IN_ZP	26
#define	SEGMENT_CLASH	27
#define REGINVALID	28
#define DIVIDE_BY_ZERO	29

#elif TARGET_PE32

typedef	uint32_t	VALUE;		/* For symbol values */

#define ARCH 		OA_PE32
#define ARCH_FLAGS	OF_BIGENDIAN
#define ARCH_CPUFLAGS	0

#define TARGET_BIGENDIAN
#define ADDR32

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TIMMED	0x0010
#define	TRX1	0x0020
#define TRX2	0x0030
#define TRX3	0x0040

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TRR	0x1100			/* r,r */
#define TRX	0x1200			/* r,addr(rx)  or r,addr */
#define TRI1	0x1300			/* r,immed(rx) or r,immed 16bit */
#define TRI2	0x1400			/* r,immed(rx) or r,immed 32bit */
#define TSF	0x1500			/* r,4bit */
#define TRRF	0x1600			/* versions where reg must be even */
#define TRXF	0x1700			/* "" */
#define TRX0	0x1800			/* RX form with R1 = 0 */
#define TR0	0x1900			/* RR form with R1 = 0 */
#define TSVC	0x1A00			/* SVC */
#define TBFRX	0x1B00			/* Branch RX form */
#define TBFRR	0x1C00			/* Branch RR form */
#define TBFSF	0x1D00			/* Branch short form */
#define TBRX	0x1E00			/* Branch alias RX form */
#define TBRR	0x1F00			/* Branch alias RR form */
#define TBSF	0x2000			/* Branch alias short form */
#define TWDCS	0x2100			/* WDCS */
#define TRDCS	0x2200			/* RDCS */

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define SFINVALID	17
#define NO0INDEX	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define REGREQUIRED	21
#define ODD_REGISTER	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define DATA_IN_ZP	26
#define	SEGMENT_CLASH	27
#define REGINVALID	28
#define DIVIDE_BY_ZERO	29

#elif TARGET_1802

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_1802
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS 0			/* For now until CPU type properly settable */

#define TARGET_BIGENDIAN

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TIMMED	0x0010			/* Immediate */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TIMM8	0x1100			/* Immediate 8bit */
#define TNOP	0x1200			/* NOP */
#define TSKIP	0x1300			/* LSKIP */
#define TREG	0x1400			/* 0-15 */
#define TREGNZ	0x1500			/* 1-15 */
#define TADDR16	0x1600			/* Long address */
#define TIOPORT	0x1700			/* I/O port num (1-7) */
#define TREL	0x1800			/* Relative address (sort of)*/

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define INVALID_REG	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define INVALID_IO	21
#define DIVIDE_BY_ZERO	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define DATA_IN_ZP	26
#define	SEGMENT_CLASH	27
#define NOT_REG0	28

#elif TARGET_TMS9900

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_TMS9900
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS OA_TMS9900_9995		/* For now until CPU type properly settable */

#define TARGET_BIGENDIAN
/* We generate intentionally wrapping 16bit maths for relocations */
#define TARGET_RELOC_OVERFLOW_OK

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TDIRECT	0x0010			/* Direct page */
#define TINDEX	0x0020			/* Indexed */
#define TIMMED	0x0030			/* Immediate */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TDOMA	0x1100			/* addr, addr */
#define TDOMAW	0x1200			/* R, addr */
#define TSMD	0x1300			/* addr */
#define TXOP	0x1400			/* XOP */
#define TSOP	0x1500			/* R */
#define TCRUM	0x1600			/* */
#define TCRUS	0x1700			/* signed offset */
#define TJUMP	0x1800			/* signed offet PC relative */
#define TSHIFT	0x1900			/* reg, num */
#define TIMM	0x1A00			/* reg, num */
#define TIRL	0x1B00			/* addr */
#define TIRLS	0x1C00			/* reg */
#define TEVEN	0x1D00			/* even macro op */
#define TLJUMP	0x1E00			/* jcc/bra expansions */
#define TLJONLY	0x1F00			/* branches always done with jcc bra */

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define REG_RANGE	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define BADMODE		21
#define DIVIDE_BY_ZERO	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define REG_REQUIRED	26
#define	SEGMENT_CLASH	27
#define REG_ZEROONLY	28
#define TOOMANYJCC	29
#define REG_NOTZERO	30
#define ALIGNMENT	31
#define BRA_BAD		32

#elif TARGET_8008

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_8008
#define ARCH_FLAGS 0
#define ARCH_CPUFLAGS 0

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TDIRECT	0x0010			/* Direct page */
#define TINDEX	0x0020			/* Indexed */
#define TIMMED	0x0030			/* Immediate */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TIMM8	0x1100			/* 8bit immediate */
#define TBRA	0x1200			/* Branch */
#define TRST	0x1300			/* RST */
/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define ADDR_REQUIRED	17
#define INVALID_ID	18
#define BADMODE		19
#define CONSTANT_RANGE  20
#define DATA_IN_BSS	21
#define SEGMENT_OVERFLOW 22
#define	SEGMENT_CLASH	23
#define DIVIDE_BY_ZERO	24

#elif TARGET_8060

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_INS8060
#define ARCH_FLAGS 0
#define ARCH_CPUFLAGS 0

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TINDEX	0x0010			/* Indexed */
#define TIMMED	0x0020			/* Immediate */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TMPD	0x1100			/* pointer/offset/autoinc */
#define TPD	0x1200			/* pointer/offset */
#define TIMM8	0x1300			/* 8bit signed */
#define	TREL8	0x1400			/* 8bit relative */
#define TPTR	0x1500			/* pointer */
#define TJS	0x1600			/* special */

#define	P0	0
#define P1	1
#define P2	2
#define P3	3


/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define ADDR_REQUIRED	17
#define INVALID_ID	18
#define BADMODE		19
#define CONSTANT_RANGE  20
#define DATA_IN_BSS	21
#define SEGMENT_OVERFLOW 22
#define	SEGMENT_CLASH	23
#define DIVIDE_BY_ZERO	24
#define NO_AUTOINDEX	25
#define BRA_RANGE	26
#define RANGE		27
#define INVALIDAMODE	28
#define POINTER_REQ	29

#elif TARGET_8070

/* We generate intentionally wrapping 16bit maths for relocations */
#define TARGET_RELOC_OVERFLOW_OK

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_INS8070
#define ARCH_FLAGS 0
#define ARCH_CPUFLAGS 0

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TINDEX	0x0010			/* Indexed */
#define TIMMED	0x0020			/* Immediate */
#define TAUTOINDEX 0x0030		/* Autoindex */
#define TDIRECT 0x0040			/* FFxx */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TP2P3	0x1100			/* Can use P2 or P3 */
#define TIMM16	0x1200			/* Immediate 16bit */
#define TPIMM16	0x1300			/* Weird PLI */
#define TLOGIC	0x1400			/* and/or/xor */
#define TLOGIC16 0x1500			/* 16bit supporting - add/sub */
#define TMEM8	0x1600			/* 8bit memory ptr */
#define TBND	0x1700			/* BND operation */
#define TBRA	0x1800			/* Branches */
#define TXCH	0x1900			/* Exchange operation */
#define TCALL	0x1A00			/* Call */
#define TEAT	0x1B00			/* EA,T ops */
#define TLOAD 	0x1C00			/* Load */
#define TSTORE	0x1D00			/* Store */
#define TAONLY	0x1E00			/* A only */
#define TAEA	0x1F00			/* A or EA */
#define TSTACK	0x2000			/* PUSH and POP */
#define TBRA16	0x2100			/* Self expanding branch helpers */
#define TADDR	0x2200			/* .addr */

#define	P0	0			/* Must be 0-3 to match encoding */
#define P1	1
#define P2	2
#define P3	3
#define EA	4
#define T	5

#define A	6			/* Don't overlap the numbers */
#define E	7			/* Makes the as1 logic easier */
#define S	8


/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define ADDR_REQUIRED	17
#define INVALID_ID	18
#define BADMODE		19
#define CONSTANT_RANGE  20
#define DATA_IN_BSS	21
#define SEGMENT_OVERFLOW 22
#define	SEGMENT_CLASH	23
#define DIVIDE_BY_ZERO	24
#define NO_AUTOINDEX	25
#define BRA_RANGE	26
#define RANGE		27
#define INVALIDAMODE	28
#define POINTER_REQ	29
#define BADREG		30
#define TOOMANYJCC	31
#define AREQUIRED	32
#define AEAREQUIRED	33

#elif TARGET_8086

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_8086
#define ARCH_FLAGS 0
#define ARCH_CPUFLAGS 0

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TIMMED	0x0010			/* Direct page */
#define TMODRM	0x0020			/* MODRM forms */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TPREFIX	0x1100			/* Prefix byte */
#define TSMOD	0x1200			/* Size modifier */
#define TMEM	0x1300			/* Usual two arg */
#define TIMPLS	0x1400			/* Implicit but sized */
#define TRM	0x1500			/* Single reg/mem */
#define TDIV	0x1600
#define TMUL	0x1700
#define	TIO	0x1800
#define TMOV	0x1900			/* All the mov forms */
#define TIMM8	0x1A00
#define TJCC	0x1B00
#define TJMP	0x1C00
#define TCALL	0x1D00
#define TLEA	0x1E00
#define TLDS	0x1F00
#define TPOP	0x2000
#define TPUSH	0x2100
#define TSHIFT	0x2200			/* Shifts and rotates */
#define TXCHG	0x2300
#define T186RM	0x2400
#define T186IMPL	0x2500
#define T186IMPLS	0x2600
#define TENTER	0x2700
#define TJCX	0x2800			/* Like JCC but no auto-extend */
#define TSETCPU 0x2900			/* .86 , .186 etc */
#define TRET	0x2A00			/* Optional 16bit integer */

/*
 * Registers. Encoding order matches instructions
 */

#define	AL		0
#define	CL		1
#define DL		2
#define BL		3
#define AH		4
#define CH		5
#define DH		6
#define BH		7

#define AX		0
#define CX		1
#define DX		2
#define BX		3
#define SP		4
#define BP		5
#define SI		6
#define DI		7

#define ES		0
#define	CS		1
#define SS		2
#define DS		3

/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define JMP_RANGE	17
#define INDX_RANGE	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define BADMODE		21
#define DIVIDE_BY_ZERO	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define NEED_186	26
#define	SEGMENT_CLASH	27
#define TOOMANYJCC	28
#define REGONLY		29
#define WREGONLY	30
#define BREGONLY	31
#define SREGONLY	32
#define RANGE		33
#define BADSIZE		34

#elif TARGET_EE200

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_WARREX
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS OA_WARREX_CPU6

#define TARGET_BIGENDIAN

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0			/* Addressing mode bits */

#define TDIRECT	0x0010			/* Direct page */
#define TINDEX	0x0020			/* Indexed */
#define TIMMED	0x0030			/* Immediate */

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (PC) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TIMPL	0x1000			/* Implicit */
#define TREL8	0x1100			/* 8bit relative from . + 2 */
/* Assembler extras for resolving branch ranges */
#define TBRA16	0x1200			/* Jcc asm magic */
#define TREGA	0x1300			/* Single accumulator, A short form */
#define TREG	0x1400			/* Single accumulator */
#define TMOVE	0x1500			/* Move.. special */
#define TMMU	0x1600			/* MMU loads */
#define TDMA	0x1700			/* DMA word register */
#define TDMAM	0x1800			/* DMA mode */
#define TREG2A	0x1900			/* Two reg with B,A short forms */
#define TREG2ANS 0x1A00			/* But without the short form */
#define TJUMP	0x1B00			/* Jump/call */
#define TLOAD	0x1C00			/* Load */
#define TSTORE	0x1D00			/* Store */
#define TJUMP8	0x1E00			/* Extending 8bit branch */
#define TREGA8	0x1F00			/* TREGA 8bit only */
#define TREG8	0x2000			/* TREG 8bit only */
#define TREG2A8	0x2100			/* TREG2 8bit only */
#define TMOVE8	0x2200			/* TMOVE 8bit only */
#define TLOADEB	0x2300
#define TLOADEW	0x2400
#define TLOADX	0x2500
#define TSTOREEB 0x2600
#define TSTOREEW 0x2700
#define TSTOREX 0x2800
#define	TSETCPU	0x2900			/* setcpu */
#define TIMPL6	0x2A00			/* Not present on CPU4 */
#define TBLOCK	0x2B00			/* Block operations CPU6 only */
#define TRANGE	0x2C00			/* Range CPU6 only */

/*
 * Registers.
 */
#define	RA	0
#define RB	2
#define	RX	4
#define	RY	6
#define RZ	8
#define RS	10
#define RC	12
#define	RP	14

#define RAH	0
#define RAL	1
#define RBH	2
#define RBL	3
#define RXH	4
#define RXL	5
#define RYH	6
#define RYL	7
#define RZH	8
#define RZL	9
#define RSH	10
#define RSL	11
#define RCH	12
#define RCL	13
#define RPH	14
#define RPL	15

#define RPC	0


/*
 *	Error message numbers
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define BRA_RANGE	17
#define INDX_RANGE	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define BADMODE		21
#define DIVIDE_BY_ZERO	22
#define CONSTANT_RANGE  23
#define DATA_IN_BSS	24
#define SEGMENT_OVERFLOW 25
#define DATA_IN_ZP	26
#define	SEGMENT_CLASH	27
#define TOOMANYJCC	28
#define REGONLY		29
#define WREGONLY	30
#define BREGONLY	31
#define REGABBYTE	32
#define REGABXWORD	33
#define BADINDIR	34
#define BADADDR		35
#define RANGE		36
#define AREGONLY	37
#define BADCPU		38
#define NOAIDX		39

#elif TARGET_316

#define TARGET_WORD_MACHINE
#define TARGET_BIGENDIAN

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_H316
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS 0

/* We generate intentionally wrapping 16bit maths for relocations */
#define TARGET_RELOC_OVERFLOW_OK
#define TARGET_BIGENDIAN

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */

#define TMADDR	0x00F0
#define TINDEX	0x0010
#define TINDIRECT 0x0020
#define TIMMED	0x0040
#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (I, R) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define	TSETFLAG	0x0F00			/* flag bits */
#define	TCLRFLAG	0x1000			/* flag bits off */
#define TXAC	0x1100			/* XAC */
#define	TSETBASE	0x1200		/* .setb */
#define TMR	0x1300			/* MR form */
#define TMRHSA	0x1400			/* MR form, needs HSA */
#define TMRX	0x1500			/* MR form with tag weirdness */
#define TG	0x1600			/* G (implicit) form */
#define TSH	0x1700			/* SH (shift) form */
#define TIO	0x1800			/* IO form */
#define TG5	0x1900			/* G form for 516 only */
#define TGHSA	0x1A00			/* G form for HSA */
#define	TCC	0x1B00			/* unused, needed for core */
#define TLITMARK 0x1C00			/* space for literals */

/*
 *	Error message numbers: FIXME - sort general first
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define RANGE		17
#define REQUIRE_FEATURE	18
#define ADDR_REQUIRED	19
#define INVALID_ID	20
#define CONSTANT_RANGE  21
#define DATA_IN_BSS	 22
#define SEGMENT_OVERFLOW 23
#define UNKNOWN_SYMBOL	24
#define DIVIDE_BY_ZERO	25
#define	SEGMENT_CLASH	26
#define NOTINDEX	27

#elif TARGET_TMS7000

typedef	uint16_t	VALUE;		/* For symbol values */

#define ARCH OA_TMS7000
#define ARCH_FLAGS OF_BIGENDIAN
#define ARCH_CPUFLAGS 0

/* We generate intentionally wrapping 16bit maths for relocations */
#define TARGET_RELOC_OVERFLOW_OK
#define TARGET_BIGENDIAN

/*
 * Types. These are used
 * in both symbols and in address
 * descriptions. Observe the way the
 * symbol flags hide in the register
 * field of the address.
 */
#define	TMREG	0x000F			/* Register code */
#define	TMMDF	0x0001			/* Multidef */
#define	TMASG	0x0002			/* Defined by "=" */
#define	TMMODE	0xFF00			/* Mode */
#define	TMINDIR	0x8000			/* Indirect flag in mode */
#define TPUBLIC	0x0080			/* Exported symbol */
#define TMADDR	0x00F0
#define TINDEX	0x0010
#define	TR	0x0020
#define TPR	0x0030
#define TIMMED	0x0040
#define TIDXB	0x0050

#define	TNEW	0x0000			/* Virgin */
#define	TUSER	0x0100			/* User name */
#define	TBR	0x0200			/* Byte register */
#define	TWR	0x0300			/* Word register */
#define	TSR	0x0400			/* Special register (PC) */
#define	TDEFB	0x0500			/* defb */
#define	TDEFW	0x0600			/* defw */
#define	TDEFS	0x0700			/* defs */
#define	TDEFM	0x0800			/* defm */
#define	TORG	0x0900			/* org */
#define	TEQU	0x0A00			/* equ */
#define	TCOND	0x0B00			/* conditional */
#define	TENDC	0x0C00			/* end conditional */
#define TSEGMENT 0x0D00			/* segments by number */
#define TEXPORT 0x0E00			/* symbol export */
#define TCC	0x0F00
/* CPU specific codes */
#define TMOV	0x1000			/* Moves */
#define TMULT	0x1100			/* Multi forms */
#define TMULTO	0x1200			/* Multi + offset */
#define TIND	0x1300			/* 3 op forms with indirect */
#define TR3	0x1400			/* 3 op forms */
#define TPP	0x1500			/* Push/pop */
#define TR3O	0x1600			/* 3 op form with offset */
#define TMOVP	0x1700			/* Movp is special */
#define TFORM	0x1800			/* Other P forms */
#define TFORMO	0x1900			/* P form with offset */
#define TIMPL	0x1A00			/* Implicit forms */
#define TJMP	0x1B00			/* Jump is special */
#define TBRA	0x1C00			/* Conditional branching */
#define TTRAP	0x1D00			/* Trap is special */
#define TMOVD	0x1E00			/* Movd is special */
#define TREL	0x1F00			/* JP */

#define RA	0x00
#define RB	0x01
#define RST	0x02

/*
 *	Error message numbers: FIXME - sort general first
 */

#define BRACKET_EXPECTED 1
#define MISSING_COMMA	2
#define SQUARE_EXPECTED 3
#define PERCENT_EXPECTED 4
#define UNEXPECTED_CHR	10
#define PHASE_ERROR	11
#define MULTIPLE_DEFS	12
#define SYNTAX_ERROR	13
#define MUST_BE_ABSOLUTE	14
#define MISSING_DELIMITER 15
#define INVALID_CONST	16
#define ADDR_REQUIRED	17
#define INVALID_ID	18
#define BAD_MODE	19
#define CONSTANT_RANGE  20
#define DATA_IN_BSS	 21
#define SEGMENT_OVERFLOW 22
#define	SEGMENT_CLASH	23
#define BRANCH_RANGE	24
#define RANGE		25
#define TOOMANYJCC	26
#define DIVIDE_BY_ZERO	27
#define REG_REQUIRED	28
#define PREG_REQUIRED	29

#else
#error "Unknown target"
#endif

/*
 * Segments
 */
#define UNKNOWN		15
#define ABSOLUTE	0
#define CODE		1
#define DATA		2
#define BSS		3

/*
 * Expression priority
 */

#define	LOPRI	0
#define	ADDPRI	1
#define	MULPRI	2
#define	HIPRI	3

#ifndef SEGMENT_LIMIT
#define SEGMENT_LIMIT	0xFFFF
#endif

/*
 * Address description.
 */
typedef	struct	ADDR	{
	uint16_t a_type;			/* Type */
	VALUE	a_value;		/* Index offset, etc */
	uint8_t	a_segment;		/* Segment relative to */
	uint8_t a_flags;		/* To track high */
#define A_HIGH		1
#define A_LOW		2
	struct SYM *a_sym;		/* Symbol tied to this address */
					/* NULL indicates simple own segment */
}	ADDR;

/*
 * Symbol.
 */
typedef	struct	SYM	{
	struct	SYM *s_fp;		/* Link in hash */
	char	s_id[NCPS];		/* Name */
	int	s_type;			/* Type */
	VALUE	s_value;		/* Value */
	uint16_t s_number;		/* Symbol number 1..n, also usable for
					   tokens as extra data */
	int	s_segment;		/* Segment this symbol is relative to */
}	SYM;

/*
 * External variables.
 */
extern	char	*cp;
extern	char	*ip;
extern	char	ib[];
extern	FILE	*ifp;
extern	FILE	*ofp;
extern	FILE	*lfp;
extern	int	line;
extern	int	lmode;
extern	VALUE	laddr;
extern	SYM	sym[];
extern	int	pass;
extern	SYM	*phash[];
extern	SYM	*uhash[];
extern	int	lflag;
extern	jmp_buf	env;
extern	VALUE   dot[OSEG];
extern  int	segment;
extern	int	debug_write;
extern	char	*fname;
extern  char	*listname;
extern	int	noobj;
extern	int	cpu_flags;

extern int passbegin(int pass);
extern void list_addbyte(uint8_t);
extern void asmline(void);
extern void doflush(void);
extern void comma(void);
extern void istuser(ADDR *);
extern int symhash(char *);
extern void err(char, uint8_t);
extern void uerr(char *);
extern void aerr(uint8_t);
extern void qerr(uint8_t);
extern void storerror(int);
extern int is_symstart(int c);
extern void getid(char *, int);
extern SYM *lookup(char *, SYM *[], int);
extern int symeq(char *, char *);
extern void symcopy(char *, char *);
extern int get(void);
extern int getnb(void);
extern void unget(int);
extern void getaddr(ADDR *);
extern void expr1(ADDR *, int, int);
extern void expr2(ADDR *);
extern void expr3(ADDR *, int);
extern void isokaors(ADDR *, int);
extern int outpass(void);
extern void outabsolute(addr_t);
extern void outsegment(int);
extern void outab(uint8_t);
extern void outabyte(uint8_t);
extern void outab2(uint8_t);
extern void outabchk2(ADDR *);
extern void outrab(ADDR *);
extern void outraw(ADDR *);
extern void outrat(ADDR *);
extern void outrad(ADDR *);
extern void outrabrel(ADDR *);
extern void outrawrel(ADDR *);
extern void outratrel(ADDR *);
extern void outradrel(ADDR *);
extern void outeof(void);
extern void outbyte(uint8_t);
extern void outflush(void);
extern void syminit(void);
extern void reservebyte(void);
extern void outscale(unsigned, unsigned);

extern char *etext[];

#include "obj.h"
