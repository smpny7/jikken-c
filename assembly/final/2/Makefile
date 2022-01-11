CC = gcc
CFLAGS = -O -Wall
LEX = flex
YACC = bison -d
HDRS = parse.tab.h
LDFLAGS = -lfl -ly
LIBS =
OBJS = parse.tab.o lex.yy.o ast.o
PROGRAM = main

all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(HDRS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

lex.yy.c: lex.l
	$(LEX) lex.l
parse.tab.c: parse.y
	$(YACC) parse.y

clean:; rm -f *.o *~ parse.tab*

###
ast.o: ast.c ast.h
