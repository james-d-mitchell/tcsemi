#include "global.h"
#include "timer.h"
#include "coinc.h"
#include <limits.h>

static int next(curr)
int curr;
/* Returns the next definition to be pushed through the relations, stepping
   over any deleted entries */
{	curr++;
	while (*(table+INDEX(curr,1))<0) curr++;
	return curr;
}

static void print_results()
{	printf("\nOrder of semigroup is %d.\nDefined : %d\n",last,defs);
}

static int nextdef()
/* Returns a new definition, initialising a row in the table for it.
   If there is no space for another definition returns -1 */
{	register int i;
	++last;
	++defs;
	if (LIST_START(last)>=tablesize) {--last;--defs;return -1;}
	*(table+LIST_START(last))=-1;
	for (i=1;i<=numgens;i++) put(last,i,0);
	return last;
}

static void initialize()
/* Initialise variables for new enumeration. */
{	register int i;
	last=0; 
	defs=0;
	killed=0;
	q_empty=1;
	rows_complete=0;
	cstackptr=0;
	cstackqptr=0;
	deleted=0;
	lpp=LINES_PER_PAGE;
	stack_depth=stack_max=0;
}

static int process_relation(def,n,la)
int def,n,la;
{	register int i=def,t,test,j=def;
	int *ptrl,*ptrr;
	int lh,rh;
	if (get(def,1)<0) return 0;
	ptrl=(*(rels+n)).lhs;   /* Work along the LHS */
	while (*(ptrl+1)!=0)
	{	t=get(i,*ptrl);
		if (t==0&&la) return 0;
		if (t==0)
		{	t=nextdef();
			if (t==-1) return la_pack(def);
			test=put(i,*ptrl,t);
			if (test!=0) return la_pack(def);
		}
		i=t;
		ptrl++;
	}
	ptrr=(*(rels+n)).rhs;	/* Work along the RHS */
	while (*(ptrr+1)!=0)
	{	t=get(j,*ptrr);
		if (t==0&&la) return 0;
		if (t==0)
		{	t=nextdef();
			if (t==-1) return la_pack(def);
			test=put(j,*ptrr,t);
			if (test!=0) return la_pack(def);
		}
		j=t;
		ptrr++;
	}
	lh=get(i,*ptrl);	/* Compare the values at the end of LHS,RHS */
	rh=get(j,*ptrr);	/* and take appropriate action */
	if (lh==rh&&rh!=0) return 0;
	if (!la&&lh==0&&rh==0)
	{	t=nextdef();
		if (t==-1) return la_pack(def);
		test=put(i,*ptrl,t);
		if (test!=0) return la_pack(def);
		return put(j,*ptrr,t)==0?0:la_pack(def);
	}
	if (lh==0&&rh!=0) return put(i,*ptrl,rh)==0?0:la_pack(def);
	if (lh!=0&&rh==0) return put(j,*ptrr,lh)==0?0:la_pack(def);
	(*cprocessor)(lh,rh);
	return 0;
}

void lookahead()
{	register int i,j,discard;
	printf("Entering lookahead phase.\n");
	for (i=rows_complete;i<=last;i++)
	for (j=0;j<numrels;j++)
		if (get(i,1)>0) discard=process_relation(i,j,1);
	printf("Lookahead done.\n");
}

int newpresentation(infile)
FILE *infile;
{	register int i,j;
	register int curr=0,t;
	struct time start_time,end_time;
	int usert,syst;
	int la_ptr=0,la_next=la_pos[0];
	get_timer(&start_time);
	if (table==NULL) message("Must allocate table before enumerating.\n");
	initialize();
	readin(infile);
#ifdef VADVISE
	vadvise(VA_ANOM);
#endif
	enumerating=1;
	valid_table=0;
	packing=0;
#ifdef UNIX
	signal(SIGALRM,status);
	signal(SIGINT,abrt);
	alarm(STATUS_INT);
#endif
	for (i=1;i<=numgens;i++)
	{	*(table+LIST_START(i))=-1;
		for (j=1;j<=numgens;j++) put(i,j,0);
		put(0,i,i);
	}
	defs=numgens+1; /* Count definition 0 - it takes up space even though
	                   it's not an element of the semigroup! */
	last=numgens;
	while (curr<=last)
	{	for (i=0;i<numrels;i++)
		{	t=process_relation(curr,i,0);
			if (t==-2) {curr=0;break;}
			if (t==-1) 
				{	enumerating=0;
					printf("Enumeration Failed : Insufficient Space.\n");
#ifdef VADVISE
					vadvise(VA_NORM);
#endif
					return;
				}
			if (t!=0) {curr=t-1;i=0;break;}
		}
		rows_complete=curr;
		if (last>=la_next)
		{	lookahead();
			la_next=la_ptr==LAMAX?INT_MAX:la_pos[la_ptr++];
		}
		curr=next(curr);
	}
	enumerating=0;
	{int discard=la_pack(1);}
	print_results();
	for (i=1;i<=last;i++) *(table+IN_CURID(i))=0;
	valid_table=1;
	get_timer(&end_time);
	usert=end_time.user-start_time.user;
	syst=end_time.system-start_time.system;
	printf("Total enumeration time : %d user, %d system, %d total.\n",usert,syst,usert+syst);
#ifdef VADVISE
	vadvise(VA_NORM);
#endif
	return;
}

void e_from_file()
{	char s[40];
	FILE *f;
	printf("File: ");
	gets(s);
	f=fopen(s,"r");
	if (f==NULL) {printf("Can't open %s.\n",s);return;}
	newpresentation(f);
	fclose(f);
	return;
}

