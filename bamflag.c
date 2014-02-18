//	Copyright 2012 Dmitri Pervouchine (dp@crg.eu), Lab Roderic Guigo
//	Bioinformatics and Genomics Group @ Centre for Genomic Regulation
//	Parc de Recerca Biomedica: Dr. Aiguader, 88, 08003 Barcelona
//	
//	This file is a part of the 'bamflag' package.
//	'bamflag' package is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//	
//	'bamflag' package is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//	
//	You should have received a copy of the GNU General Public License
//	along with 'bamflag' package.  If not, see <http://www.gnu.org/licenses/>.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <bam.h>

#define MAXFILEBUFFLENGTH 1000
#define INFTY 65535

#define COUNT 10

#define BAM_UNIQUE_MAP 0x800

#define UPDATE_NH_TAG 1
#define MARK_FLAG_FIELD 2 
#define SUPPRESS_MULTI 3

/*************************************************************************************************************************************************/

struct elem {
    char character;
    int  count[2];
    struct elem* next;
    struct elem* tree;
};

typedef struct elem node_type;

/*************************************************************************************************************************************************/

void add_str(node_type **node, char *str, int strand) {
    node_type** ptr = node;
    node_type* new = NULL;

    char c = str[0];

    if(c==0) return;

    while((*ptr)!=NULL) {
        if((*ptr)->character < c) ptr = &((*ptr)->next); else break;
    }

    int flag = 1;
    if((*ptr)!=NULL) {
        if((*ptr)->character == c) {
            flag=0;
        }
    }

    if(flag) {
        new = (node_type*) malloc(sizeof(node_type));
	if(new == NULL) {
	    fprintf(stderr, "[ERROR: cannot allocate memory for a new node, exiting]");
	    exit(1); 
	}
        new->character = c;
        new->count[0] = new->count[1] = 0;
        new->next = (*ptr);
        new->tree = NULL;
        (*ptr) = new;
    }

    if(str[1]==0) {
        (*ptr)->count[strand]++;
    }
    else {
        add_str(&(*ptr)->tree, str + 1, strand);
    }
}

void retrieve(node_type *node, char *pref) {
    node_type* current = node;
    char chr[256];
    int i = 0;
    while(current!=NULL) {
        if(current->count[0]+current->count[1]>0) {
            printf("%s\t%i\t%i\n", pref, current->count[0], current->count[1]);
        }
        else {
            strcpy(chr,pref);
            int l = strlen(chr);
            chr[l] = current->character;
            chr[l+1]=0;
            if(current->tree!=NULL) {
                retrieve(current->tree, chr);
            }
        }
        current = current->next;
        i++;
    }
}


int recall_count(node_type *node, char *str, int strand) {
    node_type* current = node;

    while(current!=NULL) {
        if(current->character == str[0]) {
            return(str[1]==0 ? current->count[strand] : recall_count(current->tree, str+1, strand));
        }
        current = current->next;
    }

    return(0);
}

void destroy(node_type *node) {
    node_type* current = node;

    while(current!=NULL) {
        if(current->tree!=NULL) destroy(current->tree);
        node_type* next = current->next;
        free(current);
        current = next;
    }
}

void remove_slash12(char *str) {
    /* removes /1 /2 at the end of the string*/
    int n;
    n = strlen(str);
    if(n>2) {
        if(str[n-2]=='/') str[n-2]=0;
    }
}

/*************************************************************************************************************************************************/

