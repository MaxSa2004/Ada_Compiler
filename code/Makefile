CC = gcc
CFLAGS = -Wall -g
BISON = bison
FLEX = flex

SRCS = main.c ast.c
PARSER_SRC = parser.y
LEXER_SRC = lexer.x

EXEC = ada  


.PHONY: all test clean

all: $(EXEC)

parser.tab.c parser.tab.h: $(PARSER_SRC)
	$(BISON) -d $(PARSER_SRC)

lex.yy.c: $(LEXER_SRC) parser.tab.h
	$(FLEX) $(LEXER_SRC)

$(EXEC): parser.tab.c lex.yy.c $(SRCS)
	$(CC) $(CFLAGS) -o $(EXEC) parser.tab.c lex.yy.c ast.c main.c -lfl -lm

clean:
	rm -f $(EXEC) parser.tab.c parser.tab.h lex.yy.c tests/actual/*.out



