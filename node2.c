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

#include "node2.h"
 
void add_str(node_type **node, char *str) {
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
	new->character = c;
    	new->count = 0;
    	new->next = (*ptr);
    	new->tree = NULL;
	(*ptr) = new;
    }

    if(str[1]==0) {
	(*ptr)->count++;
    }
    else {
	add_str(&(*ptr)->tree, str + 1);
    }
}

void retrieve(node_type *node, char *pref) {
    node_type* current = node;
    char chr[256];
    int i = 0;
    while(current!=NULL) {
	if(current->count > 0) {
	    printf("%s\t%i\n", pref, current->count);
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


int recall_count(node_type *node, char *str) {
    node_type* current = node;

    while(current!=NULL) {
	if(current->character == str[0]) {
	    return(str[1]==0 ? current->count : recall_count(current->tree, str+1));
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

