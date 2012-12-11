SAMDIR=~/samtools/
GCC=gcc
OPTN=-lz

.PHONY: all

all: bamflag bamflag2

EXPORT = bamflag-1.2

export:
	mkdir $(EXPORT)/
	cp -f *.c $(EXPORT)/
	cp -f *.h $(EXPORT)/
	cp -f *.sh $(EXPORT)/
	cp -f README $(EXPORT)/
	cp -f VERSION $(EXPORT)/
	cp -f LICENCE $(EXPORT)/
	cp makefile $(EXPORT)/
	tar -cf $(EXPORT).tar $(EXPORT)/
	gzip $(EXPORT).tar
	rm -f -r $(EXPORT)/
	mv $(EXPORT).tar.gz ..

$(SAMDIR)libbam.a:
	# You need to install samtools
	# Get it by svn:
	# svn co https://samtools.svn.sourceforge.net/svnroot/samtools/trunk/samtools
	# enter the dir and type 'make all'
	# don't forget to update the SAMDIR varibale in this makefile
	exit 1	

progressbar.o:	progressbar.c progressbar.h
	$(GCC) -c progressbar.c 

node.o: node.c node.h
	$(GCC) -c node.c

node2.o: node2.c node2.h
	$(GCC) -c node2.c

bamflag: bamflag.c progressbar.o $(SAMDIR)libbam.a node.o
	$(GCC) $(OPTN) -I $(SAMDIR) bamflag.c progressbar.o node.o $(SAMDIR)libbam.a -o bamflag

bamflag2: bamflag2.c progressbar.o $(SAMDIR)libbam.a node2.o
	$(GCC) $(OPTN) -I $(SAMDIR) bamflag2.c progressbar.o node2.o $(SAMDIR)libbam.a -o bamflag2

clean:
	rm -f -r progressbar.o node.o node2.o bamflag bamflag2
