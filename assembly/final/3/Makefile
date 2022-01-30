CC = gcc -g3
CFLAGS = -O -Wall
LEX = flex -d
YACC = bison -d
HDRS = parse.tab.h
LDFLAGS = -DYYERROR_VERBOSE -lfl -ly
LIBS =
OBJS = parse.tab.o lex.yy.o ast.o codegen.o register.o generate.o list.o
PROGRAM = main

all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(HDRS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

lex.yy.c: lex.l
	$(LEX) lex.l
parse.tab.c: parse.y
	$(YACC) parse.y
ast.o: ast.c ast.h
codegen.o: codegen.c
register.o: register.c register.h
generate.o: generate.c generate.h
list.o: list.c list.h

###

test1:; ./main < test1.c > test1.s 2> stderr.log; maps -e test1.s > output.log
test2:; ./main < test2.c > test2.s 2> stderr.log; maps -e test2.s > output.log
test31:; ./main < test3_1.c > test3_1.s 2> stderr.log; maps -e test3_1.s > output.log
test32:; ./main < test3_2.c > test3_2.s 2> stderr.log; maps -e test3_2.s > output.log
test4:; ./main < test4.c > test4.s 2> stderr.log; maps -e test4.s > output.log
test5:; ./main < test5.c > test5.s 2> stderr.log; maps -e test5.s > output.log

clean:; rm -f *.o *~ parse.tab* lex.yy.c main *.s *.s.mem stderr.log output.log
