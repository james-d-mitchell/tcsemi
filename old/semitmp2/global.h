#include "config.h"	
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <setjmp.h>
#ifdef UNIX
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#endif
#ifdef VADVISE
#include <sys/vadvise.h>
#endif

#include "version.h"	

#ifdef UNIX
#define SYSTEM "UNIX"
#else
#define SYSTEM " "
#endif

struct relation
{	int *lhs,*rhs;};

extern struct relation *rels;

struct time
{	int user,system;};

#define max(a,b) \
 (a)>(b)?a:b

#define min(a,b) \
 (a)<(b)?a:b

#define setbit(pattern,bit) \
((pattern)|(1<<(bit)))

#define clearbit(pattern,bit)  \
((pattern)&(~(1<<(bit))))

#define testbit(pattern,bit) \
((pattern)&(1<<(bit)))

#define SUBSET_STORE(x) \
(LIST_START(x)-1)

#define IS_IDEMP(x) \
(prod((x),(x))==(x))

#define IN_CURID(x) \
(LIST_START(x)-2)

#define PROMPT "semi>"

/* Commonly used character constants */
#define SPACE ' '
#define TAB '\t'
#define CR '\n'

#ifdef FUNC_PUTGET
int INDEX(x,y)
int x;
int y;
{	return ((numgens<<1)+1)*x+y;
}
#else
#define INDEX(x,y) \
(((numgens<<1)+1)*(x)+(y))
#endif

#ifdef FUNC_PUTGET
int LIST_START(x)
int x;
{	return ((numgens<<1)+1)*x+(numgens<<1)+1;
}
#else
#define LIST_START(x) \
(((numgens<<1)+1)*(x)+(numgens<<1)+1)
#endif

#ifndef FUNC_PUTGET
#define put(x,y,n)	\
(INDEX((x),(y))>=tablesize?1:(*(table+INDEX((x),(y)))=n),	\
	((n)<=0?0:(addlist((n),INDEX((x),(y)))),0))	
#endif

#ifndef FUNC_PUTGET
#define get(x,y)	\
/* Returns x*y, or 0 if x*y is undefined or outwith the table. */	\
(INDEX((x),(y))>=tablesize?0:(*(table+INDEX((x),(y)))))
#endif

extern int numgens,numrels;	/* Numbers of generators and relations */
extern int *table;		/* Definitions table */
extern int tablesize,last;	/* Table size; last element used in table */
extern int deleted;		/* Number of elements deleted as a result of
			   	   coincidences but not removed by packing */
extern int killed;		/* Number of elements deleted & removed
				   by packing */
extern int defs;		/* Total number of definitions made */
extern int rows_complete;	/* Rows up to this number are complete */
extern int cstackptr;		/* Pointer to coincidence stack/queue */
extern int cstackqptr;		/* If queueing, pointer to the end of queue */
extern int q_empty;		/* If queueing, indicates whether the
				   queue is empty */
extern int cstack[CSTACKLIMIT+1][2];	/* The coincidence stack */
extern int translate[NUMCHARS];	/* Table of generator names -> numbers */
extern int enumerating;	/* Flag indicating whether currently enumerating */
extern int valid_table;	/* Flag indicating whether table contains a semigroup */
extern int packing;	/* Flag indicating whether a pack is in progress */
extern int long_form;	/* Flag indicating form of output for ideals and
			   print_kernels commands */
extern int lpp;		/* Lines per page for output to terminals. */
extern int stack_depth,stack_max;
extern jmp_buf err_return;
extern int la_pos[LAMAX]; /* List of points at which to do a lookahead */
#define PTR_INC(p)	\
((p)>=CSTACKLIMIT?(p)=0:(p)++)
