GCC=gcc
OPTN=-lz
SAMTOOLS_DIR=samtools-0.1.18/

.PHONY: all

all: bamflag bamflag2 

${SAMTOOLS_DIR}libbam.a:
	wget http://sourceforge.net/projects/samtools/files/samtools/0.1.18/samtools-0.1.18.tar.bz2
	tar -xf samtools-0.1.18.tar.bz2
	rm -f samtools-0.1.18.tar.bz2
	make -C samtools-0.1.18 all
        # If SAMTOOLS is already installed, you might want to update SAMTOOLS_DIR path without installing a fresh copy

bamflag: bamflag.c $(SAMTOOLS_DIR)libbam.a
	$(GCC) $(OPTN) -I $(SAMTOOLS_DIR) bamflag.c $(SAMTOOLS_DIR)libbam.a -o bamflag

bamflag2: bamflag2.c $(SAMTOOLS_DIR)libbam.a
	$(GCC) $(OPTN) -I $(SAMTOOLS_DIR) bamflag2.c $(SAMTOOLS_DIR)libbam.a -o bamflag2

clean:
	rm -f -r bamflag bamflag2
