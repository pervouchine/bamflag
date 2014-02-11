bamflag
========
This utility flags uniquely mapped reads in BAM files or updates the NH tag according to the number of alignments of the query

./bamflag -in <bam_file> -out <bam_file> [-u default = NO] [-v default = NO] [-m <mode> default = 1] [-lim n_reads, default = NONE]
-in:	input BAM file
-out:	output BAM file
-m:	1 = mark NH tag, 2 = mark FLAG field, 3 = output only unique reads
-v:	suppress verbose output
-u:	treat all reads as read1 (single-end)
-lim:	process only first n_reads (for debugging)
A short report on read counts is written to stderr

bamflag and bamflag2 are slightly different.
bamflag is a bit slower but requires less memory; bamflag2 is a bit faster but requires more memory.
The recommended utility is bamflag

