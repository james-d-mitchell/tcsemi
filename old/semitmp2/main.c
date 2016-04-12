#include "global.h"
#include "words.h"
#include <limits.h>
#include "coinc.h"

/* Options in the main routine */
#define ENUM 0	/* enumerate */
#define IDEM 1	/* idempotents */
#define ELTS 2	/* list */
#define CALC 3	/* word */
#define QUIT 4	/* quit */
#define HELP 5	/* help */
#define KER0 6	/* Was kernels - no longer supported*/
#define KER1 7	/* Was print_kernels - no longer supported*/
#define LRID 8	/* ideals */
#define SFRM 9	/* int */
#define LFRM 10	/* int */
#define DERV 11	/* derive */
#define TBLE 12	/* table */
#define EQUN 13	/* solve */
#define TRNS 14 /* calculate transformations */
#define TSIZ 15 /* set table size */
#define EFIL 16 /* enumerate from file */
#define LAHD 17 /* Set positions for lookahead */
#define COIN 18 /* Select coincidence processing method */
#define STOR 19 /* Store current subset of semigroup */
#define REST 20 /* Restore a previously saved subset of semigroup */
#define INTR 21 /* Form intersection of two subsets */
#define UNON 22 /* Form union of two subsets */
#define ISGP 23 /* Determine whether current subset is a group */
#define PSET 24 /* Print subset */
#define COMP 25 /* Complement of a subset */

#define CMDLEN 80 /* Maximum length of a command */
int numgens,numrels;	/* Numbers of generators and relations */
int *table;		/* Definitions table */
int tablesize,last;	/* Table size; last element used in table */
int deleted;		/* Number of elements deleted as a result of
			   coincidences but not yet removed by packing */
int killed;		/* Number of elements deleted & removed by packing */
int defs;		/* Total number of definitions made */
int rows_complete;	/* Rows up to this number are complete */
int cstackptr;		/* Pointer to coincidence stack/queue */
int cstackqptr;		/* If queueing, pointer to the end of the queue */
int q_empty;		/* If queueing, indicates whether the queue is empty */
int cstack[CSTACKLIMIT+1][2];	/* The coincidence stack */
int translate[NUMCHARS];	/* Table of generator names -> numbers */
int enumerating;	/* Flag indicating whether currently enumerating */
int valid_table;	/* Flag indicating whether table contains a semigroup */
int packing;		/* Flag indicating whether a pack is in progress */
int long_form;		/* Flag indicating form of output for ideals command */
int lpp;		/* Lines per page for output to terminals. */
int stack_depth,stack_max;
jmp_buf err_return;
struct relation *rels;
int la_pos[LAMAX];

void set_la_points()
{	register i;
	int t,nextpos;
	for (i=0;i<LAMAX;i++) la_pos[i]=INT_MAX;
	printf("Enter points at which to look ahead, in ascending order separated by spaces\n");
	printf("or newlines and terminated with -1.\n");
	i=0;
	while (i<LAMAX)
	{	t=scanf("%d",&nextpos);
		if (t==0) {printf("Error during input - some lookahead points may not have been recorded\n");return;}
		if (nextpos==-1) break;
		la_pos[i++]=nextpos;
	}
	while (getchar()!=CR);
	return;
}

