all: as1802 ld1802 nm1802 osize1802 dumprelocs1802 \
     as6502 ld6502 nm6502 osize6502 dumprelocs6502 \
     as6800 ld6800 nm6800 osize6800 dumprelocs6800 \
     as6805 ld6805 nm6805 osize6805 dumprelocs6805 \
     as6809 ld6809 nm6809 osize6809 dumprelocs6809 \
     as7000 ld7000 nm7000 osize7000 dumprelocs7000 \
     as8008 ld8008 nm8008 osize8008 dumprelocs8008 \
     as8080 ld8080 nm8080 osize8080 dumprelocs8080 \
     as9900 ld9900 nm9900 osize9900 dumprelocs9900 \
     asee200 ldee200 nmee200 osizeee200 dumprelocsee200 \
     asgb ldgb nmgb osizegb dumprelocsgb \
     ashc11 ldhc11 nmhc11 osizehc11 dumprelocshc11 \
     asnova ldnova nmnova osizenova dumprelocsnova \
     assuper8 ldsuper8 nmsuper8 osizesuper8 dumprelocssuper8 \
     asz8 ldz8 nmz8 osizez8 dumprelocsz8 \
     asz80 ldz80 nmz80 osizez80 dumprelocsz80 \
     reloc template Tools

test: as316 ld316 nm316 osize316 dumprelocs316 \
      as8060 ld8060 nm8060 osize8060 dumprelocs8060 \
      as8070 ld8070 nm8070 osize8070 dumprelocs8070 \
      as8086 ld8086 nm8086 osize8086 dumprelocs8086 \
      ascp1600 ldcp1600 nmcp1600 osizecp1600 dumprelocscp1600 \
      aspdp4 ldpdp4 nmpdp4 osizepdp4 dumprelocspdp4 \
      aspe16 ldpe16 nmpe16 osizepe16 dumprelocspe16 \
      aspe32 ldpe32 nmpe32 osizepe32 dumprelocspe32

todo: as8096 ld8096 nm8096 osize8096 dumprelocs8096

.PHONY: Tools

CCROOT ?=/opt/fcc/

HDR = as.h ld.h obj.h
CORE = as0.c as2.c as3.c as4.c

CFLAGS += -Wall -pedantic

reloc: $(HDR) reloc.c
	cc $(CFLAGS) reloc.c -o reloc

template: template.c
	cc $(CFLAGS) template.c -o template

as1802: $(HDR) $(CORE) as1-1802.c as6-1802.c
	$(CC) $(CFLAGS) -DTARGET_1802 -o as1802 $(CORE) as1-1802.c as6-1802.c

ld1802: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld1802 ld.c

nm1802: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm1802 nm.c

osize1802: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize1802 osize.c

dumprelocs1802: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs1802 dumprelocs.c

as316: $(HDR) $(CORE) as1-316.c as6-316.c
	$(CC) $(CFLAGS)  -DTARGET_316 -o as316 $(CORE) as1-316.c as6-316.c

ld316: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld316 ld.c

nm316: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm316 nm.c

osize316: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize316 osize.c

dumprelocs316: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs316 dumprelocs.c

as6502: $(HDR) $(CORE) as1-6502.c as6-6502.c
	$(CC) $(CFLAGS)  -DTARGET_6502 -o as6502 $(CORE) as1-6502.c as6-6502.c

ld6502: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld6502 ld.c

nm6502: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm6502 nm.c

osize6502: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize6502 osize.c

dumprelocs6502: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs6502 dumprelocs.c

as6800: $(HDR) $(CORE) as1-6800.c as6-6800.c
	$(CC) $(CFLAGS)  -DTARGET_6800 -o as6800 $(CORE) as1-6800.c as6-6800.c

ld6800: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld6800 ld.c

nm6800: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm6800 nm.c

osize6800: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize6800 osize.c

dumprelocs6800: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs6800 dumprelocs.c

as6805: $(HDR) $(CORE) as1-6805.c as6-6805.c
	$(CC) $(CFLAGS)  -DTARGET_6805 -o as6805 $(CORE) as1-6805.c as6-6805.c

ld6805: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld6805 ld.c

nm6805: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm6805 nm.c

osize6805: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize6805 osize.c

dumprelocs6805: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs6805 dumprelocs.c

as6809: $(HDR) $(CORE) as1-6809.c as6-6809.c
	$(CC) $(CFLAGS)  -DTARGET_6809 -o as6809 $(CORE) as1-6809.c as6-6809.c

ld6809: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld6809 ld.c

