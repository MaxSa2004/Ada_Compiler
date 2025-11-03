%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int yylex (void);
void yyerror (char const *);
%}
%define api.value.type{float}

/* Tokens */
%%token PLUS MINUS
%%token MULT DIV
%%LBRACE RBRACE
%% NUM
%% EOL
%%left PLUS MINUS
%%left MULT DIV

/* Regras de produção */

%%

/* Código C adicional (erro) */
void yyerror(char const *msg) {
   printf("parse error: %s\n", msg);
   exit(-1);
}

