#define IMPLADR 0
#define ACCADR 1
#define IMMEDADR 2
#define ZPADR 3
#define ZPXADR 4
#define ZPYADR 5
#define ABSADR 6
#define ABSXADR 7
#define ABSYADR 8
#define INDRADR 9
#define INDRXADR 10
#define INDRYADR 11

typedef struct {
  char opname[4];
  int flags;
  unsigned char opcodes[12];
} opentry;

typedef struct {
  int labadr;
  char *labname;
  void *llink, *rlink;
} treenode;

extern int lookupop(char *name, int *flags);
extern int lookupadr(char *name, int amode);
 
extern void asmlisting(char *fmt, int opc, int amode, int operand,
		                                  int opflags);
extern void printerror(char *msg);
extern unsigned char reladr(int aadr);

extern char *checklabel(char *label, int addr);
extern void inittree(void);
extern int deletetree(treenode *tree);
int treelookup(treenode *tree, char *symname);

extern int assemble(char *line);

extern void asmdirect(char *line);

#define skipspace(x) while(*x == ' ' || *x == '\x09') x++
#define skiptospace(x) while(*x != ' ' && *x != '\x09') x++
