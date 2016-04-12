#include "global.h"
#include "words.h"

void print_subset()
{       register int i;
	int lines=0,online=0,count=0;
	for (i=1;i<=last;i++) if (*(table+IN_CURID(i))) count++;
	printf("Current subset contains %d elements.\n",count);
	for (i=1;i<=last;i++)
	if (*(table+IN_CURID(i)))
	{       if (lines%lpp==0) {WAITKEYB;lines++;}
		if (long_form) {printf("[%d]\t%s\n",i,trans_word(i));lines++;}
		else {printf("[%d]",i);online++;if (online%10==0) {printf("\n");lines++;}}
	}
	printf("\n");
}

static int check_ideal()
{       register int i;
	register int p,j;
	for (i=1;i<=last;i++)
	if (*(table+IN_CURID(i)))
	for (j=1;j<=numgens;j++)
	{       p=prod(i,j);
		if (!*(table+IN_CURID(p))) return 0;
	}
	return 1;
}

static int check_inverses(idem)
int idem;
{       register int i,j;
	int found_inv;
	for (i=1;i<=last;i++)
	if (*(table+IN_CURID(i)))
	{       found_inv=0;
		{       for (j=1;j<=last;j++)
			if (*(table+IN_CURID(j))&&prod(i,j)==idem) {found_inv=1;break;}
			if (!found_inv) return 0;
		}
	}
	return 1;
}

void store_subset()
{       int subset_no;
	register i;
	printf("Store as subset number? ");
	scanf("%d",&subset_no);
	while (getchar()!=CR);
	if (subset_no>31||subset_no<0) {printf("Invalid subset number.\n");return;}
	for (i=1;i<=last;i++)
	  *(table+SUBSET_STORE(i))=clearbit(*(table+SUBSET_STORE(i)),subset_no);
	for (i=1;i<=last;i++)
	  if (*(table+IN_CURID(i)))
	    *(table+SUBSET_STORE(i))=setbit(*(table+SUBSET_STORE(i)),subset_no);
}

void restore_subset()
{       int subset_no;
	register i;
	printf("Restore which subset? ");
	scanf("%d",&subset_no);
	while (getchar()!=CR);
	if (subset_no>31||subset_no<0) {printf("Invalid subset number.\n");return;}
	for (i=1;i<=last;i++)
	  *(table+IN_CURID(i))=0;
	for (i=1;i<=last;i++)
	  if (testbit(*(table+SUBSET_STORE(i)),subset_no))
	    *(table+IN_CURID(i))=1;
}

void intersection()
{       int subset_no;
	register i;
	printf("Intersect current subset with which subset? ");
	scanf("%d",&subset_no);
	while (getchar()!=CR);
	if (subset_no>31||subset_no<0) {printf("Invalid subset number.\n");return;}
	for (i=1;i<=last;i++)
	   if (testbit(*(table+SUBSET_STORE(i)),subset_no)&&*(table+IN_CURID(i))) *(table+IN_CURID(i))=1;else *(table+IN_CURID(i))=0;
}

void form_union()
{       int subset_no;
	register i;
	printf("Form union of current subset with which subset? ");
	scanf("%d",&subset_no);
	while (getchar()!=CR);
	if (subset_no>31||subset_no<0) {printf("Invalid subset number.\n");return;}
	for (i=1;i<=last;i++)
	   if (testbit(*(table+SUBSET_STORE(i)),subset_no)||*(table+IN_CURID(i))) *(table+IN_CURID(i))=1;else *(table+IN_CURID(i))=0;
}

