CC = gcc
CFLAGS = -Wall -g
BISON = bison
FLEX = flex

SRCS = main.c ast.c
PARSER_SRC = parser.y
LEXER_SRC = lexer.x

EXEC = ada  

all: $(EXEC)

scanner.c: scanner.flex parser.h
	flex -o scanner.c scanner.flex

parser.tabc parser.tab.h: $(PARSER_SRC)
	b$(BISON) -d $(PARSER_SRC)

lex.yy.c: $(LEXER_SRC) parser.tab.h
	$(FLEX) $(LEXER_SRC)

$(EXEC): parser.tab.c lex.yy.c $(SRCS)
	$(CC) $(CFLAGS) -o $(EXEC) parcer.tab.c lex.yy.c ast.c main.c -lfl -lm
clean:
	rm -f $(EXEC) parser.tab.c parser.tab.h lex.yy.c tests/actual/*.out

test: $(EXEC)
	./run_tests.sh

.PHONY: all test clean
