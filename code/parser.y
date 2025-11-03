%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int yylex (void);
void yyerror (char const *);
%}
%define api.value.type{float}

/* Tokens */
-- Expressions
%%token PLUS MINUS -- + -
%%token MULT DIV -- * /
%%token LPAREN RPAREN -- ()
%%token EQ INEQ -- = /=
%%token OR -- or
%%token AND  -- and
%%token XOR -- xor
%%token LESS GREATER -- < >
%%token LEQ GEQ -- <= >=
%%token MOD REM -- mod rem
%%token ASSIGN -- :=
-- Literals
%%token ID
%%token TRUE FALSE -- true false
%%token STRING_LITERAL
%%token NUM
-- IO
%%token PUT GET -- put_line get_line ?
-- Control Flow
%%token IF THEN ELSE -- if A then B else C
%%token WHILE LOOP END -- while A loop end
-- Basic Types
%%token INTEGER
%%token BOOLEAN
%%token STRING
-- Precendence
%%left PLUS MINUS
%%left MULT DIV MOD REM

/* Regras de produção */

%%

/* Código C adicional (erro) */
void yyerror(char const *msg) {
   printf("parse error: %s\n", msg);
   exit(-1);
}

