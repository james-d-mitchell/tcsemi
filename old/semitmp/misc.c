#include "global.h"
#include "words.h"

getelements()
/* Calculates and prints each element as a word in the generators
   (the 'list' command ) */
{	register int i;
	char *s=(char *) malloc(MAXELTLEN);
	if (!valid_table) {printf("\nNo enumeration has been done!\n");return;}
	sprintf(s,"[%%%dd] %%s\n",numdigits(last));
	for (i=1;i<=last;i++)
	{	if (i%lpp==0) WAITKEYB; 
		printf(s,i,trans_word(i));
	}
}

solve_equation()
{	char side;
	char *s=(char *) malloc(MAXELTLEN);
	char *t=(char *) malloc(MAXELTLEN);
	register int se,te,i;
	int fd=0;
	if (s==NULL||t==NULL) {printf("No workspace!\n");return;}
	printf("Solve ax=b (l) or xa=b (r)? ");
	scanf("%c",&side);
	printf("Enter a : ");scanf("%s",s);
	printf("Enter b : ");scanf("%s",t);
	{char discard=0;while (discard!=CR) scanf("%c",&discard);}
	side=tolower(side);
	if (side!='l'&&side!='r') {printf("Invalid response.\n");return;}
	se=calcwords(1,s);te=calcwords(1,t);
	if (side=='l')
	{	printf("Solving %s*x=%s\n",s,t);
		for (i=1;i<=last;i++)
		if (prod(se,i)==te) {fd=1;printf("%s\n",trans_word(i));}
	}
	else
	{	printf("Solving x*%s=%s\n",s,t);
		for (i=1;i<=last;i++)
		if (prod(i,se)==te) {fd=1;printf("%s\n",trans_word(i));}
	}
	if (fd==0) printf("No solutions found.\n");
	return;
}

getidempotents()
{	register int i;
	register int elt;
	if (!valid_table) {printf("No enumeration has been done!\n");return;}
	for (i=1;i<=last;i++)
	{	if (IS_IDEMP(i))
			printf("Element %d = %s is idempotent.\n",i,trans_word(i));
	}
}

print_table()
{	register int i,j;
	if (!valid_table) {printf("No enumeration has been done!\n");return;}
	printf("Def.\t");
	for (i=1;i<=numgens;i++) printf("%c\t",backtrans(i));
	printf("\n");
	for (i=0;i<=last;i++)
	{	printf("%d\t",i);
		for (j=1;j<=numgens;j++) printf("%d\t",get(i,j));
		printf("%-16s",trans_word(i));
		for (j=1;j<=numgens;j++) printf("%-16s",trans_word(get(i,j)));
		printf("\n");
		if ((i+1)%lpp==0) WAITKEYB;
	}
}

transformations()
{	char *s;
	int i,j,elt;
	if (!valid_table) {printf("No enumeration has been done!\n");return;}
	if (numgens==1) {printf("Command not available for monogenic semigroups.\n");return;}
	s=getword(0);
	elt=calcwords(1,s);
	for (i=0;i<=last/10;i++)
	{	for (j=0;j<10;j++)
		if (10*i+j<=last)
		printf("%d\t",10*i+j);
		printf("\n");
		for (j=0;j<10;j++)
		if (10*i+j<=last)
		printf("%d\t",prod(10*i+j,elt));
		printf("\n\n");
	}
}

