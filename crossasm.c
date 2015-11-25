#include <stdio.h>
#include <string.h>
#include "asm.h"

extern int lineno, codeorig, destadr;

unsigned char *codebuf;
treenode *symboltab;
int asmpass, machine;
FILE *listfile;

int processfile(FILE *srcfile, int start, int flags) {
  char srctemp[80];
  char *srcline;
  int thispass;

  for(thispass=0; thispass<=1; thispass++) {
      asmpass=flags+(thispass<<1);
      rewind(srcfile);
      destadr=start;
      lineno=1;
      while(fgets(srctemp, 80, srcfile) != NULL) {
        srcline=srctemp;
	skipspace(srcline);
	if(*srcline=='.') {
	  srcline=checklabel(srcline+1, codeorig+destadr);
	  skipspace(srcline);
	}
	if(*srcline=='#')
	  asmdirect(srcline);
	else if(*srcline!='/')
	  assemble(srcline);
	lineno++;
      }
  }
  return(destadr);
}

int main(int argc, char *argv[]) {
  FILE *srcfile, *destfile;
  char *end, *sfile;
  int i, passflags, codelen, nfiles;

  printf("6502 Cross Assembler, version 0.7\n");
  inittree();
  codebuf=(unsigned char *)malloc(512);
  i=1;
  codeorig=0;
  machine=0;
  passflags=0;
  sfile=NULL;
  while(*argv[i]=='-' && i<argc) {
    switch(argv[i][1]) {
      case 'o': codeorig=strtol(argv[i+1], &end, 16); i++; break;
      case 'l': passflags|=1; break;
      case 't': passflags|=4; break;
      case 's': sfile=argv[i]+2; break;
      case 'm':
        switch(argv[i][2]) {
	  case 'c': machine=8; break;
	  case 'r': machine=16; break;
  	  default: machine=0;
	}
	break;
    }
    i++;
  }
  codelen=0;
  nfiles=0;
  if(sfile != NULL)
    listfile=fopen(sfile, "w");
  else
    listfile=stdout;
  while(i<argc) {
      printf("Assembling file %s:\n", argv[i]);
      srcfile=fopen(argv[i++], "r");
      codelen=processfile(srcfile, codelen, passflags);
      fclose(srcfile);
      nfiles++;
  }
  destfile=fopen("asmout", "wb");
  fwrite(codebuf, sizeof(unsigned char), destadr, destfile);
  fclose(destfile);
  printf("Assembly complete, processed %d files, output size %d bytes\n", nfiles, codelen);
  deletetree(symboltab);
  return(0);
}
