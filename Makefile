all: as1802 ld1802 nm1802 osize1802 dumprelocs1802 \
     as6502 ld6502 nm6502 osize6502 dumprelocs6502 \
     as6800 ld6800 nm6800 osize6800 dumprelocs6800 \
     as8008 ld8008 nm8008 osize8008 dumprelocs8008 \
     as8080 ld8080 nm8080 osize8080 dumprelocs8080 \
     as9900 ld9900 nm9900 osize9900 dumprelocs9900 \
     asee200 ldee200 nmee200 osizeee200 dumprelocsee200 \
     asz8 ldz8 nmz8 osizez8 dumprelocsz8 \
     asz80 ldz80 nmz80 osizez80 dumprelocsz80 

test: as8060 ld8060 nm8060 osize8060 dumprelocs8060 \
     asnova ldnova nmnova osizenova dumprelocsnova

todo: as6809 ld6809 nm6809 osize6809 dumprelocs6809 \
      as8070 ld8070 nm8070 osize8070 dumprelocs8070 \
      as8096 ld8096 nm8096 osize8096 dumprelocs8096 \
      aspdp4 ldpdp4 nmpdp4 osizepdp4 dumprelocspdp4

CCROOT ?=/opt/fcc/

HDR = as.h ld.h obj.h
CORE = as0.c as2.c as3.c as4.c

CFLAGS += -Wall -pedantic

as1802: $(HDR) $(CORE) as1-1802.c as6-1802.c
	cc  -DTARGET_1802 -o as1802 $(CORE) as1-1802.c as6-1802.c

ld1802: $(HDR) ld.c
	cc -o ld1802 ld.c

nm1802: $(HDR) nm.c
	cc -o nm1802 nm.c

osize1802: $(HDR) osize.c
	cc -o osize1802 osize.c

dumprelocs1802: $(HDR) dumprelocs.o
	cc -o dumprelocs1802 dumprelocs.o

as6502: $(HDR) $(CORE) as1-6502.c as6-6502.c
	cc  -DTARGET_6502 -o as6502 $(CORE) as1-6502.c as6-6502.c

ld6502: $(HDR) ld.c
	cc -o ld6502 ld.c

nm6502: $(HDR) nm.c
	cc -o nm6502 nm.c

osize6502: $(HDR) osize.c
	cc -o osize6502 osize.c

dumprelocs6502: $(HDR) dumprelocs.o
	cc -o dumprelocs6502 dumprelocs.o

as6800: $(HDR) $(CORE) as1-6800.c as6-6800.c
	cc  -DTARGET_6800 -o as6800 $(CORE) as1-6800.c as6-6800.c

ld6800: $(HDR) ld.c
	cc -o ld6800 ld.c

nm6800: $(HDR) nm.c
	cc -o nm6800 nm.c

osize6800: $(HDR) osize.c
	cc -o osize6800 osize.c

dumprelocs6800: $(HDR) dumprelocs.o
	cc -o dumprelocs6800 dumprelocs.o

as6809: $(HDR) $(CORE) as1-6809.c as6-6809.c
	cc  -DTARGET_6809 -o as6809 $(CORE) as1-6809.c as6-6809.c

ld6809: $(HDR) ld.c
	cc -o ld6809 ld.c

nm6809: $(HDR) nm.c
	cc -o nm6809 nm.c

osize6809: $(HDR) osize.c
	cc -o osize6809 osize.c

dumprelocs6809: $(HDR) dumprelocs.o
	cc -o dumprelocs6809 dumprelocs.o

as8008: $(HDR) $(CORE) as1-8008.c as6-8008.c
	cc  -DTARGET_8008 -o as8008 $(CORE) as1-8008.c as6-8008.c

ld8008: $(HDR) ld.c
	cc -o ld8008 ld.c

nm8008: $(HDR) nm.c
	cc -o nm8008 nm.c

osize8008: $(HDR) osize.c
	cc -o osize8008 osize.c

dumprelocs8008: $(HDR) dumprelocs.o
	cc -o dumprelocs8008 dumprelocs.o

as8060: $(HDR) $(CORE) as1-8060.c as6-8060.c
	cc  -DTARGET_8060 -o as8060 $(CORE) as1-8060.c as6-8060.c

ld8060: $(HDR) ld.c
	cc -o ld8060 ld.c

nm8060: $(HDR) nm.c
	cc -o nm8060 nm.c

osize8060: $(HDR) osize.c
	cc -o osize8060 osize.c

dumprelocs8060: $(HDR) dumprelocs.o
	cc -o dumprelocs8060 dumprelocs.o

as8070: $(HDR) $(CORE) as1-8070.c as6-8070.c
	cc  -DTARGET_8070 -o as8070 $(CORE) as1-8070.c as6-8070.c

ld8070: $(HDR) ld.c
	cc -o ld8070 ld.c

nm8070: $(HDR) nm.c
	cc -o nm8070 nm.c

osize8070: $(HDR) osize.c
	cc -o osize8070 osize.c

dumprelocs8070: $(HDR) dumprelocs.o
	cc -o dumprelocs8070 dumprelocs.o

as8080: $(HDR) $(CORE) as1-8080.c as6-8080.c
	cc  -DTARGET_8080 -o as8080 $(CORE) as1-8080.c as6-8080.c

ld8080: $(HDR) ld.c
	cc -o ld8080 ld.c

nm8080: $(HDR) nm.c
	cc -o nm8080 nm.c

osize8080: $(HDR) osize.c
	cc -o osize8080 osize.c

dumprelocs8080: $(HDR) dumprelocs.o
	cc -o dumprelocs8080 dumprelocs.o