nm6809: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm6809 nm.c

osize6809: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize6809 osize.c

dumprelocs6809: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs6809 dumprelocs.c

as7000: $(HDR) $(CORE) as1-tms7000.c as6-tms7000.c
	$(CC) $(CFLAGS)  -DTARGET_TMS7000 -o as7000 $(CORE) as1-tms7000.c as6-tms7000.c

ld7000: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld7000 ld.c

nm7000: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm7000 nm.c

osize7000: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize7000 osize.c

dumprelocs7000: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs7000 dumprelocs.c

as8008: $(HDR) $(CORE) as1-8008.c as6-8008.c
	$(CC) $(CFLAGS)  -DTARGET_8008 -o as8008 $(CORE) as1-8008.c as6-8008.c

ld8008: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld8008 ld.c

nm8008: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm8008 nm.c

osize8008: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize8008 osize.c

dumprelocs8008: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs8008 dumprelocs.c

as8060: $(HDR) $(CORE) as1-8060.c as6-8060.c
	$(CC) $(CFLAGS)  -DTARGET_8060 -o as8060 $(CORE) as1-8060.c as6-8060.c

ld8060: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld8060 ld.c

nm8060: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm8060 nm.c

osize8060: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize8060 osize.c

dumprelocs8060: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs8060 dumprelocs.c

as8070: $(HDR) $(CORE) as1-8070.c as6-8070.c
	$(CC) $(CFLAGS)  -DTARGET_8070 -o as8070 $(CORE) as1-8070.c as6-8070.c

ld8070: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld8070 ld.c

nm8070: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm8070 nm.c

osize8070: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize8070 osize.c

dumprelocs8070: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs8070 dumprelocs.c

as8080: $(HDR) $(CORE) as1-8080.c as6-8080.c
	$(CC) $(CFLAGS)  -DTARGET_8080 -o as8080 $(CORE) as1-8080.c as6-8080.c

ld8080: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld8080 ld.c

nm8080: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm8080 nm.c

osize8080: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize8080 osize.c

dumprelocs8080: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs8080 dumprelocs.c

as8086: $(HDR) $(CORE) as1-8086.c as6-8086.c
	$(CC) $(CFLAGS)  -DTARGET_8086 -o as8086 $(CORE) as1-8086.c as6-8086.c

ld8086: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld8086 ld.c

nm8086: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm8086 nm.c

osize8086: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize8086 osize.c

dumprelocs8086: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs8086 dumprelocs.c

as9900: $(HDR) $(CORE) as1-tms9900.c as6-tms9900.c
	$(CC) $(CFLAGS)  -DTARGET_TMS9900 -o as9900 $(CORE) as1-tms9900.c as6-tms9900.c

ld9900: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ld9900 ld.c

nm9900: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nm9900 nm.c

osize9900: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osize9900 osize.c

dumprelocs9900: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocs9900 dumprelocs.c

ascp1600: $(HDR) $(CORE) as1-cp1600.c as6-cp1600.c
	$(CC) $(CFLAGS)  -DTARGET_CP1600 -o ascp1600 $(CORE) as1-cp1600.c as6-cp1600.c

ldcp1600: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ldcp1600 ld.c

nmcp1600: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nmcp1600 nm.c

osizecp1600: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osizecp1600 osize.c

dumprelocscp1600: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocscp1600 dumprelocs.c

asee200: $(HDR) $(CORE) as1-ee200.c as6-ee200.c
	$(CC) $(CFLAGS)  -DTARGET_EE200 -o asee200 $(CORE) as1-ee200.c as6-ee200.c

ldee200: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ldee200 ld.c

nmee200: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nmee200 nm.c

osizeee200: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osizeee200 osize.c

dumprelocsee200: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocsee200 dumprelocs.c

ashc11: $(HDR) $(CORE) as1-hc11.c as6-hc11.c
	$(CC) $(CFLAGS)  -DTARGET_HC11 -o ashc11 $(CORE) as1-hc11.c as6-hc11.c

ldhc11: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ldhc11 ld.c

nmhc11: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nmhc11 nm.c

osizehc11: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osizehc11 osize.c

dumprelocshc11: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocshc11 dumprelocs.c

asnova: $(HDR) $(CORE) as1-nova.c as6-nova.c
	$(CC) $(CFLAGS)  -DTARGET_DGNOVA -o asnova $(CORE) as1-nova.c as6-nova.c

ldnova: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ldnova ld.c

nmnova: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nmnova nm.c

osizenova: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osizenova osize.c

