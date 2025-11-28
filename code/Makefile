CC = gcc
CFLAGS = -Wall -g
BISON = bison
FLEX = flex

SRCS = main.c ast.c ast_debug.c symbolTable.c codeGenerator.c
PARSER_SRC = parser.y
LEXER_SRC = lexer.x

EXEC = printAST  

all: $(EXEC)


parser.tab.c parser.tab.h: $(PARSER_SRC)
	$(BISON) -d $<

lex.yy.c: $(LEXER_SRC) parser.tab.h
	$(FLEX) $<


$(EXEC): parser.tab.c lex.yy.c $(SRCS)
	$(CC) $(CFLAGS) -o $@ parser.tab.c lex.yy.c $(SRCS) -lm


