#include "global.h"

message(s)
char *s;
{	printf("%s",s);
	longjmp(err_return,0);
}

abrt()
{	signal(SIGINT,abrt);
	enumerating=0;
	packing=0;
	longjmp(err_return,0);
}