dumprelocsnova: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocsnova dumprelocs.c

aspdp4: $(HDR) $(CORE) as1-pdp4.c as6-pdp4.c
	$(CC) $(CFLAGS) -DTARGET_PDP4 -DARCH32 -o aspdp4 $(CORE) as1-pdp4.c as6-pdp4.c

ldpdp4: $(HDR) ld.c
	$(CC) $(CFLAGS) -DARCH32 -o ldpdp4 ld.c

nmpdp4: $(HDR) nm.c
	$(CC) $(CFLAGS) -DARCH32 -o nmpdp4 nm.c

osizepdp4: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osizepdp4 osize.c

dumprelocspdp4: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocspdp4 dumprelocs.c

aspe16: $(HDR) $(CORE) as1-pe16.c as6-pe16.c
	$(CC) $(CFLAGS)  -DTARGET_PE16 -o aspe16 $(CORE) as1-pe16.c as6-pe16.c

ldpe16: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ldpe16 ld.c

nmpe16: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nmpe16 nm.c

osizepe16: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osizepe16 osize.c

dumprelocspe16: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocspe16 dumprelocs.c

aspe32: $(HDR) $(CORE) as1-pe32.c as6-pe32.c
	$(CC) $(CFLAGS) -DTARGET_PE32 -DARCH32 -o aspe32 $(CORE) as1-pe32.c as6-pe32.c

ldpe32: $(HDR) ld.c
	$(CC) $(CFLAGS) -DARCH32 -o ldpe32 ld.c

nmpe32: $(HDR) nm.c
	$(CC) $(CFLAGS) -DARCH32 -o nmpe32 nm.c

osizepe32: $(HDR) osize.c
	$(CC) $(CFLAGS) -DARCH32 -o osizepe32 osize.c

dumprelocspe32: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -DARCH32 -o dumprelocspe32 dumprelocs.c

asz8: $(HDR) $(CORE) as1-z8.c as6-z8.c
	$(CC) $(CFLAGS)  -DTARGET_Z8 -o asz8 $(CORE) as1-z8.c as6-z8.c

ldz8: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ldz8 ld.c

nmz8: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nmz8 nm.c

osizez8: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osizez8 osize.c

dumprelocsz8: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocsz8 dumprelocs.c

assuper8: $(HDR) $(CORE) as1-super8.c as6-super8.c
	$(CC) $(CFLAGS)  -DTARGET_SUPER8 -o assuper8 $(CORE) as1-super8.c as6-super8.c

ldsuper8: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ldsuper8 ld.c

nmsuper8: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nmsuper8 nm.c

osizesuper8: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osizesuper8 osize.c

dumprelocssuper8: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocssuper8 dumprelocs.c

asgb: $(HDR) $(CORE) as1-gb.c as6-gb.c
	$(CC) $(CFLAGS)  -DTARGET_GB -o asgb $(CORE) as1-gb.c as6-gb.c

ldgb: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ldgb ld.c

nmgb: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nmgb nm.c

osizegb: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osizegb osize.c

dumprelocsgb: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocsgb dumprelocs.c

asz80: $(HDR) $(CORE) as1-z80.c as6-z80.c
	$(CC) $(CFLAGS)  -DTARGET_Z80 -o asz80 $(CORE) as1-z80.c as6-z80.c

ldz80: $(HDR) ld.c
	$(CC) $(CFLAGS) -o ldz80 ld.c

nmz80: $(HDR) nm.c
	$(CC) $(CFLAGS) -o nmz80 nm.c

osizez80: $(HDR) osize.c
	$(CC) $(CFLAGS) -o osizez80 osize.c

dumprelocsz80: $(HDR) dumprelocs.c
	$(CC) $(CFLAGS) -o dumprelocsz80 dumprelocs.c

Tools:
	(cd Tools; make)

