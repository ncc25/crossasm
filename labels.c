#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "asm.h"

#define treesize sizeof(treenode)

extern treenode *symboltab;
extern int asmpass;

void inittree(void) {
  char *lomem;

  symboltab=malloc(treesize);
  lomem=malloc(8);
  strcpy(lomem, "lomem");
  symboltab->labname=lomem;
  symboltab->labadr=0xE00;
  symboltab->llink=NULL; symboltab->rlink=NULL;
}

int deletetree(treenode *tree) {

  if(tree == NULL)
    return(0);
  if(tree->llink != NULL) {
    deletetree(tree->llink);
    tree->llink=NULL;
  }
  if(asmpass&4)
    printf("%20s %04X\n", tree->labname, tree->labadr);
  if(tree->rlink != NULL) {
    deletetree(tree->rlink);
    tree->rlink=NULL;
  }
  free(tree->labname);
  free(tree);
  return(0);
}

int treeinsert(treenode *tree, int symadr, char *symname) {
  int fork;
  treenode *parent, *newnode;
  char *newstr;

  while(tree != NULL) {
    fork=strcmp(symname, tree->labname);
    parent=tree;
    if(fork == 0)
      tree=NULL;
    else if(fork < 0)
      tree=tree->llink;
    else
      tree=tree->rlink;
  }
  if(fork != 0) {
    newnode=malloc(treesize);
    newstr=malloc(strlen(symname)+1);
    strcpy(newstr, symname);
    if(fork < 0)
      parent->llink=newnode;
    else
      parent->rlink=newnode;
    newnode->llink=NULL;
    newnode->rlink=NULL;
    newnode->labname=newstr;
    newnode->labadr=symadr;
  }
  return(0);
}

int treelookup(treenode *tree, char *symname) {
  int fork;
  int rval;

  rval=0;
  while(tree != NULL) {
    fork=strcmp(symname, tree->labname);
    if(fork == 0) {
      rval=tree->labadr;
      tree=NULL;
    } else if(fork < 0)
      tree=tree->llink;
    else
      tree=tree->rlink;
  }
  return(rval);
}

char *checklabel(char *label, int addr) {
  char *result;

  result=label+1;
  skiptospace(result);
  *result='\0';
  result++;

  if((asmpass&2)==2) {
    /* Only lookup and print label on second pass */
    addr=treelookup(symboltab, label);
    printf("%04X : %12c .%s\n", addr, ' ', label);
    return(result);
  }
  if(treelookup(symboltab, label) != 0)
    printerror("Duplicate definition of label");
  else {
    treeinsert(symboltab, addr, label);
  }
  return(result);
}
