#include "global.h"
#include "words.h"

convert(s,arr)
char *s;
int **arr;
/* Convert one side of a relation s to an array of generator numbers. */
{	int i;
	*arr=(int *) calloc((strlen(s)+1),sizeof(int));
	if (*arr==NULL) message("\nNo space to store relations.\n");
	for (i=0;i<strlen(s);i++)
	*((*arr)+i)=trans(*(s+i));
	*((*arr)+strlen(s))=0;
}

char *next_rel(s)
char **s;
/* Returns the next side of a generator from *s; *s is moved to point
   to the remaining string. */
{	char *ans=(char *) calloc(MAXRELSIZE,sizeof(char));
	char *ans1=ans;
	(*s)++; /* s should begin with / at this point - remove it. */
	if (**s=='\0' || **s=='=' || **s=='/') message("\nInvalid Presenation.\n");
	while (**s!='\0'&&**s!='/'&&**s!='=') {*ans=**s;ans++;(*s)++;};
	return ans1;
}

int count_rels(s)
char *s;
/* Returns the number of relations in string s */
{	int ans=0;
	while (*s!='\0')
	{	if (*s=='/') ans++;
		s++;
	}
	return ans;
}

int verify_syntax(s)
char *s;
{	register int i,openbra=0,openrel=0,length;
	length=strlen(s);
	if (length==0) return 0;
	if (*s!='/') return 0;
	for (i=0;i<length;i++,s++)
	{	switch (*s)
		{	case '(' : openbra++;
			           break;
			case ')' : if(--openbra<0) return 0;
			           break;
			case '/' : if (++openrel!=1) return 0;
			           break;
			case '=' : if (--openrel) return 0;
			           break;
		        default  : ;
		}
	}
	return openrel?0:1;
}

void readin(f)
FILE *f;
/* Read in the relations for an enumeration 
   Returns 0 if the previous presentation is re-used, 1 if
   a new presentation is read in */
{	char *s=(char *) malloc(MAXTOTRELSIZE);
	int d,i;
	struct relation *relptr;
	d=fscanf(f,"%s",s);
	if (f==stdin) while (getchar()!=CR);
	if (d==0) message("\nNo input!\n");
	if (!strcmp(s,"!")) {printf("Re-using last presentation.\n");return;}
	if (!verify_syntax(s)) message("Invalid presentation.\n");
	numgens=0;
	for (i=0;i<NUMCHARS;translate[i++]=0);
	numrels=count_rels(s);
	rels=(struct relation *) calloc(numrels,sizeof(struct relation));
	relptr=rels;
	if (rels==NULL) message("\n No space for relations!\n");
	for (i=1;i<=numrels;i++,relptr++)
	{	convert(expand(next_rel(&s)),&(relptr->lhs));
		convert(expand(next_rel(&s)),&(relptr->rhs));
	}
	return;
}