static int getoption(s)
char *s;
{	if (strcmp(s,"e")==0) return ENUM;
	if (strcmp(s,"i")==0) return IDEM;
	if (strcmp(s,"l")==0) return ELTS;
	if (strcmp(s,"w")==0) return CALC;
	if (strcmp(s,"dw")==0) return DERV;
	if (strcmp(s,"q")==0) return QUIT;
	if (strcmp(s,"h")==0) return HELP;
	if (strcmp(s,"k")==0) return KER0;
	if (strcmp(s,"kp")==0) return KER1;
	if (strcmp(s,"id")==0) return LRID;
	if (strcmp(s,"sf")==0) return SFRM;
	if (strcmp(s,"lf")==0) return LFRM;
	if (strcmp(s,"t")==0) return TBLE;
	if (strcmp(s,"se")==0) return EQUN;
	if (strcmp(s,"tf")==0) return TRNS;
	if (strcmp(s,"ts")==0) return TSIZ;
	if (strcmp(s,"ef")==0) return EFIL;
	if (strcmp(s,"la")==0) return LAHD;
	if (strcmp(s,"cm")==0) return COIN;
	if (strcmp(s,"st")==0) return STOR;
	if (strcmp(s,"rs")==0) return REST;
	if (strcmp(s,"in")==0) return INTR;
	if (strcmp(s,"un")==0) return UNON;
	if (strcmp(s,"gp")==0) return ISGP;
	if (strcmp(s,"ps")==0) return PSET;
	if (strcmp(s,"cp")==0) return COMP;
	return -1;
}
	
static void help()
{	printf("e  - Enumerate presentation\n");
	printf("ef - Enumerate presentation from file\n");
        printf("i  - Find idempotents\n");
        printf("l  - List elements\n");
        printf("w  - Evaluate word\n");
	printf("dw - Evaluate word, attempting to show derivation\n");
	printf("id - Evaluate a left or right ideal\n");
	printf("sf - Print only element numbers when evaluating ideals\n");
	printf("lf - Print element numbers & representations as words when evaluating ideals\n");
	printf("t  - Print definitions table\n");
	printf("tf - Calculate transformations\n");
	printf("ts - Alter table size\n");
	printf("se - Solve equation\n");
	printf("la - Set lookahead positions\n");
	printf("cm - Select coincidence processing method\n");
	printf("st - Store surrent subset\n");
	printf("rs - Restore a stored subset\n");
	printf("in - Intersect subsets\n");
	printf("un - Take union of two subsets\n");
	printf("gp - Determine whether current subset is a group\n");
	printf("ps - Print current subset\n");
	printf("cp - Complement current subset\n");
        printf("h  - Help\n");
        printf("q  - Quit\n");
}

void title()
{	printf("                      Semigroup enumerator - T.G.W\n");
	printf("                      Lookahead version %s %s\n",VERSION,SYSTEM);
}


main()
{	char *s= (char *) malloc(CMDLEN);
	register i;
	int opt;
	title();
	help();
	if (!setjmp(err_return))
		init_table(INITABLESIZE);
	else
	{	printf("Could not initialise table.\n");
		exit(1);
	}
	setjmp(err_return);
	valid_table=0;
	cprocessor=qcoinc;
	for (i=0;i<LAMAX;i++) la_pos[i]=INT_MAX;
	long_form=0; /* Use short form printing for ideals */
	for(;;)
	{	s=getword(1);
		opt=getoption(s);
		switch (opt)
		{	case ENUM : newpresentation(stdin);
			            break;
			case IDEM : getidempotents();
			            break;
			case ELTS : getelements();
			            break;
			case CALC : calcwords(0,getword(0));
			            break;
			case DERV : calcwords(2,getword(0));
			            break;
			case HELP : help();
			            break;
			case KER0 : kernels(0);
			            break;
			case KER1 : kernels(1);
			            break;
			case LRID : calcideal();
			            break;
			case SFRM : long_form=0;
			            break;
			case LFRM : long_form=1;
			            break;
			case TBLE : print_table();
			            break;
			case EQUN : solve_equation();
			            break;
			case TRNS : transformations();
			            break;
			case TSIZ : setsize();
			            break;
			case EFIL : e_from_file();
			            break;
			case LAHD : set_la_points();
			            break;
			case COIN : select_processor();
			            break;
			case STOR : store_subset();
			            break;
			case REST : restore_subset();
			            break;
			case INTR : intersection();
			            break;
			case UNON : form_union();
			            break;
			case ISGP : check_group();
			            break;
			case PSET : print_subset();
			            break;
			case COMP : complement();
			            break;
			case QUIT : exit(0);
			default   : printf ("\nUnrecognised Command - Type h for list of commands.\n");
		}
	}
}