clean:
	rm -f *.o *~
	rm -f as1802 ld1802 nm1802 osize1802 dumprelocs1802
	rm -f as316 ld316 nm316 osize316 dumprelocs316
	rm -f as6502 ld6502 nm6502 osize6502 dumprelocs6502
	rm -f as6800 ld6800 nm6800 osize6800 dumprelocs6800
	rm -f as6805 ld6805 nm6805 osize6805 dumprelocs6805
	rm -f as6809 ld6809 nm6809 osize6809 dumprelocs6809
	rm -f as7000 ld7000 nm7000 osize7000 dumprelocs7000
	rm -f as8008 ld8008 nm8008 osize8008 dumprelocs8008
	rm -f as8060 ld8060 nm8060 osize8060 dumprelocs8060
	rm -f as8070 ld8070 nm8070 osize8070 dumprelocs8070
	rm -f as8080 ld8080 nm8080 osize8080 dumprelocs8080
	rm -f as8086 ld8086 nm8086 osize8086 dumprelocs8086
	rm -f as9900 ld9900 nm9900 osize9900 dumprelocs9900
	rm -f ascp1600 ldcp1600 nmcp1600 osizecp1600 dumprelocscp1600
	rm -f asee200 ldee200 nmee200 osizeee200 dumprelocsee200
	rm -f asgb ldgb nmgb osizegb dumprelocsgb
	rm -f ashc11 ldhc11 nmhc11 osizehc11 dumprelocshc11
	rm -f asnova ldnova nmnova osizenova dumprelocsnova
	rm -f aspdp4 ldpdp4 nmpdp4 osizepdp4 dumprelocspdp4
	rm -f aspe16 ldpe16 nmpe16 osizepe16 dumprelocspe16
	rm -f aspe32 ldpe32 nmpe32 osizepe32 dumprelocspe32
	rm -f assuper8 ldsuper8 nmsuper8 osizesuper8 dumprelocssuper8
	rm -f asz8 ldz8 nmz8 osizez8 dumprelocsz8
	rm -f asz80 ldz80 nmz80 osizez80 dumprelocsz80
	rm -f reloc
	(cd Tools; make clean)

# FIXME: allow root specification

install: all
	mkdir -p $(CCROOT)/bin
	cp as1802 ld1802 nm1802 osize1802 dumprelocs1802 $(CCROOT)/bin
	cp as6502 ld6502 nm6502 osize6502 dumprelocs6502 $(CCROOT)/bin
	cp as6800 ld6800 nm6800 osize6800 dumprelocs6800 $(CCROOT)/bin
	cp as6805 ld6805 nm6805 osize6805 dumprelocs6805 $(CCROOT)/bin
	cp as6809 ld6809 nm6809 osize6809 dumprelocs6809 $(CCROOT)/bin
	cp as7000 ld7000 nm7000 osize7000 dumprelocs7000 $(CCROOT)/bin
	cp as8008 ld8008 nm8008 osize8008 dumprelocs8008 $(CCROOT)/bin
	cp as8080 ld8080 nm8080 osize8080 dumprelocs8080 $(CCROOT)/bin
	cp ashc11 ldhc11 nmhc11 osizehc11 dumprelocshc11 $(CCROOT)/bin
	cp as9900 ld9900 nm9900 osize9900 dumprelocs9900 $(CCROOT)/bin
	cp asee200 ldee200 nmee200 osizeee200 dumprelocsee200 $(CCROOT)/bin
	cp asnova ldnova nmnova osizenova dumprelocsnova $(CCROOT)/bin
	cp asz8 ldz8 nmz8 osizez8 dumprelocsz8 $(CCROOT)/bin
	cp asz80 ldz80 nmz80 osizez80 dumprelocsz80 $(CCROOT)/bin
	cp asgb ldgb nmgb osizegb dumprelocsgb $(CCROOT)/bin
	cp assuper8 ldsuper8 nmsuper8 osizesuper8 dumprelocssuper8 $(CCROOT)/bin
	cp reloc $(CCROOT)/bin/relocz80
	cp template $(CCROOT)/bin/template
	cp Tools/Flex/binify $(CCROOT)/bin/flex-binify
	cp Tools/Flex/flexfs $(CCROOT)/bin/flex-fs
	cp Tools/MC10/tapeify $(CCROOT)/bin/mc10-tapeify

installtest: test
	mkdir -p $(CCROOT)/bin
	cp as316 ld316 nm316 osize316 dumprelocs316 $(CCROOT)/bin
	cp as8060 ld8060 nm8060 osize8060 dumprelocs8060 $(CCROOT)/bin
	cp as8070 ld8070 nm8070 osize8070 dumprelocs8070 $(CCROOT)/bin
	cp as8086 ld8086 nm8086 osize8086 dumprelocs8086 $(CCROOT)/bin
	cp ascp1600 ldcp1600 nmcp1600 osizecp1600 dumprelocscp1600 $(CCROOT)/bin
	cp aspe16 ldpe16 nmpe16 osizepe16 dumprelocspe16 $(CCROOT)/bin
	cp aspe32 ldpe32 nmpe32 osizepe32 dumprelocspe32 $(CCROOT)/bin
	cp aspdp4 ldpdp4 nmpdp4 osizepdp4 dumprelocspdp4 $(CCROOT)/bin
