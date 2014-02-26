# $Id: Makefile,v 1.25 2001/05/23 09:41:34 ilsoe Exp $
#

DIRNAME=gbvm

all:
	cd src/common && $(MAKE)
#	cd src/gc && $(MAKE)
	cd src/gc_new && $(MAKE)
#	cd src/allocator && $(MAKE)
	cd src/vm && $(MAKE)

.PHONY: clean backup html

clean:
	rm -f *~
	cd src/common && $(MAKE) clean
#	cd src/gc && $(MAKE) clean
	cd src/gc_new && $(MAKE) clean
#	cd src/allocator && $(MAKE) clean
	cd src/vm && $(MAKE) clean

backup: html
	cd ..;tar zcvf /user/ilsoe/p10/backups/$(DIRNAME)-`date +%Y-%m-%d-%H.%M`.tar.gz $(DIRNAME)

html:
	./rebuildhtml

# making dependencies...

depend:
	cd src/common && makedepend -f$(DEPFILE) $(DEPDIRS) *.cc
	cd src/gc && makedepend -f$(DEPFILE) $(DEPDIRS) -I../common -I../gc_common *.cc
	cd src/gc_new && makedepend -f$(DEPFILE) $(DEPDIRS) -I../common -I../gc_common *.cc
	cd src/allocator && makedepend -f$(DEPFILE) $(DEPDIRS) -I../common -I../gc_common *.cc
	cd src/vm && makedepend -f$(DEPFILE) $(DEPDIRS) -I../common -I../gc_new -I../gc_common *.cc