int main(int argc,char* argv[]) {
    time_t timestamp, current;
    int i,j,k, n,m;
    char *pc;
    int verbose=1;
    node_type* root = NULL;

    int count_table[2][COUNT];

    bamFile bam_input;
    bamFile bam_output;
    bam_header_t *header;
    bam1_t* b;
    bam1_core_t *c;


    char inp_file_name[MAXFILEBUFFLENGTH]="";
    char out_file_name[MAXFILEBUFFLENGTH]="";
    char buff[MAXFILEBUFFLENGTH];
    char chr[MAXFILEBUFFLENGTH];
    int ref_id, ref_id_prev, label;
    char ch;
    int *cigar;
    uint8_t* ptr;
    uint8_t tag_value;

    int n_reads = 2;
    int read_count[2] = {0,0};
    int read;
    int limit=0;
    int examples=0;

    unsigned int curr_pos, last_pos;

    int mode = UPDATE_NH_TAG;

    /** reading input from the command line **/

    timestamp = time(NULL);

    if(argc==1) {
	fprintf(stderr, "This utility counts the number of hits (NH) in a bam file and marks reads by updating the NH tag\n", argv[0]);
	fprintf(stderr, "NOTE: updating the FLAG field (bit 0x800) has been deprecated\n");
        fprintf(stderr, "%s -in <bam_file> -out <bam_file> [-u default = NO] [-v default = NO] [-m <mode> default = %i] [-lim n_reads, default = NONE]\n",argv[0], mode);
	fprintf(stderr, "-in:\tinput BAM file\n-out:\toutput BAM file\n-m:\t%i = mark NH tag, %i = output only uniquely mapped reads\n",UPDATE_NH_TAG, SUPPRESS_MULTI);
	fprintf(stderr, "-v:\tsuppress verbose output\n-u:\ttreat all reads as read1 (single-end)\n-lim:\tprocess only first n_reads (for debugging)\n");
	fprintf(stderr, "A short report on read counts is written to stderr\nPlease send your feedback to dp@crg.eu\n");
        exit(1);
    }

    for(i=1;i<argc;i++) {
        pc = argv[i];
        if(*pc == '-') {
	    if(strcmp(pc+1,"in")  == 0) sscanf(argv[++i], "%s", &inp_file_name[0]);
	    if(strcmp(pc+1,"out") == 0) sscanf(argv[++i], "%s", &out_file_name[0]);
	    if(strcmp(pc+1,"u") == 0) 	n_reads = 1;
	    if(strcmp(pc+1,"v") == 0) 	verbose = 0;
	    if(strcmp(pc+1,"e") == 0)   examples= 1;
	    if(strcmp(pc+1,"m") == 0)	sscanf(argv[++i], "%i", &mode);
	    if(strcmp(pc+1,"lim") == 0) sscanf(argv[++i], "%i", &limit);
	}
    }

    if(inp_file_name[0]==0) {
	fprintf(stderr,"Bam not specified, exiting\n");
	exit(1);
    }

    bam_input = bam_open(inp_file_name, "r");
    header = bam_header_read(bam_input);
    
    if(bam_input == NULL || header == NULL) {
        fprintf(stderr,"Input BAM file can't be opened or contains no header, exiting\n");
        exit(1);
    }

    bam_output = bam_open(out_file_name, "w");
    if(bam_output==NULL) {
        fprintf(stderr,"Output BAM file can't be opened, exiting\n");
        exit(1);
    }
    bam_header_write(bam_output, header);

    if(verbose && (mode == UPDATE_NH_TAG))   fprintf(stderr,"[mode: update NH tag]\n");
    if(verbose && (mode == SUPPRESS_MULTI))  fprintf(stderr,"[mode: output only uniquely-mapped reads]\n");


    /*** pass 1 ***/
    if(verbose) fprintf(stderr,"[pass 1, reading %s", inp_file_name);
    b = bam_init1();
    n = 0;
    while(bam_read1(bam_input, b)>=0) {
        c   = &b->core;
	ref_id = c->tid;
	if(ref_id<0) continue;


        read = (c->flag & BAM_FREAD1) ? 0 : 1;
	if(n_reads == 1) read = 0; 

	pc =  bam1_qname(b);
        remove_slash12(pc);
	if(!(c->flag & BAM_FUNMAP)) {
	    add_str(&root, pc, read);
	    read_count[read]++;
	}
	n++;
	if(limit>0 && n>limit) break;
    }
    bam_header_destroy(header);
    bam_close(bam_input);
    bam_destroy1(b);
    if(verbose) for(read = 0;read < n_reads; read++) fprintf(stderr,", read%i=%i", read, read_count[read]);
    if(verbose) fprintf(stderr,"]\n");
    /*** end of pass 1 ***/


    /*** pass 2 ***/
    if(verbose) fprintf(stderr,"[pass 2, reading %s, writing to %s", inp_file_name, out_file_name);
    bam_input = bam_open(inp_file_name, "r");
    header = bam_header_read(bam_input);

    b = bam_init1();
    for(i=0;i<COUNT;i++) for(read = 0;read < n_reads; read++) count_table[read][i]=0;
    n = 0;
    while(bam_read1(bam_input, b)>=0) {
        c   = &b->core;
        ref_id = c->tid;
        if(ref_id<0) continue;

	read = (c->flag & BAM_FREAD1) ? 0 : 1;
        if(n_reads == 1) read = 0;

        pc =  bam1_qname(b);
	remove_slash12(pc);
	i = recall_count(root, pc, read);

	if(mode == UPDATE_NH_TAG) {
	    ptr = bam_aux_get(b, "NH");
	    if(ptr==NULL) {
		tag_value = (uint8_t)i;
	    	bam_aux_append(b, "NH", 'I', 4, &tag_value);
	    }
	    else {
		(*(ptr+1)) = (uint8_t)i;
	    }
	}

/*	if(mode == MARK_FLAG_FIELD) {
	    c->flag = (c->flag | BAM_UNIQUE_MAP) ^ BAM_UNIQUE_MAP;
	    if(i==1) c->flag = c->flag | BAM_UNIQUE_MAP;
	} */

	if(mode == SUPPRESS_MULTI) {
	    if(i==1) bam_write1(bam_output, b);
	}
	else {
	    bam_write1(bam_output, b);
	}

	if(i>=COUNT) i=COUNT-1;
	if(i>1) if(count_table[read][i]==0 && examples) fprintf(stderr, "(read=%i, count=%i, id=%s)",read,i,pc);
	count_table[read][i]++;
	n++;
	if(limit>0 && n>limit) break;
    }
    bam_header_destroy(header);
    bam_close(bam_input);
    bam_close(bam_output);
    bam_destroy1(b);
    if(verbose) fprintf(stderr,"]\n");
    /*** end of pass 2 ***/


    if(verbose) fprintf(stderr,"[destroying data structures");
    destroy(root);
    if(verbose) fprintf(stderr,"]\n");

    current = time(NULL);

    /*** output ***/
    fprintf(stderr,"count\t");
    for(read = 0;read < n_reads; read++) fprintf(stderr, "read%i\t(%)\t", 1 + read);
    fprintf(stderr,"\n");
    for(i=0;i<8*(n_reads*2+1);i++) fprintf(stderr, "-");
    fprintf(stderr,"\n");

    for(i=1;i<COUNT;i++) {
	if(i==COUNT-1) fprintf(stderr,"%i+\t", i); else fprintf(stderr,"%i\t", i);
	for(read = 0;read < n_reads; read++) {
	    fprintf(stderr, "%i\t%2.1lf\t",count_table[read][i], (double)100*count_table[read][i]/read_count[read]);
	}
	fprintf(stderr,"\n");
    }

    for(i=0;i<8*(n_reads*2+1);i++) fprintf(stderr, "-");
    fprintf(stderr,"\ntotal\t");
    for(read = 0;read < n_reads; read++) fprintf(stderr, "%i\t100\t", read_count[read]);
    fprintf(stderr,"\n");
    /*** end of output ***/

    if(verbose) fprintf(stderr,"[completed in %1.0lf seconds]\n",difftime(current,timestamp));

    return 0;
}
