#include "global.h"

addlist(x,ind)
int x,ind;
{	/* Add the fact that x has been placed at point ind in table. */
	int t,t1;
	t=t1=*(table+LIST_START(x));
	if (t==-1)
	{	*(table+LIST_START(x))=ind;
		*(table+ind+numgens)=-1;
		return;
	}
/*	while (t!=-1)
	{	t=*(table+t+numgens);
		if (t!=-1) t1=t;
	}
	*(table+t1+numgens)=ind;
	*(table+ind+numgens)=-1; */

	*(table+LIST_START(x)) = ind;
	*(table+ind+numgens) = t;
}

#ifdef FUNC_PUTGET
	int put(x,y,n)
	int x,y,n;
/* Insert the definition x*y=n in the table.  Returns 0 on success,
   1 if there is no space available */
	{	if (INDEX(x,y)>=tablesize) return 1;
		*(table+INDEX(x,y))=n;
		if (n<=0) return 0;	/* n<0 means that the element is being marked as
				   deleted. n=0 means that the element is being
				   marked as undefined.  In either case, we do
				   not want the entry added to the lists. */
		addlist(n,INDEX(x,y));
		return 0;
	}
	
	int get(x,y)
	int x,y;
	{	if (INDEX(x,y)>=tablesize) return 0;
		return (*(table+INDEX(x,y)));
	}
#endif

extern void *lookahead();

int la_pack(n)
/* Packs the table, returning the new position of n */
int n;
{	int register i,j,ptr;
	int move=0,newpos;
	struct time start_time,end_time;
	int usert,syst;
	lookahead();
	get_timer(&start_time);
#ifdef VADVISE
	vadvise(VA_SEQL);
#endif
	printf("Packing table...\n");
	packing=1;
	for (i=1;i<=last;i++)	/* First change the table entries */
	{	if (i==rows_complete) rows_complete-=move;
		if (get(i,1)<0) 
		{	move++;
			if (i==n) newpos= -2;
			continue;
		}
		ptr=*(table+LIST_START(i));
		while (ptr!=-1)
		{	if (*(table+ptr)>0) *(table+ptr)=i-move;
			ptr=*(table+ptr+numgens);
		}
		if (i==n) newpos=i-move;
	}
	for (i=1;i<=last-move;i++) *(table+LIST_START(i))=-1;
				/* Empty all the lists */
	move=0;
	for (i=0;i<=last;i++)
		/* Now do the move. Use put so that the lists are rebuilt.
		   Start from i=0 so that the lists contain entries in row 0! */
	{	if (get(i,1)<0) {move++;continue;}
		for (j=1;j<=numgens;j++)
		put(i-move,j,get(i,j));
	}
	last-=move;deleted=0;killed+=move;
	printf("Packing done : %d definitions recovered\n",move);
	packing=0;
	get_timer(&end_time);
	usert=end_time.user-start_time.user;
	syst=end_time.system-start_time.system;
	printf("Packing time : %d user, %d system, %d total.\n",usert,syst,usert+syst);
#ifdef VADVISE
	vadvise(VA_ANOM);
#endif
	return move==0?-1:newpos;
}

init_table(size)
int size;
{	char c;
	if (valid_table)
	{	printf("Warning - changing size will destroy current table : Continue? (Y/N) ");
		scanf("%c",&c);while (getchar()!=CR);
		if (c!='Y'&&c!='y') return;
	}
	valid_table=0;
	if (table!=NULL) table=(int *) realloc(table,sizeof(int)*(size+1));
		else table=(int *) malloc(sizeof(int)*(size+1));
	tablesize=size;
	if (table==NULL) message("\n No space to initialize table.\n");
	else printf("Table size set to %d.\n",size);
}

setsize()
{	int sz;
	printf("size: ");
	scanf("%d",&sz);while (getchar()!=CR);
	if (sz<=10) {printf("Too small.\n");return;}
	init_table(sz);
}

replace(x,y)
int x,y;
/* Replace all occurrences of x in the table by y. Add the new positions
   of x to the list for x */
{	int ptr=*(table+LIST_START(x));
	int ptr1;
	if (x<=numgens) genreplace(x,y); /* Word translation would otherwise
	                                    go wrong when a generator is
	                                    deleted.  Note that when this
	                                    happens, the deleted generator
	                                    must be set equal to some other
	                                    generator */
	while (ptr!=-1)
	{	ptr1=ptr;
		ptr=*(table+ptr+numgens);
		if (*(table+ptr1)>0) {*(table+ptr1)=y;addlist(y,ptr1);}
	}
	*(table+LIST_START(x))=-1;
}

int prod(x,y)
int x,y;
{	register int ind;
	static int temp[MAXELTLEN];
	register int tempptr=MAXELTLEN-1;
	while (y!=0)
	{	ind=*(table+LIST_START(y));
		if (ind==-1) message ("\nError in prod.\n");
		y=ind/(2*numgens+1);
		temp[tempptr--]=ind%(2*numgens+1);
	}
	tempptr++;
	while (tempptr<MAXELTLEN) x=get(x,temp[tempptr]),tempptr++;
	return x;
}

