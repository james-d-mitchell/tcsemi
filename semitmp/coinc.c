#include "global.h"
#include "coinc.h"

#define PTR_INC(p)	\
((p)>=CSTACKLIMIT?(p)=0:(p)++)

void select_processor()
{	char c;
	printf("Processing method? (q-queue, s-stack, r-recursive) ");
	scanf("%c",&c);
	while (getchar()!=CR);
	switch (c)
	{	case 'q' : cprocessor=qcoinc;
		           break;
		case 's' : cprocessor=scoinc;
		           break;
		case 'r' : cprocessor=rcoinc;
		           break;
		default  : printf("Invalid response - Coincidence processor unaltered.\n");
	}
}

static void stack_coinc(s,t)
int s,t;
/* Stack the coincidence s=t */
{	if (cstackptr==CSTACKLIMIT) message("\n Coincidence stack full. \n");
	cstack[++cstackptr][0]=s;
	cstack[cstackptr][1]=t;
}

static void q_coinc(s,t)
int s,t;
/* Queue the coincidence s=t */
{	if (cstackqptr==cstackptr&&!q_empty) message("\n Coincidence queue full. \n");
	cstack[cstackptr][0]=s;
	cstack[cstackptr][1]=t;
	PTR_INC(cstackptr);
	q_empty=0;
}

void rcoinc(s,t)
/* The recursive coincidence processor */
int s,t;
{	int u,v,i,temp;
	stack_depth++;
	stack_max=max(stack_depth,stack_max);
	if (t<s) {temp=s;s=t;t=temp;}
	if (s==t) return;
	replace(t,s);
	for (i=1;i<=numgens;i++)
	{	int s_copy=s,t_copy=t;
		if (get(s,1)<0) {while (get(s,1)<0) s=-get(s,1);i=1;}
		if (get(t,1)<0) while (get(t,1)<0) t=-get(t,1);
		if (s==t) {stack_depth--;return;}
		if (t<s) {temp=s;s=t;t=temp;}
		if (s_copy!=s||t_copy!=t) replace(t,s);
		u=get(s,i);v=get(t,i);
		if (u==s&&v==t||u==t&&v==s) continue;
		if (u==v) continue;
		if (u==0&&v!=0) {put(s,i,v);continue;}
		if (v==0) continue;
		rcoinc(u,v);
	}
	put(t,1,-s);
	deleted++;
	stack_depth--;
	return;
}

void qcoinc(s,t)
/* The queueing coincidence processor */
int s,t;
{	int u,v,i,temp;
	for(;;)
	{	stack_depth=cstackptr-cstackqptr;
		if (stack_depth<0) stack_depth+=CSTACKLIMIT;
		stack_max=max(stack_depth,stack_max);
		while (get(s,1)<0) s= -get(s,1);
		while (get(t,1)<0) t= -get(t,1);
		if (t<s) {temp=s;s=t;t=temp;}
		if (s!=t) 
		{	replace(t,s);
			for (i=1;i<=numgens;i++)
			{	u=get(s,i);v=get(t,i);
				if (u==s&&v==t||u==t&&v==s) continue;
					/* Don't allow a coincidence to re-generate itself! */
				if (u==v) continue;
				if (u==0&&v!=0) {put(s,i,v);continue;}
				if (v==0) continue;
				q_coinc(u,v);
			}
			put(t,1,-s);
			deleted++;
		}
		if (q_empty) break;		/* No more coincidences pending */
		s=cstack[cstackqptr][0];
		t=cstack[cstackqptr][1];
		PTR_INC(cstackqptr);
		q_empty=(cstackqptr==cstackptr);
	}
	return;
}

void scoinc(s,t)
/* The stacking coincidence processor */
int s,t;
{	register int u,v,i,temp;
	for(;;)
	{	stack_depth=cstackptr;
		stack_max=max(stack_depth,stack_max);
		while (get(s,1)<0) s= -get(s,1);
		while (get(t,1)<0) t= -get(t,1);
		if (t<s) {temp=s;s=t;t=temp;}
		if (s!=t) 
		{	replace(t,s);
			for (i=1;i<=numgens;i++)
			{	u=get(s,i);v=get(t,i);
				if (u==s&&v==t||u==t&&v==s) continue;
					/* Don't allow a coincidence to re-generate itself! */
				if (u==v) continue;
				if (u==0&&v!=0) {put(s,i,v);continue;}
				if (v==0) continue;
				stack_coinc(u,v);
			}
			put(t,1,-s);
		}	deleted++;
		if (cstackptr==0) break;		/* No more coincidences pending */
		s=cstack[cstackptr][0];
		t=cstack[cstackptr--][1];
	}
	return;
}
