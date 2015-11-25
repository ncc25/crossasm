#include "asm.h"
#include <stdio.h>

extern unsigned char *codebuf;
extern int asmpass;
extern FILE *listfile;

int lineno;
int errors;
int destadr, codeorig;

void printerror(char *msg) {

  if((asmpass&2)==2) {
    /* Only report errors on the second pass */
    printf("%s at line %d\n", msg, lineno);
    errors++;
  }
}

int checkvalidadr(int amode, int base) {
  int max;

  if(amode>=ABSADR && amode<=INDRADR)
    max=65535;
  else
    max=255;
  if(base<0 || base>max) {
    printerror("Base address out of range");
    return(1);
  }
  return(0);
}

void storeasm(int nbytes, unsigned char *instr, char *fmt) {
  int i;

  for(i=0; i<nbytes; i++)
    codebuf[destadr+i]=instr[i];
  if((asmpass&3)==3) {
    /* Only print listing on second pass */
      fprintf(listfile, "%04X : ", destadr+codeorig);
      for(i=0; i<4; i++) {
	if(i<nbytes)
	  fprintf(listfile, "%02X ", (int) instr[i]);
	else
	  fprintf(listfile, "   ");
      }
      fprintf(listfile, "  %s", fmt);
  }
  destadr+=nbytes;
}

unsigned char reladr(int aadr) {
  int result;
  
  result=aadr-(codeorig+destadr+2);
  if(result<-128 || result>127)
    printerror("Address out of range for branch instruction");
  return(result&0xFF);
}

void asmlisting(char *fmt, int opc, int amode, int operand, int opflags) {
  int i, nbytes;
  unsigned char asminstr[3];

  if(amode>=ZPADR)  
    checkvalidadr(amode, operand);
  if(amode<=ACCADR)
    nbytes=1;
  else if(amode>=ABSADR && amode<=INDRADR) {
    nbytes=3;
    asminstr[1]=operand & 0xFF;
    asminstr[2]=(operand & 0xFF00) >> 8;
  } else {
    nbytes=2;
    asminstr[1]=operand & 0xFF;
  }
  if(opflags&2) {
    nbytes=2;
    asminstr[1]=reladr(operand);
  }
  asminstr[0]=opc & 0xFF;
  storeasm(nbytes, asminstr, fmt);
}

  
