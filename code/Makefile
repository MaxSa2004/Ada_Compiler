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
	$(CC) $(CFLAGS) -o $(EXEC) parser.tab.c lex.yy.c ast.c main.c -lm
test: $(EXEC)
ifndef FILE
	@echo "Usage: make test FILE=path/to/input.ada [OUT=path/to/outfile]"
	@exit 1
endif
	@mkdir -p $(dir $(or $(OUT),test_outputs/placeholder))
	@echo "Running test: $(FILE)"
	@./$(EXEC) "$(FILE)" > "$(or $(OUT),test_outputs/$(notdir $(basename $(FILE))).out)" 2>&1
	@echo "Output written to: $(or $(OUT),test_outputs/$(notdir $(basename $(FILE))).out)"
clean:
	rm -f $(EXEC) parser.tab.c parser.tab.h lex.yy.c tests/actual/*.out



