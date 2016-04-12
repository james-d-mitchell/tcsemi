OBJECTS = coinc.o enum.o error.o ideals.o main.o misc.o readpres.o table.o \
       timer.o words.o

INSTALLDIR = /usr/local/packages/semi

CFLAGS = -g

semi : $(OBJECTS)
	cc $(CFLAGS) $(OBJECTS) -o semi

debug:
	make semi CFLAGS=-g

install : 
	cp *.c ${INSTALLDIR} 
	cp *.h ${INSTALLDIR}
	cp Makefile ${INSTALLDIR}
	chmod 644 ${INSTALLDIR}/*.c ${INSTALLDIR}/*.h 
	(cd ${INSTALLDIR};make semi CFLAGS=-O4;rm -f *.o)
	chmod 755 ${INSTALLDIR}/semi

coinc.o : coinc.c
	cc -c $(CFLAGS) coinc.c

enum.o : enum.c
	cc -c $(CFLAGS) enum.c

error.o : error.c
	cc -c $(CFLAGS) error.c

ideals.o : ideals.c
	cc -c $(CFLAGS) ideals.c

main.o : main.c
	cc -c $(CFLAGS) main.c

misc.o : misc.c
	cc -c $(CFLAGS) misc.c

readpres.o : readpres.c
	cc -c $(CFLAGS) readpres.c

table.o : table.c
	cc -c $(CFLAGS) table.c

timer.o : timer.c
	cc -c $(CFLAGS) timer.c

words.o : words.c
	cc -c $(CFLAGS) words.c
