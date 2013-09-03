COURSE = cs551f13

PROJECT = prj1

TARGET = matrix-ops

OBJS = \
  main.o \
  matrix.o \
  stack.o

SRC_FILES = \
  main.c \
  matrix.c \
  matrix.h \
  stack.c \
  stack.h \
  Makefile \
  README

CC = gcc
CFLAGS = -g -Wall -std=c99
CPPFLAGS=
LDFLAGS=


#builds main target for project
$(TARGET): 	$(OBJS)
		$(CC) $(LDFLAGS) $(OBJS) -o $@

#builds test program for stack ADT
test-stack:	stack.c stack.h
		$(CC) -DTEST_STACK $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $< -o $@

#Removes all objects and executables.
clean:
		rm -f $(OBJS) $(TARGET) test-stack $(PROJECT).tar.gz *~

#This target makes a archive containing all the project source files,
#suitable for submission.
submit:		$(SRC_FILES)
		tar -cvf $(PROJECT).tar $(SRC_FILES)
		gzip $(PROJECT).tar

##This is a Perl program used by the DEPEND target below
PERL_FILTER_INCLUDE = '$$line .= $$_; \
		       if (!/\\$$/) { \
		         @a = split(/\s+/, $$line); \
		         @b = grep { $$_ !~ "$(COURSE)" } @a; \
		         print "@b\n"; \
		         $$line = ""; \
		       } \
		       else { \
		         $$line =~ s/\\$$//; \
		       }'

#This target outputs dependencies of all C files on stdout.  The output can
#then be cut-and-pasted into this Makefile.  The perl program removes
#dependencies on the course library files.
DEPEND:
		$(CC) $(CPPFLAGS) -MM *.c | \
		perl -ne $(PERL_FILTER_INCLUDE)

#Automatically produced dependencies produced by 'make DEPEND'.
main.o:   main.c matrix.h stack.h
matrix.o: matrix.c matrix.h
stack.o:  stack.c stack.h
