/* Remove the definition of UNIX if not on a unix system.
   If on a unix system that does not support the vadvise()
   call, also remove that definition. */

#define UNIX
#ifdef UNIX
#if 0
#define VADVISE	/* Remove or comment out unless your system supports the
		   vadvise() system call.  If this line is present, semi
		   will expect to call vadvise as follows:
			vadvise(VA_ANOM); to advise the system of anomalous
				          virtual memory use (enumerating)
			vadvise(VA_SEQL); to advise the system of sequential
					  virtual memory use (packing)
			vadvise(VA_NORM); to set virtual memory use to normal.*/
#endif
#endif
#ifndef CLK_TCK
#define CLK_TCK 1000000
	/* The number of ticks per second of the clock() function
	   This value should be pre-defined by an ANSI C compiler. */ 
#endif

#ifdef DEBUG
#define FUNC_PUTGET
#endif

#define LINES_PER_PAGE 20
#define MAXRELSIZE 2000		/* Maximum length of one side of one relation */
#define MAXTOTRELSIZE 5000	/* Maximum total length of the relations */
#define INITABLESIZE 5000000	/* Initial table size */
#define CSTACKLIMIT 50000	/* Maximum coincidence stack/queue size */
#define STATUS_INT 10		/* Interval between status reports */
#define NUMCHARS 128		/* Number of characters that can be used to
				   represent generators. */
#define MAXELTLEN 2000		/* Maximum size of an element as a word in the
				   generators */
#define WORDSIZE MAXELTLEN
#define LAMAX 100		/* The maximum number of points at which a 
				   lookahead may be specified */

/* The idea of the macro below is to wait during long output for
   the user to type at the keyboard.   As written, it will only do
   so if both the standard input and standard output are terminals,
   but on some non-unix systems the isatty() or fileno() calls
   may not be available & you'll have to substitute something
   else
*/
#define WAITKEYB \
{	char c; \
	if (isatty(fileno(stdin))&&isatty(fileno(stdout))) \
	{	printf("--RETURN TO CONTINUE, Q TO QUIT--"); \
		while ((c=getchar())!=CR) if (c=='Q'||c=='q') \
		{while (getchar()!=CR);return;} \
	}  \
}
