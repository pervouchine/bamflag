Copyright 2012 Dmitri Pervouchine (dp@crg.eu), Lab Roderic Guigo
Bioinformatics and Genomics Group @ Centre for Genomic Regulation
Parc de Recerca Biomedica: Dr. Aiguader, 88, 08003 Barcelona

This file is a part of the 'bamflag' package.
'bamflag' package is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

'bamflag' package is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with 'bamflag' package.  If not, see <http://www.gnu.org/licenses/>.

========================================================================

INSTALLATION INSTRUCTIONS:

To install, enter the directory and type:
make all

Prerequisites:
	You need to install samtools

	Get it by svn:
	svn co https://samtools.svn.sourceforge.net/svnroot/samtools/trunk/samtools
	enter the directory and type 'make all'

(!)	do not forget to update the SAMDIR varibale in makefile

========================================================================

This utility marks the uniquely mapped reads in a bam file by updating the FLAG field (bit 0x800) or by updating the NH tag (see SAMtools) or by reporting ONLY uniquely mapped reads

It consists of two slightly different implementations, bamflag and bamflag2; 
  - bamflag is slightly faster but takes more memory
  - bamflag2 is less fast but takes less memory

USAGE:
./bamflag -in <bam_file> -out <bam_file> [-u default = NO] [-v default = NO] [-m <mode> default = 1] [-lim n_reads, default = NONE]
	-in:	input BAM file
	-out:	output BAM file
	-m:	1 = mark NH tag, 2 = mark FLAG field, 3 = output ONLY uniquely mapped reads
	-v:	suppress verbose output
	-u:	treat all reads as read1 (unstranded)
	-lim:	process only first n_reads (for debugging)

A short report on read counts is written to stderr. Below is a short example of verbose output

========================================================================

[mode: update NH tag]
[pass 1, reading wgEncodeCshlLongRnaSeqBjCellPapAlnRep1.bam]
[pass 2, reading wgEncodeCshlLongRnaSeqBjCellPapAlnRep1.bam, writing to output.bam]
[destroying data structures]

count   read1   (%)     read2   (%)     
 ----------------------------------------
1       97317030        88.6    99331858        88.6    
2       7003492 6.4     6992832 6.2     
3       2366796 2.2     2662251 2.4     
4       1701016 1.5     1506784 1.3     
5       396040  0.4     463460  0.4     
6       488850  0.4     532668  0.5     
7       177660  0.2     195202  0.2     
8       146264  0.1     173976  0.2     
9+      249223  0.2     279437  0.2     
 ----------------------------------------
total   109846371       100     112138468       100     
[completed in 5319 seconds]

========================================================================


2013-04-12
	Corrected: unmapped reads and not counted how many time they are present

2014-02-18
	A bug with counting unmapped reads as being uniquely mapped has been fixed, thanks to Emilio Palumbo!