as9900: $(HDR) $(CORE) as1-tms9900.c as6-tms9900.c
	cc  -DTARGET_TMS9900 -o as9900 $(CORE) as1-tms9900.c as6-tms9900.c

ld9900: $(HDR) ld.c
	cc -o ld9900 ld.c

nm9900: $(HDR) nm.c
	cc -o nm9900 nm.c

osize9900: $(HDR) osize.c
	cc -o osize9900 osize.c

dumprelocs9900: $(HDR) dumprelocs.o
	cc -o dumprelocs9900 dumprelocs.o

asee200: $(HDR) $(CORE) as1-ee200.c as6-ee200.c
	cc  -DTARGET_EE200 -o asee200 $(CORE) as1-ee200.c as6-ee200.c

ldee200: $(HDR) ld.c
	cc -o ldee200 ld.c

nmee200: $(HDR) nm.c
	cc -o nmee200 nm.c

osizeee200: $(HDR) osize.c
	cc -o osizeee200 osize.c

dumprelocsee200: $(HDR) dumprelocs.o
	cc -o dumprelocsee200 dumprelocs.o

asnova: $(HDR) $(CORE) as1-nova.c as6-nova.c
	cc  -DTARGET_DGNOVA -o asnova $(CORE) as1-nova.c as6-nova.c

ldnova: $(HDR) ld.c
	cc -o ldnova ld.c

nmnova: $(HDR) nm.c
	cc -o nmnova nm.c

osizenova: $(HDR) osize.c
	cc -o osizenova osize.c

dumprelocsnova: $(HDR) dumprelocs.o
	cc -o dumprelocsnova dumprelocs.o

asz8: $(HDR) $(CORE) as1-z8.c as6-z8.c
	cc  -DTARGET_Z8 -o asz8 $(CORE) as1-z8.c as6-z8.c

ldz8: $(HDR) ld.c
	cc -o ldz8 ld.c

nmz8: $(HDR) nm.c
	cc -o nmz8 nm.c

osizez8: $(HDR) osize.c
	cc -o osizez8 osize.c

dumprelocsz8: $(HDR) dumprelocs.o
	cc -o dumprelocsz8 dumprelocs.o

asz80: $(HDR) $(CORE) as1-z80.c as6-z80.c
	cc  -DTARGET_Z80 -o asz80 $(CORE) as1-z80.c as6-z80.c

ldz80: $(HDR) ld.c
	cc -o ldz80 ld.c

nmz80: $(HDR) nm.c
	cc -o nmz80 nm.c

osizez80: $(HDR) osize.c
	cc -o osizez80 osize.c

dumprelocsz80: $(HDR) dumprelocs.o
	cc -o dumprelocsz80 dumprelocs.o

clean:
	rm -f *.o *~
	rm -f as1802 ld1802 nm1802 osize1802 dumprelocs1802
	rm -f as6502 ld6502 nm6502 osize6502 dumprelocs6502
	rm -f as6800 ld6800 nm6800 osize6800 dumprelocs6800
	rm -f as6809 ld6809 nm6809 osize6809 dumprelocs6809
	rm -f as8008 ld8008 nm8008 osize8008 dumprelocs8008
	rm -f as8060 ld8060 nm8060 osize8060 dumprelocs8060
	rm -f as8070 ld8070 nm8070 osize8070 dumprelocs8070
	rm -f as8080 ld8080 nm8080 osize8080 dumprelocs8080
	rm -f as9900 ld9900 nm9900 osize9900 dumprelocs9900
	rm -f asee200 ldee200 nmee200 osizeee200 dumprelocsee200
	rm -f asnova ldnova nmnova osizenova dumprelocsnova
	rm -f aspdp4 ldpdp4 nmpdp4 osizepdp4 dumprelocspdp4
	rm -f asz8 ldz8 nmz8 osizez8 dumprelocsz8
	rm -f asz80 ldz80 nmz80 osizez80 dumprelocsz80

# FIXME: allow root specification

install: all
	mkdir -p /opt/fcc/bin
	cp as1802 ld1802 nm1802 osize1802 dumprelocs1802 $(CCROOT)/bin
	cp as6502 ld6502 nm6502 osize6502 dumprelocs6502 $(CCROOT)/bin
	cp as6800 ld6800 nm6800 osize6800 dumprelocs6800 $(CCROOT)/bin
	cp as8008 ld8008 nm8008 osize8008 dumprelocs8008 $(CCROOT)/bin
	cp as8080 ld8080 nm8080 osize8080 dumprelocs8080 $(CCROOT)/bin
	cp as9900 ld9900 nm9900 osize9900 dumprelocs9900 $(CCROOT)/bin
	cp asee200 ldee200 nmee200 osizeee200 dumprelocsee200 $(CCROOT)/bin
	cp asz8 ldz8 nmz8 osizez8 dumprelocsz8 $(CCROOT)/bin
	cp asz80 ldz80 nmz80 osizez80 dumprelocsz80 $(CCROOT)/bin

installtest: test
	mkdir -p /opt/fcc/bin
	cp as8060 ld8060 nm8060 osize8060 dumprelocs8060 $(CCROOT)/bin
	cp asnova ldnova nmnova osizenova dumprelocsnova $(CCROOT)/bin

#	cp as6809 ld6809 nm6809 osize6809 dumprelocs6809 $(CCROOT)/bin
#	cp as8070 ld8070 nm8070 osize8070 dumprelocs8070 $(CCROOT)/bin
#	cp aspdp4 ldpdp4 nmpdp4 osizepdp4 dumprelocspdp4 $(CCROOT)/bin
