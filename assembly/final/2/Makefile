CC = gcc
CFLAGS = -O -Wall
LEX = flex -d
YACC = bison -d
HDRS = parse.tab.h
LDFLAGS = -DYYERROR_VERBOSE -lfl -ly
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

clean:; rm -f *.o *~ parse.tab* lex.yy.c main

###
ast.o: ast.c ast.h
