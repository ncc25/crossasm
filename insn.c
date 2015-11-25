#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "asm.h"

extern treenode *symboltab;
extern int machine;

int assemble(char *line) {
  char *op, *adr, ch;
  char op_adr[24];
  int am, opc, opr, opf;

  if(*line == '\0' || *line == '\n')
    return(0);
  op=line;
  while(*line != ' ' && *line != '\n')
    line++;
  *line='\0';
  if(lookupop(op, &opf)<0) {
    printerror("Unknown instruction nmemonic");
    return(0);
  }
  strcpy(op_adr, op);
  op_adr[3]=' ';
  line++;
  skipspace(line);
  adr=line;
  ch=*line;
  if(ch != '/' && ch != '\n' && ch != '\0') {
    switch(ch) {
      case 'A': am=am_accum(adr, op_adr+4); break;
      case '#': am=am_immed(adr, op_adr+4, &opr); break;
      case '(': am=am_indir(op, adr, op_adr+4, &opr); break;
      case '~': am=am_bbrs(adr, op_adr+4, &opr); break;
      default: am=am_absadr(op, adr, op_adr+4, &opr); break;
    }
  } else {
    am=IMPLADR;
    op_adr[4]='\n';
    op_adr[5]='\0';
  }
  if(opf&4) {
    opc=lookupadr(op, ABSADR)+(am-1)*16;
    am=(opc&8) ? ABSADR : ZPADR;
  } else
    opc=lookupadr(op, am);
  if(opc==0 && !(opf&1))
    printerror("Invalid addressing mode for instruction");
  if((opf&24)>machine) {
    printerror("Warning, Use of extended opcode");
  }
  asmlisting(op_adr, opc, am, opr, opf);
  return(0);
}

int checkterm(char *str) {

  skipspace(str);
  if(*str != '/' && *str != '\n' && *str != '\0') {
    printerror("Unrecognised addressing mode");
    return(0);
  } else {
    return(1);
  }
}

int readlabel(char *labname, char **finish) {
  char ncopy[20];
  int i, base;

  i=0;
  while(isalnum(*labname))
    ncopy[i++]=*labname++;
  ncopy[i]='\0';
  *finish=labname;
  base=treelookup(symboltab, ncopy);
  if(base == 0) {
    printerror("Undefined label");
    base=0x100;
  }
  return(base);
}

int readnum(char *str, char **finish, int *zpflag) {
  int base, forcew;

  *zpflag=0;
  forcew=0;
  if(*str == '$') {
    forcew=1;
    str++;
  }
  if(*str == '&')
    base=(int) strtol(++str, finish, 16);
  else if(isdigit(*str))
    base=(int) strtol(str, finish, 10);
  else
    base=readlabel(str, finish);
  if(base<256 && !forcew)
    *zpflag=1;
  return(base);
}

int am_accum(char *adr, char *fmtout) {

  adr++;
  checkterm(adr);
  sprintf(fmtout, "A\n");
  return(ACCADR);
}

int am_immed(char *adr, char *fmtout, int *operand) {
  int iconst;
  char *end;

  adr++;
  if(*adr == '&')
    iconst=(int) strtol(++adr, &end, 16);
  else if(*adr == '\'') {
    iconst=(int) *(++adr);
    end=++adr;
  } else
    iconst=(int) strtol(adr, &end, 10);
  checkterm(end);
  if(iconst<-128 || iconst>255) {
    printerror("Immediate constant out of range");
    iconst &= 0xFF;
  }
  sprintf(fmtout, "#&%2x\n", iconst);
  *operand=iconst;
  return(IMMEDADR);
}

int am_absadr(char *op, char *adr, char *fmtout, int *operand) {
  int base, zp, index, opf;
  char *end, ch;

  index=0;
  base=readnum(adr, &end, &zp);
  if(*end == ',') {
    ch=*(++end);
    end++;
    switch(ch) {
      case 'X': index=1; sprintf(fmtout, "&%4x,X\n", base); break;
      case 'Y': index=2; sprintf(fmtout, "&%4x,Y\n", base); break;
      default: printerror("Unrecognised index register");
    }
  }
  *operand=base;
  if(index==0)
    sprintf(fmtout, "&%4x\n", base);
  checkterm(end);
  if(zp && lookupadr(op, ZPADR+index)!=0)
    return(ZPADR+index);
  else
    return(ABSADR+index);
}

int am_indir(char *op, char *adr, char *fmtout, int *operand) {
  int base, zp, mode;
  char *end, ch;

  adr++;
  base=readnum(adr, &end, &zp);
  if(strncmp(end, ",X", 2)==0) {
    sprintf(fmtout, "(&%4x,X)\n", base);
    mode=INDRXADR;
  } else if(strncmp(end, "),Y", 3)==0) {
    sprintf(fmtout, "(&%4x),Y\n", base);
    mode=INDRYADR;
  } else {
    sprintf(fmtout, "(&%4x)\n", base);
    mode=INDRADR;
    if(*end != ')')
      printerror("Unknown form of indirect addressing");
  }
  *operand=base;
  return(mode);
}

int am_bbrs(char *adr, char *fmtout, int *operand) {
  int bitno, zp, zpadr, dest;
  char *end;

  adr++;
  bitno=(int) (*adr)-48;
  if(bitno<1 || bitno>8) {
    printf("Bit reference out of range");
    bitno=1;
  }
  zpadr=readnum(adr+2, &end, &zp);
  if(*end == ',') {
    dest=readnum(end+1, &end, &zp);
    *operand=(reladr(dest)<<8)+zpadr;
    sprintf(fmtout, "~%1d,&%2x,&%4x\n", bitno, zpadr, dest);
    if(!zp)
      printerror("Memory address out of range");
  } else {
    *operand=zpadr;
    sprintf(fmtout, "~%1d,&%2x\n", bitno, zpadr);
  }
  return(bitno);
}

 
  

