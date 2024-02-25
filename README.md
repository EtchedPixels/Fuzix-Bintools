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

Used heavily by the Fuzix 6303, 6800, and 6803 trees. 68HC11 needs adding.

### 8008

Support for the Intel 8008 processor. Should be stable.

### 8080

Used by Fuzix 8080 and 8085 so heavily tested. Supports the 8085
undocumented extensions.

### 9900

TMS9900 and TMS9995 instruction set. Used by the ANSI PCC cross compiler so
has some degree of testing at this point.

### Z8

Used by the Z8 Compiler.

### Z80

The original assembler target from MWC. Should be solid.

## Test Targets

### 8060

The INS8060 (SC/MP)

### NOVA

DG Nova series machines. Mostly used at this point to try and debug word
based addressing support in the linker

