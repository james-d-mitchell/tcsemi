#include "global.h"

char *expand(s)
char *s;
/* Expand a word in the generators, possibly containing parenthesis and powers,
   to a string of generators (e.g. a2(bc)3 would become aabcbcbc. */
{	char *ans=(char *) calloc(MAXRELSIZE,sizeof(char));
	char *t=s,*temp,*temp1,*ans1=ans,*c;
	char ch;
	int count,n,i;
	if (ans==NULL) message("Can't malloc workspace.\n");
	while (*t!='('&&*t!='\0')
	{	if (!isdigit(*t)) {*ans=*t;ans++;t++;continue;};
		sscanf(t,"%d",&n);
		while (isdigit(*t)) t++;
		ch=*(ans-1);
		for (i=2;i<=n;i++) {*ans=ch;ans++;};
	}
	if (*t=='\0')
		if (!strcmp(s,ans1)) return ans1;
		else
		{	temp=expand(ans1);
			free(ans1);
			return temp;
		}
	count=1;t++;temp=t;
	while (count!=0)
	{	if (*t=='(') count++;
		if (*t==')') count--;
		t++;
	}
	if ((i=sscanf(t,"%d",&n))==0||i==EOF) n=1;
	count=1; /* The closing bracket */
	while (isdigit(*t)) t++,count++;
	temp1=temp;
	for (i=1;i<=n;i++)
	{	temp=temp1;
		for (c=temp;c<t-count;c++)
		{	*ans=*temp;ans++;temp++;}
	}
	while (*t!='\0')
	{	*ans=*t;ans++;t++;}
	*ans='\0';
	if (*t=='\0')
		if (!strcmp(s,ans1)) return ans1;
		else
		{	temp=expand(ans1);
			free(ans1);
			return temp;
		}
}

int trans(ch)
char ch;
/* Given a generator, return its generator number, allocating a
   generator number if necessary. */
{	if (translate[ch]!=0) return translate[ch];
	return translate[ch]=++numgens;
}

char backtrans(i)
int i;
/* Return the generator associated with generator i, or '\0' if none */
{	int j;
	for (j=1;j<=NUMCHARS;j++)
	if (translate[j]==i) return j;
	return '\0';
}	

void genreplace(x,y)
int x,y;
/* Replace generator x with generator y */
{	register i;
	for (i=1;i<=NUMCHARS;i++)
	if (translate[i]==x) translate[i]=y;
}

int numdigits(n)
int n;
{	register int c=1;
	while ((n/=10)!=0) c++;
	return c;
}

static void moveup(s)
char *s;
/* Make space for another character at the beginning of string s */
{	register int i=0;
	while (*(s+i)!='\0') i++;
	while (i>=0)
	{*(s+i+1)=*(s+i);i--;}
}

static char *tidy(s)
char *s;
/* Takes an element string and replaces successive occurrences of elements
   by powers of that element, e.g. aaabb would become a^3b^2 */
{	char *t= (char *) calloc(MAXELTLEN,sizeof(char));
	char *tc=t;
	char ch=*s;
	int count=0;
	int i;
	if (t==NULL) {printf("\nNo space to tidy element. Untidied form : ");return s;}
	while (*s!='\0')
	{	if (*s==ch) {count++,s++;continue;}
		*tc=ch;tc++;
		if (count==1) {ch=*(s++);continue;}
		*tc='^';tc++;
		sprintf(tc,"%d",count);
		tc+=numdigits(count);
		count=1;
		ch=*(s++);
	}
	if (count==1) {*tc=ch;return t;}
	*tc=ch;*(tc+1)='^';sprintf(tc+2,"%d",count);
	return t;
}

char *trans_word(elt)
/* Returns element elt as a word in the generators */
int elt;
{	char *res=(char *)calloc(MAXELTLEN,sizeof(char));
	int ind;
	if (res==NULL) message("No space for calculations!\n");
	while (elt!=0)
	{	moveup(res);
		ind=*(table+LIST_START(elt));
		if (ind==-1) 
		{	printf ("trans_word : Can't find element %d !\n",elt);
			message("\nInternal error in trans_word.\n");
		}
		elt=ind/(2*numgens+1);
		*res=backtrans(ind%(2*numgens+1));
	}
	return tidy(res);
}

char *getword(prompt)
int prompt; /*If non-zero, issue the PROMPT prompt */
/* returns the next 'word' from the standard input */
{	char *s=(char *) malloc(WORDSIZE);
	char *s1=s;
	char t;
	if (s==NULL) message("No space to allocate command buffer.\n");
	if (prompt) printf(PROMPT);
	t=getchar();
	while (t!=SPACE && t!=TAB && t!=CR) {*s1=t;s1++;t=getchar();}
	*s1='\0';
	return s;
}

static char *el_expand(s)
char *s;
/* This is used to convert a word typed by the user containing the '$' escape
   to a normal word in the generators. */
{	char *t= (char *) calloc(WORDSIZE,sizeof(char));
	char *t1=t,*exp;
	int test,elt;
	if (t==NULL) message("Couldn't calloc workspace.\n");
	while (*s!='\0')
	{	while (*s!='\0'&&*s!='$')
		*(t1++)=*(s++);
		if (*s=='\0') break;
		s++;
		test=sscanf(s,"%d",&elt);
		if (test==0) {printf("\nBad word format.\n");return " ";}
		exp=trans_word(elt);
		while (*exp!='\0')
		(*exp)=='^'?(void)(exp++):(void)(*(t1++)=*(exp++));
		while (isdigit(*s)) s++;
	}
	*t1='\0';
	return t;
}

int calcwords(flag,word)
int flag;
char *word;
{	int elt=0;
	if (!valid_table) {printf("\nNo enumeration has been done!\n");return 0;}
	if (flag!=1) printf("Calculating word %s\n",word);
	word=el_expand(word);
	word=expand(word);
	while (*word!='\0')
	{elt=get(elt,(int) translate[*word]);word++;if (flag==2) printf("=%s * %s\n",trans_word(elt),word);}
	if (flag==1) return elt;
	printf("\nWord evaluates to element %d = %s\n",elt,trans_word(elt));
}

