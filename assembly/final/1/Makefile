TARGET = main

all:
	flex -d $(TARGET).l
	bison -d $(TARGET).y
	gcc -o $(TARGET) $(TARGET).tab.c lex.yy.c -DYYERROR_VERBOSE -lfl -ly

test:
	./$(TARGET) < test.c

clean:
	rm -f $(TARGET) $(TARGET).tab* lex.yy.c