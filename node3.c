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

#include "node3.h"

void init_node(node_type *node) {
    int i,j;
    for(i=0;i<ALPHANUM_RANGE;i++) {
	node->next[i]=NULL;
	for(j=0;j<2;j++) node->count[i][j]=0;
    }
}
 
int add_str(node_type *node, char *str, int strand) {
    int i;

    i = (int)str[0] - ALPHANUM_START;
    if(i<0 || i>=ALPHANUM_RANGE) return(1);

    if(str[1]==0) {
	node->count[i][strand]++;
	return(0);
    }

    if(node->next[i]==NULL) {
	node->next[i] = (node_type*)malloc(sizeof(node_type));
	if(node->next[i]==NULL) {
	    fprintf(stderr,"Cannot allocate enough memory, exiting\n");
	    exit(1);
	}
	init_node(node->next[i]);
    }

    return(add_str(node->next[i], str+1, strand));
}

void retrieve(node_type *node, char *pref) {
    char chr[256];
    int i,j,l;

    for(i=0;i<ALPHANUM_RANGE;i++) {
	strcpy(chr,pref);
	l = strlen(chr);
	chr[l] = (char)(i+ALPHANUM_START);
	chr[l+1]=0;
	for(j=0;j<2;j++) if(node->count[i][j]>0) printf("%s\t%i\t%i\n",chr,j,node->count[i][j]);
	if(node->next[i]!=NULL) {
	    retrieve(node->next[i],chr);	
	}
    }
}


int recall_count(node_type *node, char *str, int strand) {
    int i;

    i = (int)str[0] - ALPHANUM_START;
    if(i<0 || i>=ALPHANUM_RANGE) return(0);

    if(str[1]==0) {
	return(node->count[i][strand]);
    }
    if(node->next[i]==NULL) return(0);
    return(recall_count(node->next[i], str+1, strand));

}

void destroy(node_type *node) {
    int i;
    for(i=0;i<ALPHANUM_RANGE;i++) {
        if(node->next[i]!=NULL) {
	    destroy(node->next[i]);
	    free(node->next[i]);
	}
    }
}

