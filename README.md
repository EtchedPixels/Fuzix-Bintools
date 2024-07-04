# Fuzix BinTools

Assembler, linker and supporting tools for the Fuzix C Compiler

## Installation

````
make
make install
````

The default installation target is /opt/fcc to match the Fuzix Compiler Kit.
Install this before the compiler kit and ensure this is on the path.

## Targets

### 1802

Should be reasonably stable, not yet heavily used.

### 6502

Used by the Fuzix 65C816 compiler. Reasonably complete except for 24bit
65C816 operation support which needs work (mostly to sort out linker
support)

### 6800

Used heavily by the Fuzix 6303, 6800, and 6803 trees.

### 6809

Motorola 6809. Has been fairly heavily tested but not yet used much in
anger.

### 8008

Support for the Intel 8008 processor. Should be stable.

### 8080

Used by Fuzix 8080 and 8085 so heavily tested. Supports the 8085
undocumented extensions.

### 9900

TMS9900 and TMS9995 instruction set. Used by the ANSI PCC cross compiler so
has some degree of testing at this point.

### EE200

Electrodata EE200, Warrex CPU 5 / 6.

### Gameboy

8080 like processor used in the Gameboy systems.

### HC11

68HC11.Should be fairly solid 

### Z8

Used by the Z8 Compiler.

### Z80

The original assembler target from MWC. Should be solid.

## Test Targets

### 316

Honeywell 316 and similar machines

### 6805/68HC08

Motorola 6805 and 68HC08 (not 6808 - see 6800 for that)

## 8060

The INS8060 (SC/MP)

### 8070

INS 8070

### NOVA

DG Nova series machines. Test case for word addressing support. Should now
be reasonably solid.

### PE16

Perkin/Elmer aka Interdata 16. More testing required.

## Early Work Only

### 8051

Scoping only

### 8086

Work in progress. Not all instruction forms yet finished.

### 8096

Intel 80x96. Very early work only

### PDP4

Early scoping only to prototype non 16bit sizes

### Super8

Zilog Z8 related processor. Needs a lot more testing.

### TMS7000

Work in progress