void check_group()
{       register idemp,idemp_found=0;
	register i,j;
	char isclosed;
	printf("Is this subset known to be closed? ");
	scanf("%c",&isclosed);
	while (getchar()!=CR);
	for (i=1;i<=last;i++)
	if (*(table+IN_CURID(i))&&IS_IDEMP(i))
	{       idemp=i;
		if (idemp_found) {printf("Current subset is NOT a group.\n");return;}
		else idemp_found=1;
	}
	for (i=1;i<=last;i++)
	if (*(table+IN_CURID(i)))
	   if (prod(i,idemp)!=i||prod(idemp,i)!=i)
	      {printf("Current subset is NOT a group.\n");return;}
	if (isclosed=='Y'||isclosed=='y')
		{printf("Current subset IS a group.\n");return;}
	for (i=1;i<=last;i++)
	for (j=1;j<=last;j++)
	if (IN_CURID(i)&&IN_CURID(j)&&!IN_CURID(prod(i,j)))
		{printf("Current subset is NOT a group.\n");return;}
	printf("Current subset IS a group.\n");
	return;
}

void complement()
{       register i;
	for (i=1;i<=last;i++) *(table+IN_CURID(i))=!*(table+IN_CURID(i));
}

void kernels(elprt)
/* This option has been withdrawn because:
/*  a) It was not being used; the semigroups it helps with are simpler
/*     than those now being studied and
/*  b) The function of this command can now be achieved with the
/*     id and gp commands.
*/
int elprt;
{       printf("Option no longer supported. Use the id and gp commands instead.\n");
}
/* {    register int i,j;
/*      if (!valid_table) {printf("No enumeration has been done!\n");return;}
/*      if (numgens==1) {printf("Can't evaluate ideals of monogenic semigroup.\n");return;}
/*      for (i=1;i<=last;i++)
/*      if (IS_IDEMP(i))
/*      {       for (j=1;j<=last;j++)
/*                      *(table+IN_CURID(j))=0;
/*              for (j=1;j<=last;j++)
/*                      *(table+IN_CURID(prod(j,i)))=1;
/*              if (elprt)
/*              {       printf("\nLeft ideal I for idempotent %d is:\n",i);
/*                      print_subset();
/*              }
/*              else printf("Calculating ideal S%s for element %d:\n",trans_word(i),i);
/*              if (check_ideal())
/*              {       printf("I is an ideal.  Checking for inverses...\n");
/*                      if (check_inverses(i)) printf("I is a group kernel.\n");
/*                      else printf("I is NOT a group kernel.\n");
/*              }
/*              else printf("I is NOT an ideal.\n");
/*      
/*      }
/* }
*/

void ideal(elt,side)
int elt;
int side;
/*Calculates the left/right ideal for element elt.  side<0 means right,
  side>0 means left, side==0 means two-sided. */
{       register int i,j;
	char *s;
	if (!valid_table) {printf("No enumeration has been done!\n");return;}
	if (numgens==1) {printf("Can't calculate ideals in monogenic semigroup.\n");return;}
	s=(char *) malloc(8);
	if (s==NULL) {printf("Can't malloc workspace.\n");return;}
	for (i=1;i<=last;i++) *(table+IN_CURID(i))=0;
	if (side!=0)
	for (i=1;i<=last;i++) {register int p=side<0?prod(elt,i):prod(i,elt);*(table+IN_CURID(p))=1;}
	else
	for (i=1;i<=last;i++)
	for (j=1;j<=last;j++)
		*(table+IN_CURID(prod(j,prod(elt,i))))=1;
	side<0?strcpy(s,"Right I"):strcpy(s,"Left I");
	if (side==0) s="I";
	printf("%sdeal for element %d=%s is :\n",s,elt,trans_word(elt));
	print_subset();
}

void calcideal()
{       char *s;
	register int elt;
	int side;
	printf("Left, right or two-sided? (l/r/t) ");
	s=getword(0);
	side=99;
	if (strcmp(s,"L")==0||strcmp(s,"l")==0) side=1;
	if (strcmp(s,"R")==0||strcmp(s,"r")==0) side=-1;
	if (strcmp(s,"T")==0||strcmp(s,"t")==0) side=0;
	if (side==99) {printf("Invalid response - returning to main prompt\n");return;}
	printf("Enter element : ");
	elt=calcwords(1,getword(0));
	ideal(elt,side);
}

