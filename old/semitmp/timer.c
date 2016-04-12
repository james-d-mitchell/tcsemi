#include "global.h"

#ifdef UNIX
void get_timer(t)
struct time *t;
{	struct rusage temp;
	getrusage(RUSAGE_SELF,&temp);
	t->user=temp.ru_utime.tv_sec;
	t->system=temp.ru_stime.tv_sec;
	if (temp.ru_utime.tv_usec>=500) t->user++;
	if (temp.ru_stime.tv_usec>=500) t->system++;
}
#else
void get_timer(t)
struct time *t;
{	t->user=clock()/CLK_TCK;
	t->system=0;}
#endif

void status(i)
int i;
{	if (!enumerating) return;
	if (!packing) printf("Defined : %d Alive : %d SD : %d SM : %d\n",defs,defs-deleted-killed,stack_depth,stack_max);
	fflush(stdout);
#ifdef UNIX
	signal(SIGALRM, status);
	alarm(STATUS_INT);
#endif
}

