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
ast.o: ast.c ast.h

###

test0:; ./main < test0.c
test1:; ./main < test1.c
test2:; ./main < test2.c
test31:; ./main < test3_1.c
test32:; ./main < test3_2.c
test4:; ./main < test4.c
test5:; ./main < test5.c
test6:; ./main < test6.c

clean:; rm -f *.o *~ parse.tab* lex.yy.c main
