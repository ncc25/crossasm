#include <stdio.h>
#include <string.h>
#include "asm.h"

extern int readnum(char *str, char **finish, int *zpflag);
extern void storeasm(int nbytes, unsigned char *instr, char *fmt);
extern treenode *symboltab;
extern int asmpass;

extern int codeorig;

int org_direct(char *line) {
  int i, zp;
  char *end;

  line+=4;
  skipspace(line);
  i=readnum(line, &end, &zp);
  if(zp)
    printerror("Warning, assembling code to zero page");
  if(codeorig==0)
    codeorig=i;
  else if(i != codeorig)
    printerror("Attempt to change base address");
  return(1);
}

int equs(char *line) {
  unsigned char acode[4];
  char *temp, *endp;
  char fmtout[80];

  line+=5;
  skipspace(line);
  temp=line;
  endp=strchr(++line, '\"');
  *endp='\0';
  while(strlen(line)>=4) {
    strncpy((char *) acode, line, 4);
    storeasm(4, acode, "\n");
    line+=4;
  }
  sprintf(fmtout, "#EQUS %s\"\n", temp);
  strncpy((char *) acode, line, strlen(line));
  storeasm(strlen(line), acode, fmtout);
  return(0);
}

int equ_direct(char *line) {
  int i, data, len, zp;
  char *end;
  unsigned char acode[4];
  char fmtout[20];

  switch(line[4]) {
    case 'B': len=1; break;
    case 'W': len=2; break;
    case 'D': len=4; break;
    case 'S': len=equs(line); break;
  }
  if(len!=0) {
      line+=5;
      skipspace(line);
      data=readnum(line, &end, &zp);
      sprintf(fmtout, "#EQU &%x\n", data);
      zp=0xFF;
      for(i=0; i<len; i++) {
	acode[i]=(data&zp)>>(i*8);
	zp=zp<<8;
      }
      storeasm(len, acode, fmtout);
  }
  return(len);
}

void const_direct(char *line) {
  char *adline, *end;
  int addr, zp;

  if((asmpass&2)==2)
    return; /* Ignore definition on second pass */
  line+=5;
  skipspace(line);
  adline=line;
  skiptospace(adline);
  *adline='\0';
  adline++;
  addr=readnum(adline, &end, &zp);
  if(treelookup(symboltab, line) != 0)
    printerror("Duplicate definition of label");
  else {
    treeinsert(symboltab, addr, line);
  }
}

void asmdirect(char *line) {

    if(strncmp(line, "#ORG ", 5)==0)
      org_direct(line);
    else if(strncmp(line, "#SYMB ", 6)==0)
      const_direct(line);
    else if(strncmp(line, "#EQU", 4)==0)
      equ_direct(line);
    else
      printerror("Unknown assembler directive");
}
